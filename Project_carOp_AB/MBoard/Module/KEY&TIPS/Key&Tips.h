#ifndef KEY$TIPS_H
#define KEY$TIPS_H

#define osObjectsPublic                     // define objects in main module

#include "stm32f10x.h"
#include "stdint.h"
#include "osObjects.h"                      // RTOS object definitions
#include "Driver_USART.h"

#define KEY_DEBUG		1		//�Ƿ����������ԣ�����1����������Ϣ��

#define K1	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)		//����1���
#define K2	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)		//����2���
#define K3	GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6)		//����1���
#define K4	GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)		//����2���
#define K5	GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)		//����1���
#define K6	GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9)		//����2���

#define KEY_TICK						20			 // �������� / (ms)

#define KEY_COMFIRM					3			 // ����������������ֶ�ÿ�ʼ������ȷ��ʱ�� / ����KEY_LONG_PERIOD * KEY_TICK(ms)

#define KEY_LONG_PERIOD				150		 // ����ʱ������ / ����KEY_TICK(ms) ������8λ ���ֵ254
#define KEY_KEEP_PERIOD				40			 // �������ּ�� / ����KEY_TICK(ms)
#define KEY_CONTINUE_PERIOD		40		*1000000		// ������� / (us)

#define KEY_VALUE_1	0x0010		//������ֵ1
#define KEY_VALUE_2	0x0020		//������ֵ2
#define KEY_VALUE_3	0x0030		//������ֵ1
#define KEY_VALUE_4	0x0040		//������ֵ2
#define KEY_VALUE_5	0x0050		//������ֵ1
#define KEY_VALUE_6	0x0060		//������ֵ2

#define KEY_DOWN		0x1000		//����״̬������
#define KEY_CONTINUE	0x2000		//����״̬����������
#define KEY_LONG		0x3000		//����״̬����������
#define KEY_KEEP		0x4000		//����״̬�����������󱣳�
#define KEY_UP			0x5000		//����״̬����������
#define KEY_NULL		0x6000		//����״̬���ް����¼�
#define KEY_CTOVER	0x7000		//����״̬����������

#define KEY_STATE_INIT		0x0100	//���״̬��״̬����ʼ��
#define KEY_STATE_WOBBLE	0x0200	//���״̬��״̬���������
#define KEY_STATE_PRESS		0x0300	//���״̬��״̬�������̰����
#define KEY_STATE_CONTINUE	0x0400	//���״̬��״̬�������������	���ϣ���KEY_STATE_RECORD���
#define KEY_STATE_LONG		0x0500	//���״̬��״̬�������������
#define KEY_STATE_KEEP		0x0600	//���״̬��״̬�����������󱣳ּ��
#define KEY_STATE_RELEASE	0x0700	//���״̬��״̬�������ͷż��

#define KEY_STATE_RECORD	0x0800	//���״̬��״̬��������¼������ȷ�ϼ��

#define KEY_OVER_SHORT		0x01
#define KEY_OVER_LONG		0x02
#define KEY_OVER_KEEP		0x03

void keyInit(void);
void keyTest_Thread(const void *argument);
void keyTest(void);

#endif
