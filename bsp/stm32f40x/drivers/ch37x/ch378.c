#include "ch378.h"
#include "stdio.h"
#include "delay.h"
#include "rtthread.h"

void ch378_power_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure PH11 as output */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);//enable gpio a clk
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //?? 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   //??
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //??
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //??  
	GPIO_Init(GPIOI, &GPIO_InitStructure);  //??GPIO_InitStructure??????????
	GPIO_SetBits(GPIOI,GPIO_Pin_10);//���߹ص�GL823 power

	for(long i=0;i<10000000;i++);
		/* Configure PC13 as output */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//enable gpio a clk
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     //?? 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   //??
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //??
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //??  
	GPIO_Init(GPIOC, &GPIO_InitStructure);  //??GPIO_InitStructure??????????
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);//���ʹ�ch378 power
}
void ch378_power_on(void)
{
	GPIO_SetBits(GPIOI,GPIO_Pin_10);//���߹ر�gl823 power
	for(long i=0;i<10000000;i++);
	GPIO_ResetBits(GPIOC,GPIO_Pin_13);//���ʹ�ch378 power
}
void ch378_power_off(void)
{
	GPIO_SetBits(GPIOC,GPIO_Pin_13);//���߹ر�ch378 power
}
static void init_spi(void)
{
	RCC ->AHB1ENR |= (1<<0);    //ʹ��PA
	RCC ->APB2ENR |= (1<<12);   //ʹ��SPI1
	GPIO_Set(GPIOA,1<<4 ,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //CS
	SPI1_CS = 1;
	GPIO_Set(GPIOA,1<<3 ,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //INT
	GPIO_Set(GPIOA,0x7<<5 ,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);
	GPIO_AF_Set(GPIOA,5,5);
	GPIO_AF_Set(GPIOA,6,5);
	GPIO_AF_Set(GPIOA,7,5);
	//SPI��ʼ��
	SPI1 ->CR1 = 0X0000;
	SPI1 ->CR1 |= (1<<9);   //���nss����
	SPI1 ->CR1 |= (1<<8);
	SPI1 ->CR1 |= (1<<3);   //�ķ�Ƶ 24M
	SPI1 ->CR1 |= (1<<2);   //��ģʽ
//	SPI1 ->CR1 |= (1<<1);   //CPOL
//	SPI1 ->CR1 |= (1<<0);   //CPHA
	SPI1 ->I2SCFGR &= ~(1<<11); //ѡ��SPIģʽ
	SPI1 ->CR1 |= (1<<6);   //����SPI
}
//spi�շ�һ���ֽ�
u8 spi_trans_byte(u8 dat)
{
	while(!(SPI1->SR & 0x02));   //����Ϊ��
	SPI1 ->DR = dat;
	while(!(SPI1->SR & 0x01));   //���շǿ�
	return (SPI1 ->DR );
}

void CH37x_WR_CMD_PORT( unsigned char cmd ) { 				 /* ��CH375������˿�д������,���ڲ�С��2uS,�����Ƭ���Ͽ�����ʱ */
	u8 temp=20;                 
	SPI1_CS = 0;
	spi_trans_byte(cmd);
	while(temp--)
		__nop();
}
void CH37x_WR_DAT_PORT( unsigned char dat ) { 				 /* ��CH375�����ݶ˿�д������,���ڲ�С��1uS,�����Ƭ���Ͽ�����ʱ */
	u8 temp=1;   //SD������temp=1�� U���ֽ�temp=1������5
	spi_trans_byte(dat);
	while(temp--)
		__nop();
}
unsigned char CH37x_RD_DAT_PORT(void) {  				     /* ��CH375�����ݶ˿ڶ�������,���ڲ�С��1uS,�����Ƭ���Ͽ�����ʱ */
	u8 temp=3;   //SD������temp=3�� U���ֽ�temp=0������10
	while(temp--)
		__nop();		
	return( spi_trans_byte(0xff) );
}
void xEndCH37xCmd(void)
{
	SPI1_CS = 1;
}

/*******************************************************************************
* Function Name  : mInitCH378Host
* Description    : ��ʼ��CH378
* Input          : None
* Output         : None
* Return         : ���ز���״̬
*******************************************************************************/
u8 mInitCH37xHost( void ) 
{
	u8 i, res;

	/* ���CH378�����Ƿ����� */
	ch378_power_init();
	init_spi( );  										 /* �ӿ�Ӳ����ʼ�� */
	delay_ms(500);

	CH37x_WR_CMD_PORT( CMD11_CHECK_EXIST );  						 /* ���Ե�Ƭ����CH378֮���ͨѶ�ӿ� */
	CH37x_WR_DAT_PORT( 0x65 );
	res = CH37x_RD_DAT_PORT( );
	xEndCH37xCmd( );  											 /* ���ڷ�ʽ����Ҫ */
	
	if( res != 0x9A ) 
	{
		/* ͨѶ�ӿڲ�����,����ԭ����:�ӿ������쳣,�����豸Ӱ��(Ƭѡ��Ψһ),���ڲ�����,һֱ�ڸ�λ,���񲻹��� */
		return( res );  
	}

	/* ����CH378����ģʽ */
	CH37x_WR_CMD_PORT( CMD11_SET_USB_MODE );  						 /* �豸USB����ģʽ */
	CH37x_WR_DAT_PORT( 0x03 );									     /* ����SD�洢�豸 */
//	CH37x_WR_DAT_PORT( 0x07 );									     /* ����USB�洢�豸 */

	/* �ȴ�ģʽ�������,���ڲ���SD�������Ҫ10mS����ʱ��,���ڲ���USB�豸�����Ҫ35mS����ʱ�� */	
	/* ���ڲ���ģʽ���Զ�ζ�ȡ�����������ж� */
	for( i = 0; i < 10; i++ )
	{
		delay_ms( 10 );											 
		//rt_thread_delay(1);
		
		res = CH37x_RD_DAT_PORT( );
		if( res == CMD_RET_SUCCESS ) 
		{
			xEndCH37xCmd( );   									 /* ���ڷ�ʽ����Ҫ */
			return( ERR_SUCCESS );
		}
	}
	xEndCH37xCmd( );  											 /* ���ڷ�ʽ����Ҫ */
	return( ERR_USB_UNKNOWN );  						  	     /* ����ģʽ���� */
}

/*******************************************************************************
* Function Name  : Query378Interrupt
* Description    : ��ѯCH378�ж�(INT#�͵�ƽ)
* Input          : None
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
u8 Query37xInterrupt( void )
{
	if( CH37x_INT_WIRE == 0x00 )
	{
		return( TRUE );
	
	}
	return( FALSE );  
}

