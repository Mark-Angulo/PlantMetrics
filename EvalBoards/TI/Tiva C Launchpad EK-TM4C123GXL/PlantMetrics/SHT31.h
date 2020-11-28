#ifndef SHT31_H
#define SHT31_H

#include "stdint.h"
#include "stdio.h"
#include "tm4c123gh6pm.h"
#include "../bsp/driverlib/i2c.h"
#include "inc/hw_memmap.h"
#include "easy_i2c.h"

#define SHT31_DEFAULT_ADDR 0x44 /**< SHT31 Default Address */

#define SHT31_MEAS_HIGHREP_STRETCH                                             \
  0x2C06 /**< Measurement High Repeatability with Clock Stretch Enabled */
#define SHT31_MEAS_MEDREP_STRETCH                                              \
  0x2C0D /**< Measurement Medium Repeatability with Clock Stretch Enabled */
#define SHT31_MEAS_LOWREP_STRETCH                                              \
  0x2C10 /**< Measurement Low Repeatability with Clock Stretch Enabled*/
#define SHT31_MEAS_HIGHREP                                                     \
  0x2400 /**< Measurement High Repeatability with Clock Stretch Disabled */
#define SHT31_MEAS_MEDREP                                                      \
  0x240B /**< Measurement Medium Repeatability with Clock Stretch Disabled */
#define SHT31_MEAS_LOWREP                                                      \
  0x2416 /**< Measurement Low Repeatability with Clock Stretch Disabled */
	
#define SHT31_READSTATUS 0xF32D   /**< Read Out of Status Register */
#define SHT31_CLEARSTATUS 0x3041  /**< Clear Status */
#define SHT31_SOFTRESET 0x30A2    /**< Soft Reset */

bool SHT31_begin(void);
uint32_t SHT31_readTemperature(void);
uint32_t SHT31_readHumidity(void);
// uint16_t SHT31_readStatus(void);
uint16_t SHT31_readStatus(void);
void SHT31_reset(void);

bool SHT31_readTempHum(void);
bool SHT31_writeCommand(uint16_t cmd);
void SHT31_readCommand(uint16_t command, uint8_t* buf, uint8_t bytecount);




#endif
