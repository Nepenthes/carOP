#ifndef WIRELESSTRABS_USART_H
#define WIRELESSTRABS_USART_H

#define osObjectsPublic                     // define objects in main module

#include "stm32f10x.h"
#include "osObjects.h"                      // RTOS object definitions
#include <WirelessTrans_USART.h>
#include "Driver_USART.h"
#include "string.h"
#include "carOp.h"

#define BEEP PEout(6)

void myUSART1_callback(uint32_t event);
void myUSART4_callback(uint32_t event);
void USARTInit1(void);
void USART1Init2(void);
void USART4Init2(void);
void USARTTest_Thread(const void *argument);
void USARTTest(void);
void BEEP_Init(void);

#endif
