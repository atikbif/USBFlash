/*
 * at45db081e.c
 *
 *  Created on: Mar 5, 2021
 *      Author: User
 */

#include "at45db081e.h"
#include "main.h"

extern SPI_HandleTypeDef hspi2;

static uint8_t write_data(uint8_t *buf, uint16_t len, uint8_t ms) {
	if(HAL_SPI_Transmit(&hspi2, buf, len,ms)==HAL_OK) return 1;
	return 0;
}

static uint8_t read_data(uint8_t *buf, uint16_t len, uint8_t ms) {
	if(HAL_SPI_Receive(&hspi2, buf, len,ms)==HAL_OK) return 1;
	return 0;
}

static void chip_select(uint8_t dev_num) {
	if(dev_num==1) HAL_GPIO_WritePin(CSN_GPIO_Port, CSN_Pin, GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(CSO_GPIO_Port, CSO_Pin, GPIO_PIN_RESET);
}

static void chip_deselect(uint8_t dev_num) {
	if(dev_num==1) HAL_GPIO_WritePin(CSN_GPIO_Port, CSN_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(CSO_GPIO_Port, CSO_Pin, GPIO_PIN_SET);
}

uint8_t at45_get_info(uint8_t dev_num, struct at_info *info) {
	uint8_t cmd = 0x9F;
	chip_select(dev_num);
	if(write_data(&cmd, 1, 50)) {
		if(read_data((uint8_t*)info, sizeof(info), 50)) {
			chip_deselect(dev_num);
			return 1;
		}
	}
	chip_deselect(dev_num);
	return 0;
}

uint8_t at45_read_page(uint8_t dev_num, uint16_t page_num, uint8_t *buf) {
	if(page_num>4095) return 0;
	static uint8_t tx_buf[8] = {0,0,0,0,0,0,0,0};
	HAL_Delay(1);
	tx_buf[0] = 0xD2;
	tx_buf[1] = page_num >> 8;
	tx_buf[2] = page_num & 0xFF;
	tx_buf[3] = 0x00;
	chip_select(dev_num);
	if(write_data(tx_buf, sizeof(tx_buf), 50)) {
		if(read_data(buf, 256, 50)) {
			chip_deselect(dev_num);
			return 1;
		}
	}
	chip_deselect(dev_num);
	return 0;
}

uint8_t at45_write_page(uint8_t dev_num, uint16_t page_num, uint8_t *buf) {
	if(page_num>4095) return 0;
	HAL_Delay(1);
	static uint8_t tx_buf[4] = {0,0,0,0};
	tx_buf[0] = 0x82;
	tx_buf[1] = page_num >> 8;
	tx_buf[2] = page_num & 0xFF;
	tx_buf[3] = 0x00;
	chip_select(dev_num);
	if(write_data(tx_buf, sizeof(tx_buf), 50)) {
		if(write_data(buf, 256,50)) {
			chip_deselect(dev_num);
			volatile uint16_t reg = 0;
			uint32_t t1 = HAL_GetTick();
			uint32_t t2 = 0;
			while(1) {
				t2 = HAL_GetTick();
				if(t2-t1>=55) return 0;
				if(at45_read_status_reg(dev_num, (uint16_t*)&reg)) {
					if((reg&=0x8080)==0x8080) {
						break;
					}
				}
			}
			return 1;
		}
	}
	chip_deselect(dev_num);
	return 0;
}

uint8_t at45_read_status_reg(uint8_t dev_num, uint16_t *reg) {
	uint8_t cmd = 0xD7;
	chip_select(dev_num);
	if(write_data(&cmd, 1, 50)) {
		if(read_data((uint8_t*)reg, 2, 50)) {
			chip_deselect(dev_num);
			return 1;
		}
	}
	chip_deselect(dev_num);
	return 0;
}

uint8_t at45_erase(uint8_t dev_num) {
	static uint8_t tx_buf[4] = {0xC7,0x94,0x80,0x9A};
	chip_select(dev_num);
	if(write_data(tx_buf, sizeof(tx_buf), 50)) {
		chip_deselect(dev_num);
		return 1;
	}
	chip_deselect(dev_num);
	return 0;
}

uint8_t at45_set_page_256(uint8_t dev_num) {
	static uint8_t tx_buf[4] = {0x3D,0x2A,0x80,0xA6};
	chip_select(dev_num);
	if(write_data(tx_buf, sizeof(tx_buf), 50)) {
		chip_deselect(dev_num);
		return 1;
	}
	chip_deselect(dev_num);
	return 0;
}
