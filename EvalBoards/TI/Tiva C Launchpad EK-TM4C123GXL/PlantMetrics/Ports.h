#ifndef _Ports_h
#define _Ports_h


#include <stdint.h>
#include "Math.h"
#include  "..\bsp\driverlib\i2c.h"
#include "inc/hw_memmap.h"
#include "VEML7700.h"
#include "SHT31.h"

void Ports_Init(void);

void Init_I2C0(void);
uint32_t I2CReceive(uint32_t slave_addr, uint8_t reg);

uint8_t Get_Temp(void); //Gets the soil temperature from the thermistor
uint32_t Get_Brightness(void); //Gets the brightness from the VEML7700
uint32_t Get_SoilMoisture(void);
uint32_t Get_EnviromentInfo(char infoType);
#endif
