#include "spi.h"

void rt_hw_spi2_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
	
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource1, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource2, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource3, GPIO_AF_SPI2);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0|GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
	GPIO_Init(GPIOI, &GPIO_InitStructure);
	
	//CS(NSS)
	/*
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	*/

	
	SPI_InitStruct.SPI_Direction			= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode 				= SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize 			= SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL 				= SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA 				= SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS 					= SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler 	= SPI_BaudRatePrescaler_4;
	SPI_InitStruct.SPI_FirstBit 			= SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial 		= 7;

	SPI_Init(SPI2, &SPI_InitStruct); 
 
	SPI_Cmd(SPI2, ENABLE);
}

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
rt_uint8_t SPI2_ReadWriteByte(rt_uint8_t TxData)
{		
//	rt_uint8_t retry=0;			
	while((SPI2->SR & SPI_I2S_FLAG_TXE) == RESET)
//	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
//		retry++;
//		if(retry>200)return 0;
	}			  
//	SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ������
	SPI2->DR = TxData;
//	retry=0;

	while((SPI2->SR & SPI_I2S_FLAG_RXNE) == RESET)
//	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
//		retry++;
//		if(retry>200)return 0;
	}	  						    
//	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����		
	return (SPI2->DR);
}
