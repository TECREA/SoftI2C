#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SoftI2C.h>

void SDAPinControl(uint8_t x);
void SCLPinControl(uint8_t x);
void SDADirectionChange(uint8_t dir);
void SCLDirectionChange(uint8_t dir);

void SDAPinControl(uint8_t x){
    if(x) puts("A1");
    else  puts("A0");
}

void SCLPinControl(uint8_t x){
    if(x) puts("C1");
    else  puts("C0");
}

void SDADirectionChange(uint8_t x){
    if(x) puts("DDIR1");
    else  puts("DDIR0");
}

void SCLDirectionChange(uint8_t x){
    if(x) puts("CDIR1");
    else  puts("CDIR0");;
}

uint8_t SDAGetValue(void){
    static uint8_t x = 1;
    x^=x;
    return  x;
}

int main(int argc, char *argv[]){
    I2C_Drive_t testi2c;
    I2C_Config_Init( &testi2c, SDAPinControl, SCLPinControl,  SDADirectionChange, SCLDirectionChange, SDAGetValue, NULL, 1000);
    I2C_ReadByte( &testi2c, 0x80, 0xE3);
    
    return EXIT_SUCCESS;
}