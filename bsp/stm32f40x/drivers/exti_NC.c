#include <stm32f10x.h>
#include <rtthread.h>
#include "fsmc.h"
#include "exti.h"
#include "ads1299_config.h"

void NVIC_Configuration(void)
{
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
}

//�ⲿ�жϳ�ʼ������
void rt_hw_exti_init(void)
{
// 	EXTI_InitTypeDef EXTI_InitStructure;
// 	NVIC_InitTypeDef NVIC_InitStructure;

  //	�˿ڳ�ʼ��
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO, ENABLE);//ʹ��PORTGʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;//PG2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
	GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��GPIOG2
	
	//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;//PA15
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
	//	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA15
	
//	//GPIOA.15 �ж����Լ��жϳ�ʼ������   �����ش���
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);

//	EXTI_InitStructure.EXTI_Line = EXTI_Line10;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���


	
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;							
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//�����ȼ�0
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
//	NVIC_Init(&NVIC_InitStructure); 
}

void EXTI2_Enable(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

//  //	�˿ڳ�ʼ��
//	GPIO_InitTypeDef GPIO_InitStructure;

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);//ʹ��PORTGʱ��

//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;//PG2
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
//	GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��GPIOG2

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��

  //GPIOG.2 �ж����Լ��жϳ�ʼ������   �½��ش���
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource2);

	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

 	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;							
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�1�� 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); 
}

void EXTI2_Disable(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

  //GPIOG.2 �ж����Լ��жϳ�ʼ������   �½��ش���
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG,GPIO_PinSource2);

	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

 	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;							
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); 
}

//�ⲿ�ж�2������� 
void EXTI2_IRQHandler(void)
{
	/* enter interrupt */
   rt_interrupt_enter();
	
	//����SRAM Available��Ϣ
	rt_event_send(&sram_state_event, SRAM_AVALIABLE);
	
	EXTI_ClearITPendingBit(EXTI_Line2); //���LINE2�ϵ��жϱ�־λ  
	
	/* leave interrupt */
  rt_interrupt_leave();
}

////�ⲿ�ж�10~15�������
//void EXTI15_10_IRQHandler(void)
//{
//	/* enter interrupt */
//   rt_interrupt_enter();
//	
//	if(EXTI_GetITStatus(EXTI_Line10) == SET)
//	{
//		EXTI_ClearITPendingBit(EXTI_Line10);  //���LINE10�ϵ��жϱ�־λ		
//	}		
//	
//	if(EXTI_GetITStatus(EXTI_Line11) == SET)
//	{
//		EXTI_ClearITPendingBit(EXTI_Line11);  //���LINE11�ϵ��жϱ�־λ 
//	}		
//	
//	if(EXTI_GetITStatus(EXTI_Line12) == SET)
//	{
//		EXTI_ClearITPendingBit(EXTI_Line12);  //���LINE12�ϵ��жϱ�־λ 
//	}		
//	
//	if(EXTI_GetITStatus(EXTI_Line13) == SET)
//	{
//		EXTI_ClearITPendingBit(EXTI_Line13);  //���LINE13�ϵ��жϱ�־λ 
//	}		
//	
//	if(EXTI_GetITStatus(EXTI_Line14) == SET)
//	{
//		EXTI_ClearITPendingBit(EXTI_Line14);  //���LINE14�ϵ��жϱ�־λ 
//	}		
//	
//	if(EXTI_GetITStatus(EXTI_Line15) == SET)
//	{
//		EXTI_ClearITPendingBit(EXTI_Line15);  //���LINE15�ϵ��жϱ�־λ
//	}	
//	
//	/* leave interrupt */
//  rt_interrupt_leave();
//}
