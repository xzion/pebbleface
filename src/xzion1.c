#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static TextLayer *time_layer;
static TextLayer *date_layer;
static TextLayer *ampm_layer;
static TextLayer *icon_layer;
static TextLayer *temp_layer;
static TextLayer *batt_layer;
static TextLayer *btc_layer;
static TextLayer *fitbit_layer;



// Minute Tick Handler
void handle_minute_tick(struct tm *now, TimeUnits units_changed) {

  // Change the time
  if(units_changed & MINUTE_UNIT) {
      static char time_text[6];
              strftime(time_text, 6, "%I:%M", now);
              
      text_layer_set_text(time_layer, time_text);
  }
  
  // Change the date
  if(units_changed & DAY_UNIT) {
      static char date_text[21];
      strftime(date_text, 21, "%a - %d/%m/%y",  now);
      text_layer_set_text(date_layer, date_text);
  }

  // Change AM/PM
  if(units_changed & HOUR_UNIT) {
    if (now->tm_hour > 11) {
        text_layer_set_text(ampm_layer, "P\nM");
    }
    else {
        text_layer_set_text(ampm_layer, "A\nM");
    }
  }

  
  // Debugging
  /*
  static char testtext[10];
  snprintf(testtext, 10, "%d", now->tm_hour);
  text_layer_set_text(fitbit_layer, testtext);
  */

}


// Watchface initialization
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  // Set Background color to Black (For testing only)
  window_set_background_color(window, GColorBlack);



  GRect bounds = layer_get_bounds(window_layer);

  // For debugging
  static char boundtext[20];
  snprintf(boundtext, 20, "w=%d, h=%d\ntest", bounds.size.w, bounds.size.h);

  // Build the time layer
  time_layer = text_layer_create(GRect(0, -8, 122, 48));
  // text_layer_set_background_color(time_layer, GColorBlack);
  // text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_text(time_layer, "TIME");
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  // Build the ampm layer
  ampm_layer = text_layer_create(GRect(124, -2, 20, 42));
  text_layer_set_font(ampm_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text(ampm_layer, "A\nM");
  text_layer_set_text_alignment(ampm_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ampm_layer));

  // Build the date layer
  date_layer = text_layer_create(GRect(0, 42, 144, 25));
  text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text(date_layer, "Date");
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(date_layer));  

  // Build the weather icon layer
  icon_layer = text_layer_create(GRect(0, 69, 40, 40));
  text_layer_set_text(icon_layer, "icon");
  text_layer_set_text_alignment(icon_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(icon_layer));

  // Build the temp layer
  temp_layer = text_layer_create(GRect(42, 69, 80, 40));
  text_layer_set_text(temp_layer, "temp");
  text_layer_set_text_alignment(temp_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(temp_layer));

  // Build the battery layer
  batt_layer = text_layer_create(GRect(124, 69, 20, 40));
  text_layer_set_text(batt_layer, "b\na");
  text_layer_set_text_alignment(batt_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(batt_layer));

  // Build the bitcoin price layer
  btc_layer = text_layer_create(GRect(0, 111, 144, 25));
  text_layer_set_text(btc_layer, "BTC price");
  text_layer_set_text_alignment(btc_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(btc_layer));

  // Build the fitbit layer
  fitbit_layer = text_layer_create(GRect(0, 138, 144, 30));
  text_layer_set_text(fitbit_layer, "Fitbit");
  text_layer_set_text_alignment(fitbit_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(fitbit_layer));

  // Subscribe to the minute tick
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);

  // Force initial tick to inialize values
  time_t then = time(NULL);
  struct tm *now = localtime(&then);
  handle_minute_tick(now, SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT | DAY_UNIT | MONTH_UNIT | YEAR_UNIT);

  /*
  // Build the debugging text layer
  text_layer = text_layer_create(GRect(2, 54, 140, 80));
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_text(text_layer, boundtext);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  */
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
  text_layer_destroy(time_layer);
  text_layer_destroy(date_layer);
  text_layer_destroy(ampm_layer);
  text_layer_destroy(icon_layer);
  text_layer_destroy(temp_layer);
  text_layer_destroy(batt_layer);
  text_layer_destroy(btc_layer);
  text_layer_destroy(fitbit_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
