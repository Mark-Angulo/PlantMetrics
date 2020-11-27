#include "VEML7700.h"
#include "easy_i2c.h"


uint32_t reg_cache[4];

#define VEML7700_SLAVE_ADDR 0x10 //Light

#ifdef __TI_COMPILER_VERSION__
  void Delay(unsigned long ulCount) {
		__asm (" subs r0, #1\n"
		"	bne Delay \n"
		"	bx LR\n");
	}
#else
  void Delay(unsigned long ulCount) { 
		unsigned long d;
		for (d=0; d<ulCount; d++) {}
	}
#endif

#define CLOCKSPEED 80000000

//A General Purpose Delay
void delay(uint32_t t){ // t is in ms
	// The Delay function is exactly 3 ARM instructions and is therefore 3 clocks
	// Do (delay_in_seconds)*clock_speed/3 to get wanted value
  // (500ms)*80MHz/3 = 13,333,333.33
	//Delay(13333333); // Delay's 500ms
	Delay((t*CLOCKSPEED)/1000/3);
}


uint8_t veml_getGain(als_gain_t* gain)
{
  *gain = (als_gain_t)(
    (reg_cache[COMMAND_ALS_SM] & ALS_SM_MASK) >> ALS_SM_SHIFT );
  return STATUS_OK;
}

uint8_t veml_setGain(als_gain_t gain)
{ // yo
  uint32_t reg = ( (reg_cache[COMMAND_ALS_SM] & ~ALS_SM_MASK) | 
                   (((uint32_t)(gain) << ALS_SM_SHIFT) & ALS_SM_MASK) );
  reg_cache[COMMAND_ALS_SM] = reg;
  return veml_sendData(COMMAND_ALS_SM, reg);
}

uint8_t veml_setIntegrationTime(als_itime_t itime)
{
  uint32_t reg = ( (reg_cache[COMMAND_ALS_IT] & ~ALS_IT_MASK) | 
                   (((uint32_t)(itime) << ALS_IT_SHIFT) & ALS_IT_MASK) );
  reg_cache[COMMAND_ALS_IT] = reg;
  return veml_sendData(COMMAND_ALS_IT, reg);
}

uint8_t veml_getIntegrationTime(als_itime_t* itime)
{
  *itime = (als_itime_t)(
    (reg_cache[COMMAND_ALS_IT] & ALS_IT_MASK) >> ALS_IT_SHIFT );
  return STATUS_OK;
}

void veml_begin(uint8_t als_gain)
{
	uint8_t i = 0;
	
  // write initial state to DFRobot_VEML7700
  reg_cache[0] = ( ((uint32_t)(als_gain) << ALS_SM_SHIFT) |
                        ((uint32_t)(ALS_INTEGRATION_100ms) << ALS_IT_SHIFT) |
                        ((uint32_t)(ALS_PERSISTENCE_1) << ALS_PERS_SHIFT) |
                        ((uint32_t)(0) << ALS_INT_EN_SHIFT) |
                        ((uint32_t)(0) << ALS_SD_SHIFT) );
  reg_cache[1] = 0x0000;
  reg_cache[2] = 0xffff;
  reg_cache[3] = ( ((uint32_t)(ALS_POWER_MODE_3) << PSM_SHIFT) |
                        ((uint32_t)(0) << PSM_EN_SHIFT));
  
	for (i=0; i<4; i++){
    veml_sendData(i, reg_cache[i]);
  }

  // wait at least 2.5ms as per datasheet
  delay(3);
}


uint8_t veml_sendData(uint8_t command, uint32_t data)
{
	I2CSend(VEML7700_SLAVE_ADDR, 3, command, (uint8_t)(data&0xFF), (uint8_t)((data>>8)&0xFF));
	return I2CMasterErr(I2C0_BASE);
}

uint8_t veml_receiveData(uint8_t command, uint32_t* data)
{
	*data = I2CReceive(VEML7700_SLAVE_ADDR, command);
  return I2CMasterErr(I2C0_BASE);
}

uint8_t veml_getALS(uint32_t* als)
{
  return veml_receiveData(COMMAND_ALS, als);
}

//uint8_t veml_getALSLux(float* lux)
//{
//  uint32_t raw_counts;
//  uint8_t status = veml_getALS(&raw_counts);
//  veml_scaleLux(raw_counts, lux);
//  return status;
//}

//void veml_scaleLux(uint32_t raw_counts, float* lux)
//{
//	float factor1, factor2, result;
//  static uint8_t x1=0, x2=1, d8=0;
//	
//  als_gain_t gain;
//  als_itime_t itime;
//  veml_getGain(&gain);
//  veml_getIntegrationTime(&itime);

//  switch(gain & 0x3){
//  case ALS_GAIN_x1:
//    factor1 = 1.f;
//    break;
//  case ALS_GAIN_x2:
//    factor1 = 0.5f;
//    break;
//  case ALS_GAIN_d8:
//    factor1 = 8.f;
//    break;
//  case ALS_GAIN_d4:
//    factor1 = 4.f;
//    break;
//  default:
//    factor1 = 1.f;
//    break;
//  }

//  switch(itime){
//  case ALS_INTEGRATION_25ms:
//    factor2 = 0.2304f;
//    break;
//  case ALS_INTEGRATION_50ms:
//    factor2 = 0.1152f;
//    break;
//  case ALS_INTEGRATION_100ms:
//    factor2 = 0.0576f;
//    break;
//  case ALS_INTEGRATION_200ms:
//    factor2 = 0.0288f;
//    break;
//  case ALS_INTEGRATION_400ms:
//    factor2 = 0.0144f;
//    break;
//  case ALS_INTEGRATION_800ms:
//    factor2 = 0.0072f;
//    break;
//  default:
//    factor2 = 0.2304f;
//    break;
//  }

//  result = raw_counts * factor1 * factor2;
//  if((result > 1880.00f) && (result < 3771.00f)){
//	  if(x1 == 1){
//		veml_begin(ALS_GAIN_x1);
//		x1 = 0; x2 = 1; d8 = 1;
//	  }
//  }else if(result>3770.00f){
//	  if(d8 == 1){
//		veml_begin(ALS_GAIN_d8);
//		x1 = 1; x2 = 1; d8 = 0;
//	  }
//  }else{
//	  if(x2 == 1){
//		veml_begin(ALS_GAIN_x2);  
//		x1 = 1; x2 = 0; d8 = 1;
//	  }
//  }
//  *lux = result;
//  // apply correction from App. Note for all readings
//  //   using Horner's method
//  *lux = result * (1.0023f + result * (8.1488e-5f + result * (-9.3924e-9f + 
//                                                    result * 6.0135e-13f)));
//}
