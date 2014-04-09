#include "cmd_manager.h"
#include "cmd_handler.h"
#include "rtc.h"
#include <stdio.h>
#include <stm32f4xx.h>
#include <led.h>
#include <adc.h>
#include <math.h>

#define VoltMax  3.04
#define ADCvalue 4095
#define sampling 20
#define Prescale 2000
#define NumofMid 10

volatile uint16_t msec = 0;
volatile uint8_t BurstMode_Enabled = 0;
volatile uint8_t Calibrate_Enabled = 0;

float ADC1array[(sampling*2)];
float ADC2array[(sampling*2)];
float ADC3array[(sampling*2)];

float Volt[3][sampling*2];
float sum[3]={0,0,0};

uint8_t toggle = 0;
uint8_t toggleIT = 0;

__IO int count = 0;

float abs(float val) {
	return (val < 0) ? -val : val;
}

void SysTick_Handler()
{
  if (msec < 999) {
    msec++;
  }
	else{
	 msec = 0;
	}
}

void delay(uint32_t dly_msec)
{
  msec = dly_msec;
  while(msec > 0);
}

int fputc(int ch, FILE *f){
	
  return ITM_SendChar(ch);
}

void interuptConf(uint16_t period){
	
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	//TIM_OCInitTypeDef  TIM_OCInitStructure;

	TIM_DeInit(TIM3);
	
  /* Enable the TIM3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = period;
  TIM_TimeBaseStructure.TIM_Prescaler = Prescale-1; 
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);	
	
  /* TIM IT enable */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}

void TIM3_IRQHandler(void){
	
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {				
		if(count < sampling*2)
		{
			ADC1array[count] = (adcRead1()*VoltMax/ADCvalue);
			ADC2array[count] = (adcRead2()*VoltMax/ADCvalue);
			ADC3array[count] = (adcRead3()*VoltMax/ADCvalue);
			count++;
		}
		
		else
		{
			printf("Out of ??\n");
		}
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
  }
}

int getperiod(int samplingtime){
	
	int period;
	
	period = ((84*10000)/(samplingtime*Prescale))-1;
	
	return period;
}

float mid(float  volt[3][sampling*2],int i){
	
	float min;
	float max;
	int j;
	float Vmid;
	
	min = volt[i][0]; 
	max = volt[i][0];
	
	for(j=0;j<sampling*2;j++){
		if(volt[i][j] < min)
				min = volt[i][j];
		if(volt[i][j] > max)
				max = volt[i][j];
	}

	Vmid = (min + max)/2;
			
	return Vmid;
}

void ReceiveRef(float Ref[3]){
	
	int i,num;
	float midpoint[3][NumofMid];

	for(num=0;num<NumofMid;num++){
		
		while(count < sampling*2);
			
		count = 0;
		
		for(i=0;i<sampling*2;i++){
			Volt[0][i] = ADC1array[i];
			Volt[1][i] = ADC2array[i];
			Volt[2][i] = ADC3array[i];
		}
		
		for(i=0;i<3;i++)
		midpoint[i][num] = mid(Volt,i);
	
	}
	
	for(i=0;i<NumofMid;i++){	
		sum[0] += midpoint[0][i];
		sum[1] += midpoint[1][i];
		sum[2] += midpoint[2][i];
	}
		
	Ref[0] = sum[0]/(float)NumofMid;
	Ref[1] = sum[1]/(float)NumofMid;
	Ref[2] = sum[2]/(float)NumofMid;
		
	sum[0] = 0;
	sum[1] = 0;
	sum[2] = 0;
	
	count = 0;
		
}


void ReceiveRMS(float ref[3],float Ref_RMS[3]){
	
	int i,num;
	
	Ref_RMS[0] = 0;
	Ref_RMS[1] = 0;
	Ref_RMS[2] = 0;
	
	for(num=0;num<NumofMid;num++){		
		
		while(count < sampling);
			
		for(i=0;i<sampling;i++){
				Volt[0][i] = ADC1array[i];
				Volt[1][i] = ADC2array[i];
				Volt[2][i] = ADC3array[i];	
		}

						///  RMS //// 
		for(i = 0;i<sampling;i++){					
				sum[0] += (float)pow(Volt[0][i]-ref[0],2);
				sum[1] += (float)pow(Volt[1][i]-ref[1],2);
				sum[2] += (float)pow(Volt[2][i]-ref[2],2);
		}
			
		for(i = 0;i<3;i++)
				Ref_RMS[i] += sqrt(sum[i]/sampling);   // root((2*v^2)/t)
				
		sum[0] = 0;
		sum[1] = 0;
		sum[2] = 0;
	
	}
	
	for(i = 0;i<3;i++)
		Ref_RMS[i] = (float)Ref_RMS[i]/NumofMid;
		
	count =0;
	
}

void GetRMS(float ref[3],float Ref_RMS[3], float RMS[3], uint8_t PercentRMS[3]){
	
	int i;
		
		if (count == sampling){

			count = 0;

			for(i=0;i<sampling;i++){
					Volt[0][i] = ADC1array[i];
					Volt[1][i] = ADC2array[i];
					Volt[2][i] = ADC3array[i];	
			}		
		
				///  RMS //// 
			for(i = 0;i<sampling;i++){					
					sum[0] += (float)pow(Volt[0][i]-ref[0],2);
					sum[1] += (float)pow(Volt[1][i]-ref[1],2);
					sum[2] += (float)pow(Volt[2][i]-ref[2],2);
			}
			
			for(i = 0;i<3;i++)
					RMS[i] =  sqrt(sum[i]/sampling);   // root((2*v^2)/t)
				
			sum[0] = 0;
			sum[1] = 0;
			sum[2] = 0;
			
			for(i=0;i<3;i++){
				
					PercentRMS[i] = abs((((Ref_RMS[i]-RMS[i])/Ref_RMS[i]) * 100));
					PercentRMS[i] = ((PercentRMS[i]/5)*5);
					
					if(RMS[i]>Ref_RMS[i] && PercentRMS[i] >= 10)
							PercentRMS[i] += 100;
					
			}			
		}
}



int main(void) {
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	CmdTx_RmsChanged_Report_t cmd;
	
	int   period,sec=0;
	float RefRMS[3] = {0.7,0.7,0.7},RMS[3];
	float ref[3] = {1.52,1.52,1.52};
	uint8_t PercentRMS[3];
	float tmpPercentRMST[3];
	// setup
  ledInit();
 	adcInit();
	
	usbInit();
	rtcInit();
	SysTick_Handler();
	/////Set Interupt sampling : 1/2 T
	period=getperiod(sampling);
	interuptConf(period);
	
	while(1) {
		
		GetRMS(ref,RefRMS,RMS,PercentRMS);
		
		
		rtcGet(&time, &date);
		if(sec != time.RTC_Seconds)
		{
			msec = 0;
			sec = time.RTC_Seconds;
		}
		
		if(Calibrate_Enabled) {
			ReceiveRef(ref);
			ReceiveRMS(ref,RefRMS);
			// send calibrate back
			Calibrate_Enabled = 0;
		}
		
		else if(BurstMode_Enabled) {
			if(abs(tmpPercentRMST[0] - PercentRMS[0]) >= 10 
					|| abs(tmpPercentRMST[1] - PercentRMS[1]) >= 10 
					|| abs(tmpPercentRMST[2] - PercentRMS[2]) >= 10) {
				rtcGet(&time, &date);
				cmd.status = CmdStatus_Success;
				cmd.year = date.RTC_Year;
				cmd.month = date.RTC_Month;
				cmd.date = date.RTC_Date;
				cmd.hours = time.RTC_Hours;
				cmd.minutes = time.RTC_Minutes;
				cmd.seconds = time.RTC_Seconds;
				cmd.milliseconds = msec;
				cmd.pu1 = (uint8_t)PercentRMS[0];//(abs(tmpPercentRMST[0] - PercentRMS[0]) >= 10) ? (uint8_t)PercentRMS[0] : 0xFF;
				cmd.pu2 = (uint8_t)PercentRMS[1];//(abs(tmpPercentRMST[1] - PercentRMS[0]) >= 10) ? (uint8_t)PercentRMS[1] : 0xFF;
				cmd.pu3 = (uint8_t)PercentRMS[2];//(abs(tmpPercentRMST[2] - PercentRMS[0]) >= 10) ? (uint8_t)PercentRMS[2] : 0xFF;
				sendCmd(CmdTx_RmsChanged_Report, (uint8_t*)&cmd, sizeof(CmdTx_RmsChanged_Report_t));
				
				tmpPercentRMST[0] = PercentRMS[0];
				tmpPercentRMST[1] = PercentRMS[1];
				tmpPercentRMST[2] = PercentRMS[2];
			}
		}
		
	}
}
