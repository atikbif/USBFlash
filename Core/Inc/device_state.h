/*
 * device_state.h
 *
 *  Created on: 9 мар. 2021 г.
 *      Author: User
 */

#ifndef INC_DEVICE_STATE_H_
#define INC_DEVICE_STATE_H_

#include <stdint.h>

struct dev_state {
	uint8_t insert;
	uint8_t flash1_error;
	uint8_t flash2_error;
	uint8_t usb_cmd;
	uint8_t usb_init;
	uint8_t dev_init;
};

void init_device();
void check_insert();

#endif /* INC_DEVICE_STATE_H_ */
