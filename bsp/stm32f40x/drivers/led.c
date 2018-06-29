/*
 * FILE                : led.c
 * DESCRIPTION         : This file is led driver.
 * Author              : ysloveivy
 * Copyright           :
 *
 * History
 * --------------------
 * Rev                 : 0.00
 * Date                : 11/21/2015
 *
 * create.
 * --------------------
 */
//--------------------------- Include ---------------------------//
#include "led.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

//--------------------------- Function --------------------------//
/*
 * Name                : initialize
 * Description         : ---
 * Author              : ysloveivy.
 *
 * History
 * --------------------
 * Rev                 : 0.00
 * Date                : 11/21/2015
 * 
 * create.
 * --------------------
 */
int led_hw_initialize(void)
{
	GPIO_InitTypeDef   GPIO_uInitStructure;

	//LED IO��ʼ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	GPIO_uInitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;    //����������ɫLED�Ƶ�IO�˿�
	GPIO_uInitStructure.GPIO_Mode = GPIO_Mode_OUT;                          //���ö˿�Ϊ���ģʽ
	GPIO_uInitStructure.GPIO_OType = GPIO_OType_PP;                         //�������
	GPIO_uInitStructure.GPIO_PuPd = GPIO_PuPd_UP;                           //����
	GPIO_uInitStructure.GPIO_Speed = GPIO_Speed_100MHz;                     //�����ٶ�Ϊ������

	GPIO_Init(GPIOE,&GPIO_uInitStructure);

	//PI5��PI6��PI7����ɫLED�ƣ�PI5��PI6��PI7�øߵ�λ����Ϩ��
	GPIO_SetBits(GPIOE,GPIO_Pin_0);
	GPIO_SetBits(GPIOE,GPIO_Pin_1);
	return 0;
}
