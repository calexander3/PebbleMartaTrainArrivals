#include "marta.h"

Window *s_menu_window;
MenuLayer *s_menu_layer;
MenuMode s_menu_mode = MENU_MODE_LOADING;
bool s_phone_ready;
int32_t s_request_id = 1;
int s_closest_indices[MAX_CLOSEST_STATIONS];
char s_closest_distances[MAX_CLOSEST_STATIONS][TEXT_SHORT];
int s_closest_count;
int s_selected_station = -1;
Window *s_station_window;
Layer *s_station_layer;
ScrollLayer *s_train_scroll_layer;
Layer *s_train_content_layer;
GFont s_train_direction_font;
TrainRow s_trains[MAX_TRAINS];
int s_train_count;
char s_last_updated[TEXT_MEDIUM];
char s_station_error[TEXT_LONG];
