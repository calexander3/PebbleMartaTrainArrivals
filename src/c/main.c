#include "marta.h"

static void init(void) {
  menu_window_init();
  station_window_init();
  app_message_init();
}

static void deinit(void) {
  station_window_deinit();
  menu_window_deinit();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
