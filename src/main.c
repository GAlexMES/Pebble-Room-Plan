#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_current_room;
static TextLayer *s_next_room;

static TextLayer *s_time_layer;

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
  text_layer_set_text(s_current_room, "128");
  text_layer_set_font(s_current_room, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_current_room, GTextAlignmentCenter);
  
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  text_layer_set_background_color(s_next_room, GColorClear);
  text_layer_set_text_color(s_next_room, GColorBlack);
  text_layer_set_text(s_next_room, "260");
  text_layer_set_font(s_next_room, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_next_room, GTextAlignmentCenter);


  layer_add_child(window_layer, text_layer_get_layer(s_current_room));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_next_room));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_current_room);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_next_room);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
