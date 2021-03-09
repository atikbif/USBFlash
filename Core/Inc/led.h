/*
 * led.h
 *
 *  Created on: 9 мар. 2021 г.
 *      Author: User
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include <stdint.h>

enum led_mode {LED_FLASH_BY_CMD, LED_TOGGLE, LED_ON_OFF};

struct led_state{
	enum led_mode mode;	// режим работы (вспышка по команде, мигание, статичный режим)
	uint8_t out_state;	// состояние выхода
	uint8_t on_cmd;		// команда для режима вспышки
	uint8_t off_cmd;	// запрещение работы диода
	uint16_t on_time;	// время во вкл состоянии
	uint16_t off_time;	// время в выкл состоянии
	uint16_t tmr;		// внутр. таймер
	uint8_t inverse;	// инверсный режим
};

void led_cycle(uint8_t add_value);
void init_leds();

#endif /* INC_LED_H_ */
