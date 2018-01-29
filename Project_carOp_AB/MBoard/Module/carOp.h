#ifndef TEST_H
#define TEST_H

#define osObjectsPublic                     // define objects in main module

#include "stm32f10x.h"
#include "osObjects.h"                      // RTOS object definitions
#include "Driver_USART.h"
#include "IO_Map.h"
#include "string.h"

#define M1 	1
#define M2 	2
#define M3 	3
#define M4 	4
#define M5 	5

#define SPI_M1 0x01
#define SPI_M2 0x02
#define SPI_M3 0x04
#define SPI_M4 0x08
#define SPI_M5 0x10

//HC595¶Ë¿Ú¶¨Òå
#define HC595M1_LCLK PEout(14)	// LCLK
#define HC595M1_SCLK PEout(13)	// SCLK
#define HC595M1_EN   PEout(12)	// EN
#define HC595M1_A    PEout(11)	// A

#define HC595M2_LCLK PEout(5)	// LCLK
#define HC595M2_SCLK PEout(4)	// SCLK
#define HC595M2_EN   PEout(3)	// EN
#define HC595M2_A    PEout(2)	// A

#define HC595M3_LCLK PBout(12)	// LCLK
#define HC595M3_SCLK PBout(13)	// SCLK
#define HC595M3_EN   PBout(14)	// EN
#define HC595M3_A    PBout(15)	// A

#define HC595M4_LCLK PCout(6)	// LCLK
#define HC595M4_SCLK PCout(7)	// SCLK
#define HC595M4_EN   PCout(8)	// EN
#define HC595M4_A    PCout(9)	// A

#define HC595M5_LCLK PBout(5)	// LCLK
#define HC595M5_SCLK PBout(6)	// SCLK
#define HC595M5_EN   PBout(7)	// EN
#define HC595M5_A    PBout(8)	// A

#define STATUS_NULL		0x00
#define STATUS_START	0x01
#define STATUS_SPLOW	0x02
#define STATUS_SPNML	0x03
#define STATUS_SPFUL	0x04
#define STATUS_SPDOW	0x05
#define STATUS_STOP		0x06

#define STATUS_SWD		0x01
#define STATUS_SWN		0x02
#define STATUS_SWR		0x03
#define STATUS_SWSTP	0x04

#define KEY_START	GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6)
#define KEY_SPLOW	GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)
#define KEY_SPNML	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)
#define KEY_SPFUL	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6)
#define KEY_SPDOW	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)
#define KEY_STOP	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)

#define SW_D		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)
#define SW_N		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)
#define SW_R		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)
#define SW_STP		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)

#define SPEED_LOW	200
#define SPEED_LOWR	280
#define SPEED_NML	360
#define SPEED_HIGH	440

#define SPEED_LIMIT	500
#define SPEED_INIT	300

void PedalmtInit(void);
void carOpInit(uint8_t SPIch);
void opKeyInit(void);
void CAR_Operation(void);
void frameClr(uint8_t Mch,uint8_t length);

void carOperationG_thread(const void *argument);
void carOperationY_thread(const void *argument);
void carOperationR_thread(const void *argument);
void PedalxSWMonitor_thread(const void *argument);
void Task_KEYMT(void const *argument);

#endif
