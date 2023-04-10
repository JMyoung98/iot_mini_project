

/******************************************************************************
* General-purpose timers TIM10

  포트 연결:

  PA0,PA1 : UART4 RX,TX
  PA2,PA3 : UART2 RX,TX
  PA5 ~ PA12 : CLCD D0~D7
  PB0 ~ PB2  : CLCD RS, RW, E
  PC0~PC7 ==> LED
   
******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "uart2.h"
#include "uart4.h"
#include "lcd.h"

#include <string.h>
#include "led.h"
#define CMD_SIZE 50
#define ARR_CNT 5  
#define PORTC_LED 

extern volatile unsigned char rx2Flag;
extern char rx2Data[50];
extern volatile unsigned char rx4Flag;
extern char rx4Data[50];

int main()
{
  UART2_init();
  UART4_init();
  GPIOAB_LCD_Init();
  portc_led_init();
  Serial2_Send_String("start main()\r\n");
  while(1)
  {
    if(rx2Flag)
    {
      printf("rx2Data : %s\r\n",rx2Data);
      Serial4_Send_String(rx2Data);
     Serial4_Send_String("\r\n");
      rx2Flag = 0;
    }  
    if(rx4Flag)
    {
      Serial4_Event();
      rx4Flag = 0;
    }
    
  }
}

void Serial4_Event()
{
  
  int i=0;
  
  char * pToken;
  char * pArray[ARR_CNT]={0};
  char recvBuf[CMD_SIZE]={0};
  strcpy(recvBuf,rx4Data);
  i=0; 

  printf("rx4Data : %s\r\n",recvBuf);
     
  pToken = strtok(recvBuf,"[@]");

  while(pToken != NULL)
  {
    pArray[i] =  pToken;
    if(++i >= ARR_CNT)
      break;
    pToken = strtok(NULL,"[@]");
  }
  
  
  if(strcmp(pArray[1],"correct")==0){
      pArray[2][strlen(pArray[2])-1]='\0' ;
      lcd(0,0,pArray[2]);
      Delay(3000);
       Init_LCD();
  }
  
  if(strcmp(pArray[1],"Decline\r")==0){
     GPIO_SetBits(GPIOC, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
     Delay(3000);
     GPIO_ResetBits(GPIOC, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
  }
}

