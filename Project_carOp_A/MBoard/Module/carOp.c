#include "carOp.h"

uint8_t OP_KEYVAL;
uint8_t OP_SWVAL;

uint16_t CAR_OPSPEED_MAX  = SPEED_LIMIT;
uint16_t CAR_OPSPEED_BASE = SPEED_LOW;
uint16_t CAR_OPSPEED	  = SPEED_INIT;

osThreadId tid_carOpG,tid_carOpR,tid_PedalMTxSW; 

osThreadDef(carOperationG_thread,osPriorityNormal,1,1024);
osThreadDef(carOperationR_thread,osPriorityNormal,1,1024);

osThreadDef(PedalxSWMonitor_thread,osPriorityNormal,1,1024);

osTimerDef(Tim_KEYMT,Task_KEYMT);

void carOpInit(uint8_t SPIch){

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE,ENABLE);		
//----------------------------------流水灯595初始化----------------------------------------------------//

if(SPIch & 0x01){

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			
	GPIO_Init(GPIOE, &GPIO_InitStructure);							
}	

if(SPIch & 0x02){

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			
	GPIO_Init(GPIOE, &GPIO_InitStructure);							
}	

if(SPIch & 0x04){

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			
	GPIO_Init(GPIOB, &GPIO_InitStructure);							
}	

if(SPIch & 0x08){

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			
	GPIO_Init(GPIOC, &GPIO_InitStructure);							
}	

if(SPIch & 0x10){

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			
	GPIO_Init(GPIOB, &GPIO_InitStructure);							
}		
	PedalmtInit();
	opKeyInit();
}

void opKeyInit(void){

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);
	
//----------------------------------按键及档位初始化（高电平触发）------------------------------------------//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;				
	GPIO_Init(GPIOC, &GPIO_InitStructure);						

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;				
	GPIO_Init(GPIOB, &GPIO_InitStructure);							
}

void PedalmtInit(void){

	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE );	
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
                    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	ADC_DeInit(ADC1); 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
	ADC_InitStructure.ADC_NbrOfChannel = 1;	
	ADC_Init(ADC1, &ADC_InitStructure);	

	ADC_Cmd(ADC1, ENABLE);	
	
	ADC_ResetCalibration(ADC1);
	 
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	ADC_StartCalibration(ADC1);	
 
	while(ADC_GetCalibrationStatus(ADC1));
}

uint16_t GetPedalVal(uint8_t ch){

	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);	    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));

	return ADC_GetConversionValue(ADC1);
}

/*
 * main: initialize and start the system
 */

void DatsIn595(uint8_t Dats,uint8_t Mch){		
   
   uint8_t loop;
   
   for(loop = 0;loop < 8;loop ++){

		switch(Mch){
		
			case 1:     HC595M1_SCLK 	= 0;
						HC595M1_A		= Dats & 0x01;
						HC595M1_SCLK	= 1;
						Dats	  		= Dats>>1;
						break;
			
			case 2:     HC595M2_SCLK 	= 0;
						HC595M2_A		= Dats & 0x01;
						HC595M2_SCLK	= 1;
						Dats	  		= Dats>>1;
						break;

			case 3:     HC595M3_SCLK 	= 0;
						HC595M3_A		= Dats & 0x01;
						HC595M3_SCLK	= 1;
						Dats	  		= Dats>>1; 
						break;

			case 4:     HC595M4_SCLK 	= 0;
						HC595M4_A		= Dats & 0x01;
						HC595M4_SCLK	= 1;
						Dats	  		= Dats>>1;
						break;

			case 5:     HC595M5_SCLK 	= 0;
						HC595M5_A		= Dats & 0x01;
						HC595M5_SCLK	= 1;
						Dats	  		= Dats>>1; 
						break;
							
			default:	break;
		}
   }
}

void frameSend(uint8_t Frame[],uint8_t length,uint8_t Mch){

	uint8_t loop;
	
	switch(Mch){
	
		case M1:	HC595M1_EN = 0;
					HC595M1_LCLK = 0;
					for(loop = 0; loop < length; loop++)DatsIn595(Frame[loop],Mch);	
					HC595M1_LCLK = 1;
					HC595M1_LCLK = 0;
					break;
		
		case M2:	HC595M2_EN = 0;
					HC595M2_LCLK = 0;
					for(loop = 0; loop < length; loop++)DatsIn595(Frame[loop],Mch);	
					HC595M2_LCLK = 1;
					HC595M2_LCLK = 0;
					break;
		
		case M3:	HC595M3_EN = 0;
					HC595M3_LCLK = 0;
					for(loop = 0; loop < length; loop++)DatsIn595(Frame[loop],Mch);	
					HC595M3_LCLK = 1;
					HC595M3_LCLK = 0;
					break;
						
		case M4:	HC595M4_EN = 0;
					HC595M4_LCLK = 0;
					for(loop = 0; loop < length; loop++)DatsIn595(Frame[loop],Mch);	
					HC595M4_LCLK = 1;
					HC595M4_LCLK = 0;
					break;
						
		case M5:	HC595M5_EN = 0;
					HC595M5_LCLK = 0;
					for(loop = 0; loop < length; loop++)DatsIn595(Frame[loop],Mch);	
					HC595M5_LCLK = 1;
					HC595M5_LCLK = 0;
					break;
						
		default:	break;
	}
}

void frameClr(uint8_t Mch,uint8_t length){
	
	uint8_t loop;
	
	switch(Mch){
	
		case M1:HC595M1_EN = 0;
				HC595M1_LCLK = 0;
				for(loop = 0; loop < length; loop++)DatsIn595(0,Mch);	
				HC595M1_LCLK = 1;
				HC595M1_LCLK = 0;
				break;
		
		case M2:HC595M2_EN = 0;
				HC595M2_LCLK = 0;
				for(loop = 0; loop < length; loop++)DatsIn595(0,Mch);	
				HC595M2_LCLK = 1;
				HC595M2_LCLK = 0;
				break;
		
		case M3:HC595M3_EN = 0;
				HC595M3_LCLK = 0;
				for(loop = 0; loop < length; loop++)DatsIn595(0,Mch);	
				HC595M3_LCLK = 1;
				HC595M3_LCLK = 0;
				break;
						
		case M4:HC595M4_EN = 0;
				HC595M4_LCLK = 0;
				for(loop = 0; loop < length; loop++)DatsIn595(0,Mch);	
				HC595M4_LCLK = 1;
				HC595M4_LCLK = 0;
				break;
				
		case M5:HC595M5_EN = 0;
				HC595M5_LCLK = 0;
				for(loop = 0; loop < length; loop++)DatsIn595(0,Mch);	
				HC595M5_LCLK = 1;
				HC595M5_LCLK = 0;
				break;
				
		default:break;
	}	
}

void frame_Send(uint8_t Frame[],uint8_t length,uint8_t Mch){
	
	frameSend(Frame,length,Mch);
	osDelay(CAR_OPSPEED_MAX - CAR_OPSPEED);
}

void OPR_324x67(void){

	const uint8_t FLEN = 7;			//查找表表长，即使用了几个灯条
	const uint8_t FORM_LEN = 3;		//变化个数
	const uint8_t FORM[FORM_LEN] = {2,1,3};	//变化条目索引
	uint8_t loop,loopa;
	uint8_t FrameR[FLEN] = {0};	//查找表
	
	for(loop = 0;loop < FORM_LEN;loop++){
	
		for(loopa = 0;loopa < 8;loopa++){
		
			FrameR[FORM[loop]] |= (0x80 >> loopa);
			frame_Send((uint8_t *)FrameR,FLEN,M1);
		}
	}	
	
	for(loopa = 0;loopa < 3;loopa++){
	
		FrameR[6] |= (0x80 >> loopa);
		frame_Send((uint8_t *)FrameR,FLEN,M1);
	}
	
	for(loopa = 0;loopa < 5;loopa++){
	
		FrameR[6] |= (0x80 >> (loopa + 3));
		FrameR[5] |= (0x80 >> loopa);
		frame_Send((uint8_t *)FrameR,FLEN,M1);
	}
	
	for(loopa = 0;loopa < 3;loopa++){
	
		FrameR[5] |= (0x80 >> (loopa + 5));
		frame_Send((uint8_t *)FrameR,FLEN,M1);
	}
	
	memset(FrameR,0,sizeof(uint8_t) * FLEN);	
}

void OPR_3245x67(void){

	const uint8_t FLEN = 7;			//查找表表长，即使用了几个灯条
	const uint8_t FORM_LEN = 2;		//变化个数
	const uint8_t FORM[FORM_LEN] = {2,1};	//变化条目索引
	uint8_t loop,loopa;
	uint8_t FrameR[FLEN] = {0};	//查找表
	
	for(loop = 0;loop < FORM_LEN;loop++){
	
		for(loopa = 0;loopa < 8;loopa++){
		
			FrameR[FORM[loop]] |= (0x80 >> loopa);
			frame_Send((uint8_t *)FrameR,FLEN,M1);
		}
	}	
	
	for(loopa = 0;loopa < 8;loopa++){
	
		FrameR[3] |= (0x80 >> loopa);
		FrameR[4] |= (0x80 >> loopa);
		frame_Send((uint8_t *)FrameR,FLEN,M1);
	}
	
	for(loopa = 0;loopa < 3;loopa++){
	
		FrameR[6] |= (0x80 >> loopa);
		frame_Send((uint8_t *)FrameR,FLEN,M1);
	}
	
	for(loopa = 0;loopa < 5;loopa++){
	
		FrameR[6] |= (0x80 >> (loopa + 3));
		FrameR[5] |= (0x80 >> loopa);
		frame_Send((uint8_t *)FrameR,FLEN,M1);
	}
	
	for(loopa = 0;loopa < 3;loopa++){
	
		FrameR[5] |= (0x80 >> (loopa + 5));
		frame_Send((uint8_t *)FrameR,FLEN,M1);
	}
	
	memset(FrameR,0,sizeof(uint8_t) * FLEN);	
}

void OPR_1(void){

	const uint8_t FLEN = 7;
	uint8_t loopa;
	uint8_t FrameR[FLEN] = {0};	//查找表
	
	for(loopa = 0;loopa < 8;loopa++){
	
		FrameR[0] |= (0x80 >> loopa);
		frame_Send((uint8_t *)FrameR,FLEN,M1);
	}		
}

void OPG_67x324(void){

	const uint8_t FLEN = 5;			//查找表表长，即使用了几个灯条
	const uint8_t FORM_LEN = 3;		//变化个数
	const uint8_t FORM[FORM_LEN] = {2,1,0};	//变化条目索引
	uint8_t loop,loopa;
	uint8_t FrameG[FLEN] = {0};	//查找表	
	
	for(loopa = 0;loopa < 3;loopa++){
	
		FrameG[3] |= (0x80 >> loopa);
		frame_Send((uint8_t *)FrameG,FLEN,M2);
	}
	
	for(loopa = 0;loopa < 5;loopa++){
	
		FrameG[3] |= (0x80 >> (loopa + 3));
		FrameG[4] |= (0x80 >> loopa);
		frame_Send((uint8_t *)FrameG,FLEN,M2);
	}
	
	for(loopa = 0;loopa < 3;loopa++){
	
		FrameG[4] |= (0x80 >> (loopa + 5));
		frame_Send((uint8_t *)FrameG,FLEN,M2);
	}
	
	for(loop = 0;loop < FORM_LEN;loop++){
	
		for(loopa = 0;loopa < 8;loopa++){
		
			FrameG[FORM[loop]] |= (0x80 >> loopa);
			frame_Send((uint8_t *)FrameG,FLEN,M2);
		}
	}
	
	memset(FrameG,0,sizeof(uint8_t) * FLEN);	
}



void swGet(void){

	if(SW_D){
		
		OP_SWVAL = STATUS_SWD;
	}else
	if(SW_N){
		
		OP_SWVAL = STATUS_SWN;
	}else
	if(SW_R){
		
		OP_SWVAL = STATUS_SWR;
	}
}

void keyGet(void){

	if(KEY_START){
	
		OP_KEYVAL = STATUS_START;
	}else 
	if(KEY_SPLOW){
	
		OP_KEYVAL = STATUS_SPLOW;
	}else 
	if(KEY_SPNML){
	
		OP_KEYVAL = STATUS_SPNML;
	}else 
	if(KEY_SPFUL){
	
		OP_KEYVAL = STATUS_SPFUL;
	}else 
	if(KEY_SPDOW){
	
		OP_KEYVAL = STATUS_SPDOW;
	}else 
	if(KEY_STOP){
	
		OP_KEYVAL = STATUS_STOP;
	}else	OP_KEYVAL = STATUS_NULL;
}

void Task_KEYMT(void const *argument){

	static uint8_t OPKEY = STATUS_NULL;
	
	keyGet();
	
	if(!SW_STP){
	
		osThreadTerminate(tid_carOpG);
		osThreadTerminate(tid_carOpR);
		
		frameClr(M2,5);
		frameClr(M1,7);		
	}else	
	if(OPKEY != OP_KEYVAL){
	
		OPKEY = OP_KEYVAL;
		
		osThreadTerminate(tid_carOpG);
		osThreadTerminate(tid_carOpR);
		
		frameClr(M2,5);
		frameClr(M1,7);
		
		switch(OPKEY){
			
			case STATUS_START:	tid_carOpR	=	osThreadCreate(osThread(carOperationR_thread), NULL);
								CAR_OPSPEED_BASE = 	SPEED_LOW;
								break;
			
			case STATUS_SPLOW:	tid_carOpR	=	osThreadCreate(osThread(carOperationR_thread), NULL);
								CAR_OPSPEED_BASE = 	SPEED_LOWR;
								break;
			
			case STATUS_SPNML:	tid_carOpR	=	osThreadCreate(osThread(carOperationR_thread), NULL);
								CAR_OPSPEED_BASE = 	SPEED_NML;
								break;
			
			case STATUS_SPFUL:	tid_carOpR	=	osThreadCreate(osThread(carOperationR_thread), NULL);
								CAR_OPSPEED_BASE = 	SPEED_HIGH;
								break;
			
			case STATUS_SPDOW:	tid_carOpG	=	osThreadCreate(osThread(carOperationG_thread), NULL);
								CAR_OPSPEED_BASE = 	SPEED_NML;
								break;
			
			case STATUS_STOP :	tid_carOpR	=	osThreadCreate(osThread(carOperationR_thread), NULL);
								CAR_OPSPEED_BASE = 	SPEED_NML;
								break;
								
			case STATUS_NULL :	break;
			
					  default:	break;
		}
	}
}

void PedalxSWMonitor_thread(const void *argument){
	
	uint16_t PedalVal;

	for(;;){
	
		PedalVal 	= (GetPedalVal(ADC_Channel_10) - 96) / 15;
		CAR_OPSPEED = CAR_OPSPEED_BASE + PedalVal;
		if(CAR_OPSPEED > (CAR_OPSPEED_MAX - 1))CAR_OPSPEED = CAR_OPSPEED_MAX - 1;
		
		if(SW_D){
		
			//档位选项保留
		}
		
		if(SW_R){
		
			//档位选项保留
		}
		
		if(SW_N){
		
			//档位选项保留
		}
	}
}

void orderTest(uint8_t Mch,uint8_t testChar,uint8_t num,uint32_t time){

	uint8_t loop,loopa;
	
	num += 1;
	for(;;){
			
		switch(Mch){
		
			case M1: 	for(loop = 0;loop < num;loop ++){
				
							frameClr(M1,num);

							HC595M1_EN	 = 0;
							HC595M1_LCLK = 0;
							DatsIn595(testChar,M1);	
							for(loopa = 0;loopa < loop;loopa ++)DatsIn595(0,M1);
							HC595M1_LCLK = 1;
							osDelay(time);
						}
						break;
						
			case M2: 	for(loop = 0;loop < num;loop ++){
				
							frameClr(M2,num);

							HC595M2_EN	 = 0;
							HC595M2_LCLK = 0;
							DatsIn595(testChar,M2);	
							for(loopa = 0;loopa < loop;loopa ++)DatsIn595(0,M2);
							HC595M2_LCLK = 1;
							osDelay(time);
						}			
						break;
						
			case M3: 	for(loop = 0;loop < num;loop ++){
				
							frameClr(M3,num);

							HC595M3_EN	 = 0;
							HC595M3_LCLK = 0;
							DatsIn595(testChar,M3);	
							for(loopa = 0;loopa < loop;loopa ++)DatsIn595(0,M3);
							HC595M3_LCLK = 1;
							osDelay(time);
						}
						break;
						
			case M4: 	for(loop = 0;loop < num;loop ++){
				
							frameClr(M4,num);

							HC595M4_EN	 = 0;
							HC595M4_LCLK = 0;
							DatsIn595(testChar,M4);	
							for(loopa = 0;loopa < loop;loopa ++)DatsIn595(0,M4);
							HC595M4_LCLK = 1;
							osDelay(time);
						}
						break;
						
			case M5: 	for(loop = 0;loop < num;loop ++){
				
							frameClr(M5,num);

							HC595M5_EN	 = 0;
							HC595M5_LCLK = 0;
							DatsIn595(testChar,M5);	
							for(loopa = 0;loopa < loop;loopa ++)DatsIn595(0,M5);
							HC595M5_LCLK = 1;
							osDelay(time);
						}
						break;
		}
	}
}

void orderPushTest(uint8_t Mch,uint8_t num,uint32_t time){

	uint8_t loop;
	
	for(;;){
			
		frameClr(Mch,num);

		switch(Mch){
		
			case M1: 	for(loop = 0;loop < num * 8;loop ++){
			
							HC595M1_LCLK 	= 0;
							HC595M1_SCLK 	= 0;
							HC595M1_A		= 1;
							HC595M1_SCLK	= 1;
							HC595M1_LCLK 	= 1;
							osDelay(time);		
						}
						break;
						
			case M2: 	for(loop = 0;loop < num * 8;loop ++){
			
							HC595M2_LCLK 	= 0;
							HC595M2_SCLK 	= 0;
							HC595M2_A		= 1;
							HC595M2_SCLK	= 1;
							HC595M2_LCLK 	= 1;
							osDelay(time);		
						}
						break;
						
			case M3: 	for(loop = 0;loop < num * 8;loop ++){
			
							HC595M3_LCLK 	= 0;
							HC595M3_SCLK 	= 0;
							HC595M3_A		= 1;
							HC595M3_SCLK	= 1;
							HC595M3_LCLK 	= 1;
							osDelay(time);		
						}
						break;
						
			case M4: 	for(loop = 0;loop < num * 8;loop ++){
			
							HC595M4_LCLK 	= 0;
							HC595M4_SCLK 	= 0;
							HC595M4_A		= 1;
							HC595M4_SCLK	= 1;
							HC595M4_LCLK 	= 1;
							osDelay(time);		
						}
						break;
						
			case M5: 	for(loop = 0;loop < num * 8;loop ++){
			
							HC595M5_LCLK 	= 0;
							HC595M5_SCLK 	= 0;
							HC595M5_A		= 1;
							HC595M5_SCLK	= 1;
							HC595M5_LCLK 	= 1;
							osDelay(time);		
						}
						break;
		}
	}
}

void carOperationG_thread(const void *argument){
	

	for(;;){
	
		switch(OP_KEYVAL){
					
			case STATUS_START:	break;
			
			case STATUS_SPLOW:	break;
			
			case STATUS_SPNML:	break;
			
			case STATUS_SPFUL:	break;
			
			case STATUS_SPDOW:	OPG_67x324();
								break;
			
			case STATUS_STOP :	break;
								
			case STATUS_NULL :	break;
			
					  default:	break;
		}
	}
}

void carOperationR_thread(const void *argument){
	
	for(;;){
	
		switch(OP_KEYVAL){
					
			case STATUS_START:	OPR_324x67();
								break;
			
			case STATUS_SPLOW:	OPR_324x67();
								break;
			
			case STATUS_SPNML:	OPR_324x67();
								break;
			
			case STATUS_SPFUL:	OPR_3245x67();
								break;
			
			case STATUS_SPDOW:	break;
			
			case STATUS_STOP :	OPR_1();
								break;
								
			case STATUS_NULL :	break;
			
					  default:	break;
		}
	}
}

void CAR_Operation(void) {

	osTimerId Tim_id_KEYMT;

	Tim_id_KEYMT = osTimerCreate (osTimer(Tim_KEYMT), osTimerPeriodic, &Task_KEYMT);
	osTimerStart (Tim_id_KEYMT, 50);
	
	tid_PedalMTxSW = osThreadCreate(osThread(PedalxSWMonitor_thread), NULL);
}

