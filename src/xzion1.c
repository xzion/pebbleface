#include <pebble.h>
#include "xzion1.h"
#include "messaging.h"
#include "bitcoin.h"
#include "resources.h"

#define UPDATE_INTERVAL_MINS 5

static Window *window;
TextLayer *text_layer;
TextLayer *time_layer;
TextLayer *date_layer;
TextLayer *ampm_layer;
TextLayer *icon_layer;
TextLayer *temp_layer;
BitmapLayer *batt_layer;
TextLayer *btc_layer;
TextLayer *fitbit_layer;

static uint8_t mincount;
static GBitmap *batt_img;

// Bluetooth Event Handler
static void handle_bluetooth(bool connected) {
	if (connected) {
		text_layer_set_text(icon_layer, "icon");
		vibes_long_pulse();
	} else {
		text_layer_set_text(icon_layer, "conn\nlost");
		vibes_long_pulse();
	}
}


// Battery Event Handler
// ** Always fires twice when connected/disconnect from charge
static void handle_battery(BatteryChargeState charge_state) {
	// Destroy previous bitmap
	gbitmap_destroy(batt_img);

	// Create new bitmap
	uint32_t batt_res_id = get_battery_resource(charge_state);
	batt_img = gbitmap_create_with_resource(batt_res_id);

	// Load onto layer
	bitmap_layer_set_bitmap(batt_layer, batt_img);

	// Debugging
	static bool doubletick = true;
	// Live Testing
	if (doubletick && js_initialized) {
		doubletick = false;

		request_bitcoin_price();

	} else {
		doubletick = true;
	}
}


// Minute Tick Handler
static void handle_minute_tick(struct tm *now, TimeUnits units_changed) {

	if(units_changed & MINUTE_UNIT) {
		// Update the interval counter      
		mincount++;

		// Change the time
		static char time_text[6];
		strftime(time_text, 6, "%I:%M", now);      
		text_layer_set_text(time_layer, time_text);

		// Try make this interval customizable?
		if (mincount >= UPDATE_INTERVAL_MINS && js_initialized) {
			// FIRE LAZERS HERE
			request_bitcoin_price();

			// Update the Battery Level
			//handle_battery(battery_state_service_peek());

			// Reset the counter
			mincount = 0;
		}

		// Debugging
		// static char mintext[20];
		// snprintf(mintext, 20, "Fitbit - %d", mincount);
		// text_layer_set_text(fitbit_layer, mintext);
	  
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

	// Initialize Globals
	mincount = 0;
	batt_img = gbitmap_create_with_resource(RESOURCE_ID_BATT_100);

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
	text_layer_set_background_color(icon_layer, GColorBlack);
	text_layer_set_text_color(icon_layer, GColorWhite);
	text_layer_set_text(icon_layer, "icon");
	text_layer_set_text_alignment(icon_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(icon_layer));

	// Build the temp layer
	temp_layer = text_layer_create(GRect(42, 69, 76, 40));
	text_layer_set_background_color(temp_layer, GColorBlack);
	text_layer_set_text_color(temp_layer, GColorWhite);
	text_layer_set_font(temp_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
	text_layer_set_text(temp_layer, "temp");
	text_layer_set_text_alignment(temp_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(temp_layer));

	// Build the battery layer
	batt_layer = bitmap_layer_create(GRect(120, 69, 24, 40));
	bitmap_layer_set_background_color(batt_layer, GColorBlack);
	layer_add_child(window_layer, bitmap_layer_get_layer(batt_layer));

	// Build the bitcoin price layer
	btc_layer = text_layer_create(GRect(0, 111, 144, 25));
	text_layer_set_font(btc_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text(btc_layer, "Loading...");
	text_layer_set_text_alignment(btc_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(btc_layer));

	// Build the fitbit layer
	fitbit_layer = text_layer_create(GRect(0, 138, 144, 30));
	text_layer_set_text(fitbit_layer, "Loading...");
	text_layer_set_text_alignment(fitbit_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(fitbit_layer));


	// Verify persistent storage for BTC daily value is setup
	check_bitcoin_persist();

	// Setup AppMessage
	app_message_init();

	// Subscribe to the minute tick
	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);

	// Force initial tick to inialize values
	time_t then = time(NULL);
	struct tm *now = localtime(&then);
	handle_minute_tick(now, SECOND_UNIT | MINUTE_UNIT | HOUR_UNIT | DAY_UNIT | MONTH_UNIT | YEAR_UNIT);

	// Subscribed to battery change events
	battery_state_service_subscribe(handle_battery);

	// Force initial battery check
	handle_battery(battery_state_service_peek());

	// Subscribe to bluetooth change events
	bluetooth_connection_service_subscribe(handle_bluetooth);

	// Force initial bluetooth state check
	handle_bluetooth(bluetooth_connection_service_peek());

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
	// Unsubscibe from events
	battery_state_service_unsubscribe();
	bluetooth_connection_service_unsubscribe();
	tick_timer_service_unsubscribe();

	// Destroy Graphics
	text_layer_destroy(text_layer);
	text_layer_destroy(time_layer);
	text_layer_destroy(date_layer);
	text_layer_destroy(ampm_layer);
	text_layer_destroy(icon_layer);
	text_layer_destroy(temp_layer);
	bitmap_layer_destroy(batt_layer);
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
