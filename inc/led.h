/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_H
#define __LED_H

enum LED {
  LED_ORANGE,
  LED_GREEN,  
	LED_BLUE,
	LED_RED
};

void ledInit(void);
void ledOn(enum LED Color);
void ledOff(enum LED Color);

#endif /* __LED_H */
