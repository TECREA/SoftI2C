/*
 * I2C.c
 *
 *  Created on: 15/01/2019
 *      Author: Manuel Posada
 */

#include "SoftI2C.h"

static void I2C_WaitInstr(uint32_t x);

/*============================================================================*/
void I2C_Config_Init(I2C_Drive_t *Obj, PinCtrlFcn_t SDA_State, PinCtrlFcn_t SCL_State, PinCtrlFcn_t DirSDA, PinCtrlFcn_t DirSCL, PinReadFcn_t SDAIn, DelayusFcn_t usWaitFcn, uint32_t TimeBase){
	Obj->CtrlSCL = SCL_State;
	Obj->CtrlSDA = SDA_State;
	Obj->DirSCL = DirSCL;
	Obj->DirSDA = DirSDA;
	Obj->GetSDA = SDAIn;
	Obj->Wait_us = (usWaitFcn==NULL)? I2C_WaitInstr : usWaitFcn;
	Obj->BaseTime = TimeBase;
	Obj->Wait_us(100);
	Obj->DirSCL(I2C_OUTPUT);
	Obj->DirSDA(I2C_OUTPUT);
	Obj->CtrlSCL(I2C_HIGH);
	Obj->CtrlSDA(I2C_HIGH);
}
/*============================================================================*/
void I2C_BitStart(I2C_Drive_t *Obj) {
	Obj->CtrlSCL(I2C_HIGH);
	Obj->CtrlSDA(I2C_HIGH);
	Obj->Wait_us(Obj->BaseTime);
	Obj->CtrlSDA(I2C_LOW);
	Obj->Wait_us(Obj->BaseTime);
}
/*============================================================================*/
void I2C_BitStop(I2C_Drive_t *Obj) {
    Obj->CtrlSCL(I2C_LOW);
    Obj->CtrlSDA(I2C_LOW);
    Obj->CtrlSCL(I2C_HIGH);
    Obj->Wait_us(Obj->BaseTime);
    Obj->CtrlSDA(I2C_HIGH);
    Obj->Wait_us(Obj->BaseTime);
}
/*============================================================================*/
uint8_t I2C_Send(I2C_Drive_t *Obj, uint8_t data) {
    uint8_t ReturnValue = 0x00u;
    int i;
    for( i=7; i>=0; i--) {
        Obj->CtrlSCL(I2C_LOW);
        Obj->CtrlSDA((data>>i) & 0x01u);
        Obj->Wait_us(Obj->BaseTime);
        Obj->CtrlSCL(I2C_HIGH);
        Obj->Wait_us(Obj->BaseTime);
    }

    Obj->CtrlSCL(I2C_LOW);
    Obj->DirSDA(I2C_INPUT);
    Obj->Wait_us(Obj->BaseTime);
    Obj->CtrlSCL(I2C_HIGH);
    Obj->Wait_us(Obj->BaseTime);

    ReturnValue = !Obj->GetSDA();
    Obj->Wait_us(Obj->BaseTime);
    Obj->CtrlSCL(I2C_LOW);
    Obj->DirSDA(I2C_OUTPUT);
    return ReturnValue;
}
/*============================================================================*/
uint8_t I2C_Get(I2C_Drive_t *Obj) {
    uint8_t data = 0;
    int i;
    Obj->DirSDA(I2C_INPUT);
    for(i=7; i>=0; i--) {
        Obj->CtrlSCL(I2C_LOW);
        Obj->Wait_us(Obj->BaseTime);
        Obj->CtrlSCL(I2C_HIGH);
        Obj->Wait_us(Obj->BaseTime);
        data |= ((Obj->GetSDA()&0x01u) << i);
        Obj->Wait_us(Obj->BaseTime);
    }
    Obj->CtrlSCL(I2C_LOW);
    Obj->DirSDA(I2C_OUTPUT);
    Obj->CtrlSDA(I2C_HIGH);
    Obj->Wait_us(Obj->BaseTime);
    Obj->CtrlSCL(I2C_HIGH);

    return data;
}
/*============================================================================*/
void I2C_WriteByte(I2C_Drive_t *Obj, uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t Data) {
    I2C_BitStart(Obj);
    I2C_Send(Obj, DeviceAddress);
    I2C_Send(Obj, RegisterAddress);
    I2C_Send(Obj, Data);
    I2C_BitStop(Obj);
}
/*============================================================================*/
uint8_t I2C_ReadByte(I2C_Drive_t *Obj, uint8_t DeviceAddress, uint8_t RegisterAddress) {
    uint8_t RetValue = 0x00;

    I2C_BitStart(Obj);
    I2C_Send(Obj, DeviceAddress);
    I2C_Send(Obj, RegisterAddress);
    I2C_BitStart(Obj);
    I2C_Send(Obj, DeviceAddress|0x01u);
    RetValue = I2C_Get(Obj);
    I2C_BitStop(Obj);
    return RetValue;
}
/*============================================================================*/
void I2C_ReadBlock(I2C_Drive_t *Obj, uint8_t *Buffer, uint8_t BytesToRead,uint8_t DeviceAddress, uint8_t RegisterAddress){
	uint8_t i, RetValue = 0x00;
	I2C_BitStart(Obj);
	I2C_Send(Obj, DeviceAddress);
	I2C_Send(Obj, RegisterAddress);
	I2C_BitStart(Obj);
	I2C_Send(Obj, DeviceAddress|0x01u);
	Obj->Wait_us(100);

	for (i=0; i<BytesToRead; i++) {
	    RetValue = I2C_Get(Obj);
	    Obj->Wait_us(100);
	}
	I2C_BitStop(Obj);
}
/*============================================================================*/
static void I2C_WaitInstr(uint32_t x){
    volatile uint32_t i,j;
    for(i=0 ; i< x; i++){
        j++;
    }
}
/*============================================================================*/