#include "cmd_manager.h"
#include "cmd_handler.h"
#include "rtc.h"

#define Cmd_Callback CMD_CB

extern uint8_t BurstMode_Enabled;
extern uint8_t Calibrate_Enabled;

/* Private function prototypes -----------------------------------------------*/
static void CB_CmdRx_Calibrate_Request     (CmdRx_Calibrate_Request_t *req);   
static void CB_CmdRx_EnableBurst_Request   (CmdRx_EnableBurst_Request_t *req);   
static void CB_CmdRx_DisableBurst_Request  (CmdRx_DisableBurst_Request_t *req);

CmdRx_Callback_t Cmd_Callback = 
{
  CB_CmdRx_Calibrate_Request,
	CB_CmdRx_EnableBurst_Request,
	CB_CmdRx_DisableBurst_Request
};

static void CB_CmdRx_Calibrate_Request(CmdRx_Calibrate_Request_t *req)
{
	CmdTx_Calibrate_Confirm_t resp;
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	
	date.RTC_Year = req->year;
	date.RTC_Month = req->month;
	date.RTC_Date = req->date;
	date.RTC_WeekDay = 0;
	time.RTC_Hours = req->hours;
	time.RTC_Minutes = req->minutes;
	time.RTC_Seconds = req->seconds;
	
	rtcSet(&time, &date);
	
	Calibrate_Enabled = 1;
	
	resp.status = CmdStatus_Success;
	sendCmd(CmdTx_Calibrate_Confirm, (uint8_t*)&resp, sizeof(CmdTx_Calibrate_Confirm_t));
}

static void CB_CmdRx_EnableBurst_Request(CmdRx_EnableBurst_Request_t *req)
{
	CmdTx_EnableBurst_Confirm_t resp;
	
	BurstMode_Enabled = 1;
	
	resp.status = CmdStatus_Success;
	sendCmd(CmdTx_EnableBurst_Confirm, (uint8_t*)&resp, sizeof(CmdTx_EnableBurst_Confirm_t));
}

static void CB_CmdRx_DisableBurst_Request(CmdRx_DisableBurst_Request_t *req)
{
	CmdTx_DisableBurst_Confirm_t resp;
	
	BurstMode_Enabled = 0;
	
	resp.status = CmdStatus_Success;
	sendCmd(CmdTx_DisableBurst_Confirm, (uint8_t*)&resp, sizeof(CmdTx_DisableBurst_Confirm_t));
}
