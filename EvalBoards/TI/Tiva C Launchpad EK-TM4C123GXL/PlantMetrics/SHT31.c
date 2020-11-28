#include "SHT31.h"

int32_t humidity;
int32_t temp;
int i,j;
static uint8_t crc8(const uint8_t *data, int len);

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

/**
 * Initialises the I2C bus, and assigns the I2C address to us.
 *
 * @param i2caddr   The I2C address to use for the sensor.
 *
 * @return True if initialisation was successful, otherwise False.
 */
bool SHT31_begin(void) {

  SHT31_reset();
  return SHT31_readStatus() != 0xFFFF;
}

/**
 * Gets the current status register contents.
 *
 * @return The 16-bit status register.
 */
uint16_t SHT31_readStatus(void) {
  uint8_t* data;
	uint16_t stat;
	
	SHT31_readCommand(SHT31_READSTATUS, data, 3);

	//TODO: Translate this
  //i2c_dev->read(data, 3);

  stat = data[0];
  stat <<= 8;
  stat |= data[1];
	
  // Serial.println(stat, HEX);
  return stat;
}

/**
 * Performs a reset of the sensor to put it into a known state.
 */
void SHT31_reset(void) {
  SHT31_writeCommand(SHT31_SOFTRESET);
  delay(10);
}


/**
 * Gets a single temperature reading from the sensor.
 *
 * @return A float value indicating the temperature.
 */
uint32_t SHT31_readTemperature(void) {
  return (uint32_t)temp;
}

/**
 * Gets a single relative humidity reading from the sensor.
 *
 * @return A float value representing relative humidity.
 */
uint32_t SHT31_readHumidity(void) {
  return humidity;
}

/**
 * Performs a CRC8 calculation on the supplied values.
 *
 * @param data  Pointer to the data to use when calculating the CRC8.
 * @param len   The number of bytes in 'data'.
 *
 * @return The computed CRC8 value.
 */
static uint8_t crc8(const uint8_t *data, int len) {
  /*
   *
   * CRC-8 formula from page 14 of SHT spec pdf
   *
   * Test data 0xBE, 0xEF should yield 0x92
   *
   * Initialization data 0xFF
   * Polynomial 0x31 (x8 + x5 +x4 +1)
   * Final XOR 0x00
   */

  uint8_t POLYNOMIAL = 0x31;
  uint8_t crc = 0xFF;

  for (j = len; j; --j) {
    crc ^= *data++;

    for (i = 8; i; --i) {
      crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
    }
  }
  return crc;
}

/**
 * Internal function to perform a temp + humidity read.
 *
 * @return True if successful, otherwise false.
 */
bool SHT31_readTempHum(void) {
	uint8_t* readbuffer; 
	int32_t stemp, shum;
	
	SHT31_readCommand(SHT31_MEAS_MEDREP, readbuffer, 6);

  delay(20);

	
	/*
  if (readbuffer[2] != crc8(readbuffer, 2) ||
      readbuffer[5] != crc8(readbuffer + 3, 2))
    return false;
	*/
	
	// UARTprintf("data: %x,%x,%x,%x,%x,%x", readbuffer[0], readbuffer[1], readbuffer[2], readbuffer[3], readbuffer[4], readbuffer[5]);

	stemp = (int32_t)(((uint32_t)readbuffer[0] << 8) | readbuffer[1]);
  // simplified (65536 instead of 65535) integer version of:
  // temp = (stemp * 175.0f) / 65535.0f - 45.0f;
	
  stemp = ((4375 * stemp) >> 14) - 4500;
  temp = stemp / 100;

	shum = ((uint32_t)readbuffer[3] << 8) | readbuffer[4];
  // simplified (65536 instead of 65535) integer version of:
  // humidity = (shum * 100.0f) / 65535.0f;
  shum = (625 * shum) >> 12;
   humidity = shum / 100;

  return true;
}

void SHT31_readCommand(uint16_t command, uint8_t* buf, uint8_t bytecount) {
		I2CReceive(SHT31_DEFAULT_ADDR, command, buf, bytecount);
}


/**
 * Internal function to perform and I2C write.
 *
 * @param cmd   The 16-bit command ID to send.
 */
bool SHT31_writeCommand(uint16_t command) {
  uint8_t cmd[2];

  cmd[0] = command >> 8;
  cmd[1] = command & 0xFF;
	
	I2CSend(SHT31_DEFAULT_ADDR, 2, cmd[0], cmd[1]);
		
	return 0;
}
