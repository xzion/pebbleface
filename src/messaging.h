#ifndef xzion_messaging_h
#define xzion_messaging_h

extern bool js_initialized;

enum AppMessageKey {
  REQUEST_BTC = 1,
  REQUEST_WEATHER = 2,
  RETURN_BTC = 3,
  RETURN_TEMP = 4,
  JS_LOADED = 5,
  REQUEST_FITBIT = 6,
  RETURN_FITBIT = 7
};

void app_message_init(void);
void out_sent_handler(DictionaryIterator *sent, void *context);
void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context);
void in_received_handler(DictionaryIterator *received, void *context);
void in_dropped_handler(AppMessageResult reason, void *context);




#endif