#include "usb.h"
#include "rtc.h"
#include <stdio.h>

volatile uint32_t msec = 0;

void SysTick_Handler()
{
  if (msec > 0) {
    msec--;
  }
}

void delay(uint32_t dly_msec)
{
  msec = dly_msec;
  while(msec > 0);
}

int fputc(int c, FILE *file)
{
  return ITM_SendChar(c);
}

int main(void) {
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
		
	usbInit();
	rtcInit();
	
	while(1) {
		rtcGet(&time, &date);
		
		delay(1);
	}
}
