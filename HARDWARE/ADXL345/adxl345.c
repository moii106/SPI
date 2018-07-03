#include "sys.h"
#include "delay.h"
#include "math.h"   
#include "led.h"
#include "adxl345.h" 
#define ADXL345_SDI   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)//��ȡB9����ֵ
//��ʼ��ADXL345.
//����ֵ:0,��ʼ���ɹ�;1,��ʼ��ʧ��.
//д����
/**
  * @brief  ��ʼ��IIC��
  * @param  ��
  * @retval ��
  */
void SPI_IO_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//ʹ��GPIOBʱ��
	   	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		    //��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);					      //�����趨������ʼ��
	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );	//ʹ��GPIOBʱ��
	   	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_4); 	//PC4 �����
}


void ADXL345_WriteData(u8 addr, u8 data)
{
	u8 i;
	u16 temp;
	ADXL345_SCLK_SET;
	temp = addr | 0x00;
	temp = (temp<<8) | data;
//	delay_us(1);
	ADXL345_CS_CLR;
//	delay_us(1);
	for(i=0;i<16;i++)
	{
		ADXL345_SCLK_CLR;
		if(temp & 0x8000)
		  	ADXL345_SDI_SET;
		else
		 	ADXL345_SDI_CLR;
//		delay_us(1);
		ADXL345_SCLK_SET;
		temp = temp<<1;	
//		delay_us(1);
	}
	ADXL345_CS_SET;
}


void delay (signed int length)
{
	while (length >0)
    	length--;
}

/*
//������
void ReadFromADXL345ViaSpi(unsigned char RegisterAddress, unsigned char NumberofRegisters, unsigned char *RegisterData)
{
	unsigned	char	ControlValue = 0;
	signed		char	RegisterIndex = 0;
	unsigned	char	ReceiveData = 0;
	unsigned	char	i = 0;
	unsigned	int		iTemp = 0;

	//Create the 8-bit header
	ControlValue = RegisterAddress;

//	SET_SCL();
//	delay(1);	
	ADXL345_CS_SET;
	ADXL345_SCLK_SET;
	delay(3);
	ADXL345_CS_CLR;	 //bring CS low
	delay(3);

	ADXL345_SCLK_CLR;
	ADXL345_SDO_SET;	 //set READ bit
	delay(3);
	ADXL345_SCLK_SET;
	delay(3);
	ControlValue <<= 1;

	ADXL345_SCLK_CLR;
	ADXL345_SDO_SET;	//set MB bit
	delay(3);
	ADXL345_SCLK_SET;
	delay(3);
	ControlValue <<= 1;

	//Write out the control word
	for(i=0; i<6; i++)
	{
	ADXL345_SCLK_CLR;
		if(0x80 == (ControlValue & 0x80))
		{
			ADXL345_SDO_SET;	  //Send one to SDO pin
		}
		else
		{
			ADXL345_SDO_CLR;	  //Send zero to SDO pin
		}
		delay(3);
	ADXL345_SCLK_SET;
		delay(3);
		ControlValue <<= 1;	//Rotate data
	}

	//Read data in
	for (RegisterIndex=NumberofRegisters; RegisterIndex>0; RegisterIndex--)
	{
		for(i=0; i<8; i++)
		{
	ADXL345_SCLK_CLR;
			ReceiveData <<= 1;		//Rotate data
			delay(3);
	ADXL345_SCLK_SET;
		//	iTemp = GP1DAT;			//Read SDI of ADXL345
			if(ADXL345_SDI== 1)			   //SDI  P1.5
			{
				ReceiveData |= 1;	
			}
			delay(2);
		}
		*(RegisterData + RegisterIndex - 1) = ReceiveData;
	}
		ADXL345_CS_SET;//bring CS high again
} */

u8 ADXL345_ReadData(u8 addr)
{
	u8 i,data=0;
	ADXL345_SCLK_SET;
	addr = addr | 0x80;
//	delay_us(1);
	ADXL345_CS_CLR;
//	delay_us(1);
	for(i=0;i<8;i++)
	{
		ADXL345_SCLK_CLR;
		if(addr & 0x80)
		  	ADXL345_SDI_SET;
		else
		 	ADXL345_SDI_CLR;
//		delay_us(1);
		ADXL345_SCLK_SET;
		addr = addr<<1;	
//		delay_us(1);
	}
	for(i=0;i<8;i++)
	{
		data = data<<1;	
		ADXL345_SCLK_CLR;
//		delay_us(1);
		ADXL345_SCLK_SET;
//		delay_us(1);
		if(ADXL345_SDO_H)
		  data = data | 0x01;
//		delay_us(1);
	}
	ADXL345_CS_SET;
	return data;
}

//��ʼ����������ȡ�豸ID������ɹ�����1����������Ӧģʽ
u8 ADXL345_Init(void)
{
	u8 temp;
	SPI_IO_Init();
	temp=ADXL345_ReadData(DEVICE_ID);
	if(temp == 0xE5)
	{
		ADXL345_WriteData(BW_RATE,0x0C);
		ADXL345_WriteData(DATA_FORMAT, 0x2B);
		ADXL345_WriteData(INT_ENABLE,0x00);
		ADXL345_WriteData(POWER_CTL,0x28);
		ADXL345_WriteData(OFSX,0x00);		
		ADXL345_WriteData(OFSY,0x00);
		ADXL345_WriteData(OFSZ,0x00);
		return 1;
	}
	else
		return 0;
}

//��ø���������ٶ�ԭʼֵ
void Get_ADXL345_data(int16_t *p_xdata, int16_t *p_ydata, int16_t *p_zdata)
{
	u8 data[6];
	u8 i;
	int16_t Acceleration[3];
	for(i=0;i<6;i++)
	{
		data[i] = ADXL345_ReadData(DATA_X0 + i);
	}
	for(i=0;i<3;i++)
	{
//		data[(i<<1)+1] &= 0x1f;
		Acceleration[i] = data[i<<1] + ((u16)data[(i<<1)+1]<<8);
		Acceleration[i] = Acceleration[i]>>3;
//		if(Acceleration[i] < 4096)
//			Acceleration[i] = Acceleration[i] + 4096;
//		else if(Acceleration[i] >= 4096)
//			Acceleration[i] = Acceleration[i] - 4096;
	}
	*p_xdata = Acceleration[0];
	*p_ydata = Acceleration[1];
	*p_zdata = Acceleration[2];
}

//��ø���������ٶ�ʵ��ֵ
void Get_ADXL345_Value(float *p_xValue, float *p_yValue, float *p_zValue)
{
	int16_t xdata,ydata,zdata;
	Get_ADXL345_data(&xdata, &ydata, &zdata);
	*p_xValue = (float)xdata * 2.9;
	*p_yValue = (float)ydata * 2.9;
	*p_zValue = (float)zdata * 2.9;
}



//��ȡ���������ֵ
void Get_ADXL345_Max_Value(float *p_xValue, float *p_yValue, float *p_zValue)
{
	float xValue[32];
	float yValue[32];
	float zValue[32];
	float xValue_Max=0,yValue_Max=0,zValue_Max=0;
	u8 xValue_i,yValue_i,zValue_i;
	u8 i;
	for(i=0;i<32;i++)//���32��FIFO��ֵ
	{
		Get_ADXL345_Value(&xValue[i],&yValue[i],&zValue[i]);
	}
	for(i=0;i<32;i++)
	{
		if((xValue[i]>xValue_Max) || (-xValue[i]>xValue_Max))//��ȡ�����������ϵ����ֵ������¼��ǰ���ֵ������λ��
		{
			xValue_Max = xValue[i];
			xValue_i = i;
		}
		if((yValue[i]>yValue_Max) || (-yValue[i]>yValue_Max))
		{
			yValue_Max = yValue[i];
			yValue_i = i;
		}
		if((zValue[i]>zValue_Max) || (-zValue[i]>zValue_Max))
		{
			zValue_Max = zValue[i];
			zValue_i = i;
		}
	}
	
	if(((xValue_Max > yValue_Max) && (xValue_Max > zValue_Max)) || ((-xValue_Max > yValue_Max) && (-xValue_Max > zValue_Max)))
	{
		*p_xValue = xValue[xValue_i];
		*p_yValue = yValue[xValue_i];
		*p_zValue = zValue[xValue_i];
	}
	else if(((yValue_Max > xValue_Max) && (yValue_Max > zValue_Max)) || ((-yValue_Max > xValue_Max) && (-yValue_Max > zValue_Max)))
	{
		*p_xValue = xValue[yValue_i];
		*p_yValue = yValue[yValue_i];
		*p_zValue = zValue[yValue_i];		
	}
	else if(((zValue_Max > xValue_Max) && (zValue_Max > yValue_Max)) || ((-zValue_Max > xValue_Max) && (-zValue_Max > yValue_Max)))
	{
		*p_xValue = xValue[zValue_i];
		*p_yValue = yValue[zValue_i];
		*p_zValue = zValue[zValue_i];		
	}
}






