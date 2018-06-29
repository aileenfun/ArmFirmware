#include <stm32f10x.h>
#include <rtthread.h>
#include "adc.h"
#include "dma.h"

void DMA_NVIC_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;							
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x04;	//��ռ���ȼ�3�� 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); 
}

void DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//ʹ��DMAʱ��
	
	/*��ʼ��ADC DMAͨ��*/
	DMA_DeInit(DMA1_Channel1);//��λDMAͨ��1
	DMA_InitStructure.DMA_PeripheralBaseAddr = (rt_uint32_t)&(ADC1->DR); //����DMAͨ���������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (rt_uint32_t)adc_value; //����DMAͨ���洢����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//ָ������ΪԴ��ַ
	DMA_InitStructure.DMA_BufferSize = ADC_BUFFER_SIZE;//����DMA��������С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//��ǰ�����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//��ǰ�洢����ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//������������λ��16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //����洢�����ݿ��16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMAͨ������ģʽΪ���λ���ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//DMAͨ�����ȼ���
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//��ֹDMAͨ���洢�����洢������
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);//��ʼ��DMAͨ��1
	DMA_Cmd(DMA1_Channel1, ENABLE); //ʹ��DMAͨ��1
	DMA_NVIC_init();
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);//ʹ��DMA��������ж�
}

void DMA1_Channel1_IRQHandler(void)
{
	/* enter interrupt */
   rt_interrupt_enter();
	
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
	{
		get_battery_voltage();		
		
		DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
	
	/* leave interrupt */
  rt_interrupt_leave();
}
