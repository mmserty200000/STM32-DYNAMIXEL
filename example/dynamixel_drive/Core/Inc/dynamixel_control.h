/*
 * dynamixel_control.h
 *
 *  Created on: Jul 18, 2023
 *      Author: mmserty
 */

#ifndef INC_DYNAMIXEL_CONTROL_H_
#define INC_DYNAMIXEL_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stm32g0xx_hal.h"
//void dynamixel_init();
uint8_t calc_checksum(uint8_t *buff, uint8_t size);
HAL_StatusTypeDef dynamixel_send(UART_HandleTypeDef *huart, const uint8_t *send, uint8_t size_send, uint8_t *recieve, uint8_t size_recieve);
HAL_StatusTypeDef dynamixel_read_oneb(UART_HandleTypeDef *huart, uint8_t id, uint8_t reg, uint8_t *err, uint8_t *ret_dat);
HAL_StatusTypeDef dynamixel_read_twob(UART_HandleTypeDef *huart, uint8_t id, uint8_t reg, uint8_t *err, uint16_t *ret_dat);
HAL_StatusTypeDef dynamixel_write_oneb(UART_HandleTypeDef *huart, uint8_t id, uint8_t reg, uint8_t dat, uint8_t *err);
HAL_StatusTypeDef dynamixel_write_twob(UART_HandleTypeDef *huart, uint8_t id, uint8_t reg, uint16_t dat, uint8_t *err);
HAL_StatusTypeDef dynamixel_factory_reset(UART_HandleTypeDef *huart, uint8_t id, uint8_t *err);
HAL_StatusTypeDef dynamixel_en_torq(UART_HandleTypeDef *huart, uint8_t id,uint8_t torq, uint8_t *err);
HAL_StatusTypeDef dynamixel_set_goal_pos(UART_HandleTypeDef *huart, uint8_t id,uint16_t angl, uint8_t *err);
HAL_StatusTypeDef dynamixel_set_id(UART_HandleTypeDef *huart, uint8_t id,uint8_t new_id, uint8_t *err);
HAL_StatusTypeDef dynamixel_set_baud(UART_HandleTypeDef *huart, uint8_t id,uint8_t new_baud, uint8_t *err);


#ifdef __cplusplus
}
#endif

#endif /* INC_DYNAMIXEL_CONTROL_H_ */
