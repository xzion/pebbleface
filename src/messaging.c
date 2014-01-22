#include "pebble.h"
#include "xzion1.h"
#include "messaging.h"
#include "bitcoin.h"
#include "weather.h"

static uint8_t nackcount = 0;
static char temp_text[10];
bool js_initialized = false;


void app_message_init(void) {

	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_sent(out_sent_handler);
	app_message_register_outbox_failed(out_failed_handler);

	const int inbound_size = 64;
	const int outbound_size = 64;
	app_message_open(inbound_size, outbound_size);
}

// AppMessage Handlers
void out_sent_handler(DictionaryIterator *sent, void *context) {
   	// Outgoing message was successfully delivered (ACK)
  	//APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Out Sent!");
}


void in_dropped_handler(AppMessageResult reason, void *context) {
	// Incoming message dropped
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message In Dropped! Code: %d", reason);
}


void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
	// Outgoing message failed (Received NACK)
	nackcount++;
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Out Failed! Code: %d, Count: %d", reason, nackcount);

	if (nackcount > 3) {
		text_layer_set_text(btc_layer, "Multiple NACKs");
		vibes_short_pulse();
	}
}


void in_received_handler(DictionaryIterator *received, void *context) {
	// Incoming message received
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Got an App Message!!");
	nackcount = 0;

	// Check the dictionary for known keys
	Tuple *load_tuple = dict_find(received, JS_LOADED);
	if (load_tuple) {
		// JS has loaded, call an initial update of stuff!
		APP_LOG(APP_LOG_LEVEL_DEBUG, "JS Loaded Received, calling intial update");

		// Request initial data
		request_bitcoin_price();

		// Allow other components to send requests
		js_initialized = true;
	}

	Tuple *btc_tuple = dict_find(received, RETURN_BTC);
	if (btc_tuple) {
		// New BTC price received
		int32_t btcpr = btc_tuple->value->int32;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "BTC Price Received: %li", btcpr);

		update_bitcoin_price(btc_tuple->value->int32);

		// Call the next update
		if (use_uq_weather) {
			request_uq_temperature();
		} else {
			request_openweather();
		}
	}

	Tuple *uqtemp_tuple = dict_find(received, RETURN_TEMP);
	if (uqtemp_tuple) {
		snprintf(temp_text, 10, "%s", uqtemp_tuple->value->cstring);
		// New UQ temp received
		APP_LOG(APP_LOG_LEVEL_DEBUG, "UQ Temp Received: %s", uqtemp_tuple->value->cstring);

		// Set the temperature
		text_layer_set_text(temp_layer, temp_text);

		// Call the next update
		request_openweather();

	}

	Tuple *owtemp_tuple = dict_find(received, RETURN_OWTEMP);
	if (owtemp_tuple) {
		// New UQ temp received
		APP_LOG(APP_LOG_LEVEL_DEBUG, "OW Temp Received: %s", owtemp_tuple->value->cstring);

		// Set the temperature
		if (!use_uq_weather) {
			snprintf(temp_text, 10, "%s", owtemp_tuple->value->cstring);
			text_layer_set_text(temp_layer, temp_text);
		}
	}

	Tuple *cond_tuple = dict_find(received, RETURN_OW);
	if (cond_tuple) {
		// New weather conditions received
		int32_t cond = cond_tuple->value->int32;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "OW Condition Code Received: %li", cond);

		// Update the icon
		update_weather_conditions(cond);

		// Call the next update

	}



}

