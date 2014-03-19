#ifndef __RTC_H
#define __RTC_H

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_rtc.h>
#include <stdio.h>

void rtcInit(void);
void rtcGet(RTC_TimeTypeDef* time, RTC_DateTypeDef* date);
void rtcSet(RTC_TimeTypeDef* time, RTC_DateTypeDef* date);

#endif /* __RTC_H */
