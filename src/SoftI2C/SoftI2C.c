#include "SoftI2C.h"

static void SoftI2C_WaitInstr(uint32_t x);

/*============================================================================*/
/*void SoftI2C_Init(SoftI2C_t *Obj, PinCtrlFcn_t SDA_State, PinCtrlFcn_t SCL_State, PinCtrlFcn_t SDA_Direc, PinReadFcn_t SDAIn, DelayusFcn_t usWaitFcn, uint32_t TimeBase)
 
Initializes the instance of the Software-I2C Driver.

Parameters:

    - Obj : A pointer to the SoftI2C Instance
    - SDA_State : The wrapper function with hardware-specific code to control the output of the SDA pin.
                  See Note A.
    - SCL_State : The wrapper function with hardware-specific code to control the output of the SCL pin.
                  See Note A.
    - SDA_Direc : The wrapper function with hardware-specific code to control the direction of the SDA pin
                  See Note A.
    - SDA_VRead : The wrapper function with hardware-specific code to read the value of the SDA pin
                  See Note B.
    - uSWaitFcn :  The wrapper function with hardware-specific code to producte a blocking delay in Microseconds
                  See Note C. 
                  This argument can be ignored passing NULL. This cause a default emulated instruction delay
                  produced by the instance itself.
    - TimeBase :  The value passed as argument in the uSWaitFcn to produce the I2C bus clock. This value should
                  be tunned according to the desired bus frequency. 

    Notes:
        A.  Wrapper prototype for SoftI2C_BitCtrlFcn_t: void WrapperFcn(uint8_t x);
            where the argument x is the value to write in the pin output/direction register  
        B.  Wrapper prototype for SoftI2C_BitReadFcn_t: uint8_t WrapperFcn(void);
            where the return value should be the readed value of the target pin.
        C.  Wrapper prototype for SoftI2C_WaituSFcn_t: void WrapperFcn(uint32_t x);
            where the argument x is the value to wait in instruction cycles or microseconds    

*/
/*============================================================================*/
void SoftI2C_Init(SoftI2C_t *Obj, SoftI2C_BitCtrlFcn_t SDA_State, SoftI2C_BitCtrlFcn_t SCL_State, SoftI2C_BitCtrlFcn_t SDA_Direc, SoftI2C_BitReadFcn_t SDA_VRead, SoftI2C_WaituSFcn_t uSWaitFcn, uint32_t TimeBase){
	Obj->CtrlSCL = SCL_State;
	Obj->CtrlSDA = SDA_State;
	Obj->DirSDA = SDA_Direc;
	Obj->GetSDA = SDA_VRead;
	Obj->Wait_us = (uSWaitFcn==NULL)? SoftI2C_WaitInstr : uSWaitFcn;
	Obj->BaseTime = TimeBase;
	Obj->Wait_us(100);
	Obj->DirSDA(SOFTI2C_OUTPUT);
	Obj->CtrlSCL(SOFTI2C_HIGH);
	Obj->CtrlSDA(SOFTI2C_HIGH);
}
/*============================================================================*/
/*void SoftI2C_BitStart(SoftI2C_t *Obj)
 
Produce a Start condition.

Parameters:

    - Obj : A pointer to the SoftI2c Instance

*/
void SoftI2C_BitStart(SoftI2C_t *Obj){
	Obj->CtrlSCL(SOFTI2C_HIGH);
	Obj->CtrlSDA(SOFTI2C_HIGH);
	Obj->Wait_us(Obj->BaseTime);
	Obj->CtrlSDA(SOFTI2C_LOW);
	Obj->Wait_us(Obj->BaseTime);
}
/*============================================================================*/
/*void SoftI2C_BitStop(SoftI2C_t *Obj)
 
Produce a Stop condition.

Parameters:

    - Obj : A pointer to the SoftI2C Instance

*/
void SoftI2C_BitStop(SoftI2C_t *Obj) {
    Obj->CtrlSCL(SOFTI2C_LOW);
    Obj->CtrlSDA(SOFTI2C_LOW);
    Obj->CtrlSCL(SOFTI2C_HIGH);
    Obj->Wait_us(Obj->BaseTime);
    Obj->CtrlSDA(SOFTI2C_HIGH);
    Obj->Wait_us(Obj->BaseTime);
}
/*============================================================================*/
/*uint8_t SoftI2C_Send(SoftI2C_t *Obj, uint8_t data)
 
Attempts to write a single byte.

Parameters:

    - Obj : A pointer to the SoftI2C Instance
    - data: The data to be written

Return value:

    The dummy data readed after the write operation
*/
uint8_t SoftI2C_Send(SoftI2C_t *Obj, uint8_t data) {
    uint8_t ReturnValue = 0x00u;
    int i;
    for( i=7; i>=0; i--) {
        Obj->CtrlSCL(SOFTI2C_LOW);
        Obj->CtrlSDA((data>>i) & 0x01u);
        Obj->Wait_us(Obj->BaseTime);
        Obj->CtrlSCL(SOFTI2C_HIGH);
        Obj->Wait_us(Obj->BaseTime);
    }

    Obj->CtrlSCL(SOFTI2C_LOW);
    Obj->DirSDA(SOFTI2C_INPUT);
    Obj->Wait_us(Obj->BaseTime);
    Obj->CtrlSCL(SOFTI2C_HIGH);
    Obj->Wait_us(Obj->BaseTime);

    ReturnValue = !Obj->GetSDA();
    Obj->Wait_us(Obj->BaseTime);
    Obj->CtrlSCL(SOFTI2C_LOW);
    Obj->DirSDA(SOFTI2C_OUTPUT);
    return ReturnValue;
}
/*============================================================================*/
/*uint8_t SoftI2C_Get(SoftI2C_t *Obj)
 
Attempts to read a single byte.

Parameters:

    - Obj : A pointer to the SoftI2c Instance

Return value:

    The data readed
*/
uint8_t SoftI2C_Get(SoftI2C_t *Obj) {
    uint8_t data = 0;
    int i;
    Obj->DirSDA(SOFTI2C_INPUT);
    for(i=7; i>=0; i--) {
        Obj->CtrlSCL(SOFTI2C_LOW);
        Obj->Wait_us(Obj->BaseTime);
        Obj->CtrlSCL(SOFTI2C_HIGH);
        Obj->Wait_us(Obj->BaseTime);
        data |= ((Obj->GetSDA()&0x01u) << i);
        Obj->Wait_us(Obj->BaseTime);
    }
    Obj->CtrlSCL(SOFTI2C_LOW);
    Obj->DirSDA(SOFTI2C_OUTPUT);
    Obj->CtrlSDA(SOFTI2C_HIGH);
    Obj->Wait_us(Obj->BaseTime);
    Obj->CtrlSCL(SOFTI2C_HIGH);

    return data;
}
/*============================================================================*/
/*void SoftI2C_WriteByte(SoftI2C_t *Obj, uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t Data)
 
Attempts to write a single byte with addressing.

Parameters:

    - Obj : A pointer to the SoftI2c Instance
    - DeviceAddress : The target Slave-device  address in 8bit-format 
    - RegisterAddress: The requested register to read ot the target Slave-device. (8bit address asumed)
    - Data: The data to write in the specified RegisterAddress register to read ot the target slave.

*/
void SoftI2C_WriteByte(SoftI2C_t *Obj, uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t Data) {
    SoftI2C_BitStart(Obj);
    SoftI2C_Send(Obj, DeviceAddress);
    SoftI2C_Send(Obj, RegisterAddress);
    SoftI2C_Send(Obj, Data);
    SoftI2C_BitStop(Obj);
}
/*============================================================================*/
/*uint8_t SoftI2C_ReadByte(SoftI2C_t *Obj, uint8_t DeviceAddress, uint8_t RegisterAddress)
 
Attempts to read a single byte Attempts to with addressing.

Parameters:

    - Obj : A pointer to the SoftI2c Instance
    - DeviceAddress : The target Slave-device  address in 8bit-format 
    - RegisterAddress: The requested register to read ot the target Slave-device. (8bit address asumed)

Return value:

    The data readed
*/
uint8_t SoftI2C_ReadByte(SoftI2C_t *Obj, uint8_t DeviceAddress, uint8_t RegisterAddress) {
    uint8_t RetValue = 0x00;

    SoftI2C_BitStart(Obj);
    SoftI2C_Send(Obj, DeviceAddress);
    SoftI2C_Send(Obj, RegisterAddress);
    SoftI2C_BitStart(Obj);
    SoftI2C_Send(Obj, DeviceAddress|0x01u);
    RetValue = SoftI2C_Get(Obj);
    SoftI2C_BitStop(Obj);
    return RetValue;
}
/*============================================================================*/
/*void I2C_ReadBlock(SoftI2C_t *Obj, uint8_t DeviceAddress, uint8_t RegisterAddress, void *buf, uint8_t count)
 
Attempts to read up to <count> bytes from the SoftI2C instance into the buffer starting at <buf>.

Parameters:

    - Obj : A pointer to the SoftI2c Instance
    - DeviceAddress : The target Slave-device  address in 8bit-format 
    - RegisterAddress: The requested register to read ot the target Slave-device. (8bit address asumed)
    - buff: The memory buffer where the readed data should reside
    - count : the number of bytes requested

Return value:

    The data readed
*/
void I2C_ReadBlock(SoftI2C_t *Obj, uint8_t DeviceAddress, uint16_t RegisterAddress, uint8_t RegSize, void *buf, uint8_t count){
	uint8_t i;
    uint8_t *dat = (uint8_t*)buf;
	if (count==0) return; /*do nothing*/
    SoftI2C_BitStart(Obj);
	SoftI2C_Send(Obj, DeviceAddress);
	
    if(SOFTI2C_REG8BIT_REQUEST){
        SoftI2C_Send(Obj, RegisterAddress);
    }
    else{
        
    }
    
	
    SoftI2C_BitStart(Obj);
	SoftI2C_Send(Obj, DeviceAddress|0x01u);
	Obj->Wait_us(Obj->BaseTime);

	for (i=0; i<count; i++) {
	    dat[i] = SoftI2C_Get(Obj);
	    Obj->Wait_us(Obj->BaseTime);
	}
	SoftI2C_BitStop(Obj);
}
/*============================================================================*/
/*the default wait function when the user do not provide it*/
static void SoftI2C_WaitInstr(uint32_t x){
    volatile uint32_t i,j;
    for(i=0 ; i< x; i++){
        j++;
    }
}
/*============================================================================*/