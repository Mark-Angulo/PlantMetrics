#include "SHT31.h"

float humidity;
float temp;
int i,j;

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
  uint8_t data[3];
	uint16_t stat;
	
	SHT31_writeCommand(SHT31_READSTATUS);

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
  //delay(10);
}


/**
 * Gets a single temperature reading from the sensor.
 *
 * @return A float value indicating the temperature.
 */
uint32_t SHT31_readTemperature(void) {
  if (!SHT31_readTempHum())
    return 0;

  return (uint32_t)temp;
}

/**
 * Gets a single relative humidity reading from the sensor.
 *
 * @return A float value representing relative humidity.
 */
uint32_t SHT31_readHumidity(void) {
  if (!SHT31_readTempHum())
    return 0;
	
	SHT31_writeCommand(SHT31_MEAS_HIGHREP);

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
  uint8_t readbuffer[6];
	int32_t stemp, shum;

  SHT31_writeCommand(SHT31_MEAS_HIGHREP);

  //delay(20);

	//TODO: Translate this
	//  i2c_dev->read(readbuffer, sizeof(readbuffer));

  if (readbuffer[2] != crc8(readbuffer, 2) ||
      readbuffer[5] != crc8(readbuffer + 3, 2))
    return false;

  stemp = (int32_t)(((uint32_t)readbuffer[0] << 8) | readbuffer[1]);
  // simplified (65536 instead of 65535) integer version of:
  // temp = (stemp * 175.0f) / 65535.0f - 45.0f;
  stemp = ((4375 * stemp) >> 14) - 4500;
  temp = (float)stemp / 100.0f;

  shum = ((uint32_t)readbuffer[3] << 8) | readbuffer[4];
  // simplified (65536 instead of 65535) integer version of:
  // humidity = (shum * 100.0f) / 65535.0f;
  shum = (625 * shum) >> 12;
  humidity = (float)shum / 100.0f;

  return true;
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

  // Tell the master module what address it will place on the bus when
	// communicating with the slave.
	I2CMasterSlaveAddrSet(I2C0_BASE, SHT31_DEFAULT_ADDR, false);

	//Initiate send of data from the MCU
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	// Wait until MCU is done transferring.
	while(I2CMasterBusy(I2C0_BASE)) {}

	// VEML send
	I2CMasterDataPut(I2C0_BASE, cmd[0]);
	
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
	
	//wait for MCU to finish transaction
	while(I2CMasterBusy(I2C0_BASE));
	 
	//return data pulled from the specified register
	I2CMasterDataPut(I2C0_BASE, cmd[1]);
	
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
		
	// Wait until MCU is done transferring.
  while(I2CMasterBusy(I2C0_BASE));
		
	return 0;
}
