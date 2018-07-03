/**
  ******************************************************************************
  * @file    GPS.c
  * @author  Lux Syphurith SIe
  * @version r0
  * @date    Nov 17th, 2014
  * @brief   GPS��λ���á�
  * @attention
  * GPS�Ĵ��ں͵����ô��ڶ���#1���ڣ��ڵ���ʱ������ʹ��GPS��
  ******************************************************************************
  */
#include "led.h"
#include "GPS.h"
GPS_InfoStruct GPS_Info;
/**
  * @brief  GPS�Ľṹ���ʼ����
  * @param  info	GPS����Ľṹ�塣
  * @retval ��
  */
void GPS_Info_Init(GPS_InfoStruct* info) {
	memset(info->Latitude, 0, GPS_MARK_SIZE);
	memset(info->Longitude,0, GPS_MARK_SIZE);
	info->Latitude_Mark = 0;
	info->Longitude_Mark= 0;
}
/**
  * @brief  �ж϶�λ����Ƿ���Ч��
  * @param  info	GPS����Ľṹ�塣
  * @retval ��
  */
uint8_t GPS_Info_Mark_Valid(GPS_InfoStruct* info) {
	if (((info->Latitude_Mark == 'S') || (info->Latitude_Mark == 'N')) &&
			((info->Longitude_Mark== 'E') || (info->Longitude_Mark== 'W'))) return 1;
	return 0;
}
/**
  * @brief  GPS�豸�ĳ�ʼ����
  * @param  ��
  * @retval ��
  * @attention
  * ֵ��ע�����GPS�Ĳ�������4800����9600��
  */
void GPS_Config(void) {
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		//USART2_TX	  GPIOA.2��ʼ��
	GPIO_InitStructure.GPIO_Mode				= GPIO_Mode_AF_PP;;
	GPIO_InitStructure.GPIO_Pin					= GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Speed				= GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2
	
	//USART2_RX	  GPIOA.3��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.3 
	
	//����GPS������������9600��
	USART_InitStructure.USART_BaudRate						= 9600;
	USART_InitStructure.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode								= USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity							= USART_Parity_No;
	USART_InitStructure.USART_StopBits						= USART_StopBits_1;
	USART_InitStructure.USART_WordLength					= USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	////$$�ô������ȼ����ÿ�����Ҫ���ġ�
	NVIC_InitStructure.NVIC_IRQChannel										= USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd									= ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority					= 0;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART2, ENABLE);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	GPS_Info_Init(&GPS_Info);
}

//ʹ��GPRMC���ж�λ
const char GPS_GPRMC[] = "$GPRMC";
//ֻ������,A,������˵����GPRMC��Ч��
const char GPS_GPRMC_Available[] = ",A,";
//���GPS���յ����ݡ�
char GPS_Buffer[GPS_BUFFER_SIZE] = {0,};
//����GPS�����õĵ�������
uint8_t GPS_Buffer_Index = 0;
//�ɹ�����ɹ���λ��
uint8_t GPS_Update = 0;
////$$�ڵ�����ɺ���ܱ�ȥ����
//�ӿ�ʼ��λ����λ�ɹ������˶����ֽڡ�
uint32_t GPS_Byte_Count = 0;
/**
  * @brief  ��ͼ����GPS���ݡ�
  * @param  ��
  * @retval ��
  */
void GPS_Execute(void) {
	uint8_t GPS_Comma_Count = 0, GPS_Mark_Index = 0;
	if (strstr(GPS_Buffer, GPS_GPRMC) != 0) {
		if (strstr(GPS_Buffer, GPS_GPRMC_Available) == 0) return;
		memset(GPS_Info.Latitude, 0, GPS_MARK_SIZE);    //γ��
		memset(GPS_Info.Longitude,0, GPS_MARK_SIZE);    //����
		for (GPS_Buffer_Index = 0; GPS_Buffer_Index < GPS_BUFFER_SIZE; GPS_Buffer_Index ++) {
			if (GPS_Buffer[GPS_Buffer_Index] == '\0') {
				break;
			}
			if (GPS_Buffer[GPS_Buffer_Index] == ',') {
				GPS_Comma_Count ++;
				GPS_Mark_Index = 0;
				continue;
			}
			switch (GPS_Comma_Count) {
			case 5: GPS_Info.Longitude[GPS_Mark_Index] = GPS_Buffer[GPS_Buffer_Index]; GPS_Mark_Index++; break;
			case 6: GPS_Info.Longitude_Mark = GPS_Buffer[GPS_Buffer_Index]; break;
			case 3: GPS_Info.Latitude[GPS_Mark_Index] = GPS_Buffer[GPS_Buffer_Index]; GPS_Mark_Index++; break;
			case 4: GPS_Info.Latitude_Mark = GPS_Buffer[GPS_Buffer_Index]; break;
			}
		}
		if (GPS_Info_Mark_Valid(&GPS_Info)) {
			GPS_Update = 1;
			GPS_Byte_Count = 0;
		}
		GPS_Buffer_Index = 0;
	} else {
		return;
	}
}
/**
  * @brief  ��GPS����С������ǰ��λ��������2ΪС������3416.12��Ϊ34.16
  * @param  ��
  * @retval ��
  */
void GPS_GetRealValue(void)
{
	uint8_t i = 0;
	for(i = 0;i < GPS_MARK_SIZE;i++)
	{
		if(GPS_Info.Longitude[i] == '.') break;
	}
	GPS_Info.Longitude[i+4] = 0;
	GPS_Info.Longitude[i+3] = GPS_Info.Longitude_Mark;
	GPS_Info.Longitude[i] =GPS_Info.Longitude[i-1];
	GPS_Info.Longitude[i-1] = GPS_Info.Longitude[i-2];
	GPS_Info.Longitude[i-2] = '.';
	
	for(i = 0;i < GPS_MARK_SIZE;i++)
	{
		if(GPS_Info.Latitude[i] == '.') break;
	}
	GPS_Info.Latitude[i+4] = 0;
	GPS_Info.Latitude[i+3] = GPS_Info.Latitude_Mark;
	GPS_Info.Latitude[i] =GPS_Info.Latitude[i-1];
	GPS_Info.Latitude[i-1] = GPS_Info.Latitude[i-2];
	GPS_Info.Latitude[i-2] = '.';
}


//���չ����ĵ���GPS�ַ���
uint8_t GPS_Char = '\0';
/**
  * @brief  GPS�жϣ�����GPS���ݵĽ��ա�
  * @param  ��
  * @retval ��
  */
void USART2_IRQHandler(void) {
	if (SET == USART_GetITStatus(USART2, USART_IT_RXNE)) {
		uint8_t i = 0;	
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		LED0=0;
		GPS_Byte_Count ++;
		GPS_Char = USART_ReceiveData(USART2);
		if (GPS_Char == '$') {GPS_Buffer_Index = 0;}
		GPS_Buffer[GPS_Buffer_Index] = GPS_Char;
		GPS_Buffer_Index ++;
		if (GPS_Buffer_Index >= GPS_BUFFER_SIZE) GPS_Buffer_Index = 0;
		//GPS����ÿһ�н�β����\r\n����Ӧ0x0D��0x0A��
		if (GPS_Buffer[GPS_Buffer_Index] == 0x0A) {
			GPS_Buffer[GPS_Buffer_Index+1] = '\0';
			GPS_Execute();
			GPS_GetRealValue();
					
	for(i = 0;i < GPS_MARK_SIZE;i++)
	{
		if(GPS_Info.Longitude[i] == '.') break;
	}
  USART_SendData(USART1,GPS_Info.Latitude[i-2]);
	USART_SendData(USART1,GPS_Info.Latitude[i-1]);
	USART_SendData(USART1,GPS_Info.Latitude[i]);
	USART_SendData(USART1,GPS_Info.Latitude[i+1]);
	USART_SendData(USART1,GPS_Info.Latitude[i+2]);
		}
	}
}

