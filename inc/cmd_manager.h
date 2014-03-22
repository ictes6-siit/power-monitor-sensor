#include "usb.h"
#include "rtc.h"

#define SYNC_BYTE		0xAA

typedef enum {
	CmdStatus_Success = 0x00,
	CmdStatus_PacketError = 0xFF,
	CmdStatus_CommandNotSupport = 0x01,
} CmdStatus_t;

typedef enum {
	/* Configuration */
	CmdTx_Calibrate_Confirm = 0xA0,
	
	/* Burst Mode */
	CmdTx_EnableBurst_Confirm = 0xB0,
	CmdTx_DisableBurst_Confirm = 0xB1,
	
	CmdTx_RmsChanged_Report = 0xB2,
} CmdTx;

typedef enum CmdRx {
	/* Configuration */
	CmdRx_Calibrate_Request = 0xA0,
	
	/* Burst Mode */
	CmdRx_EnableBurst_Request = 0xB0,
	CmdRx_DisableBurst_Request = 0xB1,
} CmdRx_t;

/* ---------------- Command Tx ----------------- */
typedef struct CmdTx_Calibrate_Confirm
{
	uint8_t status;
} CmdTx_Calibrate_Confirm_t;

typedef struct CmdTx_EnableBurst_Confirm
{
	uint8_t status;
} CmdTx_EnableBurst_Confirm_t;

typedef struct CmdTx_DisableBurst_Confirm
{
	uint8_t status;
} CmdTx_DisableBurst_Confirm_t;

typedef struct CmdTx_RmsChanged_Report
{
	uint8_t status;
	uint8_t year;
	uint8_t month;
	uint8_t date;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint16_t milliseconds;
	uint8_t pu1;
	uint8_t pu2;
	uint8_t pu3;
} __attribute__((packed)) CmdTx_RmsChanged_Report_t;

/* ---------------- End Command Tx ----------------- */

/* ---------------- Command Rx ----------------- */
typedef struct CmdRx_Calibrate_Request
{
	uint8_t year;
	uint8_t month;
	uint8_t date;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint16_t milliseconds;
} __attribute__((packed)) CmdRx_Calibrate_Request_t;

typedef struct CmdRx_EnableBurst_Request
{
	uint8_t data; // not use
} CmdRx_EnableBurst_Request_t;

typedef struct CmdRx_DisableBurst_Request
{
	uint8_t data; // not use
} CmdRx_DisableBurst_Request_t;

/* ---------------- End Command Rx ----------------- */

/* -------- Rx Command Callback function ----------- */
// return fbc
typedef struct CmdRx_Callback
{
  void (*pIf_CmdRx_Calibrate_Request)     (CmdRx_Calibrate_Request_t *req);   
  void (*pIf_CmdRx_EnableBurst_Request)   (CmdRx_EnableBurst_Request_t *req);   
  void (*pIf_CmdRx_DisableBurst_Request)  (CmdRx_DisableBurst_Request_t *req);
} CmdRx_Callback_t;
/* -------- End Rx Command Callback function -------- */

typedef struct CmdPacket
{
  uint8_t sync;
	uint8_t cmd;
	uint8_t len;
	uint8_t *data;
  uint8_t crc;
} CmdPacket_t;

void sendCmd(uint8_t cmd, uint8_t *data, uint8_t len);
