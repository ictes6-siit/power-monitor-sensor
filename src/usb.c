#include "usb.h"
#include "usb_core.h"
#include "usbd_core.h"
																		 
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
   
__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* These are external variables imported from CDC core to be used for IN 
   transfer management. */
extern uint8_t  APP_Rx_Buffer []; /* Write CDC received data in this buffer.
                                     These data will be sent over USB IN endpoint
                                     in the CDC core functions. */
extern uint32_t APP_Rx_ptr_in;    /* Increment this pointer or roll it back to
                                     start address when writing received data
                                     in the buffer APP_Rx_Buffer. */

/* Private function prototypes -----------------------------------------------*/
static uint16_t CDC_Init     (void);
static uint16_t CDC_DeInit   (void);
static uint16_t CDC_Ctrl     (uint32_t Cmd, uint8_t* Buf, uint32_t Len);
static uint16_t CDC_DataTx   (uint8_t* Buf, uint32_t Len);
static uint16_t CDC_DataRx (uint8_t* Buf, uint32_t Len);

CDC_IF_Prop_TypeDef CDC_fops = 
{
  CDC_Init,
  CDC_DeInit,
  CDC_Ctrl,
  CDC_DataTx,
  CDC_DataRx
};

/* Private function prototypes -----------------------------------------------*/
extern USB_OTG_CORE_HANDLE           USB_OTG_dev;
extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
extern uint32_t USBD_OTG_EP1IN_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
extern uint32_t USBD_OTG_EP1OUT_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#endif

void usbInit(void) {
	
  USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS 
            USB_OTG_HS_CORE_ID,
#else            
            USB_OTG_FS_CORE_ID,
#endif  
            &USR_desc, 
            &USBD_CDC_cb,
            &USR_cb);
}

void usbWrite(uint8_t* Buf, uint32_t Len) {
	CDC_DataTx(Buf, Len);
}

/******************************************************************************/
/*             Cortex-M Processor Exceptions Handlers                         */
/******************************************************************************/

/**
* @brief   This function handles NMI exception.
* @param  None
* @retval None
*/
void NMI_Handler(void)
{
}

/**
* @brief  This function handles Hard Fault exception.
* @param  None
* @retval None
*/
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
* @brief  This function handles Memory Manage exception.
* @param  None
* @retval None
*/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
* @brief  This function handles Bus Fault exception.
* @param  None
* @retval None
*/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
* @brief  This function handles Usage Fault exception.
* @param  None
* @retval None
*/
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
* @brief  This function handles SVCall exception.
* @param  None
* @retval None
*/
void SVC_Handler(void)
{
}

/**
* @brief  This function handles Debug Monitor exception.
* @param  None
* @retval None
*/
void DebugMon_Handler(void)
{
}

/**
* @brief  This function handles PendSVC exception.
* @param  None
* @retval None
*/
void PendSV_Handler(void)
{
}

/**
* @brief  This function handles SysTick Handler.
* @param  None
* @retval None
*/
volatile uint16_t msec = 0;
void SysTick_Handler(void)
{
		char* hello = "Hello";
		usbWrite((uint8_t*)hello, 5);
}

/**
* @brief  This function handles EXTI15_10_IRQ Handler.
* @param  None
* @retval None
*/
void EXTI0_IRQHandler(void)
{
	
}

/**
* @brief  This function handles EXTI15_10_IRQ Handler.
* @param  None
* @retval None
*/
#ifdef USE_USB_OTG_FS  
void OTG_FS_WKUP_IRQHandler(void)
{
  if(USB_OTG_dev.cfg.low_power)
  {
    /* Reset SLEEPDEEP and SLEEPONEXIT bits */
    SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
    
#ifdef USE_STM3210C_EVAL
    
    SystemInit();
#else    
    /* After wake-up from sleep mode, reconfigure the system clock */
    RCC_HSEConfig(RCC_HSE_ON);
    
    /* Wait till HSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
    {}
    
    /* Enable PLL */
    RCC_PLLCmd(ENABLE);
    
    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {}
    
    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    
    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08)
    {}
    
#endif    
    USB_OTG_UngateClock(&USB_OTG_dev);
  }
  EXTI_ClearITPendingBit(EXTI_Line18);
}
#endif

/**
* @brief  This function handles EXTI15_10_IRQ Handler.
* @param  None
* @retval None
*/
#ifdef USE_USB_OTG_HS  
void OTG_HS_WKUP_IRQHandler(void)
{
  
  if (remote_wakeup ==1)
  {
    remote_wakeup = 0;
  }
  else
  {
    if(USB_OTG_dev.cfg.low_power)
    {
      /* Reset SLEEPDEEP and SLEEPONEXIT bits */
      SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
      
      /* After wake-up from sleep mode, reconfigure the system clock */
      RCC_HSEConfig(RCC_HSE_ON);
      
      /* Wait till HSE is ready */
      while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
      {}
      
      /* Enable PLL */
      RCC_PLLCmd(ENABLE);
      
      /* Wait till PLL is ready */
      while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
      {}
      
      /* Select PLL as system clock source */
      RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
      
      /* Wait till PLL is used as system clock source */
      while (RCC_GetSYSCLKSource() != 0x08)
      {}
      
      USB_OTG_UngateClock(&USB_OTG_dev);
    }
  }
  EXTI_ClearITPendingBit(EXTI_Line20);
}
#endif

/**
* @brief  This function handles OTG_HS Handler.
* @param  None
* @retval None
*/
#ifdef USE_USB_OTG_HS  
void OTG_HS_IRQHandler(void)
#else
void OTG_FS_IRQHandler(void)
#endif
{
  USBD_OTG_ISR_Handler (&USB_OTG_dev);
}

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED 
/**
* @brief  This function handles EP1_IN Handler.
* @param  None
* @retval None
*/
void OTG_HS_EP1_IN_IRQHandler(void)
{
  USBD_OTG_EP1IN_ISR_Handler (&USB_OTG_dev);
}

/**
* @brief  This function handles EP1_OUT Handler.
* @param  None
* @retval None
*/
void OTG_HS_EP1_OUT_IRQHandler(void)
{
  USBD_OTG_EP1OUT_ISR_Handler (&USB_OTG_dev);
}
#endif

/******************************************************************************/
/*                 STM32Fxxx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32fxxx.s).                                               */
/******************************************************************************/

/**
* @brief  This function handles PPP interrupt request.
* @param  None
* @retval None
*/
/*void PPP_IRQHandler(void)
{
}*/


/******************************************************************************/
/*             Cortex-M Processor CRC Callback			                          */
/******************************************************************************/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  CDC_Init
  *         Initializes the CDC media low layer
  * @param  None
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static uint16_t CDC_Init(void)
{
  /*
     Add your initialization code here 
  */  
  return USBD_OK;
}

/**
  * @brief  CDC_DeInit
  *         DeInitializes the CDC media low layer
  * @param  None
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static uint16_t CDC_DeInit(void)
{
  /*
     Add your deinitialization code here 
  */  
  return USBD_OK;
}


/**
  * @brief  CDC_Ctrl
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static uint16_t CDC_Ctrl (uint32_t Cmd, uint8_t* Buf, uint32_t Len)
{ 
  switch (Cmd)
  {
  case SEND_ENCAPSULATED_COMMAND:
    /* Add your code here */
    break;

  case GET_ENCAPSULATED_RESPONSE:
    /* Add your code here */
    break;

  case SET_COMM_FEATURE:
    /* Add your code here */
    break;

  case GET_COMM_FEATURE:
    /* Add your code here */
    break;

  case CLEAR_COMM_FEATURE:
    /* Add your code here */
    break;

  case SET_LINE_CODING:
    /* Add your code here */
    break;

  case GET_LINE_CODING:
    /* Add your code here */
    break;

  case SET_CONTROL_LINE_STATE:
    /* Add your code here */
    break;

  case SEND_BREAK:
     /* Add your code here */
    break;    
    
  default:
    break;
  }

  return USBD_OK;
}

/**
  * @brief  CDC_DataTx
  *         CDC received data to be send over USB IN endpoint are managed in 
  *         this function.
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static uint16_t CDC_DataTx (uint8_t* Buf, uint32_t Len)
{
	uint32_t i;
  /* Get the data to be sent */
  for (i = 0; i < Len; i++)
  {
    APP_Rx_Buffer[APP_Rx_ptr_in] = *(Buf+i);
		
		/* Increment the in pointer */
		APP_Rx_ptr_in++;
  }
  
  /* To avoid buffer overflow */
  if(APP_Rx_ptr_in > APP_RX_DATA_SIZE)
  {
    APP_Rx_ptr_in = 0;
  }  
  
  return USBD_OK;
}

/**
  * @brief  CDC_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  *           
  *         @note
  *         This function will block any OUT packet reception on USB endpoint 
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result 
  *         in receiving more data while previous ones are still not sent.
  *                 
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the opeartion: USBD_OK if all operations are OK else USBD_FAIL
  */
static uint16_t CDC_DataRx (uint8_t* Buf, uint32_t Len)
{
  uint32_t i;
  
  /* Send the received buffer */
  for (i = 0; i < Len; i++)
  {
    /* XXXX_SendData(XXXX, *(Buf + i) ); */
  } 
 
  return USBD_OK;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
