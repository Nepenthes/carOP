#include <WirelessTrans_USART.h>
#include <stdio.h>

const char *TestCMD[] = {

	"天王盖地虎"
};

const char *TestREP[] = {

	"小鸡炖蘑菇\r\n"
};

const char FRAME_HEAD[9] = {0x7E,0x10,0x00,0x00,0x01,0x00,0x00,0x00,0x02};

const char Engine_Frame[12] = {0x00,0x00,0x03,0x30,0x55,0x55,0x05,0x99,0x50,0x05,0x00,0x00};

const char CMD_BLE[][13] = {

	{0x7E,0x10,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x01,0x00,0x31,0x02}, //系统启动
	{0x7E,0x10,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x30,0x92}, //系统停止
	{0x7E,0x10,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x01,0x01,0xF0,0xC2}, //启动工况
	{0x7E,0x10,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x01,0x02,0xB0,0xC3}, //低速行驶
	{0x7E,0x10,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x01,0x03,0x71,0x03}, //正常行驶(串联)
	{0x7E,0x10,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x01,0x04,0x30,0xC1}, //正常行驶（并联）
	{0x7E,0x10,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x01,0x05,0xF1,0x01}, //剩余容量充电
	{0x7E,0x10,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x01,0x06,0xB1,0x00}, //高速行驶
	{0x7E,0x10,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x01,0x07,0x70,0xC0}, //能量回收
	{0x7E,0x10,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x01,0x08,0x30,0xC4}, //停驶工况
	{0x7E,0x10,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x01,0x09,0xF1,0x04}, //停车充电
	{0x7E,0x10,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x01,0x0A,0xB1,0x05}, //外插充电
};

extern ARM_DRIVER_USART Driver_USART1;
extern ARM_DRIVER_USART Driver_USART4;

osThreadId tid_USARTTest_Thread;

osThreadDef(USARTTest_Thread,osPriorityNormal,1,1024);

osMutexDef (uart_mutex);    // Declare mutex

static uint8_t OP_STATUS = 0;

void BEEP_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
 
	GPIO_ResetBits(GPIOE,GPIO_Pin_6);
}


void USARTInit1(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	  
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure); 
	USART_Cmd(USART1, ENABLE);
}

void USART1Init2(void){
	
	/*Initialize the USART driver */
	Driver_USART1.Initialize(myUSART1_callback);
	/*Power up the USART peripheral */
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 4800 Bits/sec */
	Driver_USART1.Control(ARM_USART_MODE_ASYNCHRONOUS |
									ARM_USART_DATA_BITS_8 |
									ARM_USART_PARITY_NONE |
									ARM_USART_STOP_BITS_1 |
							ARM_USART_FLOW_CONTROL_NONE, 115200);

	/* Enable Receiver and Transmitter lines */
	Driver_USART1.Control (ARM_USART_CONTROL_TX, 1);
	Driver_USART1.Control (ARM_USART_CONTROL_RX, 1);

	Driver_USART1.Send("Press Enter to receive a message\n", 33);
	//Driver_USART1.Send((char*)(TestREP[0]),strlen((char*)(TestREP[0])));
}

void USART4Init2(void){
	
	/*Initialize the USART driver */
	Driver_USART4.Initialize(myUSART4_callback);
	/*Power up the USART peripheral */
	Driver_USART4.PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 4800 Bits/sec */
	Driver_USART4.Control(ARM_USART_MODE_ASYNCHRONOUS |
									ARM_USART_DATA_BITS_8 |
									ARM_USART_PARITY_NONE |
									ARM_USART_STOP_BITS_1 |
							ARM_USART_FLOW_CONTROL_NONE, 115200);

	/* Enable Receiver and Transmitter lines */
	Driver_USART4.Control (ARM_USART_CONTROL_TX, 1);
	Driver_USART4.Control (ARM_USART_CONTROL_RX, 1);
}

void myUSART1_callback(uint32_t event)
{

}

void myUSART4_callback(uint32_t event)
{
	
}

void USARTTest_Thread(const void *argument){

	char cmd[30] = "abc";
	char buf_rx[5];
	char buf_rx_hsy[5];
	int  pt;
	uint8_t loop;
	
	for(;;){

		osDelay(20);						
		Driver_USART4.Receive(cmd,20);
		
		pt = memcmp(cmd,FRAME_HEAD,9);
		
		if(!pt){		
			
			memcpy(&buf_rx[0],&cmd[9],5);

			if(memcmp(buf_rx_hsy,buf_rx,4)){

				USART_SendData(USART1, OP_STATUS);
				BEEP = 1;
				osDelay(80);
				BEEP = 0;
				memcpy(buf_rx_hsy,buf_rx,5);
				memset(cmd,0,30*sizeof(char));
				
				for(loop = 0;loop < 12;loop ++){
				
					if(!memcmp(buf_rx,(const char*)&CMD_BLE[loop][9],4)){
					
						OP_STATUS = loop + 'A';
						frameSend((uint8_t *)&Engine_Frame[loop],1,M1);
					}
				}
			}
		}
	}
}

void USARTTest(void){
	
	tid_USARTTest_Thread = osThreadCreate(osThread(USARTTest_Thread),NULL);
}
