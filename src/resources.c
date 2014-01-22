#include "pebble.h"
#include "resources.h"

uint32_t get_battery_resource(BatteryChargeState charge_state) {
	char batt_text[30];
	uint32_t res_id;
	
	if(charge_state.is_charging) {
		// Load the new image
		res_id = RESOURCE_ID_BATT_CH;
		snprintf(batt_text, 30, "Battery img set: CH");
	} else {
		// Load the new image
		switch(charge_state.charge_percent/10) {
			case 10:
				res_id = RESOURCE_ID_BATT_100;
				snprintf(batt_text, 30, "Battery img set: 100");
				break;
			case 9:
				res_id = RESOURCE_ID_BATT_90;
				snprintf(batt_text, 30, "Battery img set: 90");
				break;
			case 8:
				res_id = RESOURCE_ID_BATT_80;
				snprintf(batt_text, 30, "Battery img set: 80");
				break;
			case 7:
				res_id = RESOURCE_ID_BATT_70;
				snprintf(batt_text, 30, "Battery img set: 70");
				break;
			case 6:
				res_id = RESOURCE_ID_BATT_60;
				snprintf(batt_text, 30, "Battery img set: 60");
				break;
			case 5:
				res_id = RESOURCE_ID_BATT_50;
				snprintf(batt_text, 30, "Battery img set: 50");
				break;
			case 4:
				res_id = RESOURCE_ID_BATT_40;
				snprintf(batt_text, 30, "Battery img set: 40");
				break;
			case 3:
				res_id = RESOURCE_ID_BATT_30;
				snprintf(batt_text, 30, "Battery img set: 30");
				break;
			case 2:
				res_id = RESOURCE_ID_BATT_20;
				snprintf(batt_text, 30, "Battery img set: 20");
				break;
			case 1:
				res_id = RESOURCE_ID_BATT_10;
				snprintf(batt_text, 30, "Battery img set: 10");
				break;
			default:
				res_id = RESOURCE_ID_BATT_0;
				snprintf(batt_text, 30, "Battery img set: 0");
				break;
		}
	}
	APP_LOG(APP_LOG_LEVEL_DEBUG, batt_text);
	return res_id;
}


uint32_t get_weather_resource(int32_t cond) {


	switch (cond) {
		case 200:
		case 201:
		case 210:
		case 211:
		case 230:
		case 231:
			return RESOURCE_ID_COND_LIGHT_STORM;
			break;
		case 202:
		case 212:
		case 221:
		case 232:
		case 781:
		case 900:
		case 901:
		case 902:
		case 960:
		case 961:
		case 962:
			return RESOURCE_ID_COND_HEAVY_STORM;
			break;
		case 300:
		case 301:
		case 302:
		case 310:
		case 311:
		case 313:
		case 321:
		case 500:
		case 501:
		case 520:
		case 521:
			return RESOURCE_ID_COND_LIGHT_RAIN;
			break;
		case 312:
		case 314:
		case 502:
		case 503:
		case 504:
		case 522:
		case 531:
			return RESOURCE_ID_COND_HEAVY_RAIN;
			break;
		case 600:
		case 601:
		case 615:
		case 616:
		case 620:
		case 621:
			return RESOURCE_ID_COND_LIGHT_SNOW;
			break;
		case 602:
		case 622:
			return RESOURCE_ID_COND_HEAVY_SNOW;
			break;
		case 511:
		case 611:
		case 612:
		case 903:
		case 906:
			return RESOURCE_ID_COND_HAIL;
			break;
		case 701:
		case 711:
		case 721:
		case 731:
		case 741:
		case 751:
		case 761:
			return RESOURCE_ID_COND_FOG;
			break;
		case 771:
		case 905:
		case 952:
		case 953:
		case 954:
		case 955:
		case 956:
		case 957:
		case 958:
		case 959:
			return RESOURCE_ID_COND_WINDY;
			break;
		case 950:
		case 951:
		case 800:
		case 904:
			// check night/day
			return RESOURCE_ID_COND_CLEAR_DAY;
			break;
		case 801:
		case 802:
		case 803:
			return RESOURCE_ID_COND_LIGHT_CLOUD;
			break;
		case 804:
			return RESOURCE_ID_COND_HEAVY_CLOUD;
			break;
		default:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Weather Condition ID not assigned: %li", cond);
			return RESOURCE_ID_COND_NA;
			break;
	}

}