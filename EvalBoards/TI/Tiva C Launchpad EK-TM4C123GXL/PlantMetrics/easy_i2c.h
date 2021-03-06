#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "../bsp/driverlib/i2c.h"
#include "../bsp/driverlib/sysctl.h"
#include "../bsp/driverlib/gpio.h"
#include "../bsp/driverlib/pin_map.h"

void InitI2C0(void);
void I2CSend(uint8_t slave_addr, uint8_t num_of_args, ...);
void I2CReceive(uint32_t slave_addr, uint8_t reg, uint8_t* recv, uint8_t recv_count);
