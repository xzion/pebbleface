#include "pebble.h"
#include "xzion1.h"
#include "messaging.h"
#include "bitcoin.h"

static uint8_t nackcount = 0;
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
		
	}

	Tuple *temp_tuple = dict_find(received, RETURN_TEMP);
	if (temp_tuple) {
		// New temp received
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Temp Received: %s", temp_tuple->value->cstring);
	}
}


void in_dropped_handler(AppMessageResult reason, void *context) {
	// Incoming message dropped
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message In Dropped! Code: %d", reason);
}