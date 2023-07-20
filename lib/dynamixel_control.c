/*
 * dynamixel_control.c
 *
 *  Created on: Jul 18, 2023
 *      Author: mmserty
 */

#include <dynamixel_control.h>

//AX-12A const

//reg addr
#define DYNAMIXEL_ID_REG           0x03
#define DYNAMIXEL_BAUD_REG         0x04
#define DYNAMIXEL_TORQ_EN_REG      0x18
#define DYNAMIXEL_GOAL_POS_REG     0x1E
#define DYNAMIXEL_MOV_FLG_REG      0x2E

//const id
#define DYNAMIXEL_BROADCAST_ID     0xFE

//instructions
#define DYNAMIXEL_PING_INST        0x01
#define DYNAMIXEL_READ_INST        0x02
#define DYNAMIXEL_WRITE_INST       0x03
#define DYNAMIXEL_FACTORY_RES_INST 0x06


//error
/*
 * Bit 7	0	                 All ok
 * Bit 6	Instruction Error	 In case of sending an undefined instruction or delivering the action instruction without the Reg Write instruction, it is set as 1
 * Bit 5	Overload Error	         When the current load cannot be controlled by the set Torque, it is set as 1
 * Bit 4	Checksum Error	         When the Checksum of the transmitted Instruction Packet is incorrect, it is set as 1
 * Bit 3	Range Error	         When an instruction is out of the range for use, it is set as 1
 * Bit 2	Overheating Error	 When internal temperature of DYNAMIXEL is out of the range of operating temperature set in the Control table, it is set as 1
 * Bit 1	Angle Limit Error	 When Goal Position is written out of the range from CW Angle Limit to CCW Angle Limit , it is set as 1
 * Bit 0	Input Voltage Error	 When the applied voltage is out of the range of operating voltage set in the Control table, it is as 1
 */


uint8_t calc_checksum(uint8_t *buff, uint8_t size) {
	uint16_t checksum = 0;
	for (uint8_t ind = 2; ind < (size - 1); ind++) {
		checksum = checksum + buff[ind];
	}
	return ~(checksum & 0xFF);
}

HAL_StatusTypeDef dynamixel_send(UART_HandleTypeDef *huart, const uint8_t *send,
		uint8_t size_send, uint8_t *recieve, uint8_t size_recieve) {
	HAL_StatusTypeDef res;
	res = HAL_HalfDuplex_EnableTransmitter(huart);
	if (res != HAL_OK) {
		return res;
	}
	res = HAL_UART_Transmit(huart, send, size_send, 100);
	if (res != HAL_OK) {
		return res;
	}
	//Status Packet will not be returned if Broadcast ID(0xFE) is used.
	if (send[2] != DYNAMIXEL_BROADCAST_ID) {
		res = HAL_HalfDuplex_EnableReceiver(huart);
		if (res != HAL_OK) {
			return res;
		}
		res = HAL_UART_Receive(huart, recieve, size_recieve, 100);
		if (res != HAL_OK) {
			return res;
		}
	} else {
		//Need set checksum and id in buff to pass checksum verification
		recieve[2] = DYNAMIXEL_BROADCAST_ID;
		recieve[size_recieve - 1] = 0x01;
	}
	return HAL_OK;
}

HAL_StatusTypeDef dynamixel_read_oneb(UART_HandleTypeDef *huart, uint8_t id,
		uint8_t reg, uint8_t *err, uint8_t *ret_dat) {
	HAL_StatusTypeDef ret;
	uint8_t buff_send[8] = { 0xFF, 0xFF, id, 0x04, DYNAMIXEL_READ_INST, reg,
			0x01, 0xFF };
	uint8_t buff_recieve[7];
	buff_send[7] = calc_checksum(buff_send, 8);

	ret = dynamixel_send(huart, buff_send, 8, buff_recieve, 7);
	if (ret != HAL_OK) {
		return ret;
	}

	if (calc_checksum(buff_recieve, 7) != buff_recieve[6]
			|| buff_recieve[2] != id) {
		return HAL_ERROR;
	}
	*err = buff_recieve[4];
	*ret_dat = buff_recieve[5];
	return HAL_OK;
}

HAL_StatusTypeDef dynamixel_read_twob(UART_HandleTypeDef *huart, uint8_t id,
		uint8_t reg, uint8_t *err, uint16_t *ret_dat) {
	HAL_StatusTypeDef ret;
	uint8_t buff_send[8] = { 0xFF, 0xFF, id, 0x04, DYNAMIXEL_READ_INST, reg,
			0x02, 0xFF };
	uint8_t buff_recieve[8];
	buff_send[7] = calc_checksum(buff_send, 8);

	ret = dynamixel_send(huart, buff_send, 8, buff_recieve, 8);
	if (ret != HAL_OK) {
		return ret;
	}

	if (calc_checksum(buff_recieve, 8) != buff_recieve[7]
			|| buff_recieve[2] != id) {
		return HAL_ERROR;
	}
	*err = buff_recieve[4];
	*ret_dat = ((uint16_t) (buff_recieve[6]) << 8) | (uint16_t) buff_recieve[5];
	return HAL_OK;
}

HAL_StatusTypeDef dynamixel_write_oneb(UART_HandleTypeDef *huart, uint8_t id,
		uint8_t reg, uint8_t dat, uint8_t *err) {
	HAL_StatusTypeDef ret;

	uint8_t buff_send[8] = { 0xFF, 0xFF, id, 0x04, DYNAMIXEL_WRITE_INST, reg,
			dat, 0xFF };
	uint8_t buff_recieve[6];
	buff_send[7] = calc_checksum(buff_send, 8);

	ret = dynamixel_send(huart, buff_send, 8, buff_recieve, 6);
	if (ret != HAL_OK) {
		return ret;
	}

	if (calc_checksum(buff_recieve, 6) != buff_recieve[5]
			|| buff_recieve[2] != id) {
		return HAL_ERROR;
	}
	*err = buff_recieve[4];
	return HAL_OK;
}

HAL_StatusTypeDef dynamixel_write_twob(UART_HandleTypeDef *huart, uint8_t id,
		uint8_t reg, uint16_t dat, uint8_t *err) {
	HAL_StatusTypeDef ret;
	uint8_t buff_send[9] = { 0xFF, 0xFF, id, 0x05, DYNAMIXEL_WRITE_INST, reg,
			(uint8_t) (dat), (uint8_t) (dat >> 8), 0xFF };
	uint8_t buff_recieve[6];
	buff_send[8] = calc_checksum(buff_send, 9);

	ret = dynamixel_send(huart, buff_send, 9, buff_recieve, 6);
	if (ret != HAL_OK) {
		return ret;
	}

	if (calc_checksum(buff_recieve, 6) != buff_recieve[5]
			|| buff_recieve[2] != id) {
		return HAL_ERROR;
	}
	*err = buff_recieve[4];
	return HAL_OK;
}

HAL_StatusTypeDef dynamixel_factory_reset(UART_HandleTypeDef *huart, uint8_t id,
		uint8_t *err) {
	HAL_StatusTypeDef ret;
	uint8_t buff_send[6] = { 0xFF, 0xFF, id, 0x02, DYNAMIXEL_FACTORY_RES_INST,
			0xFF };
	uint8_t buff_recieve[6];
	buff_send[5] = calc_checksum(buff_send, 6);

	ret = dynamixel_send(huart, buff_send, 6, buff_recieve, 6);
	if (ret != HAL_OK) {
		return ret;
	}

	if (calc_checksum(buff_recieve, 6) != buff_recieve[5]
			|| buff_recieve[2] != id) {
		return HAL_ERROR;
	}
	*err = buff_recieve[4];
	return HAL_OK;
}

HAL_StatusTypeDef dynamixel_en_torq(UART_HandleTypeDef *huart, uint8_t id,
		uint8_t torq, uint8_t *err) {
	//Torque Enable: 0 = Off, 1 = On
	HAL_StatusTypeDef ret;
	ret = dynamixel_write_oneb(huart, id, DYNAMIXEL_TORQ_EN_REG, torq, err);
	return ret;
}

HAL_StatusTypeDef dynamixel_set_goal_pos(UART_HandleTypeDef *huart, uint8_t id,
		uint16_t angl, uint8_t *err) {
	//Angle: 0 deg = 0, 150 deg = 512, 300 deg = 1023
	//Read more in ax-12a specification
	HAL_StatusTypeDef ret;
	ret = dynamixel_write_twob(huart, id, DYNAMIXEL_GOAL_POS_REG, angl, err);
	return ret;
}

HAL_StatusTypeDef dynamixel_set_id(UART_HandleTypeDef *huart, uint8_t id,
		uint8_t new_id, uint8_t *err) {
	//ID 0~253
	HAL_StatusTypeDef ret;
	ret = dynamixel_write_oneb(huart, id, DYNAMIXEL_ID_REG, new_id, err);
	return ret;
}

HAL_StatusTypeDef dynamixel_set_baud(UART_HandleTypeDef *huart, uint8_t id,
		uint8_t new_baud, uint8_t *err) {
	/*Val  Baud(bps) Margin of Error
	 * 1	1M	      0.000% (Default)
	 * 3	500,000	  0.000%
	 * 4	400,000	  0.000%
	 * 7	250,000	  0.000%
	 * 9	200,000	  0.000%
	 * 16	115200   -2.124%
	 * 34	57600	  0.794%
	 * 103	19200    -0.160%
	 * 207	9600	 -0.160%
	 */

	HAL_StatusTypeDef ret;
	ret = dynamixel_write_oneb(huart, id, DYNAMIXEL_BAUD_REG, new_baud, err);
	return ret;
}

