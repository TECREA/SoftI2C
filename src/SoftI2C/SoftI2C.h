/*
 * I2C.h
 *
 *  Created on: 15/01/2019
 *      Author: Manuel Posada
 */

#ifndef SOFT_I2C_H_
#define SOFT_I2C_H_

	#include <stdint.h>
	#include <stdlib.h>

	#define I2C_OUTPUT 0x00u
	#define I2C_INPUT  0x01u
	#define I2C_HIGH   0x01u
	#define I2C_LOW    0x00u

	#define TEMP1 0xE3    				// Measure Temperature, hold master mode
	#define TEMP2 0xF3     	  	     	// Measure Temperature, No hold master mode.
	#define SI7021A20_ADDRESS	0x40    // Device Address

	typedef void (*PinCtrlFcn_t) (uint8_t);
	typedef void (*DelayusFcn_t) (uint32_t);
	typedef uint8_t (*PinReadFcn_t) (void);

	typedef struct {
		PinCtrlFcn_t DirSCL, DirSDA;
		PinCtrlFcn_t CtrlSCL, CtrlSDA;
		PinReadFcn_t GetSDA;
		DelayusFcn_t Wait_us;
		uint32_t BaseTime;
	}I2C_Drive_t;

	I2C_Drive_t I2C_Obj;

	void I2C_Config_Init(I2C_Drive_t *Obj, PinCtrlFcn_t SDA_State, PinCtrlFcn_t SCL_State, PinCtrlFcn_t DirSDA, PinCtrlFcn_t DirSCL, PinReadFcn_t SDAIn, DelayusFcn_t usWaitFcn, uint32_t TimeBas);
	void I2C_BitStart(I2C_Drive_t *Obj);
	void I2C_BitStop(I2C_Drive_t *Obj);
	void I2C_WriteByte(I2C_Drive_t *Obj, uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t Data);
	uint8_t I2C_Send(I2C_Drive_t *Obj, uint8_t Data);
	uint8_t I2C_Get(I2C_Drive_t *Obj);
	uint8_t I2C_ReadByte(I2C_Drive_t *Obj,  uint8_t DeviceAddress, uint8_t RegisterAddress);
	void I2C_ReadBlock(I2C_Drive_t *Obj, uint8_t *Buffer, uint8_t BytesToRead,uint8_t DeviceAddress, uint8_t RegisterAddress);

#endif /* SOFT_I2C_H_ */
