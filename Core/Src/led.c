/*
 * led.c
 *
 *  Created on: 9 мар. 2021 г.
 *      Author: User
 */

#include "led.h"
#include "device_state.h"
#include "main.h"

struct led_state sys_led_green;
struct led_state sys_led_red;

#define LED_CNT		(2)

struct led_state * const leds[LED_CNT] = {&sys_led_green,&sys_led_red};

static const GPIO_TypeDef* led_gpio_ports[LED_CNT] = {
	LED1_GPIO_Port, LED2_GPIO_Port
};

static const uint16_t led_gpio_pins[LED_CNT] = {
	LED1_Pin, LED2_Pin
};

void set_on_off_time(struct led_state *led, uint16_t on_time, uint16_t off_time) {
	led->on_time = on_time;
	led->off_time = off_time;
}

static void leds_hardware() {
	for(uint8_t i=0;i<LED_CNT;++i) {
		if(leds[i]->inverse==0) {
			if(leds[i]->out_state) HAL_GPIO_WritePin(( GPIO_TypeDef*)led_gpio_ports[i],(uint16_t)led_gpio_pins[i],GPIO_PIN_SET);
			else HAL_GPIO_WritePin(( GPIO_TypeDef*)led_gpio_ports[i],(uint16_t)led_gpio_pins[i],GPIO_PIN_RESET);
		}else {
			if(leds[i]->out_state) HAL_GPIO_WritePin(( GPIO_TypeDef*)led_gpio_ports[i],(uint16_t)led_gpio_pins[i],GPIO_PIN_RESET);
			else HAL_GPIO_WritePin(( GPIO_TypeDef*)led_gpio_ports[i],(uint16_t)led_gpio_pins[i],GPIO_PIN_SET);
		}
	}
}

void init_leds() {
	sys_led_green.mode =  LED_FLASH_BY_CMD;
	sys_led_green.on_cmd = 0;
	sys_led_green.out_state = 0;
	sys_led_green.off_cmd = 0;
	sys_led_green.tmr = 0;
	sys_led_green.inverse = 0;
	set_on_off_time(&sys_led_green,10,10);

	sys_led_red.mode =  LED_FLASH_BY_CMD;
	sys_led_red.out_state = 0;
	sys_led_red.on_cmd = 0;
	sys_led_red.off_cmd = 0;
	sys_led_red.tmr = 0;
	sys_led_red.inverse = 0;
	set_on_off_time(&sys_led_red,10,10);
}

void led_cycle(uint8_t add_value) {
	for(uint8_t i=0;i<LED_CNT;i++) {
		struct led_state *led = leds[i];
		if(led->off_cmd) {
			led->out_state = 0;
			led->tmr = 0;
		}else {
			switch(led->mode) {
				case LED_FLASH_BY_CMD:
					if(led->on_cmd) {
						led->tmr+=add_value;
						if(led->tmr<led->on_time) led->out_state = 1;
						else led->out_state = 0;
						if(led->tmr>led->on_time + led->off_time) {
							led->tmr = 0;
							led->on_cmd = 0;
						}
					}else {
						led->out_state = 0;
						led->tmr = 0;
					}
					break;
				case LED_TOGGLE:
					led->tmr+=add_value;
					if(led->tmr<led->on_time) led->out_state = 1;
					else led->out_state = 0;
					if(led->tmr>=led->on_time + led->off_time) led->tmr = 0;
					break;
				case LED_ON_OFF:
					break;
			}
		}
	}
	leds_hardware();
}
