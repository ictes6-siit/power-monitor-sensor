#include "rtc.h"

/* Private define ------------------------------------------------------------*/
/* Uncomment the corresponding line to select the RTC Clock source */
//#define RTC_CLOCK_SOURCE_LSE            /* LSE used as RTC source clock */
#define RTC_CLOCK_SOURCE_LSI      /* LSI used as RTC source clock. The RTC Clock
                                           may varies due to LSI frequency dispersion. */ 
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private Functions -------------------- */
static void RTC_Config(void);
static void RTC_TimeRegulate(void);

void rtcInit(void){
	/* RTC configuration  */
	RTC_Config();
	
	// If RTC is not set.
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)
  {
		/* Configure the time&date register */
    RTC_TimeRegulate();
		
	} else {
    /* Clear the RTC Alarm Flag */
    RTC_ClearFlag(RTC_FLAG_ALRAF);

    /* Clear the EXTI Line 17 Pending bit (Connected internally to RTC Alarm) */
    EXTI_ClearITPendingBit(EXTI_Line17);
	}
}

void rtcGet(RTC_TimeTypeDef* time, RTC_DateTypeDef* date){	
	/* Get Time hh:mm:ss */
  RTC_GetTime(RTC_Format_BIN, time);
	
	/* Get Date Week/Date/Month/Year */
	RTC_GetDate(RTC_Format_BIN, date);
}

void rtcSet(RTC_TimeTypeDef* time, RTC_DateTypeDef* date){
	/*** Unlock RTC Registers ***/
	RTC_WriteProtectionCmd(DISABLE); // Disable RTC register write protection
	
	/* Set Time hh:mm:ss */
	RTC_SetTime(RTC_Format_BIN, time);
	
	/* Set Date Week/Date/Month/Year */
	RTC_SetDate(RTC_Format_BIN, date);
	
	RTC_WriteProtectionCmd(ENABLE); // Enable RTC register write protection
}

/**
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
static void RTC_Config(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	uint32_t uwAsynchPrediv = 0;
	uint32_t uwSynchPrediv = 0;
	
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
/* The RTC Clock may varies due to LSI frequency dispersion. */
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);
	
  /* Wait till LSI is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  
  /* ck_spre(1Hz) = RTCCLK(LSI) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
  uwSynchPrediv = 0xF9;
  uwAsynchPrediv = 0x7F;

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  /* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
  uwSynchPrediv = 0xF9;
  uwAsynchPrediv = 0x7F;
    
#else
  #error Please select the RTC Clock source inside the main.c file
#endif /* RTC_CLOCK_SOURCE_LSI */

   /* Configure the RTC data register and RTC prescaler */
  RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
  RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
  
  /* Check on RTC init */
  if (RTC_Init(&RTC_InitStructure) == ERROR)
  {
    printf("RTC Prescaler Config failed." );
  }
  
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
  
  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro(); 
}

/**
  * @brief  Returns the time entered by user, using Hyperterminal.
  * @param  None
  * @retval None
  */
static void RTC_TimeRegulate(void)
{
	RTC_DateTypeDef RTC_DateStructure;
	RTC_TimeTypeDef RTC_TimeStructure;
  /* Set Time hh:mm:ss */
  RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
  RTC_TimeStructure.RTC_Hours   = 0x08;  
  RTC_TimeStructure.RTC_Minutes = 0x10;
  RTC_TimeStructure.RTC_Seconds = 0x00;
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);

  /* Set Date Week/Date/Month/Year */
  RTC_DateStructure.RTC_WeekDay = 01;
  RTC_DateStructure.RTC_Date = 0x31;
  RTC_DateStructure.RTC_Month = 0x12;
  RTC_DateStructure.RTC_Year = 0x12;
  RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
  
  /* Write BkUp DR0 */
  RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
}
