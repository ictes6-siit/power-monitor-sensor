#include "cmd_manager.h"
#include "cmd_handler.h"
#include "rtc.h"
#include <stdio.h>

volatile uint32_t msec = 0;
volatile uint8_t BurstMode_Enabled = 0;

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
	CmdTx_RmsChanged_Report_t cmd;
		
	usbInit();
	rtcInit();
	
	while(1) {
		if(BurstMode_Enabled) {
			rtcGet(&time, &date);
			cmd.year = date.RTC_Year;
			cmd.month = date.RTC_Month;
			cmd.date = date.RTC_Date;
			cmd.hours = time.RTC_Hours;
			cmd.minutes = time.RTC_Minutes;
			cmd.seconds = time.RTC_Seconds;
			cmd.milliseconds = 999;
			cmd.pu1 = 100;
			cmd.pu2 = 100;
			cmd.pu3 = 100;
			sendCmd(CmdTx_RmsChanged_Report, (uint8_t*)&cmd, sizeof(CmdTx_RmsChanged_Report_t));
		}
		delay(10);
	}
}
