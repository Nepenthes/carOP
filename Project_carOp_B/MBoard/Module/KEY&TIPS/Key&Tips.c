#include <Key&Tips.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern ARM_DRIVER_USART Driver_USART1;								//�豸�����⴮��һ�豸����

osThreadId tid_keyTest_Thread;										//����������߳�ID
osThreadDef(keyTest_Thread,osPriorityAboveNormal,1,1024);	//����������̶߳���

static	uint8_t 	keyOverFlg 	= 	0;				//�����¼�������־
static	uint16_t	sKeyCount	=	0;				//�����̰�	 ����ֵ
static	uint16_t	sKeyKeep		=	0;				//���������� ����ֵ
static	uint8_t	keyCTflg		=  0;				//����������־

/***���������ʼ��***/
void keyInit(void){	

	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);		//ʱ�ӷ���
	
//----------------------------------������ʼ��----------------------------------------------------//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//??????50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//????
	GPIO_Init(GPIOB, &GPIO_InitStructure);							//?????GPIOx???
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//??????50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//????
	GPIO_Init(GPIOC, &GPIO_InitStructure);							//?????GPIOx???
}

/***������ֵ��ȡ***/
static uint16_t keyScan(void){

	if(!K1)return KEY_VALUE_1;		//��ֵ1
	if(!K2)return KEY_VALUE_2;		//��ֵ2
	if(!K3)return KEY_VALUE_3;		//��ֵ1
	if(!K4)return KEY_VALUE_4;		//��ֵ2
	if(!K5)return KEY_VALUE_5;		//��ֵ1
	if(!K6)return KEY_VALUE_6;		//��ֵ2
	return KEY_NULL;					//�ް���
}

/***�������״̬��***/
uint16_t getKey(void){

	static	uint16_t s_u16KeyState 		= KEY_STATE_INIT;		//״̬�����״̬����ʼ��״̬
	static	uint16_t	s_u16LastKey		= KEY_NULL;				//������ʷ������ֵ	
	static	uint8_t	KeyKeepComfirm		= 0;						//������ȷ�ϱ��� ȷ������ʱ����ʱ
	static	uint16_t	s_u16KeyTimeCount	= 0;						//����ʱ�����壨������KEY_TICK���м����������������ֵ��ȷ���Ƿ����ڳ�����
				uint16_t keyTemp				= KEY_NULL;				//ʮ�����Ƶ�һλ������״̬���ڶ�λ�����ּ���ֵ������λ����ֵ������λ����������ֵ
	
	static	uint32_t osTick_last			= 0xffff0000;			//��osTick���м�¼����������һ��osTick���жԱȻ�ȡ������˼�������жϰ����Ƿ������������£�
	
	keyTemp = keyScan();		//��ȡ��ֵ
	
	switch(s_u16KeyState){	//��ȡ״̬��״̬
	
		case KEY_STATE_INIT:	//��ʼ��״̬
			
				if(keyCTflg){	//�����һ���Ƿ�Ϊ����
				
					if((osKernelSysTick() - osTick_last) > KEY_CONTINUE_PERIOD){	//��һ�����������Ȿ���Ƿ��������
					
						keyTemp	= s_u16LastKey & 0x00f0;	//���β���������������ֵΪ��������״̬��ͬʱ������־λ����
						keyTemp |= KEY_CTOVER;
						keyCTflg = 0;	
					}
				}
		
				if(KEY_NULL != keyTemp)s_u16KeyState = KEY_STATE_WOBBLE;	//��⵽�а������л�״̬���������
				break;
		
		case KEY_STATE_WOBBLE:	//����״̬���
			
				s_u16KeyState = KEY_STATE_PRESS;	//ȷ�ϰ������л�״̬���̰����
				break;
		
		case KEY_STATE_PRESS:	//�̰�״̬���
		
				if(KEY_NULL != keyTemp){		//�����Ƿ���
				
					s_u16LastKey 	= keyTemp;	//�洢������ֵ
					keyTemp 		  |= KEY_DOWN;	//����״̬ȷ��Ϊ����
					s_u16KeyState	= KEY_STATE_LONG;	//������Ȼδ�����л�״̬���������
				}else{
				
					s_u16KeyState	= KEY_STATE_INIT;	//���Ϊ�������������״̬�����س�ʼ��״̬
				}
				break;
				
		case KEY_STATE_LONG:		//����״̬���
				
				if(KEY_NULL != keyTemp){	//�����Ƿ���
					
					if(++s_u16KeyTimeCount > KEY_LONG_PERIOD){	//��������Ȼδ��������ݳ���ʱ�����м�������ȷ���Ƿ�Ϊ����
					
						s_u16KeyTimeCount	= 0;			//ȷ�ϳ�������������ֵ����
						sKeyKeep				= 0;			//���״̬���л�״̬ǰ���Ա���ȷ��״̬�������ֵ��ǰ���㣬������ֵΪ�����󱣳ּ�������
						KeyKeepComfirm		= 0;			//���״̬���л�״̬ǰ���Ա���ȷ��״̬�������ֵ��ǰ���㣬����ֵ���ڶ��峤�����ý��б��ּ���
						keyTemp			  |= KEY_LONG;	//����״̬ȷ��Ϊ����
						s_u16KeyState		= KEY_STATE_KEEP;	//������Ȼδ�����л�״̬���������ּ��
						
						keyOverFlg			= KEY_OVER_LONG;	//����
					}else	keyOverFlg	= KEY_OVER_SHORT;		//�̰�
				}else{
				
					s_u16KeyState		= KEY_STATE_RELEASE;	//���ȷ��Ϊ�����󰴼������л�״̬��������
				}
				break;
				
		case KEY_STATE_KEEP:		//�����󱣳�״̬���
			
				if(KEY_NULL != keyTemp){		//�����Ƿ���
				
					if(++s_u16KeyTimeCount > KEY_KEEP_PERIOD){	//��������Ȼδ��������ݳ���ʱ�����м�������ȷ���Ƿ�Ϊ�������������
						
						s_u16KeyTimeCount	= 0;			//ȷ�ϳ�����������֣���������ֵ����
						if(KeyKeepComfirm < (KEY_COMFIRM + 3))KeyKeepComfirm++;			//����Ƿ��������ʱ��
						if(sKeyKeep < 15 && KeyKeepComfirm > KEY_COMFIRM)sKeyKeep++; 	//��⵽���������ִ�г����󱣳ּ���
						if(sKeyKeep){		//��⵽�����󱣳ּ���ֵ��Ϊ�㣬��ȷ�ϰ���״̬Ϊ������������֣��Է���ֵ������Ӧȷ�ϴ���
						
							keyOverFlg	 = KEY_OVER_KEEP;	//״̬ȷ��Ϊ������Ϊ����
							keyTemp	|= sKeyKeep << 8;		//���ּ�����������8λ����ʮ������keyTemp�ڶ�λ
							keyTemp	|= KEY_KEEP;			//����״̬ȷ��Ϊ�������������
						}		
					}
				}else{
				
					s_u16KeyState	= KEY_STATE_RELEASE;	//����״̬ȷ��Ϊ�������������֮�����л�״̬��������
				}
				break;
				
		case KEY_STATE_RELEASE:	//����״̬���
				
				s_u16LastKey |= KEY_UP;	//�洢����״̬
				keyTemp		  = s_u16LastKey;	//����״̬ȷ��Ϊ����
				s_u16KeyState = KEY_STATE_RECORD;	//�л�״̬������������¼
				break;
		
		case KEY_STATE_RECORD:	//����������¼״̬���

				if((osKernelSysTick() - osTick_last) < KEY_CONTINUE_PERIOD){	//�����ΰ�������ʱ����С�ڹ涨ֵ�����ж�Ϊ����
					
					sKeyCount++;	//��������		
				}else{
					
					sKeyCount = 0;	//�����Ͽ�����������
				}
				
				if(sKeyCount){		//������������Ϊ�㣬��ȷ��Ϊ�����������Է���ֵ������Ӧ����
													
					keyCTflg	= 1;	//��������־
					keyTemp	= s_u16LastKey & 0x00f0;	//��ȡ��ֵ
					keyTemp	|=	KEY_CONTINUE;				//ȷ��Ϊ��������
					if(sKeyCount < 15)keyTemp += sKeyCount;	//�����������ݷ���ʮ������keyTemp����λ�����λ��
				}
				
				s_u16KeyState	= KEY_STATE_INIT;		//���״̬�����س�ʼ״̬
				osTick_last	 	= osKernelSysTick();	//��¼osTick�������´��������Ա�
				break;
		
		default:break;
	}
	return keyTemp;	//���ذ���״̬�ͼ�ֵ
}

/***����������߳�***/
void keyTest_Thread(const void *argument){

/***�������ԣ�����1����������Ϣ��****/
#if(KEY_DEBUG)
	uint16_t keyVal;						//����״̬�¼�
	uint8_t	kCount;						//��������ֵ�������������ּ�������������ʹ��ͬһ����������Ϊ����״̬����ͬʱ����
	static uint8_t	kCount_rec;			//��ʷ����ֵ����
	const	 uint8_t	tipsLen = 30;		//Tips��ӡ�ַ�������
	
	char	tips[tipsLen];					//Tips�ַ���
#endif

#if(KEY_DEBUG)
	for(;;){
		
		keyVal = getKey();
		
		memset(tips,0,tipsLen*sizeof(char));	//ÿ��Tips��ӡ�����
		strcat(tips,"Tips-");						//Tips��ʶ
		
		switch(keyVal & 0xf000){
		
			case KEY_LONG		:	
				
					switch(keyVal & 0x00f0){
						
						case KEY_VALUE_1:	strcat(tips,"����1����\r\n");	
												Driver_USART1.Send(tips,strlen(tips));	
												break;
						
						case KEY_VALUE_2:	strcat(tips,"����2����\r\n");	
												Driver_USART1.Send(tips,strlen(tips));	
												break;
						
									default:	break;
					}
					break;
					
			case KEY_KEEP		:
				
					kCount		= (uint8_t)((keyVal & 0x0f00) >> 8) + '0';  //��ȡ����ֵ��תΪASCII
					kCount_rec	= kCount;
					switch(keyVal & 0x00f0){
						
						case KEY_VALUE_1:	strcat(tips,"����1�������֣����ּ�����");	
												strcat(tips,(const char*)&kCount);	
												strcat(tips,"\r\n");	
												Driver_USART1.Send(tips,strlen(tips));	
												break;
						
						case KEY_VALUE_2:	strcat(tips,"����2�������֣����ּ�����");	
												strcat(tips,(const char*)&(kCount));	
												strcat(tips,"\r\n");	
												Driver_USART1.Send(tips,strlen(tips));	
												break;
						
									default:	break;
					}
					break;
					
			case KEY_DOWN		:
				
					switch(keyVal & 0x00f0){
						
						case KEY_VALUE_1:	
												strcat(tips,"����1����\r\n");	
												Driver_USART1.Send(tips,strlen(tips));	
												break;
						
						case KEY_VALUE_2:	
												strcat(tips,"����2����\r\n");	
												Driver_USART1.Send(tips,strlen(tips));
												break;
						
						case KEY_VALUE_3:	
												strcat(tips,"����3����\r\n");	
												Driver_USART1.Send(tips,strlen(tips));	
												break;
						
						case KEY_VALUE_4:	
												strcat(tips,"����4����\r\n");	
												Driver_USART1.Send(tips,strlen(tips));
												break;
						
						case KEY_VALUE_5:	
												strcat(tips,"����5����\r\n");	
												Driver_USART1.Send(tips,strlen(tips));	
												break;
						
						case KEY_VALUE_6:	
												strcat(tips,"����6����\r\n");	
												Driver_USART1.Send(tips,strlen(tips));
												break;
												
									default:	break;
					}
					break;
					
			case KEY_UP			:
				
					switch(keyVal & 0x00f0){
						
						case KEY_VALUE_1:	
								
								switch(keyOverFlg){
								
								
									case KEY_OVER_SHORT		:	strcat(tips,"����1�̰�����\r\n");	
																		Driver_USART1.Send(tips,strlen(tips));
																		keyOverFlg = 0;
																		break;
									
									case KEY_OVER_LONG		:	strcat(tips,"����1��������\r\n");	
																		Driver_USART1.Send(tips,strlen(tips));
																		keyOverFlg = 0;
																		break;
									
									case KEY_OVER_KEEP		:	strcat(tips,"����1�����󱣳�");
																		strcat(tips,(const char*)&kCount_rec);
																		strcat(tips,"�μ��������\r\n");
																		Driver_USART1.Send(tips,strlen(tips));
																		kCount_rec = 0;
																		break;			
									default:break;
								}
								break;
							
						
						
						case KEY_VALUE_2:	
								
								switch(keyOverFlg){
								
								
									case KEY_OVER_SHORT		:	strcat(tips,"����2�̰�����\r\n");	
																		Driver_USART1.Send(tips,strlen(tips));
																		keyOverFlg = 0;
																		break;
									
									case KEY_OVER_LONG		:	strcat(tips,"����2��������\r\n");	
																		Driver_USART1.Send(tips,strlen(tips));
																		keyOverFlg = 0;
																		break;
									
									case KEY_OVER_KEEP		:	strcat(tips,"����2�����󱣳�");
																		strcat(tips,(const char*)&kCount_rec);
																		strcat(tips,"�μ��������\r\n");
																		Driver_USART1.Send(tips,strlen(tips));
																		kCount_rec = 0;
																		break;	
									default:break;
								}
								break;
								
						default:break;
					}
					break;
					
			case KEY_CONTINUE	:
				
					kCount 		= (uint8_t)((keyVal & 0x000f) >> 0) + '0';	//��ȡ����ֵ��תΪASCII
					kCount_rec	= kCount + 1;
					switch(keyVal & 0x00f0){
						
						case KEY_VALUE_1:	strcat(tips,"����1����������������");	
												strcat(tips,(const char*)&kCount);	
												strcat(tips,"\r\n");	
												Driver_USART1.Send(tips,strlen(tips));		
												break;
						
						case KEY_VALUE_2:	strcat(tips,"����2����������������");	
												strcat(tips,(const char*)&kCount);	
												strcat(tips,"\r\n");	
												Driver_USART1.Send(tips,strlen(tips));											
												break;
						
									default:	break;
					}
					break;
					
			case KEY_CTOVER	:
				
					switch(keyVal & 0x00f0){
					
					
						case KEY_VALUE_1:	strcat(tips,"����1����");
												strcat(tips,(const char*)&kCount_rec);
												strcat(tips,"�κ����\r\n");
												Driver_USART1.Send(tips,strlen(tips));
												kCount_rec = 0;
												break;
						
						case KEY_VALUE_2:	strcat(tips,"����2����");
												strcat(tips,(const char*)&kCount_rec);
												strcat(tips,"�κ����\r\n");
												Driver_USART1.Send(tips,strlen(tips));
												kCount_rec = 0;
												break;
						
									default:	break;
					}
					
					
			default:break;
		}
		
		osDelay(KEY_TICK);
	}
#endif
}
	
void keyTest(void){
	
	tid_keyTest_Thread = osThreadCreate(osThread(keyTest_Thread),NULL);
}



