/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                           APPLICATION CODE
*
*                                      Texas Instruments TM4C129x
*                                                on the
*                                             DK-TM4C129X
*                                           Development Kit
*       		Modified by Dr. Samir A. Rawashdeh, for the TM4C123GH6PM microcontroller on the 
*						TM4C123G Tiva C Series Launchpad (TM4C123GXL), November 2014.
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : FF
*********************************************************************************************************
* Note(s)       : None.
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  "app_cfg.h"
#include  <cpu_core.h>
#include  <os.h>

#include  "..\bsp\bsp.h"
#include  "..\bsp\bsp_led.h"
#include  "..\bsp\bsp_sys.h"

// SAR Addition
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "esp8266.h"
#include "MQTTPacket.h"
#include "Ports.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_STK       AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK       Task1Stk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK       Task2Stk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK       Task3Stk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK       Task4Stk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK       Task5Stk[APP_CFG_TASK_START_STK_SIZE];

OS_EVENT* th_sem;
OS_EVENT* lum_sem;
OS_EVENT* temp_sem;
OS_EVENT* moist_sem;
OS_EVENT* print_sem; // Keep the prints in the for the sake of easy debugging


/*
*********************************************************************************************************
*                                            LOCAL MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskCreate         (void);
static  void  AppTaskStart          (void       *p_arg);
static  void  Task1          (void       *p_arg);
static  void  Task2          (void       *p_arg);
static  void  Task3          (void       *p_arg);
static  void  Task4          (void       *p_arg);
static  void  Task5          (void       *p_arg);

/*$PAGE*/
/*
*********************************************************************************************************
*                                               main()
*
* Description : Entry point for C code.
*
* Arguments   : none.
*
* Returns     : none.
*
* Note(s)     : (1) It is assumed that your code will call main() once you have performed all necessary
*                   initialization.
*********************************************************************************************************
*/

CPU_INT08U  err;

int  main (void)
{
	INT16U opts;
	


	#if (CPU_CFG_NAME_EN == DEF_ENABLED)
			CPU_ERR     cpu_err;
	#endif

	#if (CPU_CFG_NAME_EN == DEF_ENABLED)
			CPU_NameSet((CPU_CHAR *)"TM4C129XNCZAD",
									(CPU_ERR  *)&cpu_err);
	#endif
			th_sem = OSSemCreate(1);
			temp_sem = OSSemCreate(1);
			moist_sem = OSSemCreate(1);
			lum_sem = OSSemCreate(1);

			Ports_Init(); //Init ADC port
			
			CPU_IntDis();                                               /* Disable all interrupts.                              */

			OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel"          */
	
			opts = (OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR );

			OSTaskCreateExt((void (*)(void *)) AppTaskStart,           /* Create the start task                                */
											(void           *) 0,
											(OS_STK         *)&AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
											(INT8U           ) APP_CFG_TASK_START_PRIO,
											(INT16U          ) APP_CFG_TASK_START_PRIO,
											(OS_STK         *)&AppTaskStartStk[0],
											(INT32U          ) APP_CFG_TASK_START_STK_SIZE,
											(void           *) 0,
											(INT16U          )opts);

			OSTaskNameSet(APP_CFG_TASK_START_PRIO, "Start", &err);

			OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)   */

		
    while (1) {
			;
    }
}



/*$PAGE*/
/*
*********************************************************************************************************
*                                           App_TaskStart()
*
* Description : Startup task example code.
*
* Arguments   : p_arg       Argument passed by 'OSTaskCreate()'.
*
* Returns     : none.
*
* Created by  : main().
*
* Notes       : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
		CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    (void)p_arg;                                                /* See Note #1                                              */


   (void)&p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                             */

    cpu_clk_freq = BSP_SysClkFreqGet();                         /* Determine SysTick reference freq.                    */
    cnts         = cpu_clk_freq                                 /* Determine nbr SysTick increments                     */
                 / (CPU_INT32U)OS_TICKS_PER_SEC;

    OS_CPU_SysTickInit(cnts);
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity                                   */
#endif

    Mem_Init();

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif


		BSP_LED_Toggle(0);
		OSTimeDlyHMSM(0, 0, 0, 200);
		BSP_LED_Toggle(0);
		BSP_LED_Toggle(1);
		OSTimeDlyHMSM(0, 0, 0, 200);
		BSP_LED_Toggle(1);
		BSP_LED_Toggle(2);
		OSTimeDlyHMSM(0, 0, 0, 200);    
		BSP_LED_Toggle(2);

		OSTimeDlyHMSM(0, 0, 1, 0);   

		AppTaskCreate();                                            /* Creates all the necessary application tasks.         */

    while (DEF_ON) {

        OSTimeDlyHMSM(0, 0, 0, 200);			

    }
}


/*
*********************************************************************************************************
*                                         AppTaskCreate()
*
* Description :  Create the application tasks.
*
* Argument(s) :  none.
*
* Return(s)   :  none.
*
* Caller(s)   :  AppTaskStart()
*
* Note(s)     :  none.
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
OSTaskCreate((void (*)(void *)) Task1,           /* Create the second task                                */
                    (void           *) 0,							// argument
                    (OS_STK         *)&Task1Stk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) 5 );  						// Task Priority
                

OSTaskCreate((void (*)(void *)) Task2,           /* Create the second task                                */
                    (void           *) 0,							// argument
                    (OS_STK         *)&Task2Stk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) 6 );  						// Task Priority
										
OSTaskCreate((void (*)(void *)) Task3,           /* Create the third task                                */
                    (void           *) 0,							// argument
                    (OS_STK         *)&Task3Stk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) 7 );  						// Task Priority
										
OSTaskCreate((void (*)(void *)) Task4,           /* Create the four task                                */
                    (void           *) 0,							// argument
                    (OS_STK         *)&Task4Stk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) 8 );  						// Task Priority

/*OSTaskCreate((void (*)(void *)) Task5,           
                    (void           *) 0,							// argument
                    (OS_STK         *)&Task5Stk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) 9 );  						// Task Priority*/

         										
}

volatile uint8_t soil_temp[1];
volatile uint8_t lumens[2];
volatile uint8_t soil_moist[4];
volatile uint8_t env_th[4];

void PublishMQTT(char* topic,  uint8_t payload_opt, uint32_t payload_size) {
	// *** Initialization of the MQTT Packet ***
	static char *host = "192.168.1.27"; // local IP of the MQTT server (could also go through public IP if we want to better scale (TODO: setup port forwarded MQTT interface)
	static int port = 1883; // the SSL register MQTT port
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  MQTTString topicString = MQTTString_initializer; 
	char buf[200];
	int buflen = sizeof(buf);
	int len = 0;
	
	volatile uint8_t * payload;
	switch(payload_opt) {
		case 1:
			payload = soil_temp;
			break;
		case 2:
			payload = lumens;
			break;
		case 3:
			payload = soil_moist;
			break;
		case 4:
			payload = env_th;
			break;
	}

	UARTprintf("here");


	// *** Setup socket ***
	OSSemPend(print_sem, 0, &err);
	ESP8266_Init(115200); // connect to access point, set up as client
	UARTprintf("here");

  // ESP8266_GetVersionNumber();
	// ESP8266_GetStatus();
	if(ESP8266_MakeTCPConnection(host, port)) {
		// *** create MQTT packet ***
		UARTprintf("here");

	  data.clientID.cstring = "pm1";
		data.keepAliveInterval = 20;
		data.cleansession = 1;
		data.username.cstring = "mqttpub1";
		data.password.cstring = "mqttpub1";
		data.MQTTVersion = 4;
		topicString.cstring = topic;
		// add packet start
		len = MQTTSerialize_connect((unsigned char *)buf, buflen, &data);
		// add topic and payload to message
		len += MQTTSerialize_publish((unsigned char *)(buf + len), buflen - len, 0,0,0,0, topicString, (unsigned char *)payload, payload_size);
		len += MQTTSerialize_disconnect((unsigned char *)(buf + len), buflen - len);
		ESP8266_SendTCP(buf, len);
	// ******
	} else {
		UARTprintf("MQTT Server connection unsuccessful");
		return;
	}
	ESP8266_CloseTCPConnection();
	OSSemPost(print_sem);
}

//Thermistor Sensor (ADC)- Temperature of Soil
static  void  Task1 (void *p_arg)
{
   (void)p_arg;
	
    while (1) {          
			
			OSSemPend(temp_sem, 0, &err);
			soil_temp[0] = Get_Temp();
			OSSemPost(temp_sem);

			OSSemPend(print_sem, 0, &err);
			UARTprintf("T1: Soil Temp = %i\n", soil_temp);
			OSSemPost(print_sem);

      OSTimeDlyHMSM(0, 0, 2, 0);
		}
}

//Soil Moisture Sensor (I2C) - STEMMA
static  void  Task2 (void *p_arg)
{
	  uint32_t moist;
   (void)p_arg;
    while (1) {              
			BSP_LED_Toggle(1);
			OSSemPend(moist_sem, 0, &err);
			moist = Get_SoilMoisture();
			soil_moist[0] = moist & 0xff;
			soil_moist[1] = (moist >> 8) & 0xff;
			soil_moist[2] = (moist >> 16) & 0xff;
			soil_moist[3] = (moist >> 24) & 0xff; 
			OSSemPost(moist_sem);
			OSSemPend(print_sem, 0, &err);
			UARTprintf("T2: Soil Moisture = %i\n", moist);
			OSSemPost(print_sem);
			OSTimeDlyHMSM(0, 0, 2, 0);
		}
}

//Light Sensor (I2C) - VEML7700
static  void  Task3 (void *p_arg)
{		
		uint16_t lums;
		(void)p_arg;
    while (1) {              
			BSP_LED_Toggle(2);
			OSSemPend(lum_sem, 0, &err);
			lums = Get_Brightness();
			lumens[0] = lums & 0xff;
			lumens[1] = (lums >> 8) & 0xff;
			OSSemPost(lum_sem);
			OSSemPend(print_sem, 0, &err);
			UARTprintf("T3: rawALS = %x\n", Get_Brightness());
			OSSemPost(print_sem);
			OSTimeDlyHMSM(0, 0, 2, 0);
		}
}

//Enviroment Sensor (I2C) - Humidity & Outside Temperature - Sensirion SHT31-D
static  void  Task4 (void *p_arg)
{
		uint16_t temp;
		uint16_t hum;
   (void)p_arg;
    while (1) {
			OSTimeDlyHMSM(0, 0, 2, 0);
			BSP_LED_Toggle(0);
			// UARTprintf("T4: Stat = %x\n", Get_EnviromentInfo('T'));
			Get_EnviromentInfo(&hum, &temp);
			OSSemPend(th_sem, 0, &err);
			env_th[0] = hum & 0xff;
			env_th[1] = (hum >> 8) & 0xff;
			env_th[2] = temp & 0xff;
			env_th[3] = (temp >> 8) & 0xff;
			OSSemPost(th_sem);
			OSSemPend(print_sem, 0, &err);
			UARTprintf("T4: Humidity = %x\n    Temp = %x\n", hum, temp);
			OSSemPost(print_sem);
		}
}
static  void  Task5 (void *p_arg) {
	(void)p_arg;
	while(1){
		UARTprintf("here");
		OSSemPend(moist_sem, 0, &err);
		UARTprintf("here");
		PublishMQTT("plant/soil_moisture",3,4);
		OSSemPost(moist_sem);
		OSSemPend(th_sem, 0, &err);
		PublishMQTT("plant/th_env",4,4);
		OSSemPost(th_sem);
		OSSemPend(lum_sem, 0, &err);
		PublishMQTT("plant/lum",2,2);
		OSSemPost(lum_sem);
		OSSemPend(temp_sem, 0, &err);
		PublishMQTT("plant/soil_temp",1,1);
		OSSemPost(temp_sem);
	}
}