#include <pebble.h>
#define KEY_CURRENT_ROOM 0
#define KEY_NEXT_ROOM 1

static Window *s_main_window;
static TextLayer *s_current_room;
static TextLayer *s_next_room;
static TextLayer *s_time_layer;

// Will be called, when a new message from the phone arrives
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  static char current_room_buffer[32];
  static char next_room_buffer[32];
  
  Tuple *current_room_tuple = dict_find(iterator, KEY_CURRENT_ROOM);
  Tuple *next_room_tuple = dict_find(iterator, KEY_NEXT_ROOM);

  if(current_room_tuple && next_room_tuple){
    snprintf(current_room_buffer, sizeof(current_room_buffer), "%s", current_room_tuple->value->cstring);
    snprintf(next_room_buffer, sizeof(next_room_buffer), "%s", next_room_tuple->value->cstring);

    text_layer_set_text(s_current_room, current_room_buffer);
    text_layer_set_text(s_next_room, next_room_buffer);
  }
}

// loads the main window
// creates the text fields
static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_current_room = text_layer_create(
      GRect(0, 10, bounds.size.w, 50));
  s_time_layer = text_layer_create(
      GRect(0, 55, bounds.size.w, 50));
  s_next_room = text_layer_create(
      GRect(0, 100, bounds.size.w, 50));
  
  text_layer_set_background_color(s_current_room, GColorClear);
  text_layer_set_text_color(s_current_room, GColorBlack);
  text_layer_set_text(s_current_room, "");
  text_layer_set_font(s_current_room, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_current_room, GTextAlignmentCenter);
  
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  text_layer_set_background_color(s_next_room, GColorClear);
  text_layer_set_text_color(s_next_room, GColorBlack);
  text_layer_set_text(s_next_room, "");
  text_layer_set_font(s_next_room, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_next_room, GTextAlignmentCenter);


  layer_add_child(window_layer, text_layer_get_layer(s_current_room));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_next_room));
}

// updates the time text field with the current time
static void update_time() {
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);
  text_layer_set_text(s_time_layer, s_buffer);
}

// calls the update_time function
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

// is called, when the window should be unload
static void main_window_unload(Window *window) {
  text_layer_destroy(s_current_room);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_next_room);
}

// initialize importent content
static void init() {
  // creates the main window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  
  window_stack_push(s_main_window, true);
  
  // sets the function, that handles incomming messages
  app_message_register_inbox_received(inbox_received_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  //sets the tick_handler method. It will be called every Minute
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

// will destroy the main window
static void deinit() {
  window_destroy(s_main_window);
}



int main(void) {
  init();
  app_event_loop();
  deinit();
}
