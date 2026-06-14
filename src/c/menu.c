#include "marta.h"

static int station_index_for_row(int row) {
  if (s_menu_mode == MENU_MODE_CLOSEST) {
    return s_closest_indices[row];
  }
  return row;
}

static uint16_t get_num_sections_callback(MenuLayer *menu_layer, void *context) { return 1; }

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index,
                                      void *context) {
  if (s_menu_mode == MENU_MODE_LOADING) {
    return 1;
  }
  if (s_menu_mode == MENU_MODE_CLOSEST) {
    return s_closest_count + 1;
  }
  return STATION_COUNT;
}

static int16_t get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index,
                                          void *context) {
  return 28;
}

static void draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index,
                                 void *context) {
  const char *title = "Fetching Location...";
  if (s_menu_mode == MENU_MODE_CLOSEST) {
    title = "Closest Stations";
  } else if (s_menu_mode == MENU_MODE_ALL) {
    title = "Marta Stations";
  }

  GRect bounds = layer_get_bounds(cell_layer);
  graphics_context_set_fill_color(ctx, marta_black());
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  graphics_context_set_text_color(ctx, marta_white());
  graphics_draw_text(ctx, title, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
                     GRect(4, 3, bounds.size.w - 8, bounds.size.h),
                     GTextOverflowModeTrailingEllipsis,
                     PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft), NULL);
}

static void draw_title_row(GContext *ctx, const Layer *cell_layer, const char *title) {
#ifdef PBL_ROUND
  GRect bounds = layer_get_bounds(cell_layer);
  graphics_context_set_text_color(ctx, marta_white());
  graphics_draw_text(ctx, title, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
                     GRect(6, 6, bounds.size.w - 12, bounds.size.h - 6),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
#else
  menu_cell_title_draw(ctx, cell_layer, title);
#endif
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index,
                              void *context) {
  if (s_menu_mode == MENU_MODE_LOADING) {
    menu_cell_title_draw(ctx, cell_layer, " ");
    return;
  }

  if (s_menu_mode == MENU_MODE_CLOSEST && cell_index->row == s_closest_count) {
    draw_title_row(ctx, cell_layer, "All Stations");
    return;
  }

  int station_index = station_index_for_row(cell_index->row);
  if (station_index < 0 || station_index >= STATION_COUNT) {
    return;
  }

  if (s_menu_mode == MENU_MODE_CLOSEST && s_closest_distances[cell_index->row][0]) {
    menu_cell_basic_draw(ctx, cell_layer, STATIONS[station_index].name,
                         s_closest_distances[cell_index->row], NULL);
  } else {
    draw_title_row(ctx, cell_layer, STATIONS[station_index].name);
  }
}

static void select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  if (s_menu_mode == MENU_MODE_LOADING) {
    return;
  }

  if (s_menu_mode == MENU_MODE_CLOSEST && cell_index->row == s_closest_count) {
    show_all_stations();
    return;
  }

  int station_index = station_index_for_row(cell_index->row);
  if (station_index < 0 || station_index >= STATION_COUNT) {
    return;
  }

  show_station(station_index);
  send_command("SELECT_STATION", station_index, true);
}

static void select_long_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  show_all_stations();
}

static MenuLayerCallbacks s_menu_callbacks = {
  .get_num_sections = get_num_sections_callback,
  .get_num_rows = get_num_rows_callback,
  .get_header_height = get_header_height_callback,
  .draw_header = draw_header_callback,
  .draw_row = draw_row_callback,
  .select_click = select_callback,
  .select_long_click = select_long_callback,
};

static void menu_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, s_menu_callbacks);
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  menu_layer_set_normal_colors(s_menu_layer, marta_menu_bg(), marta_white());
  menu_layer_set_highlight_colors(s_menu_layer, marta_orange(), marta_menu_highlight_text());
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void menu_window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
  s_menu_layer = NULL;
}

void menu_window_init(void) {
  s_menu_window = window_create();
  window_set_background_color(s_menu_window, marta_black());
  window_set_window_handlers(s_menu_window,
                             (WindowHandlers){
                               .load = menu_window_load,
                               .unload = menu_window_unload,
                             });
  window_stack_push(s_menu_window, true);
}

void menu_window_deinit(void) {
  if (s_menu_window) {
    window_destroy(s_menu_window);
    s_menu_window = NULL;
  }
}

void menu_reload(void) {
  if (s_menu_layer) {
    menu_layer_reload_data(s_menu_layer);
    menu_layer_set_selected_index(s_menu_layer, MenuIndex(0, 0), MenuRowAlignCenter, false);
  }
}

void show_all_stations(void) {
  s_menu_mode = MENU_MODE_ALL;
  menu_reload();
}
