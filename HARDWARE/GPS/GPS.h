#ifndef __DRIVER_GPS__

#define __DRIVER_GPS__

#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include <string.h>

#define GPS_BUFFER_SIZE 128
#define GPS_MARK_SIZE   16

typedef struct {
	//��õ���100���ģ���3434.23339
	char Longitude[GPS_MARK_SIZE];	//����
	char Latitude [GPS_MARK_SIZE];	//γ��
	char Longitude_Mark, Latitude_Mark;
} GPS_InfoStruct;
//����ŵ�GPS���ݡ�
extern GPS_InfoStruct GPS_Info;
//�Ƿ�������Ч�Ķ�λ
extern uint8_t GPS_Update;

void GPS_Config(void);

#endif //__DRIVER_GPS__
