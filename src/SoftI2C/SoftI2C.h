#ifndef SOFT_I2C_H_
#define SOFT_I2C_H_

	#include <stdint.h>
	#include <stdlib.h>

	#define SOFTI2C_OUTPUT 		0x00u
	#define SOFTI2C_INPUT  		0x01u
	#define SOFTI2C_HIGH   		0x01u
	#define SOFTI2C_LOW    		0x00u

	typedef void (*SoftI2C_BitCtrlFcn_t) (uint8_t);
	typedef void (*SoftI2C_WaituSFcn_t) (uint32_t);
	typedef uint8_t (*SoftI2C_BitReadFcn_t) (void);

	typedef struct {
		SoftI2C_BitCtrlFcn_t DirSDA, CtrlSCL, CtrlSDA;
		SoftI2C_BitReadFcn_t GetSDA;
		SoftI2C_WaituSFcn_t Wait_us;
		uint32_t BaseTime;
	}SoftI2C_t;

	void SoftI2C_Init(SoftI2C_t *Obj, SoftI2C_BitCtrlFcn_t SDA_State, SoftI2C_BitCtrlFcn_t SCL_State, SoftI2C_BitCtrlFcn_t SDA_Direc, SoftI2C_BitReadFcn_t SDA_VRead, SoftI2C_WaituSFcn_t uSWaitFcn, uint32_t TimeBas);
	void SoftI2C_BitStart(SoftI2C_t *Obj);
	void SoftI2C_BitStop(SoftI2C_t *Obj);
	void SoftI2C_WriteByte(SoftI2C_t *Obj, uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t Data);
	uint8_t SoftI2C_Send(SoftI2C_t *Obj, uint8_t Data);
	uint8_t SoftI2C_Get(SoftI2C_t *Obj);
	uint8_t SoftI2C_ReadByte(SoftI2C_t *Obj,  uint8_t DeviceAddress, uint8_t RegisterAddress);
	void I2C_ReadBlock(SoftI2C_t *Obj, uint8_t DeviceAddress, uint8_t RegisterAddress, void *buf, uint8_t count);

#endif /* SOFT_I2C_H_ */
