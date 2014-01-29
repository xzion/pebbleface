#include "pebble.h"
#include "xzion1.h"
#include "weather.h"
#include "resources.h"
#include "messaging.h"

GBitmap *cond_img;

void request_uq_temperature(void) {
	// Send AppMessage to phone requesting Temp
	DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    Tuplet value = TupletInteger(REQUEST_TEMP, 1);
    dict_write_tuplet(iter, &value);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting UQ Temperature!");
    app_message_outbox_send();
} 

void request_openweather(void) {
	// Send AppMessage to phone requesting Temp
	DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    Tuplet value = TupletInteger(REQUEST_OW, 1);
    dict_write_tuplet(iter, &value);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting OpenWeather!");
    app_message_outbox_send();
} 

void update_weather_conditions(int32_t cond) {
	
	gbitmap_destroy(cond_img);
	cond_img = gbitmap_create_with_resource(get_weather_resource(cond));

	if (conn_state) {
		bitmap_layer_set_bitmap(icon_layer, cond_img);
	}

}