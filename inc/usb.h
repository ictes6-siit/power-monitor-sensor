#ifndef __USB_H
#define __USB_H

#include  "usbd_cdc_core.h"
#include  "usbd_usr.h"
#include  "usbd_desc.h"

void usbInit(void);
void usbWrite(uint8_t* Buf, uint32_t Len);
#endif
