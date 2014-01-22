#ifndef xzion_weather_h
#define xzion_weather_h

extern GBitmap *cond_img;

void request_uq_temperature(void);
void request_openweather(void);
void update_weather_conditions(int32_t cond);


#endif