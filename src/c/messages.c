#include "marta.h"

bool send_command(const char *command, int32_t index, bool include_index) {
  if (!s_phone_ready) {
    return false;
  }

  DictionaryIterator *iter;
  AppMessageResult result = app_message_outbox_begin(&iter);
  if (result != APP_MSG_OK || !iter) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "outbox begin failed: %d", result);
    return false;
  }

  int32_t request_id = s_request_id++;
  dict_write_cstring(iter, MESSAGE_KEY_COMMAND, command);
  dict_write_int32(iter, MESSAGE_KEY_REQUEST_ID, request_id);
  if (include_index) {
    dict_write_int32(iter, MESSAGE_KEY_INDEX, index);
  }

  dict_write_end(iter);
  result = app_message_outbox_send();
  if (result != APP_MSG_OK) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "outbox send failed: %d", result);
    return false;
  }

  return true;
}

void request_initial_state(void) {
  if (s_phone_ready) {
    send_command("INIT", 0, false);
  }
}

static void format_last_updated(int32_t timestamp) {
  if (timestamp <= 0) {
    copy_text(s_last_updated, sizeof(s_last_updated), "");
    return;
  }

  char time_text[TEXT_SHORT];
  format_watch_time(time_text, sizeof(time_text), (time_t)timestamp);
  snprintf(s_last_updated, sizeof(s_last_updated), "Last Updated %s", time_text);
}

static void parse_menu(DictionaryIterator *iter) {
  char kind[TEXT_SHORT];
  copy_tuple_string(iter, MESSAGE_KEY_MENU_KIND, kind, sizeof(kind));

  if (strcmp(kind, "closest") == 0) {
    s_menu_mode = MENU_MODE_CLOSEST;
    s_closest_count = tuple_int(iter, MESSAGE_KEY_MENU_COUNT, 0);
    if (s_closest_count > MAX_CLOSEST_STATIONS) {
      s_closest_count = MAX_CLOSEST_STATIONS;
    }
    for (int i = 0; i < s_closest_count; i++) {
      s_closest_indices[i] = tuple_int(iter, closest_index_key(i), 0);
      copy_tuple_string(iter, closest_distance_key(i), s_closest_distances[i],
                        sizeof(s_closest_distances[i]));
    }
  } else if (strcmp(kind, "all") == 0) {
    s_menu_mode = MENU_MODE_ALL;
  } else {
    s_menu_mode = MENU_MODE_LOADING;
  }

  menu_reload();
}

static void parse_station(DictionaryIterator *iter) {
  s_selected_station = tuple_int(iter, MESSAGE_KEY_SELECTED_STATION_INDEX, s_selected_station);
  s_train_count = tuple_int(iter, MESSAGE_KEY_TRAIN_COUNT, 0);
  if (s_train_count > MAX_TRAINS) {
    s_train_count = MAX_TRAINS;
  }

  int32_t last_updated_ts = tuple_int(iter, MESSAGE_KEY_LAST_UPDATED_TS, 0);
  if (last_updated_ts > 0) {
    format_last_updated(last_updated_ts);
  } else {
    copy_tuple_string(iter, MESSAGE_KEY_LAST_UPDATED, s_last_updated, sizeof(s_last_updated));
  }
  copy_tuple_string(iter, MESSAGE_KEY_ERROR, s_station_error, sizeof(s_station_error));

  for (int i = 0; i < s_train_count; i++) {
    copy_tuple_string(iter, train_line_key(i), s_trains[i].line, sizeof(s_trains[i].line));
    copy_tuple_string(iter, train_direction_key(i), s_trains[i].direction,
                      sizeof(s_trains[i].direction));
    copy_tuple_string(iter, train_destination_key(i), s_trains[i].destination,
                      sizeof(s_trains[i].destination));
    copy_tuple_string(iter, train_wait_key(i), s_trains[i].wait, sizeof(s_trains[i].wait));
  }

  station_update_train_scroll();
  if (s_station_window && !window_stack_contains_window(s_station_window)) {
    window_stack_push(s_station_window, true);
  }
  if (s_station_layer) {
    layer_mark_dirty(s_station_layer);
  }
}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
  if (dict_find(iter, MESSAGE_KEY_READY)) {
    s_phone_ready = true;
    request_initial_state();
    return;
  }

  Tuple *request_tuple = dict_find(iter, MESSAGE_KEY_REQUEST_ID);
  if (request_tuple && request_tuple->type == TUPLE_INT) {
    int32_t response_request_id = request_tuple->value->int32;
    if (response_request_id < s_latest_response_request_id) {
      APP_LOG(APP_LOG_LEVEL_INFO, "ignoring stale response: %ld", (long)response_request_id);
      return;
    }
    s_latest_response_request_id = response_request_id;
  }

  Tuple *screen_tuple = dict_find(iter, MESSAGE_KEY_SCREEN);
  if (!screen_tuple || screen_tuple->type != TUPLE_CSTRING) {
    return;
  }

  const char *screen = screen_tuple->value->cstring;
  if (strcmp(screen, "menu") == 0) {
    parse_menu(iter);
  } else if (strcmp(screen, "station") == 0) {
    parse_station(iter);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_WARNING, "inbox dropped: %d", reason);
}

static void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_WARNING, "outbox failed: %d", reason);
}

static void outbox_sent_callback(DictionaryIterator *iter, void *context) {
}

void app_message_init(void) {
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_open(2048, 256);
}
