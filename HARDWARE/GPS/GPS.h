#ifndef __DRIVER_GPS__

#define __DRIVER_GPS__

#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include <string.h>

#define GPS_BUFFER_SIZE 128
#define GPS_MARK_SIZE   16

typedef struct {
	//获得的是100倍的，如3434.23339
	char Longitude[GPS_MARK_SIZE];	//经度
	char Latitude [GPS_MARK_SIZE];	//纬度
	char Longitude_Mark, Latitude_Mark;
} GPS_InfoStruct;
//最后存放的GPS数据。
extern GPS_InfoStruct GPS_Info;
//是否获得有有效的定位
extern uint8_t GPS_Update;

void GPS_Config(void);

#endif //__DRIVER_GPS__
