#include <pebble.h>

Window *my_window;
TextLayer *text_layer;
static GFont time_font;
static BitmapLayer *background_layer;
static GBitmap *background_bitmap;
static GBitmap *message_bitmap;
static BitmapLayer *message_icon_layer;

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  static char time_buffer[8];
  strftime(time_buffer, sizeof(time_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  text_layer_set_text(text_layer, time_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}


static void load_window(Window *window) {
  //get window info
  Layer* window_layer = window_get_root_layer(window);
  window_set_background_color(window, GColorBlack);
    
  //set up bitmap and fonts
  background_bitmap = 
    gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
  time_font = 
    fonts_load_custom_font(resource_get_handle(RESOURCE_ID_GAMEPLAY_BOLD_36));
  message_bitmap = 
    gbitmap_create_with_resource(RESOURCE_ID_MESSAGE_ICON);
  
  
  //create layers
  GRect bounds = layer_get_bounds(window_layer);
  //TODO: fix background
  GRect bgbound = GRect(0,PBL_IF_ROUND_ELSE(58, 49), bounds.size.w, 70);
  background_layer = bitmap_layer_create(bgbound);
  message_icon_layer = bitmap_layer_create(GRect(5,125,24,24));
  bitmap_layer_set_bitmap(background_layer, background_bitmap);
  bitmap_layer_set_bitmap(message_icon_layer, message_bitmap);
  
  text_layer = 
    text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(58,64), bounds.size.w, 50));
  
  
  //set up text properties
  text_layer_set_text_color(text_layer, GColorWhite);  
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_font(text_layer, time_font);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  
  //add layers
  layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(message_icon_layer));
}

void handle_init(void) {
  my_window = window_create();

  load_window(my_window);
  window_stack_push(my_window, true);
  update_time();
}

void handle_deinit(void) {
  text_layer_destroy(text_layer);
  window_destroy(my_window);
  fonts_unload_custom_font(time_font);
  gbitmap_destroy(background_bitmap);
  bitmap_layer_destroy(background_layer);
}

int main(void) {
  handle_init();
  app_event_loop();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  handle_deinit();
}
