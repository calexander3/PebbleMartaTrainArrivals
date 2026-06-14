#include "marta.h"

#if defined(PBL_PLATFORM_EMERY) || defined(PBL_PLATFORM_GABBRO)
#define TRAIN_ROW_SIZE 28
#define TRAIN_ROW_FONT FONT_KEY_GOTHIC_28
#define TRAIN_ROW_FONT_BOLD FONT_KEY_GOTHIC_28_BOLD
#define TRAIN_WAIT_WIDTH 64
#define TRAIN_DIR_WIDTH 24
#else
#define TRAIN_ROW_SIZE 25
#define TRAIN_ROW_FONT FONT_KEY_GOTHIC_24
#define TRAIN_ROW_FONT_BOLD FONT_KEY_GOTHIC_24_BOLD
#define TRAIN_WAIT_WIDTH 50
#define TRAIN_DIR_WIDTH 15
#endif

static void draw_text(GContext *ctx, const char *text, GFont font, GRect rect,
                      GTextAlignment alignment, GColor color) {
  graphics_context_set_text_color(ctx, color);
  graphics_draw_text(ctx, text ? text : "", font, rect, GTextOverflowModeTrailingEllipsis,
                     alignment, NULL);
}

static void station_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_selected_station >= 0) {
    send_command("REFRESH", s_selected_station, true);
  }
}

static void station_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, station_select_click_handler);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (s_station_layer && s_station_window && window_stack_contains_window(s_station_window)) {
    layer_mark_dirty(s_station_layer);
    if (s_selected_station >= 0) {
      send_command("REFRESH", s_selected_station, true);
    }
  }
}

static void station_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  int row_start = PBL_IF_ROUND_ELSE(50, 41);
  int footer_height = PBL_IF_ROUND_ELSE(42, 21);
  GRect train_frame = GRect(0, row_start, bounds.size.w, bounds.size.h - row_start - footer_height);

  s_train_scroll_layer = scroll_layer_create(train_frame);
  scroll_layer_set_shadow_hidden(s_train_scroll_layer, true);
  scroll_layer_set_callbacks(s_train_scroll_layer,
                             (ScrollLayerCallbacks){
                               .click_config_provider = station_click_config_provider,
                             });
  scroll_layer_set_click_config_onto_window(s_train_scroll_layer, window);
  layer_add_child(window_layer, scroll_layer_get_layer(s_train_scroll_layer));

  s_train_content_layer = layer_create(GRect(0, 0, train_frame.size.w, train_frame.size.h));
  layer_set_update_proc(s_train_content_layer, train_content_update_proc);
  scroll_layer_add_child(s_train_scroll_layer, s_train_content_layer);

  s_station_layer = layer_create(bounds);
  layer_set_update_proc(s_station_layer, station_layer_update_proc);
  layer_add_child(window_layer, s_station_layer);
  station_update_train_scroll();
}

static void station_window_unload(Window *window) {
  layer_destroy(s_train_content_layer);
  s_train_content_layer = NULL;
  scroll_layer_destroy(s_train_scroll_layer);
  s_train_scroll_layer = NULL;
  layer_destroy(s_station_layer);
  s_station_layer = NULL;
}

void station_window_init(void) {
  s_station_window = window_create();
  window_set_background_color(s_station_window, marta_black());
  window_set_window_handlers(s_station_window,
                             (WindowHandlers){
                               .load = station_window_load,
                               .unload = station_window_unload,
                             });
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

void station_window_deinit(void) {
  tick_timer_service_unsubscribe();
  if (s_station_window) {
    window_destroy(s_station_window);
    s_station_window = NULL;
  }
}

void show_station(int station_index) {
  s_selected_station = station_index;
  s_train_count = 0;
  copy_text(s_last_updated, sizeof(s_last_updated), "");
  copy_text(s_station_error, sizeof(s_station_error), "");

  station_update_train_scroll();
  if (s_station_layer) {
    layer_mark_dirty(s_station_layer);
  }
  if (s_station_window && !window_stack_contains_window(s_station_window)) {
    window_stack_push(s_station_window, true);
  }
}

void station_layer_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  int info_offset = PBL_IF_ROUND_ELSE(20, 0);
#ifndef PBL_ROUND
  int title_offset = PBL_IF_ROUND_ELSE(40, 0);
#endif
  int clock_y = PBL_IF_ROUND_ELSE(6, 0);
  int title_y = PBL_IF_ROUND_ELSE(24, 15);
  int row_start = PBL_IF_ROUND_ELSE(50, 41);
  int footer_height = PBL_IF_ROUND_ELSE(42, 21);

  graphics_context_set_fill_color(ctx, marta_black());
  graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, row_start), 0, GCornerNone);
  graphics_fill_rect(ctx, GRect(0, bounds.size.h - footer_height, bounds.size.w, footer_height), 0,
                     GCornerNone);

  char current_time[TEXT_SHORT];
  format_watch_time(current_time, sizeof(current_time), time(NULL));
  draw_text(ctx, current_time, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
            GRect(0, clock_y, bounds.size.w, 20), GTextAlignmentCenter, marta_white());

  const char *title =
    (s_selected_station >= 0 && s_selected_station < STATION_COUNT) ? STATIONS[s_selected_station].name
                                                                    : "";
#ifdef PBL_ROUND
  draw_text(ctx, title, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
            GRect(0, title_y, bounds.size.w, 30), GTextAlignmentCenter, marta_white());
#else
  draw_text(ctx, title, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
            GRect(5 + title_offset, title_y, bounds.size.w - 10 - title_offset, 30),
            GTextAlignmentLeft, marta_white());
#endif

  if (s_station_error[0]) {
    const char *error = s_train_count > 0 ? "Refresh Failed" : "Unable to contact Marta";
    int y = s_train_count > 0 ? bounds.size.h - footer_height - 21 : row_start;
    GFont font = s_train_count > 0 ? fonts_get_system_font(FONT_KEY_GOTHIC_14)
                                   : fonts_get_system_font(FONT_KEY_GOTHIC_24);
    draw_text(ctx, error, font, GRect(5 + info_offset, y, bounds.size.w - 10 - info_offset, 50),
              s_train_count > 0 ? GTextAlignmentRight : GTextAlignmentLeft, marta_white());
    return;
  }

  if (s_train_count == 0 && !s_last_updated[0]) {
    draw_text(ctx, "Refreshing...", fonts_get_system_font(FONT_KEY_GOTHIC_24),
              GRect(5 + info_offset, row_start, bounds.size.w - 10 - info_offset, 50),
              GTextAlignmentLeft, marta_white());
    return;
  }

  if (s_last_updated[0]) {
#ifdef PBL_ROUND
    const char *updated_time = s_last_updated;
    const char *prefix = "Last Updated ";
    if (strncmp(s_last_updated, prefix, strlen(prefix)) == 0) {
      updated_time = s_last_updated + strlen(prefix);
    }
    draw_text(ctx, "Last Updated", fonts_get_system_font(FONT_KEY_GOTHIC_18),
              GRect(0, bounds.size.h - 40, bounds.size.w, 19), GTextAlignmentCenter,
              marta_white());
    draw_text(ctx, updated_time, fonts_get_system_font(FONT_KEY_GOTHIC_18),
              GRect(0, bounds.size.h - 22, bounds.size.w, 19), GTextAlignmentCenter,
              marta_white());
#else
    draw_text(ctx, s_last_updated, fonts_get_system_font(FONT_KEY_GOTHIC_18),
              GRect(0, bounds.size.h - 21, bounds.size.w - 3, 21),
              GTextAlignmentRight, marta_white());
#endif
  }
}

void train_content_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  int info_offset = PBL_IF_ROUND_ELSE(20, 0);
  int row_size = TRAIN_ROW_SIZE;

  graphics_context_set_fill_color(ctx, marta_black());
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  for (int i = 0; i < s_train_count; i++) {
    int row_position = i * row_size;
    int wait_width = TRAIN_WAIT_WIDTH;
    int line_x = 5 + info_offset;
    int heading_x = line_x + TRAIN_DIR_WIDTH;
    int wait_x = bounds.size.w - wait_width - 2 - info_offset;
    int heading_width = wait_x - heading_x;

    draw_text(ctx, s_trains[i].direction, fonts_get_system_font(TRAIN_ROW_FONT_BOLD),
              GRect(line_x, row_position, TRAIN_DIR_WIDTH, row_size), GTextAlignmentLeft,
              marta_line_color(s_trains[i].line));
    draw_text(ctx, s_trains[i].destination, fonts_get_system_font(TRAIN_ROW_FONT),
              GRect(heading_x, row_position, heading_width, row_size), GTextAlignmentLeft,
              marta_white());
    draw_text(ctx, s_trains[i].wait, fonts_get_system_font(TRAIN_ROW_FONT),
              GRect(wait_x, row_position, wait_width, row_size), GTextAlignmentRight,
              marta_white());
  }
}

void station_update_train_scroll(void) {
  if (!s_train_scroll_layer || !s_train_content_layer) {
    return;
  }

  Layer *scroll_layer = scroll_layer_get_layer(s_train_scroll_layer);
  GRect viewport = layer_get_bounds(scroll_layer);
  int row_size = TRAIN_ROW_SIZE;
  int content_height = s_train_count * row_size;
  if (content_height < viewport.size.h) {
    content_height = viewport.size.h;
  }

  layer_set_frame(s_train_content_layer, GRect(0, 0, viewport.size.w, content_height));
  scroll_layer_set_content_size(s_train_scroll_layer, GSize(viewport.size.w, content_height));
  scroll_layer_set_content_offset(s_train_scroll_layer, GPointZero, false);
  layer_mark_dirty(s_train_content_layer);
}
