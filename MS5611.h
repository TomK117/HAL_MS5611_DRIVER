#ifndef _MS5611_H
#define _MS5611_H
#include "main.h"
#include "stdio.h"
#include "stdlib.h"

#define MS5611_OK 0x01
#define MS5611_ERROR 0x00
//MS5611 ADDR CSB
#define MS5611_SLAVE_ADDR 0XEE

//COMMAND
#define MS5611_CMD_REST 0X1E 
#define MS5611_CMD_CONVERT_D1_256 0X40
#define MS5611_CMD_CONVERT_D1_512 0X42
#define MS5611_CMD_CONVERT_D1_1024 0X44
#define MS5611_CMD_CONVERT_D1_2048 0X46
#define MS5611_CMD_CONVERT_D1_4096 0X48
#define MS5611_CMD_CONVERT_D2_256 0X50
#define MS5611_CMD_CONVERT_D2_512 0X52
#define MS5611_CMD_CONVERT_D2_1024 0X54
#define MS5611_CMD_CONVERT_D2_2048 0X56
#define MS5611_CMD_CONVERT_D2_4096 0X58

#define MS6511_ADC_READ 0X00

#define MS5611_PROM_READ_0 0XA0
#define MS5611_PROM_READ_1 0XA2
#define MS5611_PROM_READ_2 0XA4
#define MS5611_PROM_READ_3 0XA6
#define MS5611_PROM_READ_4 0XA8
#define MS5611_PROM_READ_5 0XAA
#define MS5611_PROM_READ_6 0XAC
#define MS5611_PROM_READ_7 0XAE

typedef struct
{
	uint8_t adress;
	uint16_t C[6];	//PROM
	uint16_t reserve;
	uint16_t crc;
	uint32_t D[2];	//D1 temperature data & D2 pressure data
	int32_t dT;	//Difference between actual and reference temperature
	int64_t OFF;	//Offset at actual temperature
	int64_t SENS;	//Sensitivity at actual temperature
	int32_t TEMP;	//Actual temperature
	int32_t P;	//Actual pressure
}MS5611_t;

void MS5611_Rest(I2C_HandleTypeDef* I2Cx);
uint8_t MS5611_init(I2C_HandleTypeDef* I2Cx, MS5611_t* datastruct);
uint8_t MS5611_PROM_read(I2C_HandleTypeDef* I2Cx, MS5611_t* datastruct);
uint8_t MS5611_read_temp(I2C_HandleTypeDef* I2Cx, MS5611_t* datastruct, uint8_t resolution);
uint8_t NB_MS5611_request_temp(I2C_HandleTypeDef* I2Cx, MS5611_t* datastruct, uint8_t resolution);
uint8_t NB_MS5611_pull_temp(I2C_HandleTypeDef* I2Cx, MS5611_t* datastruct);
uint8_t MS5611_read_press (I2C_HandleTypeDef* I2Cx, MS5611_t* datastruct, uint8_t resolution);
uint8_t NB_MS5611_request_press (I2C_HandleTypeDef* I2Cx, MS5611_t* datastruct, uint8_t resolution);
uint8_t NB_MS5611_pull_press (I2C_HandleTypeDef* I2Cx, MS5611_t* datastruct);
uint8_t MS5611_calculate(MS5611_t* datastruct);

#endif
