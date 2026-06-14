#include "marta.h"

GColor marta_black(void) { return GColorBlack; }
GColor marta_white(void) { return GColorWhite; }

GColor marta_orange(void) {
#ifdef PBL_COLOR
  return GColorOrange;
#else
  return GColorWhite;
#endif
}

GColor marta_menu_bg(void) {
#ifdef PBL_COLOR
  return GColorBlack;
#else
  return GColorBlack;
#endif
}

GColor marta_menu_highlight_text(void) {
#ifdef PBL_COLOR
  return GColorWhite;
#else
  return GColorBlack;
#endif
}

GColor marta_line_color(const char *line) {
#ifdef PBL_COLOR
  if (strcmp(line, "BLUE") == 0) {
    return GColorVividCerulean;
  }
  if (strcmp(line, "GOLD") == 0) {
    return GColorYellow;
  }
  if (strcmp(line, "RED") == 0) {
    return GColorRed;
  }
  if (strcmp(line, "GREEN") == 0) {
    return GColorGreen;
  }
#endif
  return GColorWhite;
}

void copy_text(char *dest, size_t dest_size, const char *source) {
  if (!dest || !dest_size) {
    return;
  }
  snprintf(dest, dest_size, "%s", source ? source : "");
}

void copy_tuple_string(DictionaryIterator *iter, uint32_t key, char *dest, size_t dest_size) {
  Tuple *tuple = dict_find(iter, key);
  if (tuple && tuple->type == TUPLE_CSTRING) {
    copy_text(dest, dest_size, tuple->value->cstring);
  }
}

int32_t tuple_int(DictionaryIterator *iter, uint32_t key, int32_t fallback) {
  Tuple *tuple = dict_find(iter, key);
  return tuple ? tuple->value->int32 : fallback;
}

void format_watch_time(char *dest, size_t dest_size, time_t timestamp) {
  if (!dest || !dest_size) {
    return;
  }

  struct tm *time_info = localtime(&timestamp);
  if (!time_info) {
    copy_text(dest, dest_size, "");
    return;
  }

  if (clock_is_24h_style()) {
    strftime(dest, dest_size, "%H:%M", time_info);
    return;
  }

  strftime(dest, dest_size, "%I:%M %p", time_info);
  if (dest[0] == '0') {
    for (size_t i = 0; dest[i]; i++) {
      dest[i] = dest[i + 1];
    }
  }
}

uint32_t closest_index_key(int row) {
  switch (row) {
    case 0:
      return MESSAGE_KEY_CLOSEST_0_INDEX;
    case 1:
      return MESSAGE_KEY_CLOSEST_1_INDEX;
    case 2:
      return MESSAGE_KEY_CLOSEST_2_INDEX;
    case 3:
      return MESSAGE_KEY_CLOSEST_3_INDEX;
    default:
      return MESSAGE_KEY_CLOSEST_4_INDEX;
  }
}

uint32_t closest_distance_key(int row) {
  switch (row) {
    case 0:
      return MESSAGE_KEY_CLOSEST_0_DISTANCE;
    case 1:
      return MESSAGE_KEY_CLOSEST_1_DISTANCE;
    case 2:
      return MESSAGE_KEY_CLOSEST_2_DISTANCE;
    case 3:
      return MESSAGE_KEY_CLOSEST_3_DISTANCE;
    default:
      return MESSAGE_KEY_CLOSEST_4_DISTANCE;
  }
}

uint32_t train_line_key(int row) {
  switch (row) {
    case 0:
      return MESSAGE_KEY_TRAIN_0_LINE;
    case 1:
      return MESSAGE_KEY_TRAIN_1_LINE;
    case 2:
      return MESSAGE_KEY_TRAIN_2_LINE;
    case 3:
      return MESSAGE_KEY_TRAIN_3_LINE;
    case 4:
      return MESSAGE_KEY_TRAIN_4_LINE;
    case 5:
      return MESSAGE_KEY_TRAIN_5_LINE;
    case 6:
      return MESSAGE_KEY_TRAIN_6_LINE;
    case 7:
      return MESSAGE_KEY_TRAIN_7_LINE;
    case 8:
      return MESSAGE_KEY_TRAIN_8_LINE;
    case 9:
      return MESSAGE_KEY_TRAIN_9_LINE;
    case 10:
      return MESSAGE_KEY_TRAIN_10_LINE;
    case 11:
      return MESSAGE_KEY_TRAIN_11_LINE;
    case 12:
      return MESSAGE_KEY_TRAIN_12_LINE;
    case 13:
      return MESSAGE_KEY_TRAIN_13_LINE;
    case 14:
      return MESSAGE_KEY_TRAIN_14_LINE;
    case 15:
      return MESSAGE_KEY_TRAIN_15_LINE;
    case 16:
      return MESSAGE_KEY_TRAIN_16_LINE;
    case 17:
      return MESSAGE_KEY_TRAIN_17_LINE;
    case 18:
      return MESSAGE_KEY_TRAIN_18_LINE;
    default:
      return MESSAGE_KEY_TRAIN_19_LINE;
  }
}

uint32_t train_direction_key(int row) {
  switch (row) {
    case 0:
      return MESSAGE_KEY_TRAIN_0_DIRECTION;
    case 1:
      return MESSAGE_KEY_TRAIN_1_DIRECTION;
    case 2:
      return MESSAGE_KEY_TRAIN_2_DIRECTION;
    case 3:
      return MESSAGE_KEY_TRAIN_3_DIRECTION;
    case 4:
      return MESSAGE_KEY_TRAIN_4_DIRECTION;
    case 5:
      return MESSAGE_KEY_TRAIN_5_DIRECTION;
    case 6:
      return MESSAGE_KEY_TRAIN_6_DIRECTION;
    case 7:
      return MESSAGE_KEY_TRAIN_7_DIRECTION;
    case 8:
      return MESSAGE_KEY_TRAIN_8_DIRECTION;
    case 9:
      return MESSAGE_KEY_TRAIN_9_DIRECTION;
    case 10:
      return MESSAGE_KEY_TRAIN_10_DIRECTION;
    case 11:
      return MESSAGE_KEY_TRAIN_11_DIRECTION;
    case 12:
      return MESSAGE_KEY_TRAIN_12_DIRECTION;
    case 13:
      return MESSAGE_KEY_TRAIN_13_DIRECTION;
    case 14:
      return MESSAGE_KEY_TRAIN_14_DIRECTION;
    case 15:
      return MESSAGE_KEY_TRAIN_15_DIRECTION;
    case 16:
      return MESSAGE_KEY_TRAIN_16_DIRECTION;
    case 17:
      return MESSAGE_KEY_TRAIN_17_DIRECTION;
    case 18:
      return MESSAGE_KEY_TRAIN_18_DIRECTION;
    default:
      return MESSAGE_KEY_TRAIN_19_DIRECTION;
  }
}

uint32_t train_destination_key(int row) {
  switch (row) {
    case 0:
      return MESSAGE_KEY_TRAIN_0_DESTINATION;
    case 1:
      return MESSAGE_KEY_TRAIN_1_DESTINATION;
    case 2:
      return MESSAGE_KEY_TRAIN_2_DESTINATION;
    case 3:
      return MESSAGE_KEY_TRAIN_3_DESTINATION;
    case 4:
      return MESSAGE_KEY_TRAIN_4_DESTINATION;
    case 5:
      return MESSAGE_KEY_TRAIN_5_DESTINATION;
    case 6:
      return MESSAGE_KEY_TRAIN_6_DESTINATION;
    case 7:
      return MESSAGE_KEY_TRAIN_7_DESTINATION;
    case 8:
      return MESSAGE_KEY_TRAIN_8_DESTINATION;
    case 9:
      return MESSAGE_KEY_TRAIN_9_DESTINATION;
    case 10:
      return MESSAGE_KEY_TRAIN_10_DESTINATION;
    case 11:
      return MESSAGE_KEY_TRAIN_11_DESTINATION;
    case 12:
      return MESSAGE_KEY_TRAIN_12_DESTINATION;
    case 13:
      return MESSAGE_KEY_TRAIN_13_DESTINATION;
    case 14:
      return MESSAGE_KEY_TRAIN_14_DESTINATION;
    case 15:
      return MESSAGE_KEY_TRAIN_15_DESTINATION;
    case 16:
      return MESSAGE_KEY_TRAIN_16_DESTINATION;
    case 17:
      return MESSAGE_KEY_TRAIN_17_DESTINATION;
    case 18:
      return MESSAGE_KEY_TRAIN_18_DESTINATION;
    default:
      return MESSAGE_KEY_TRAIN_19_DESTINATION;
  }
}

uint32_t train_wait_key(int row) {
  switch (row) {
    case 0:
      return MESSAGE_KEY_TRAIN_0_WAIT;
    case 1:
      return MESSAGE_KEY_TRAIN_1_WAIT;
    case 2:
      return MESSAGE_KEY_TRAIN_2_WAIT;
    case 3:
      return MESSAGE_KEY_TRAIN_3_WAIT;
    case 4:
      return MESSAGE_KEY_TRAIN_4_WAIT;
    case 5:
      return MESSAGE_KEY_TRAIN_5_WAIT;
    case 6:
      return MESSAGE_KEY_TRAIN_6_WAIT;
    case 7:
      return MESSAGE_KEY_TRAIN_7_WAIT;
    case 8:
      return MESSAGE_KEY_TRAIN_8_WAIT;
    case 9:
      return MESSAGE_KEY_TRAIN_9_WAIT;
    case 10:
      return MESSAGE_KEY_TRAIN_10_WAIT;
    case 11:
      return MESSAGE_KEY_TRAIN_11_WAIT;
    case 12:
      return MESSAGE_KEY_TRAIN_12_WAIT;
    case 13:
      return MESSAGE_KEY_TRAIN_13_WAIT;
    case 14:
      return MESSAGE_KEY_TRAIN_14_WAIT;
    case 15:
      return MESSAGE_KEY_TRAIN_15_WAIT;
    case 16:
      return MESSAGE_KEY_TRAIN_16_WAIT;
    case 17:
      return MESSAGE_KEY_TRAIN_17_WAIT;
    case 18:
      return MESSAGE_KEY_TRAIN_18_WAIT;
    default:
      return MESSAGE_KEY_TRAIN_19_WAIT;
  }
}
