
#include "stdint.h"
#include "stdio.h"
#include "tm4c123gh6pm.h"
#include "../bsp/driverlib/i2c.h"
#include "inc/hw_memmap.h"

#define STATUS_OK 0 
#define STATUS_ERROR 0xff

typedef enum
  { ALS_GAIN_x1 = 0x0,    // x 1
    ALS_GAIN_x2 = 0x1,    // x 2
    ALS_GAIN_d8 = 0x2,    // x 1/8
    ALS_GAIN_d4 = 0x3 } als_gain_t;  // x 1/4
typedef enum 
  { ALS_INTEGRATION_25ms = 0xc,
    ALS_INTEGRATION_50ms = 0x8,
    ALS_INTEGRATION_100ms = 0x0,
    ALS_INTEGRATION_200ms = 0x1,
    ALS_INTEGRATION_400ms = 0x2,
    ALS_INTEGRATION_800ms = 0x3 } als_itime_t;
typedef enum 
  { ALS_PERSISTENCE_1 = 0x0,
    ALS_PERSISTENCE_2 = 0x1,
    ALS_PERSISTENCE_4 = 0x2,
    ALS_PERSISTENCE_8 = 0x3 } als_persist_t;
typedef enum 
  { ALS_POWER_MODE_1 = 0x0,
    ALS_POWER_MODE_2 = 0x1,
    ALS_POWER_MODE_3 = 0x2,
    ALS_POWER_MODE_4 = 0x3 } als_powmode_t;

   /*!
   *  @brief raw device commands
   */


uint8_t veml_setGain(als_gain_t gain);
uint8_t veml_getGain(als_gain_t* gain);
uint8_t veml_setIntegrationTime(als_itime_t itime);
uint8_t veml_getIntegrationTime(als_itime_t* itime);
/* Not implemented
uint8_t setPersistence(als_persist_t persist);
uint8_t setPowerSavingMode(als_powmode_t powmode);  
uint8_t setPowerSaving(uint8_t enabled);
uint8_t setInterrupts(uint8_t enabled);
uint8_t setPower(uint8_t on);
uint8_t setALSHighThreshold(uint32_t thresh);
uint8_t setALSLowThreshold(uint32_t thresh);
uint8_t getALS(uint32_t& als);
uint8_t getWhite(uint32_t& white);
uint8_t getHighThresholdEvent(uint8_t& event);
uint8_t getLowThresholdEvent(uint8_t& event);
*/

 /*!
 *  @brief higher-level API
 */
void veml_begin(uint8_t als_gain);
//uint8_t veml_getALSLux(float* lux);
//uint8_t veml_getWhiteLux(float* lux);
uint8_t veml_getALS(uint8_t* als);
/* Not implemented
uint8_t getAutoALSLux(float& lux);
uint8_t getAutoWhiteLux(float& lux);
uint8_t getAutoALSLux(float& lux,
											als_gain_t& auto_gain,
											als_itime_t& auto_itime,
											uint32_t& raw_counts);
uint8_t getAutoWhiteLux(float& lux,
												als_gain_t& auto_gain,
												als_itime_t& auto_itime,
												uint32_t& raw_counts);
*/
//uint8_t sampleDelay();


// typedef uint8_t (*getCountsFunction)(uint32_t& counts);
enum { I2C_ADDRESS = 0x10 };
enum { COMMAND_ALS_SM = 0x00, ALS_SM_MASK = 0x1800, ALS_SM_SHIFT = 11 };
enum { COMMAND_ALS_IT = 0x00, ALS_IT_MASK = 0x03c0, ALS_IT_SHIFT = 6 };
enum { COMMAND_ALS_PERS = 0x00, ALS_PERS_MASK = 0x0030, ALS_PERS_SHIFT = 4 };
enum { COMMAND_ALS_INT_EN = 0x00, ALS_INT_EN_MASK = 0x0002,
			 ALS_INT_EN_SHIFT = 1 };
enum { COMMAND_ALS_SD = 0x00,  ALS_SD_MASK = 0x0001, ALS_SD_SHIFT = 0 };
enum { COMMAND_ALS_WH = 0x01 };
enum { COMMAND_ALS_WL = 0x02 };
enum { COMMAND_PSM = 0x03, PSM_MASK = 0x0006, PSM_SHIFT = 1 };
enum { COMMAND_PSM_EN = 0x03, PSM_EN_MASK = 0x0001, PSM_EN_SHIFT = 0 };
enum { COMMAND_ALS = 0x04 };
enum { COMMAND_WHITE = 0x05 };
enum { COMMAND_ALS_IF_L = 0x06, ALS_IF_L_MASK = 0x8000, ALS_IF_L_SHIFT = 15 };
enum { COMMAND_ALS_IF_H = 0x06, ALS_IF_H_MASK = 0x4000, ALS_IF_H_SHIFT = 14 };

uint8_t veml_sendData(uint8_t command, uint32_t data);
uint8_t veml_receiveData(uint8_t command, uint8_t* data);
//void veml_scaleLux(uint32_t raw_counts, float* lux);
