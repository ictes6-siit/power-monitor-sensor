#include "cmd_manager.h"

extern CmdRx_Callback_t  CMD_CB;

void sendCmd(uint8_t cmd, uint8_t *data, uint8_t len)
{
	CmdPacket_t packet;
	uint32_t i;
	// init data
	packet.sync = SYNC_BYTE;
	packet.cmd = cmd;
	packet.len = len;
	packet.data = data;
	
	// cal checksum
	packet.crc = 0x00;
	packet.crc ^= cmd;
	packet.crc ^= len;
	for(i = 0; i < len; i++) {
		packet.crc ^= *(data+i);
	}
	
	usbWrite((uint8_t*)&packet, 3); // sync + cmd + len
	usbWrite(packet.data, len); // data
	usbWrite(&packet.crc, 1); // check sum
}

void CDC_DataRx_Handler (uint8_t* Buf, uint32_t Len) {
	uint32_t i;
	uint8_t check_crc = 0x00;
	uint8_t status = CmdStatus_Success;
	
	CmdPacket_t packet;
	packet.sync = *(Buf+0);
	packet.cmd = *(Buf+1);
	packet.len = *(Buf+2);
	if(packet.len > 0)
		packet.data = (Buf+3);
	packet.crc = *(Buf+3+packet.len);
	
	check_crc ^= packet.cmd;
	check_crc ^= packet.len;
	for(i = 0; i < packet.len; i++) {
		check_crc ^= *(packet.data + i);
	}
	
	// invalid packet
	if(packet.sync != SYNC_BYTE || packet.crc != check_crc) {
		status = CmdStatus_PacketError;
		sendCmd(packet.cmd, &status, sizeof(uint8_t));
		return;
	}
	
	switch(packet.cmd) {
		case CmdRx_Calibrate_Request:
			CMD_CB.pIf_CmdRx_Calibrate_Request((CmdRx_Calibrate_Request_t*)packet.data);
			break;
		
		case CmdRx_EnableBurst_Request:
			CMD_CB.pIf_CmdRx_EnableBurst_Request((CmdRx_EnableBurst_Request_t*)packet.data);
			break;
		
		case CmdRx_DisableBurst_Request:
			CMD_CB.pIf_CmdRx_DisableBurst_Request((CmdRx_DisableBurst_Request_t*)packet.data);
			break;
		default :
			status = CmdStatus_CommandNotSupport;
	}
}
