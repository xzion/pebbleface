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