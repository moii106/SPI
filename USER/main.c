#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "adxl345.h" 
#include "timer.h"
#include "math.h"
#include "GPS.h"
	short x,y,z;  
	short angx,angy,angz; 
	char xx,yy,zz;
	float	mold,Last_xx,Last_yy,Last_zz;
	u8 Speed_Change_Flag,Deal_Flag;
	u8 High_Speed_Count;
	u8 j;
	u16 i;
	int temp=0;
	u16 Store_X[100],Store_Y[100],Store_Z[100];
	u8 buf[2];
	u8 Store_XX[100],Store_YY[100],Store_ZZ[100];
	float Store_Mold[100],Store_Last_XX[100],Store_Last_YY[100],Store_Last_ZZ[100];//高速下ADXL345和向量模长
	double Max_Mold;//低速下ADXL345和向量模长
int main(void)
{	
	GPS_Config();
	LED_Init();		  	//初始化与LED连接的硬件接口
	delay_init();	    //延时函数初始化	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级   
	ADXL345_Init();   //ADXL345初始化
	uart_init(115200);
	TIM3_Int_Init(124,719);//100Khz的计数频率，计数到100为1ms  
	TIM2_Int_Init(30,719);//100Khz的计数频率，计数到100为1ms  
	i=0;
	j=0;
	LED1=1;
	LED0=1;	
	Speed_Change_Flag=0;//0：低速   1：高速  
	Deal_Flag=0;
	Max_Mold=1.5;
	Last_xx=30;
	Last_yy=30;
	Last_zz=30;
	Speed_Change_Flag=0;

	while(1)
	{
		delay_ms(10);
	}
	while(1)
	{	
		if((Deal_Flag==1)&&(Speed_Change_Flag==0)) //1.25ms存储一次，直到模长大于1.5g
		{				
			Deal_Flag=0;
			xx=(u8)(x>>5);//ADXL345发送数据为13位，取高8位有效位
			if(xx>127)
			{
				xx=~(xx-1);
			}
			yy=(u8)(y>>5);
			if(yy>127)
			{
				yy=~(yy-1);
			}
			zz=(u8)(z>>5);
			if(zz>127)
			{
				zz=~(zz-1);
			}
			if((Last_xx==30)&&(Last_yy==30)&&(Last_zz==30))
			{
				Last_xx=xx;
				Last_yy=yy;
				Last_zz=zz;
			}
			else
			{
				Last_xx=Last_xx*0.9+xx*0.1;
				Last_yy=Last_yy*0.9+yy*0.1;
				Last_zz=Last_zz*0.9+zz*0.1;
			}
			mold=sqrt(Last_xx*Last_xx+Last_yy*Last_yy+Last_zz*Last_zz);
			mold=mold*0.125;
			if(mold>=1.5)    //低速情况下且模长>=1.5g
			{			
				ADXL345_WriteData(BW_RATE,0x0F);
				Speed_Change_Flag=1; 
				Last_xx=30;
				Last_yy=30;
				Last_zz=30;				
			}
		}
		if((Deal_Flag==1)&&(Speed_Change_Flag==1))   //0.3ms存储一次，存储100次
		{	
			if(High_Speed_Count<100)
			{
				Store_X[High_Speed_Count]=x;
				Store_Y[High_Speed_Count]=y;
				Store_Z[High_Speed_Count]=z;	
				High_Speed_Count=High_Speed_Count+1;				
			}
			else
			{
				for(i=1;i<100;i++)
				{
					Store_XX[i]=(u8)(Store_X[i]>>5);
					if(Store_XX[i]>=128)
					{
						Store_XX[i]=~(Store_XX[i]-1);
					}
					Store_YY[i]=(u8)(Store_Y[i]>>5);
					if(Store_YY[i]>=128)
					{
						Store_YY[i]=~(Store_YY[i]-1);
					}
					Store_ZZ[i]=(u8)(Store_Z[i]>>5);
					if(Store_ZZ[i]>=128)
					{
						Store_ZZ[i]=~(Store_ZZ[i]-1);
					}
					if(i==0)
					{
						Store_Last_XX[i]=Store_XX[i];
						Store_Last_YY[i]=Store_YY[i];
						Store_Last_ZZ[i]=Store_ZZ[i];
					}
					else
					{
						Store_Last_XX[i]=0.90*Store_Last_XX[i]+0.10*Store_XX[i];
						Store_Last_YY[i]=0.90*Store_Last_YY[i]+0.10*Store_YY[i];
						Store_Last_ZZ[i]=0.90*Store_Last_ZZ[i]+0.10*Store_ZZ[i];
					}
					Store_Mold[i]=sqrt(Store_Last_XX[i]*Store_Last_XX[i]+Store_Last_YY[i]*Store_Last_YY[i]+Store_Last_ZZ[i]*Store_Last_ZZ[i]);
					USART_SendData(USART1,Store_Mold[i]);
			    while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//发送最大值
					if(Max_Mold<Store_Mold[i])
					{
						Max_Mold=Store_Mold[i];
					}
				}	
				Speed_Change_Flag=0;				
				USART_SendData(USART1,Max_Mold);
			  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//发送最大值
//				printf("%f\n\r",Max_Mold);
			}
			Deal_Flag=0;					
		}
  }	 
 }

 
//定时器2中断服务程序    高速采集
void TIM2_IRQHandler(void)   //TIM2中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx更新中断标志 
			if((Deal_Flag==0)&&(Speed_Change_Flag==1))
			{
				Get_ADXL345_data(&x,&y,&z);//读取xyz三轴数据
				Deal_Flag=1;
				LED0=0;
			}
		}
}

//定时器3中断服务程序  低速采集
void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
			if((Deal_Flag==0)&&(Speed_Change_Flag==0))
			{
				Get_ADXL345_data(&x,&y,&z);//读取xyz三轴数据
				Deal_Flag=1;
				LED1=0;
				High_Speed_Count=0;
				Max_Mold=0;
			}
		}
}

