#ifndef _Ports_h
#define _Ports_h


#include <stdint.h>
#include "Math.h"
#include  "..\bsp\driverlib\i2c.h"
#include "inc/hw_memmap.h"
#include "VEML7700.h"
#include "SHT31.h"

void Ports_Init(void);

// void Init_I2C0(void);

uint8_t Get_Temp(void); //Gets the soil temperature from the thermistor
uint16_t Get_Brightness(void);
uint32_t Get_SoilMoisture(void);
void Get_EnviromentInfo(uint16_t* hum, uint16_t* temp);
#endif
