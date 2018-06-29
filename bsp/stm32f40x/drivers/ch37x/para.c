
/* �ӿ��������� */
/* A0    --- PF0
   D0-D1 --- PD14-PD15
   D2-D3 --- PD0-PD1
   D4-D7 --- PE7-PE10
   CS    --- PG9(NE2) BANK1��NE1~NE4
   RD    --- PD4(NOE)
   WR    --- PD5(NWE)
   INT   --- PA3
   
*/

#include "para.h"

void init_para(void)
{
	//��ʱ��
	RCC ->AHB1ENR |= (0xF<<3);                               //�����˿�ʹ�� D E F G
	RCC ->AHB1ENR |= (1<<0);                                 //ʹ�� PortA
	RCC ->AHB3ENR |= 1<<0;                                   //ʹ��FMC
	//����IO�븴��ӳ��
	GPIO_Set(GPIOF,1<<0 ,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //AO
	GPIO_Set(GPIOD,(3<<14)+(3<<0) ,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //D0-D3
	GPIO_Set(GPIOE,(0xF<<7) ,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //D4-D7
	GPIO_Set(GPIOD,(3<<4) ,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //RD WR
	GPIO_Set(GPIOG,(1<<9) ,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //CS
	GPIO_Set(GPIOA,1<<3 ,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);  //INT
	GPIO_AF_Set(GPIOF,0,12);
	GPIO_AF_Set(GPIOD,14,12);
	GPIO_AF_Set(GPIOD,15,12);
	GPIO_AF_Set(GPIOD,0,12);
	GPIO_AF_Set(GPIOD,1,12);
	GPIO_AF_Set(GPIOE,7,12);
	GPIO_AF_Set(GPIOE,8,12);
	GPIO_AF_Set(GPIOE,9,12);
	GPIO_AF_Set(GPIOE,10,12);	
	GPIO_AF_Set(GPIOD,4,12);
	GPIO_AF_Set(GPIOD,5,12);
	GPIO_AF_Set(GPIOG,9,12);
	//����FMC
	FMC_Bank1->BTCR[2] = 0x00000000;
	FMC_Bank1->BTCR[3] = 0x00000000;
	FMC_Bank1E->BWTR[2] = 0x00000000;
	FMC_Bank1->BTCR[2]|=1<<14;		//��дʹ�ò�ͬ��ʱ��
	FMC_Bank1->BTCR[2]|=1<<12;		//�洢��дʹ��
	
	FMC_Bank1->BTCR[3]|=(7<<8);    //�� ���ݽ���ʱ�䣨35ns��
	FMC_Bank1->BTCR[3]|=(1<<0);    //�� ��ַ����ʱ�䣨5ns��
	
	FMC_Bank1E->BWTR[2]|=6<<8;   	//д���ݱ���ʱ��(DATAST)	
	FMC_Bank1E->BWTR[2]|=1<<0;		//д��ַ����ʱ��(ADDSET)

	FMC_Bank1->BTCR[2]|=1<<0;		//ʹ��BANK1������2	  
}

void CH37x_WR_CMD_PORT( unsigned char cmd ) { 				 /* ��CH375������˿�д������,���ڲ�С��2uS,�����Ƭ���Ͽ�����ʱ */
	u8 temp=60;
	CH37x_CMD_PORT=cmd;
	while(temp--)
		__nop();
}
void CH37x_WR_DAT_PORT( unsigned char dat ) { 				 /* ��CH375�����ݶ˿�д������,���ڲ�С��1uS,�����Ƭ���Ͽ�����ʱ */
	u8 temp=4;   //SD������temp=4��U�̶�temp=15
	CH37x_DAT_PORT=dat;
	while(temp--)
		__nop();
}
unsigned char CH37x_RD_DAT_PORT(void) {  				     /* ��CH375�����ݶ˿ڶ�������,���ڲ�С��1uS,�����Ƭ���Ͽ�����ʱ */
	u8 temp=3;   //SD������temp=3��U�̶�temp=15
	while(temp--)
		__nop();		
	return( CH37x_DAT_PORT );
}
unsigned char xReadCH37xStatus() {  				         /* ��CH375�����ݶ˿ڶ�������,���ڲ�С��1uS,�����Ƭ���Ͽ�����ʱ */
	u8 temp=15;  
	while(temp--)
		__nop();										  
	return( CH37x_CMD_PORT );
}
	
void xEndCH37xCmd(void)
{
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
	init_para( );  										 /* �ӿ�Ӳ����ʼ�� */
	delay_ms(50);
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
//	CH37x_WR_DAT_PORT( 0x06 );									     /* ����USB�洢�豸 */

	/* �ȴ�ģʽ�������,���ڲ���SD�������Ҫ10mS����ʱ��,���ڲ���USB�豸�����Ҫ35mS����ʱ�� */	
	/* ���ڲ���ģʽ���Զ�ζ�ȡ�����������ж� */
	for( i = 0; i < 10; i++ )
	{
		delay_ms( 10 );											 
		res = CH37x_RD_DAT_PORT( );
		if( res == CMD_RET_SUCCESS ) 
		{
			xEndCH37xCmd( );  									 /* ���ڷ�ʽ����Ҫ */
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
	/* ���������CH378���ж�������ֱ�Ӳ�ѯ�ж����� */
	/* ���δ����CH378���ж��������ѯ״̬�˿� */
#ifdef	CH37x_INT_WIRE

	if( CH37x_INT_WIRE == 0x00 )
	{

		return( TRUE );
	
	}
	return( FALSE );  

#else
	return( xReadCH37xStatus( ) & PARA_STATE_INTB ? FALSE : TRUE );  
#endif
}
