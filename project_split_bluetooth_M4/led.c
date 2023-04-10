// ÇÉ ¿¬°á
//PC0~PC7 : FND A~H 
//PC8~PC11 : FND C0~C3

#include "stm32f4xx.h"
#include "led.h"


static volatile int t_cnt = 0;                                      //4ms
static volatile unsigned long systick_count = 0;        //4ms
static volatile unsigned long systick_sec=0;            //1sec
static volatile int fndNumber = 0;
static volatile int systick_secFlag = 0;                                         
void portc_led_init()
{
  GPIO_InitTypeDef   GPIO_InitStructure;


  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
                                GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
  GPIO_Init(GPIOC, &GPIO_InitStructure);


}
  