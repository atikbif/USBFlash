/*
 * device_state.c
 *
 *  Created on: 9 мар. 2021 г.
 *      Author: User
 */

#include "device_state.h"
#include "at45db081e.h"
#include "main.h"

struct dev_state state;

void check_insert() {
	if(HAL_GPIO_ReadPin(INS_GPIO_Port, INS_Pin)==GPIO_PIN_RESET) {
		state.insert = 1;
	}else {
		state.insert = 0;
		state.dev_init = 0;
	}
}

void init_device() {
	struct at_info info;

	state.insert = 0;
	state.flash1_error = 0;
	state.flash2_error = 0;
	state.usb_cmd = 0;
	state.usb_init = 0;
	state.dev_init = 0;

	check_insert();

	if(state.insert) {
		uint8_t res = 0;
		res = at45_get_info(1,&info);
		if(res) {
			if(info.manufacturer_id != 0x1F) state.flash1_error = 1;
			if(info.dev_id1 != 0x25) state.flash1_error = 1;
			if(info.dev_id2 != 0x00) state.flash1_error = 1;
			//if(info.ext_dev_info1 != 0x01) state.flash1_error = 1;
			//if(info.ext_dev_info2 != 0x00) state.flash1_error = 1;
		}
		if(res && state.flash1_error==0) at45_set_page_256(1);
		res = at45_get_info(2,&info);
		if(res) {
			if(info.manufacturer_id != 0x1F) state.flash2_error = 1;
			if(info.dev_id1 != 0x25) state.flash2_error = 1;
			if(info.dev_id2 != 0x00) state.flash2_error = 1;
			//if(info.ext_dev_info1 != 0x01) state.flash2_error = 1;
			//if(info.ext_dev_info2 != 0x00) state.flash2_error = 1;
		}
		if(res&& state.flash2_error==0)	at45_set_page_256(2);
		state.dev_init = 1;
	}
}
