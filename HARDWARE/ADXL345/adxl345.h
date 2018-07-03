#ifndef __ADXL345_H
#define __ADXL345_H 												  	  

#include "sys.h"
#include "stm32f10x.h"

//相关引脚定义
// ADXL345_SCLK 引脚 */
#define  ADXL345_SCLK_PIN				GPIO_Pin_8
#define  ADXL345_SCLK_PORT			GPIOB
#define	 ADXL345_SCLK_CLR				GPIO_ResetBits(ADXL345_SCLK_PORT,ADXL345_SCLK_PIN) 	/* 置 ADXL312_SCLK 为低电平 */
#define	 ADXL345_SCLK_SET				GPIO_SetBits(ADXL345_SCLK_PORT,ADXL345_SCLK_PIN) 		/* 置 ADXL312_SCLK 为高电平 */

/* ADXL312_SDI 引脚 */
#define  ADXL345_SDI_PIN				GPIO_Pin_9
#define  ADXL345_SDI_PORT				GPIOB
#define	 ADXL345_SDI_CLR				GPIO_ResetBits(ADXL345_SDI_PORT,ADXL345_SDI_PIN)			/* 置 ADXL312_SDI 为低电平 */
#define	 ADXL345_SDI_SET				GPIO_SetBits(ADXL345_SDI_PORT,ADXL345_SDI_PIN)				/* 置 ADXL312_SDI 为高电平 */

/* ADXL312_SDO 引脚 */
#define  ADXL345_SDO_PIN				GPIO_Pin_1
#define  ADXL345_SDO_PORT				GPIOB
#define	 ADXL345_SDO_CLR				GPIO_ResetBits(ADXL345_SDO_PORT,ADXL345_SDO_PIN)			/* 置 ADXL312_SDO 为低电平 */
#define	 ADXL345_SDO_SET				GPIO_SetBits(ADXL345_SDO_PORT,ADXL345_SDO_PIN)				/* 置 ADXL312_SDO 为高电平 */
#define  ADXL345_SDO_H 					GPIO_ReadInputDataBit(ADXL345_SDO_PORT,ADXL345_SDO_PIN)

/* ADXL312_CS 引脚 */
#define  ADXL345_CS_PIN					GPIO_Pin_4
#define  ADXL345_CS_PORT				GPIOC
#define	 ADXL345_CS_CLR					GPIO_ResetBits(ADXL345_CS_PORT,ADXL345_CS_PIN)			  /* 置 ADXL312_CS 为低电平 */
#define	 ADXL345_CS_SET					GPIO_SetBits(ADXL345_CS_PORT,ADXL345_CS_PIN)				  /* 置 ADXL312_CS 为高电平 */



//相关寄存器定义
#define DEVICE_ID		0X00 	//器件ID,0XE5
#define THRESH_TAP		0X1D   	//敲击阀值
#define OFSX			0X1E
#define OFSY			0X1F
#define OFSZ			0X20
#define DUR				0X21
#define Latent			0X22
#define Window  		0X23 
#define THRESH_ACK		0X24
#define THRESH_INACT	0X25 
#define TIME_INACT		0X26
#define ACT_INACT_CTL	0X27	 
#define THRESH_FF		0X28	
#define TIME_FF			0X29 
#define TAP_AXES		0X2A  
#define ACT_TAP_STATUS  0X2B 
#define BW_RATE			0X2C 
#define POWER_CTL		0X2D 

#define INT_ENABLE		0X2E
#define INT_MAP			0X2F
#define INT_SOURCE  	0X30
#define DATA_FORMAT	    0X31
#define DATA_X0			0X32
#define DATA_X1			0X33
#define DATA_Y0			0X34
#define DATA_Y1			0X35
#define DATA_Z0			0X36
#define DATA_Z1			0X37
#define FIFO_CTL		0X38
#define FIFO_STATUS		0X39


//0X0B TO OX1F Factory Reserved	 
//如果ALT ADDRESS脚(12脚)接地,IIC地址为0X53(不包含最低位).
//如果接V3.3,则IIC地址为0X1D(不包含最低位).
//因为开发板接V3.3,所以转为读写地址后,为0X3B和0X3A(如果接GND,则为0XA7和0XA6)  
#define ADXL_READ    0XA7
#define ADXL_WRITE   0XA6

void SPI_IO_Init(void);                //初始化IIC的IO口
u8 ADXL345_Init(void);								//初始化ADXL345
void Get_ADXL345_data(int16_t *p_xdata, int16_t *p_ydata, int16_t *p_zdata);
void ADXL345_WriteData(u8 addr, u8 data);
void ReadFromADXL345ViaSpi(unsigned char RegisterAddress, unsigned char NumberofRegisters, unsigned char *RegisterData);
#endif




































