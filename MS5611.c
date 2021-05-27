#include "MS5611.h"


/*
autor arthur_chevrier_chahidi
25/03/2020
 */
I2C_HandleTypeDef hi2c1;

/*
 * driver ms5611 build for I2C communication
 */

/*
 * Function for reseting the sensor
 */
void MS5611_Rest(I2C_HandleTypeDef* I2Cx)
{
	uint8_t RESET = (uint8_t)MS5611_CMD_REST;
	HAL_I2C_Master_Transmit(I2Cx, MS5611_SLAVE_ADDR, &RESET, 1, 1000);
	HAL_Delay(4);
}
/*
 * Function for reading PROM memories of the sensor
 */
int8_t MS5611_PROM_read(I2C_HandleTypeDef* I2Cx, MS5611_t* datastruct){

	uint8_t i;
	uint8_t data[2];
	uint8_t PROM[8] = {MS5611_PROM_READ_0,
			MS5611_PROM_READ_1,
			MS5611_PROM_READ_2,
			MS5611_PROM_READ_3,
			MS5611_PROM_READ_4,
			MS5611_PROM_READ_5,
			MS5611_PROM_READ_6,
			MS5611_PROM_READ_7
	};
	I2C_HandleTypeDef* Handle = I2Cx;
	uint8_t address = datastruct->adress;

	while(HAL_I2C_Master_Transmit(Handle, address, &PROM[0], 1, 100) != HAL_OK);

	while(HAL_I2C_Master_Receive(Handle, address, data, 2, 100) != HAL_OK);

	datastruct->reserve = (uint16_t)(data[0] << 8 | data[1]);

	for (i=1;i<=6;i++){

		while(HAL_I2C_Master_Transmit(Handle, address, &PROM[i], 1, 100) != HAL_OK);

		while(HAL_I2C_Master_Receive(Handle, address, data, 2, 100) != HAL_OK);

		datastruct->C[i-1] = (uint16_t )(data[0] << 8 | data[1]);
	}

	while(HAL_I2C_Master_Transmit(Handle, address, &PROM[7], 1, 100) != HAL_OK);

	while(HAL_I2C_Master_Receive(Handle, address, data, 2, 100) != HAL_OK);

	datastruct->crc = (uint16_t)(data[0] << 8 | data[1]);

	return MS5611_OK;
}
int8_t MS5611_init(I2C_HandleTypeDef* I2Cx, MS5611_t* datastruct)
{
	MS5611_Rest(I2Cx);
	datastruct->adress = MS5611_SLAVE_ADDR;
	MS5611_PROM_read(I2Cx,datastruct);
	return MS5611_OK;
}

/*
 * Function for reading raw temperature of the sensor
 */
int8_t MS5611_read_temp(I2C_HandleTypeDef* I2Cx, MS5611_t* datastruct, uint8_t resolution)
{
	I2C_HandleTypeDef* Handle = I2Cx;
	uint8_t address = datastruct->adress;
	uint8_t reg = MS6511_ADC_READ;
	uint8_t data[5];
	uint8_t cmd;
	uint8_t conv_T;

	switch(resolution){
	case MS5611_CMD_CONVERT_D2_256 : cmd = MS5611_CMD_CONVERT_D2_256; conv_T = 1;
	break;
	case MS5611_CMD_CONVERT_D2_512 : cmd = MS5611_CMD_CONVERT_D2_512; conv_T = 2;
	break;
	case MS5611_CMD_CONVERT_D2_1024 : cmd = MS5611_CMD_CONVERT_D2_1024; conv_T = 3;
	break;
	case MS5611_CMD_CONVERT_D2_2048 : cmd = MS5611_CMD_CONVERT_D2_2048; conv_T = 5;
	break;
	case MS5611_CMD_CONVERT_D2_4096 : cmd = MS5611_CMD_CONVERT_D2_4096; conv_T = 9;
	break;
	default : cmd = MS5611_CMD_CONVERT_D2_4096; conv_T = 9;
	}

	while(HAL_I2C_Master_Transmit(Handle, address, &cmd, 1, 100) != HAL_OK);//asking adc to store data
	HAL_Delay(conv_T); 														//convertion time
	while(HAL_I2C_Master_Transmit(Handle, address, &reg, 1, 100) != HAL_OK);//asking for the data
	while(HAL_I2C_Master_Receive(Handle, address, data, 3, 100) != HAL_OK);//receive the data

	datastruct->D[1] = (data[0] << 16 | data[1] << 8 | data[2]);
	return MS5611_OK;
}

/*
 * Function for reading raw pressure of the sensor
 */
int8_t MS5611_read_press (I2C_HandleTypeDef* I2Cx, MS5611_t* datastruct, uint8_t resolution)
{
	I2C_HandleTypeDef* Handle = I2Cx;
	uint8_t address = datastruct->adress;
	uint8_t reg = MS6511_ADC_READ;
	uint8_t data[3];
	uint8_t cmd;
	uint8_t conv_T;

	switch(resolution){
	case MS5611_CMD_CONVERT_D1_256 : cmd = MS5611_CMD_CONVERT_D1_256; conv_T = 1;
	break;
	case MS5611_CMD_CONVERT_D1_512 : cmd = MS5611_CMD_CONVERT_D1_512; conv_T = 2;
	break;
	case MS5611_CMD_CONVERT_D1_1024 : cmd = MS5611_CMD_CONVERT_D1_1024; conv_T = 3;
	break;
	case MS5611_CMD_CONVERT_D1_2048 : cmd = MS5611_CMD_CONVERT_D1_2048; conv_T = 5;
	break;
	case MS5611_CMD_CONVERT_D1_4096 : cmd = MS5611_CMD_CONVERT_D1_4096; conv_T = 9;
	break;
	default : cmd = MS5611_CMD_CONVERT_D1_4096; conv_T = 9;
	}

	while(HAL_I2C_Master_Transmit(Handle, address, &cmd, 1, 100) != HAL_OK);//asking adc to store data
	HAL_Delay(conv_T); 														//convertion time
	while(HAL_I2C_Master_Transmit(Handle, address, &reg, 1, 100) != HAL_OK);//asking for the data
	while(HAL_I2C_Master_Receive(Handle, address, data, 3, 100) != HAL_OK);//receive the data

	datastruct->D[0] = (data[0] << 16 | data[1] << 8 | data[2]);
	return MS5611_OK;
}

/*
 * Function for pressure and temperature calculation
 */
int8_t MS5611_calculate(MS5611_t* datastruct)
{
	int64_t dT = 0,TEMP = 0,T2 = 0,OFF = 0,OFF2 = 0,SENS2 = 0,SENS = 0,PRES = 0;

	dT = datastruct->D[1] - ((int32_t) (datastruct->C[4])<<8);
	TEMP = 2000 + ((int32_t) (dT*(datastruct->C[5]))>>23);
	OFF = (((int64_t)(datastruct->C[1])) << 16) + (((datastruct->C[3]) * dT) >> 7);
	SENS = (((int64_t)(datastruct->C[0])) << 15) + (((datastruct->C[2]) * dT) >> 8);

	if(TEMP < 2000) { //temperature < 20°C
		T2 = ( dT*dT )>>31;
		OFF2 = 5 * (TEMP - 2000) * (TEMP - 2000) / 2;
		SENS2 = 5 * (TEMP - 2000) * (TEMP - 2000) / 4;

		if (TEMP < -1500) { //temperature < -15°C
			OFF2 = OFF2 + (7 * (TEMP + 1500) * (TEMP + 1500));
			SENS2 = SENS2 + (11 * (TEMP + 1500) * (TEMP + 1500) / 2);
		}
	}
	else { //temperature > 20°C
		T2 = 0;
		OFF2 = 0;
		SENS2 = 0;
	}

	datastruct->dT = dT;
	datastruct->OFF = OFF - OFF2;
	datastruct->TEMP = TEMP - T2;
	datastruct->SENS = SENS - SENS2;

	PRES = ((((int32_t)(datastruct->D[0]) * (datastruct->SENS))>>21) - (datastruct->OFF))>>15;
	datastruct->P = PRES;
	return MS5611_OK;
}





