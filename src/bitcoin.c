#include "pebble.h"
#include "xzion1.h"
#include "messaging.h"
#include "bitcoin.h"

#define PRICE_KEY 	0xb001
#define DAY_KEY 	0xb002


void request_bitcoin_price(void) {
	// Send AppMessage to phone requesting BTC price
	DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    Tuplet value = TupletInteger(REQUEST_BTC, 1);
    dict_write_tuplet(iter, &value);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting BTC!");
    app_message_outbox_send();
} 

void check_bitcoin_persist(void) {
	// Check price exists
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Checking persistent storage for bitcoin data");

	if (persist_exists(PRICE_KEY)) {
		int32_t current_price = persist_read_int(PRICE_KEY);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Persistent price exists! val = %li", current_price);
	} else {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Persistent price doesn't exist, setting to 0");
		persist_write_int(PRICE_KEY, 0);
	}

	// Check date exists
	if (persist_exists(DAY_KEY)) {
		int32_t current_day = persist_read_int(DAY_KEY);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Persistent day exists! val = %li", current_day);
	} else {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Persistent day doesn't exist, setting to 0");
		persist_write_int(DAY_KEY, 0);
	}
}

void update_bitcoin_price(int32_t newPrice) {

	int32_t delta_price = 0;
	int32_t delta_day = 0;

	// Get the current time
	time_t then = time(NULL);
  	struct tm *now = localtime(&then);

  	// Grab delta values from persistent storage
  	if (persist_exists(PRICE_KEY) && persist_exists(DAY_KEY)) {
  		// Both working
  		delta_price = persist_read_int(PRICE_KEY);
  		delta_day = persist_read_int(DAY_KEY);

  		if (delta_price == 0 || delta_day != now->tm_mday) {
  			// Set new daily price/date
  			APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating the Bitcoin Delta Price");
  			delta_price = newPrice;
  			delta_day = now->tm_mday;

  			// Write to persistent storage
  			persist_write_int(PRICE_KEY, delta_price);
  			persist_write_int(DAY_KEY, delta_day);
  		}
  	} else {
  		// Something not setup?
  		APP_LOG(APP_LOG_LEVEL_DEBUG, "Somehow persistent storage not setup, calling check and returning");
  		check_bitcoin_persist();
  		return;
  	}

	// Update the BTC layer with the latest price and change since midnight
	int16_t current_int = newPrice/100000;
	int8_t current_decimal = (newPrice-(current_int*100000))/1000;
	int16_t change_int = ((float)newPrice/(float)delta_price-1)*100;
	int8_t change_decimal = ((float)newPrice/(float)delta_price-1)*10000 - change_int*100;
	char change_dir = '+';
	char current_padder[3] = ".";
	char change_padder[3] = ".";
	
	if (change_decimal < 0) {
		change_dir = '-';
		change_int *= -1;
		change_decimal *= -1;
	}

	if (current_decimal < 10) {
		snprintf(current_padder, 3, ".0");
	}

	if (change_decimal < 10) {
		snprintf(change_padder, 3, ".0");
	}

	static char btc_text[30];
	snprintf(btc_text, 30, "B: $%d%s%d, %c%d%s%d%%", current_int, current_padder, 
		current_decimal, change_dir, change_int, change_padder, change_decimal);

	APP_LOG(APP_LOG_LEVEL_DEBUG, "BTC string: %s", btc_text);
	text_layer_set_text(btc_layer, btc_text);

}