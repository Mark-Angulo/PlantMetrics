#include "Ports.h"

//For I2C
#define TIME_PERIOD 40 // TP = 80MHz / (2*(SCL_LP+SCL_HP)*I2C_CLK_Freq) = 80MHz / (20*100000) 
#define VEML7700_SLAVE_ADDR 0x04 //Light
#define STEMMA_SLAVE_ADDR 	0x05 //Soil Moisture
#define SH31D_SLAVE_ADDR 		0x06 //Enviroment

//Bit-Specific Addressing
#define PE2 (*((volatile uint32_t *)0x40024010)) //Thermistor
#define PE3 (*((volatile uint32_t *)0x40024020)) //
#define PE4 (*((volatile uint32_t *)0x40024040)) //
#define PE5 (*((volatile uint32_t *)0x40024080)) //
	
void Init_I2C0(void) {
	SYSCTL_RCGCI2C_R |= 0x0001;								// activate I2C Clk
	
	//Initialize Master
	I2CMasterInitExpClk(I2C0_BASE, SYSCTL_RCGCI2C_R, false);
	
	//Initialize Slaves
	I2CSlaveInit(I2C0_BASE, VEML7700_SLAVE_ADDR);
	I2CSlaveInit(I2C0_BASE, STEMMA_SLAVE_ADDR);
	I2CSlaveInit(I2C0_BASE, SH31D_SLAVE_ADDR);
}
	
//Initialize GPIO ports for ADC and Digital I/O
//  -Port PE2 for ADC input
//  -Ports PB2,PB3 for I2C
void Ports_Init(void) {
	// All the steps in Init are needed
	SYSCTL_RCGCADC_R |= 0x0001;								// activate ADC0
	SYSCTL_RCGCGPIO_R |= 0x10; 								// 1) activate clock for Port E
	while((SYSCTL_RCGCGPIO_R & 0x10)!=0x10){}		//    allow time for clock to stabilize
		
  GPIO_PORTE_DIR_R &= ~0x04;								// 2) make PE2 input	
  GPIO_PORTE_AFSEL_R |= 0x04; 							// 3) enable alternate function on PE2
  GPIO_PORTE_DEN_R &= ~0x04;								// 4) disable digital I/O on PE2
  GPIO_PORTE_AMSEL_R |= 0x04;								// 5a) enable analog function on PE2
	
	ADC0_PC_R &= ~0xF;
  ADC0_PC_R |= 0x05;												// 7) configure for 500k samples
  ADC0_SSPRI_R = 0x0123;										// 8) Sequencer 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;									// 9) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;										// 10) seq3 is software trigger
	ADC0_SSMUX3_R &= ~0x000F;
  ADC0_SSMUX3_R += 1;												// 11) channel PE2(AIN1)
  ADC0_SSCTL3_R = 0x0006;										// 12) no TS0 D0, yes IE0 END0
	ADC0_IM_R &= ~0x0008;
  ADC0_ACTSS_R |= 0x0008;										// 13) enable sample sequencer 3

	//Init I2C
	Init_I2C0();
		
}

// Returns the temperature in Farenheit
uint8_t Get_Temp(void){
	int i;
	int32_t result=0;
	
	// Read raw result from ADC0
	// Try averaging multiple readings together to get more stable outputs
	// Your code should come beneath 
	for (i = 0; i < 50; i++) {
		ADC0_PSSI_R = 0x0008;						// 1) initiate SS3
		while((ADC0_RIS_R&0x08)==0){};	// 2) wait for conversion done
		result += ADC0_SSFIFO3_R&0xFFF;	// 3) read result
		ADC0_ISC_R = 0x0008;						// 4) acknowledge completion
	}
	result /= 50;
	
	result = (3300*result) / 4095;
	
	result = (1475 - ((750*result)/1000))/10;
	
	result = ((result*9)/5) + 32;
	
	// Return temperature
	return result;
}




//Gets the brightness from the VEML7700
uint8_t Get_Brightness(void) {
	uint32_t i2cMsg = 0;

	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
	while (!I2CMasterBusy(I2C0_BASE)) {} //Wait till Master is not busy
	if (I2CMasterErr(I2C0_BASE) == I2C_MASTER_ERR_NONE) {
		//No Communication Errors
		i2cMsg = I2CMasterDataGet(I2C0_BASE);
	}
	
	return i2cMsg;
}
