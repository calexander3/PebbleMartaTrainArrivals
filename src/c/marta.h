#pragma once

#include <pebble.h>

#define MAX_CLOSEST_STATIONS 5
#define MAX_TRAINS 20
#define TEXT_TINY 12
#define TEXT_SHORT 24
#define TEXT_MEDIUM 40
#define TEXT_LONG 80

typedef enum {
  MENU_MODE_LOADING,
  MENU_MODE_CLOSEST,
  MENU_MODE_ALL,
} MenuMode;

typedef struct {
  const char *name;
} Station;

typedef struct {
  char line[TEXT_TINY];
  char direction[TEXT_TINY];
  char destination[TEXT_MEDIUM];
  char wait[TEXT_SHORT];
} TrainRow;

extern const Station STATIONS[];
extern const int STATION_COUNT;

extern Window *s_menu_window;
extern MenuLayer *s_menu_layer;
extern MenuMode s_menu_mode;
extern bool s_phone_ready;
extern int32_t s_request_id;
extern int s_closest_indices[MAX_CLOSEST_STATIONS];
extern char s_closest_distances[MAX_CLOSEST_STATIONS][TEXT_SHORT];
extern int s_closest_count;
extern int s_selected_station;
extern Window *s_station_window;
extern Layer *s_station_layer;
extern ScrollLayer *s_train_scroll_layer;
extern Layer *s_train_content_layer;
extern GFont s_train_direction_font;
extern TrainRow s_trains[MAX_TRAINS];
extern int s_train_count;
extern char s_last_updated[TEXT_MEDIUM];
extern char s_station_error[TEXT_LONG];

GColor marta_black(void);
GColor marta_white(void);
GColor marta_orange(void);
GColor marta_menu_bg(void);
GColor marta_menu_highlight_text(void);
GColor marta_line_color(const char *line);

void copy_text(char *dest, size_t dest_size, const char *source);
void copy_tuple_string(DictionaryIterator *iter, uint32_t key, char *dest, size_t dest_size);
int32_t tuple_int(DictionaryIterator *iter, uint32_t key, int32_t fallback);
void format_watch_time(char *dest, size_t dest_size, time_t timestamp);

void app_message_init(void);
bool send_command(const char *command, int32_t index, bool include_index);
void request_initial_state(void);

void menu_window_init(void);
void menu_window_deinit(void);
void menu_reload(void);
void show_all_stations(void);

void station_window_init(void);
void station_window_deinit(void);
void show_station(int station_index);
void station_layer_update_proc(Layer *layer, GContext *ctx);
void train_content_update_proc(Layer *layer, GContext *ctx);
void station_update_train_scroll(void);

uint32_t closest_index_key(int row);
uint32_t closest_distance_key(int row);
uint32_t train_line_key(int row);
uint32_t train_direction_key(int row);
uint32_t train_destination_key(int row);
uint32_t train_wait_key(int row);
