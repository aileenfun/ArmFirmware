/********************************** (C) COPYRIGHT *******************************
* File Name          : FILE_SYS.C
* Author             : MJX
* Version            : V1.20
* Date               : 2015/11/25
* Description        : CH378оƬ �ļ�ϵͳ�� V1.2
*                      �ṩ�ļ�ϵͳ�����ӳ���,�ṩ������
*******************************************************************************/



/*******************************************************************************/
/* ��˵��: 
   (1)����ʹ�õ��ӳ������ע�͵�,�Ӷ���Լ��Ƭ���ĳ���ROM�ռ������RAM�ռ�;
   (2)��������ӳ�����ͨ�������еı������ݲ���,��������϶�,Ϊ�˽�ԼRAM,
        Ҳ���Բο�CH375�ӳ����ĳ�ͨ��ͬһȫ�ֱ���/���ϽṹCH378_CMD_DATA����;
   (3)��name ������ָ���ļ���, ���԰�����Ŀ¼��, ��������·���ָ���, �ܳ��Ȳ�����1+8+1+3+1�ֽ�;
   (4)��PathName ������ָȫ·���Ķ��ļ���, ������Ŀ¼�����༶��Ŀ¼��·���ָ������ļ���/Ŀ¼��;	
   (5)��LongName ������ָ���ļ���, ��UNICODEС��˳�����, ������0�ֽڽ���, ʹ�ó��ļ����ӳ��� */ 

/*******************************************************************************/
/* ���ֺ궨��˵��: 
   (1)������ NO_DEFAULT_CH378_INT ���ڽ�ֹĬ�ϵ�Wait378Interrupt�ӳ���,��ֹ��,Ӧ�ó������
        ���ж���һ��ͬ���ӳ���;
   (2)������ DEF_INT_TIMEOUT ��������Ĭ�ϵ�Wait378Interrupt�ӳ����еĵȴ��жϵĳ�ʱʱ��/ѭ��
        ����ֵ, 0�򲻼�鳬ʱ��һֱ�ȴ�;
   (3)������ EN_DISK_QUERY �����ṩ����������ѯ��ʣ��ռ��ѯ���ӳ���,Ĭ���ǲ��ṩ;
   (4)������ EN_DIR_CREATE �����ṩ�½��༶��Ŀ¼���ӳ���,Ĭ���ǲ��ṩ;
   (5)������ EN_SECTOR_ACCESS �����ṩ������Ϊ��λ��д�ļ����ӳ���,Ĭ���ǲ��ṩ;
   (6)������ EN_LONG_NAME �����ṩ֧�ֳ��ļ������ӳ���,Ĭ���ǲ��ṩ;
   (7)������ EN_OTHER_FUNCTION �����ṩ�����ǳ����ӳ���,Ĭ���ǲ��ṩ; */

/*******************************************************************************/
/* ͷ�ļ����� */
#include "ch378_file_sys.h"
#include "delay.h"
#include <string.h>


/*******************************************************************************/
/* ������������ */
UINT16 SectorSize = 512;										 /* ��ǰU��ÿ��������С(Ĭ��Ϊ512�ֽ�) */

/*******************************************************************************/
/* ����Ϊ�������������� */
/*******************************************************************************/

/*******************************************************************************
* Function Name  : CH378Read32bitDat
* Description    : ��CH378оƬ��ȡ32λ�����ݲ���������
* Input          : None
* Output         : None
* Return         : ����32λ����
*******************************************************************************/
UINT32 CH378Read32bitDat( void )
{
	UINT8  c0, c1, c2, c3;

	c0 = CH37x_RD_DAT_PORT( );
	c1 = CH37x_RD_DAT_PORT( );
	c2 = CH37x_RD_DAT_PORT( );
	c3 = CH37x_RD_DAT_PORT( );
	xEndCH37xCmd( );
	return( c0 | (UINT16)c1 << 8 | (UINT32)c2 << 16 | (UINT32)c3 << 24 );
}

/*******************************************************************************
* Function Name  : CH378ReadVar8
* Description    : ��CH378оƬ�ڲ���8λ����
* Input          : addr---8λ������ַ
* Output         : None
* Return         : ����8λ����
*******************************************************************************/
UINT8 CH378ReadVar8( UINT8 addr ) 
{
	UINT8 dat;

	CH37x_WR_CMD_PORT( CMD11_READ_VAR8 );
	CH37x_WR_DAT_PORT( addr );
	dat = CH37x_RD_DAT_PORT( );
	xEndCH37xCmd( );
	return( dat );
}

/*******************************************************************************
* Function Name  : CH378WriteVar8
* Description    : дCH378оƬ�ڲ���8λ����
* Input          : addr---8λ������ַ
*                  dat----8λ����ֵ 
* Output         : None
* Return         : None
*******************************************************************************/
void CH378WriteVar8( UINT8 addr, UINT8 dat ) 
{
	CH37x_WR_CMD_PORT( CMD20_WRITE_VAR8 );
	CH37x_WR_DAT_PORT( addr );
	CH37x_WR_DAT_PORT( dat );
	xEndCH37xCmd( );
}

/*******************************************************************************
* Function Name  : CH378ReadVar32
* Description    : ��CH378оƬ�ڲ���32λ����
* Input          : addr---32λ������ַ
* Output         : None
* Return         : ����32λ����
*******************************************************************************/
UINT32 CH378ReadVar32( UINT8 addr )
{
	CH37x_WR_CMD_PORT( CMD14_READ_VAR32 );
	CH37x_WR_DAT_PORT( addr );
	return( CH378Read32bitDat( ) );  							 /* ��CH378оƬ��ȡ32λ�����ݲ��������� */
}

/*******************************************************************************
* Function Name  : CH378WriteVar32
* Description    : дCH378оƬ�ڲ���32λ����
* Input          : addr---32λ������ַ
*                  dat---32λ����ֵ
* Output         : None
* Return         : None
*******************************************************************************/
void CH378WriteVar32( UINT8 addr, UINT32 dat ) 
{
	CH37x_WR_CMD_PORT( CMD50_WRITE_VAR32 );
	CH37x_WR_DAT_PORT( addr );
	CH37x_WR_DAT_PORT( (UINT8)dat );
	CH37x_WR_DAT_PORT( (UINT8)( (UINT16)dat >> 8 ) );
	CH37x_WR_DAT_PORT( (UINT8)( dat >> 16 ) );
	CH37x_WR_DAT_PORT( (UINT8)( dat >> 24 ) );
	xEndCH37xCmd( );
}

/*******************************************************************************
* Function Name  : CH378GetTrueLen
* Description    : ���ٷ�����һ������ִ����Ϻ����󳤶�����Ӧ��ʵ�ʳ���
* Input          : None
* Output         : None
* Return         : ����32λ����
*******************************************************************************/
UINT32 CH378GetTrueLen( void )
{
	UINT8  c0, c1, c2, c3;
	
	CH37x_WR_CMD_PORT( CMD02_GET_REAL_LEN );

	/* ��CH378оƬ��ȡ32λ�����ݲ��������� */  
	c0 = CH37x_RD_DAT_PORT( );
	c1 = CH37x_RD_DAT_PORT( );
	c2 = CH37x_RD_DAT_PORT( );
	c3 = CH37x_RD_DAT_PORT( );
	xEndCH37xCmd( );
	return( c0 | (UINT16)c1 << 8 | (UINT32)c2 << 16 | (UINT32)c3 << 24 );	
}

/*******************************************************************************
* Function Name  : CH378SetFileName
* Description    : ���ý�Ҫ�������ļ��������ļ���(·����)
* Input          : PathName---Ҫ���õ��ļ�����·����
* Output         : None
* Return         : None
*******************************************************************************/
void CH378SetFileName( PUINT8 PathName ) 
{
	UINT8  i, c;

	if( PathName == NULL )										 /* ���ָ��Ϊ��,��ȥ���������ļ��� */
	{
		return;
	}	
	CH37x_WR_CMD_PORT( CMD10_SET_FILE_NAME );
	for( i = MAX_FILE_NAME_LEN; i != 0; --i ) 
	{
		c = *PathName;
		CH37x_WR_DAT_PORT( c );
		if( c == 0 ) 
		{
			break;
		}
		PathName ++;
	}
	xEndCH37xCmd( );
}

/*******************************************************************************
* Function Name  : CH378GetDiskStatus
* Description    : ��ȡ���̺��ļ�ϵͳ�Ĺ���״̬
* Input          : None
* Output         : None
* Return         : ���ظ�ֵ
*******************************************************************************/
UINT8 CH378GetDiskStatus( void )  
{
	return( CH378ReadVar8( VAR8_DISK_STATUS ) );
}

/*******************************************************************************
* Function Name  : CH378GetIntStatus
* Description    : ��ȡ�ж�״̬��ȡ���ж�����
* Input          : None
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378GetIntStatus( void ) 
{
	UINT8  s;

	CH37x_WR_CMD_PORT( CMD01_GET_STATUS );
	s = CH37x_RD_DAT_PORT( );
	xEndCH37xCmd( );
	return( s );
}

/*******************************************************************************
* Function Name  : Wait378Interrupt
* Description    : �ȴ�CH378�ж�(INT#�͵�ƽ)
* Input          : None
* Output         : None
* Return         : �����ж�״̬��, ��ʱ�򷵻�ERR_USB_UNKNOWN
*******************************************************************************/
#ifndef	NO_DEFAULT_CH378_INT

UINT8 Wait378Interrupt( void ) 
{
#ifdef DEF_INT_TIMEOUT

#if	DEF_INT_TIMEOUT < 1
	while( Query37xInterrupt( ) == FALSE );  					 /* һֱ���ж� */
	return( CH378GetIntStatus( ) );  							 /* ��⵽�ж� */
#else
	UINT32 i;

	for( i = 0; i < DEF_INT_TIMEOUT; i ++ ) 
	{  
		/* ������ֹ��ʱ */
		if( Query37xInterrupt( ) ) 
		{
			return( CH378GetIntStatus( ) );  					 /* ��⵽�ж� */
		}
		delay_us( 3 );
		/* �ڵȴ�CH378�жϵĹ�����,������Щ��Ҫ��ʱ������������� */
	}
	return( ERR_USB_UNKNOWN );  								 /* ��Ӧ�÷�������� */
#endif

#else
	UINT32 i;

	for( i = 0; i < 5000000; i ++ ) 
	{  
		/* ������ֹ��ʱ,Ĭ�ϵĳ�ʱʱ��,�뵥Ƭ����Ƶ�й� */
		if ( Query37xInterrupt( ) ) 
		{
			return( CH378GetIntStatus( ) );  					 /* ��⵽�ж� */
		}
		delay_us( 3 );
		 
		/* �ڵȴ�CH378�жϵĹ�����,������Щ��Ҫ��ʱ������������� */
	}
	return( ERR_USB_UNKNOWN );  								 /* ��Ӧ�÷�������� */
#endif
}
#endif

/*******************************************************************************
* Function Name  : CH378SendCmdWaitInt
* Description    : �����������,�ȴ��ж�
* Input          : mCmd---��Ҫִ�е�������
* Output         : None
* Return         : �����ж�״̬��
*******************************************************************************/
UINT8 CH378SendCmdWaitInt( UINT8 mCmd )  
{
	CH37x_WR_CMD_PORT( mCmd );
	xEndCH37xCmd( );
	return( Wait378Interrupt( ) );
}

/*******************************************************************************
* Function Name  : CH378SendCmdDatWaitInt
* Description    : �����������һ�ֽ����ݺ�,�ȴ��ж�
* Input          : mCmd---��Ҫִ�е�������
*                  mDat---1���ֽڲ���
* Output         : None
* Return         : �����ж�״̬��
*******************************************************************************/
UINT8 CH378SendCmdDatWaitInt( UINT8 mCmd, UINT8 mDat )
{
	CH37x_WR_CMD_PORT( mCmd );
	CH37x_WR_DAT_PORT( mDat );
	xEndCH37xCmd( );
	return( Wait378Interrupt( ) );
}

/*******************************************************************************
* Function Name  : CH378GetFileSize
* Description    : ��ȡ��ǰ�ļ�����
* Input          : None
* Output         : None
* Return         : ���ص�ǰ�ļ�����
*******************************************************************************/
UINT32 CH378GetFileSize( void ) 
{
	return( CH378ReadVar32( VAR32_FILE_SIZE ) );
}

/*******************************************************************************
* Function Name  : CH378SetFileSize
* Description    : ���õ�ǰ�ļ�����
*                  ע��: �ú����������������õ�ǰ�ļ�����,������ʱ�޸ĵ�ǰ���ļ�
*                        �ĳ���,�Ա����ĳЩ����,�����ȡ�ļ������һ������������
* Input          : filesize---�ļ�����
* Output         : None
* Return         : None
*******************************************************************************/
void CH378SetFileSize( UINT32 filesize ) 
{
	CH378WriteVar32( VAR32_FILE_SIZE, filesize );
}

/*******************************************************************************
* Function Name  : CH378DiskConnect
* Description    : ���U��/SD���Ƿ�����
* Input          : None
* Output         : None
* Return         : �����ж�״̬��
*******************************************************************************/
UINT8 CH378DiskConnect( void )  
{
	if( Query37xInterrupt( ) ) 
	{
		CH378GetIntStatus( );  									 /* ��⵽�ж� */
	}
	return( CH378SendCmdWaitInt( CMD0H_DISK_CONNECT ) );
}

/*******************************************************************************
* Function Name  : CH378DiskReady
* Description    : ��ʼ�����̲����Դ����Ƿ����
* Input          : None
* Output         : None
* Return         : �����ж�״̬��
*******************************************************************************/
UINT8 CH378DiskReady( void )  
{
	return( CH378SendCmdWaitInt( CMD0H_DISK_MOUNT ) );
}

/*******************************************************************************
* Function Name  : CH378ReadReqBlock
* Description    : �ӵ�ǰ�����˵�Ľ��ջ�������ȡ�������ݿ�
* Input          : buf---���ݻ�����	
* Output         : None
* Return         : ���س���
*******************************************************************************/
UINT16 CH378ReadReqBlock( PUINT8 buf ) 
{
	UINT16 len;
	UINT16 l;
	
	CH37x_WR_CMD_PORT( CMD00_RD_HOST_REQ_DATA );					 /* д��1���ֽ������� */		
	len = CH37x_RD_DAT_PORT( );
	len += ( ( UINT16 )CH37x_RD_DAT_PORT( ) << 8 );				 /* ��ȡ2���ֽ����ݳ��� */
	delay_us( 1 ); 												 /* ���ڸ���MCU,���ڴ˴��ʵ�������ʱ */		
	l = len;
	if( len )
	{
		do
		{		
			*buf = CH37x_RD_DAT_PORT( );	
			buf++;
		}while( --l );		
	}
	xEndCH37xCmd( );
	return( len );
}

/*******************************************************************************
* Function Name  : CH378ReadBlock
* Description    : �ӵ�ǰ�����˵�Ľ��ջ�������ȡ���ݿ�
* Input          : buf---���ݻ�����
*				   len---��ȡ����	
* Output         : None
* Return         : ���س���
*******************************************************************************/
UINT16 CH378ReadBlock( PUINT8 buf, UINT16 len ) 
{
	UINT16 l;
	
	CH37x_WR_CMD_PORT( CMD20_RD_HOST_CUR_DATA );					 /* д��1���ֽ������� */		
	CH37x_WR_DAT_PORT( ( UINT8 )len );							 
	CH37x_WR_DAT_PORT( len >> 8 );								 /* д��2���ֽ����ݳ��� */	
	delay_us( 1 ); 												 /* ���ڸ���MCU,���ڴ˴��ʵ�������ʱ */
	if( len )
	{
		l = len;
		do
		{
			*buf = CH37x_RD_DAT_PORT( );
			buf++;
		}while( --l );		
	}
	xEndCH37xCmd( );
	return( len );
}

/*******************************************************************************
* Function Name  : CH378ReadOfsBlock
* Description    : ��ȡ�ڲ�ָ��������ָ��ƫ�Ƶ�ַ���ݿ�(�ڲ�ָ���Զ�����)
* Input          : buf---���ݻ�����
*				   offset---ƫ�Ƶ�ַ	
*				   len---��ȡ����	
* Output         : None
* Return         : ���س���
*******************************************************************************/
UINT16 CH378ReadOfsBlock( PUINT8 buf, UINT16 offset, UINT16 len ) 
{
	UINT16 l;
	
	CH37x_WR_CMD_PORT( CMD40_RD_HOST_OFS_DATA );					 /* д��1���ֽ������� */
	CH37x_WR_DAT_PORT( ( UINT8 )offset );							 /* д��2���ֽ�ƫ�Ƶ�ַ */			
	CH37x_WR_DAT_PORT( offset >> 8 );							
	CH37x_WR_DAT_PORT( ( UINT8 )len );							 /* д��2���ֽ����ݳ��� */
	CH37x_WR_DAT_PORT( len >> 8 );
	delay_us( 1 ); 												 /* ���ڸ���MCU,���ڴ˴��ʵ�������ʱ */							
	if( len )
	{
		l = len;
		do
		{
			*buf = CH37x_RD_DAT_PORT( );
			buf++;
		}while( --l );		
	}
	xEndCH37xCmd( );
	return( len );
}

/*******************************************************************************
* Function Name  : CH378WriteBlock
* Description    : ���ڲ�ָ����������ǰƫ�Ƶ�ַд�����ݿ�(�ڲ�ָ���Զ�����)
* Input          : buf---���ݻ�����
*				   len---д�����ݳ���	
* Output         : None
* Return         : ���س���
*******************************************************************************/
UINT16 CH378WriteBlock( PUINT8 buf, UINT16 len ) 
{
	UINT16 l;
	
	CH37x_WR_CMD_PORT( CMD20_WR_HOST_CUR_DATA );					 /* д��1���ֽ������� */		
	CH37x_WR_DAT_PORT( ( UINT8 )len );							 /* д��2���ֽ����ݳ��� */
	CH37x_WR_DAT_PORT( len >> 8 );	
	delay_us( 1 ); 												 /* ���ڸ���MCU,���ڴ˴��ʵ�������ʱ */						
	if( len )
	{
		l = len;
		do
		{
			CH37x_WR_DAT_PORT( *buf );
			buf++;
		}while( --l );		
	}
	xEndCH37xCmd( );
	return( len );
}

/*******************************************************************************
* Function Name  : CH378WriteOfsBlock
* Description    : ���ڲ�ָ��������ָ��ƫ�Ƶ�ַд�����ݿ�(�ڲ�ָ���Զ�����)
* Input          : buf---���ݻ�����
*				   offset---ƫ�Ƶ�ַ
*				   len---д�����ݳ���	
* Output         : None
* Return         : ���س���
*******************************************************************************/
UINT16 CH378WriteOfsBlock( PUINT8 buf, UINT16 offset, UINT16 len ) 
{
	UINT16 l;
	
	CH37x_WR_CMD_PORT( CMD40_WR_HOST_OFS_DATA );					 /* д��1���ֽ������� */		
	CH37x_WR_DAT_PORT( ( UINT8 )offset );							 /* д��2���ֽ�ƫ�Ƶ�ַ */			
	CH37x_WR_DAT_PORT( offset >> 8 );							
	CH37x_WR_DAT_PORT( ( UINT8 )len );							 /* д��2���ֽ����ݳ��� */
	CH37x_WR_DAT_PORT( len >> 8 );
	delay_us( 1 ); 												 /* ���ڸ���MCU,���ڴ˴��ʵ�������ʱ */							
	if( len )
	{
		l = len;
		do
		{
			CH37x_WR_DAT_PORT( *buf );
			buf++;
		}while( --l );		
	}
	xEndCH37xCmd( );
	return( len );
}

/*******************************************************************************
* Function Name  : CH378FileOpen
* Description    : ���ļ�����Ŀ¼(�ļ���)
* Input          : PathName---Ҫ�򿪵��ļ���Ŀ¼������·����
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378FileOpen( PUINT8 PathName )  
{
	CH378SetFileName( PathName );  								 /* ���ý�Ҫ�������ļ����ļ��� */
	return( CH378SendCmdWaitInt( CMD0H_FILE_OPEN ) );	
}

/*******************************************************************************
* Function Name  : CH378FileCreate
* Description    : �½��ļ�����,����ļ��Ѿ�������ô��ɾ�������½�
* Input          : PathName---Ҫ�½��ļ���·�����ļ���
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378FileCreate( PUINT8 PathName ) 
{
	CH378SetFileName( PathName );  								 /* ���ý�Ҫ�������ļ����ļ��� */
	return( CH378SendCmdWaitInt( CMD0H_FILE_CREATE ) );
}

/*******************************************************************************
* Function Name  : CH378FileErase
* Description    : ɾ���ļ����ر�
*                  ɾ���ļ�,����Ѿ�����ֱ��ɾ��,��������ļ����ȴ���ɾ��,֧�ֶ༶Ŀ¼·�� 
* Input          : PathName---��ɾ���ļ���·�������ļ���
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378FileErase( PUINT8 PathName ) 
{
	CH378SetFileName( PathName );  
	return( CH378SendCmdWaitInt( CMD0H_FILE_ERASE ) );
}

/*******************************************************************************
* Function Name  : CH378FileClose
* Description    : �رյ�ǰ�Ѿ��򿪵��ļ�����Ŀ¼(�ļ���)
* Input          : UpdateSz---0---���Զ������ļ�����; 1---�Զ������ļ�����
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378FileClose( UINT8 UpdateSz ) 
{
	return( CH378SendCmdDatWaitInt( CMD1H_FILE_CLOSE, UpdateSz ) );
}

/*******************************************************************************
* Function Name  : CH378ByteLocate
* Description    : ���ֽ�Ϊ��λ�ƶ���ǰ�ļ�ָ��
* Input          : offset---ƫ���ֽ���
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378ByteLocate( UINT32 offset ) 
{
	CH37x_WR_CMD_PORT( CMD4H_BYTE_LOCATE );
	CH37x_WR_DAT_PORT( (UINT8)offset );
	CH37x_WR_DAT_PORT( (UINT8)( (UINT16)offset >> 8 ) );
	CH37x_WR_DAT_PORT( (UINT8)( offset >> 16 ) );
	CH37x_WR_DAT_PORT( (UINT8)( offset >> 24 ) );
	xEndCH37xCmd( );
	return( Wait378Interrupt( ) );
}

/*******************************************************************************
* Function Name  : CH378ByteRead
* Description    : ���ֽ�Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ�
* Input          : buf---����������
*				   ReqCount----��Ҫ��ȡ�����ݳ���
*				   RealCount---ʵ�ʶ�ȡ�����ݳ���	
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378ByteRead( PUINT8 buf, UINT16 ReqCount, PUINT16 RealCount ) 
{
	UINT8  s;
	UINT16 len;

	/* �����ֽڶ�ȡ���� */		
	CH37x_WR_CMD_PORT( CMD2H_BYTE_READ );							 /* һ���ֽ������� */
	CH37x_WR_DAT_PORT( (UINT8)ReqCount );
	CH37x_WR_DAT_PORT( (UINT8)( ReqCount >> 8 ) );				 /* �����ֽ����ݳ��� */
	xEndCH37xCmd( );
	if( RealCount ) 
	{
		*RealCount = 0;
	}
	s = Wait378Interrupt( );
	if( s == ERR_SUCCESS )
	{
		/* ��ȡʵ�ʳ��� */
		len = CH378GetTrueLen(  );							 	 /* ��ǰ����ִ�к�ʵ�ʷ��س��� */
		if( RealCount ) 
		{
			*RealCount = len;
		}
		
		/* ����ʵ�ʿɶ�ȡ���ȶ�ȡ���� */
		if( len )
		{	
			CH378ReadBlock( buf, len );		
		}				
	}
	return( s );	
}

/*******************************************************************************
* Function Name  : CH378ByteReadPrepare
* Description    : ���ֽ�Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ�׼��
*                  ע�⣺��CH378ByteRead��������ֻ���Ͷ�ȡ���ִ��,��δ����ȡ����,
*                        �ú���ִ����Ϻ�,ʵ��������Ȼ������CH378�ڲ�20K��������
* Input          : ReqCount----��Ҫ��ȡ�����ݳ���
*				   RealCount---ʵ�ʶ�ȡ�����ݳ���	
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378ByteReadPrepare( UINT16 ReqCount, PUINT16 RealCount ) 
{
	UINT8  s;
	UINT16 len;

	/* �����ֽڶ�ȡ���� */		
	CH37x_WR_CMD_PORT( CMD2H_BYTE_READ );							 /* һ���ֽ������� */
	CH37x_WR_DAT_PORT( (UINT8)ReqCount );
	CH37x_WR_DAT_PORT( (UINT8)( ReqCount >> 8 ) );				 /* �����ֽ����ݳ��� */
	xEndCH37xCmd( );
	if( RealCount ) 
	{
		*RealCount = 0;
	}
	s = Wait378Interrupt( );
	if( s == ERR_SUCCESS )
	{
		/* ��ȡʵ�ʳ��� */
		len = CH378GetTrueLen(  );							 	 /* ��ǰ����ִ�к�ʵ�ʷ��س��� */
		if( RealCount ) 
		{
			*RealCount = len;
		}
	}
	return( s );	
}

/*******************************************************************************
* Function Name  : CH378ByteWrite
* Description    : ���ֽ�Ϊ��λ��ǰλ��д�����ݿ�
* Input          : buf---����������
*				   ReqCount----��Ҫд������ݳ���
*				   RealCount---ʵ��д������ݳ���	
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378ByteWrite( PUINT8 buf, UINT16 ReqCount, PUINT16 RealCount )  
{
	UINT8  s;

	/* ��Ҫд�������Ԥ���͵��ڲ��������� */
	CH378WriteOfsBlock( buf, 0x0000, ReqCount );

	/* �����ֽ�д������ */		
	CH37x_WR_CMD_PORT( CMD2H_BYTE_WRITE );
	CH37x_WR_DAT_PORT( (UINT8)ReqCount );
	CH37x_WR_DAT_PORT( (UINT8)( ReqCount >> 8 ) );
	xEndCH37xCmd( );
	s = Wait378Interrupt( );
	if( s != ERR_SUCCESS )
	{
		if( RealCount ) 
		{
			*RealCount = 0;
		}		
	}
	else
	{
		if( RealCount ) 
		{
			*RealCount = ReqCount;
		}				
	}	
	return( s );
}

/*******************************************************************************
* Function Name  : CH378ByteWriteExecute
* Description    : ���ֽ�Ϊ��λ��ǰλ��д�����ݿ�
*                  ע�⣺��CH378ByteWrite��������,�ú���ִ��ǰ,�����Ѿ���ǰ��ŵ�CH378�ڲ�20K
*                        ָ����������
* Input          : ReqCount----��Ҫд������ݳ���
*				   RealCount---ʵ��д������ݳ���	
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378ByteWriteExecute( UINT16 ReqCount, PUINT16 RealCount )  
{
	UINT8  s;

	/* �����ֽ�д������ */		
	CH37x_WR_CMD_PORT( CMD2H_BYTE_WRITE );
	CH37x_WR_DAT_PORT( (UINT8)ReqCount );
	CH37x_WR_DAT_PORT( (UINT8)( ReqCount >> 8 ) );
	xEndCH37xCmd( );
	s = Wait378Interrupt( );
	if( s != ERR_SUCCESS )
	{
		if( RealCount ) 
		{
			*RealCount = 0;
		}		
	}
	else
	{
		if( RealCount ) 
		{
			*RealCount = ReqCount;
		}				
	}	
	return( s );
}

/*******************************************************************************
* Function Name  : CH378FileQuery
* Description    : CH378��ѯ��ǰ�ļ�����Ϣ(���ȡ����ڡ�ʱ�䡢����)
*                  ����9���ֽ����ݣ��ֱ�Ϊ4���ֽڳ��ȡ�2���ֽ����ڡ�2���ֽ�ʱ�䡢
*                  ���ֽ�����1 
* Input          : buf---���ݻ�����
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CH378FileQuery( PUINT8 buf ) 
{
	UINT8  status;
	
	status = CH378SendCmdWaitInt( CMD0H_FILE_QUERY );
	if( status != ERR_SUCCESS )
	{
		return( status );
	}

	CH378ReadReqBlock( buf );
	return( status );
}	

/*******************************************************************************
* Function Name  : CH378FileModify
* Description    : CH378�޸ĵ�ǰ�ļ�����Ϣ(���ȡ����ڡ�ʱ�䡢����)
* Input          : filesize----�µ��ļ�����,Ϊ0xFFFFFFFF���޸�
*				   filedate----�µ��ļ�����,Ϊ0xFFFF���޸�
*				   filetime----�µ��ļ�ʱ��,Ϊ0xFFFF���޸�	
*                  fileattr----�µ��ļ�����,Ϊ0xFF���޸�
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CH378FileModify( UINT32 filesize, UINT16 filedate, UINT16 filetime, UINT8 fileattr ) 
{	
	CH37x_WR_CMD_PORT( CMD40_WR_HOST_OFS_DATA );					 /* д��1���ֽ������� */		
	CH37x_WR_DAT_PORT( 0x00 );							 		 /* д��2���ֽ�ƫ�Ƶ�ַ */			
	CH37x_WR_DAT_PORT( 0x00 );							
	CH37x_WR_DAT_PORT( 0x09 );							 		 /* д��2���ֽ����ݳ��� */
	CH37x_WR_DAT_PORT( 0x00 );								
	CH37x_WR_DAT_PORT( (UINT8)filesize );  						 /* д��4���ֽ��µ��ļ����� */					 
	CH37x_WR_DAT_PORT( (UINT8)( filesize >> 8 ) );
	CH37x_WR_DAT_PORT( (UINT8)( filesize >> 16 ) );
	CH37x_WR_DAT_PORT( (UINT8)( filesize >> 24 ) ); 	 		 
	CH37x_WR_DAT_PORT( (UINT8)filedate );  						 /* д��2���ֽ��µ��ļ����� */					 
	CH37x_WR_DAT_PORT( (UINT8)( filedate >> 8 ) );
	CH37x_WR_DAT_PORT( (UINT8)filetime );  						 /* д��2���ֽ��µ��ļ�ʱ�� */					 
	CH37x_WR_DAT_PORT( (UINT8)( filetime >> 8 ) );
	CH37x_WR_DAT_PORT( (UINT8)fileattr );  						 /* д��1���ֽ��µ��ļ����� */					 
	xEndCH37xCmd( );	
	return( CH378SendCmdWaitInt( CMD0H_FILE_MODIFY ) );
}	

/*******************************************************************************
* Function Name  : CH378DirInfoRead
* Description    : ��ȡ��ǰ���ļ���ָ���������ĵ�Ŀ¼��Ϣ
* Input          : index---Ŀ¼��Ϣ�ṹ�������е�������
*                  �����ŷ�ΧΪ00H--0FH����ȡ���������ж�Ӧ�ĵ�ǰĿ¼��Ϣ�ṹ
*                  ������ΪFFH��         ��ȡ��ǰ�򿪵��ļ����ڵ�Ŀ¼��Ϣ�ṹ
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CH378DirInfoRead( UINT8 index ) 
{
	return( CH378SendCmdDatWaitInt( CMD1H_DIR_INFO_READ, index ) );
}

/*******************************************************************************
* Function Name  : CH378DirInfoSave
* Description    : ���浱ǰ���ļ���ָ���������ĵ�Ŀ¼��Ϣ
* Input          : index---Ŀ¼��Ϣ�ṹ�������е�������
*                  �����ŷ�ΧΪ00H--0FH��������������ж�Ӧ�ĵ�ǰĿ¼��Ϣ�ṹ
*                  ������ΪFFH��         ���浱ǰ�򿪵��ļ����ڵ�Ŀ¼��Ϣ�ṹ
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CH378DirInfoSave( UINT8 index )  
{
	return( CH378SendCmdDatWaitInt( CMD1H_DIR_INFO_SAVE, index ) );
}

/**********************************************************************************************************/
/* ����Ϊ���������������������,����ͨ����EN_SECTOR_ACCESS�رջ��� */
/**********************************************************************************************************/

#ifdef	EN_SECTOR_ACCESS

/*******************************************************************************
* Function Name  : CH378SecLocate
* Description    : ������Ϊ��λ�ƶ���ǰ�ļ�ָ��
* Input          : offset---ƫ��������
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378SecLocate( UINT32 offset )
{
	CH37x_WR_CMD_PORT( CMD4H_SEC_LOCATE );
	CH37x_WR_DAT_PORT( (UINT8)offset );
	CH37x_WR_DAT_PORT( (UINT8)((UINT16)offset >> 8 ) );
	CH37x_WR_DAT_PORT( (UINT8)( offset >> 16 ) );
	CH37x_WR_DAT_PORT( 0 );  										 /* ��������ļ��ߴ� */
	xEndCH37xCmd( );
	return( Wait378Interrupt( ) );
}

/*******************************************************************************
* Function Name  : CH378SecRead
* Description    : ������Ϊ��λ�ӵ�ǰ�ļ�λ�ö�ȡ���ݿ�
* Input          : buf---���ݻ�����
*                  ReqCount------׼����ȡ��������
*                  RealCount-----ʵ�ʶ�ȡ��������
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378SecRead( PUINT8 buf, UINT8 ReqCount, PUINT8 RealCount ) 
{
	UINT8  s, count;
	UINT8  maxnum;
	UINT16 len, l;

	s = 0;
	if( RealCount ) 
	{
		*RealCount = 0;
	}	
	maxnum = MAX_BYTE_PER_OPERATE / SectorSize; 				 /* ���㵱�ο��Զ�ȡ�����������Ŀ */
	
	/* ѭ����ȡ�������� */
	while( ReqCount )
	{
		/* ���㵥�ζ�ȡ�������� */
		if( ReqCount >= maxnum )
		{
			count = maxnum;
			ReqCount = ReqCount - count;			
		}
		else
		{
			count = ReqCount;
			ReqCount = 0;			
		}

		/* ���Ͷ�ȡ�������� */		
		CH37x_WR_CMD_PORT( CMD1H_SEC_READ );
		CH37x_WR_DAT_PORT( count );
		xEndCH37xCmd( );
		s = Wait378Interrupt( );  								 /* �ȴ��жϲ���ȡ״̬ */
		if( s != ERR_SUCCESS )
		{
			return( s );
		}					

		/* ��ȡʵ�ʿɶ�ȡ���ֽ��� */
		count = CH378GetTrueLen(  );							 /* ��ǰ����ִ�к�ʵ�ʷ��س��� */			
		len = count * SectorSize;	

		/* ��ȡ�������ݵ�ָ�������� */
//		CH378ReadBlock( buf, len );  							 /* ���������滻������ٶ� */		
		CH37x_WR_CMD_PORT( CMD20_RD_HOST_CUR_DATA );				 /* д��1���ֽ������� */		
		CH37x_WR_DAT_PORT( ( UINT8 )len );							 
		CH37x_WR_DAT_PORT( len >> 8 );							 /* д��2���ֽ����ݳ��� */	
		delay_us( 1 ); 											 /* ���ڸ���MCU,���ڴ˴��ʵ�������ʱ */	
		
		/* ���Ż����²��ִ�������߶��ٶ� */
		l = len;
		while( l )
		{
			*buf++ = CH37x_RD_DAT_PORT( );			   					
			*buf++ = CH37x_RD_DAT_PORT( );
			*buf++ = CH37x_RD_DAT_PORT( );
			*buf++ = CH37x_RD_DAT_PORT( );
			*buf++ = CH37x_RD_DAT_PORT( );
			*buf++ = CH37x_RD_DAT_PORT( );
			*buf++ = CH37x_RD_DAT_PORT( );
			*buf++ = CH37x_RD_DAT_PORT( );
			*buf++ = CH37x_RD_DAT_PORT( );
			*buf++ = CH37x_RD_DAT_PORT( );
			*buf++ = CH37x_RD_DAT_PORT( );
			*buf++ = CH37x_RD_DAT_PORT( );
			*buf++ = CH37x_RD_DAT_PORT( );
			*buf++ = CH37x_RD_DAT_PORT( );
			*buf++ = CH37x_RD_DAT_PORT( );
			*buf++ = CH37x_RD_DAT_PORT( );
			l = l - 16;
		}
		xEndCH37xCmd( );
		buf	= buf + len;						
		if( RealCount ) 
		{
			*RealCount += count;
		}		
		if( count < maxnum )									 /* ��ȡ���ļ�ĩβ,��ǰ���� */		
		{
			return( s );
		}				
	}
	return( s );
}

/*******************************************************************************
* Function Name  : CH378SecWrite
* Description    : ������Ϊ��λ�ڵ�ǰλ��д�����ݿ�
*                  ע��: ��д���������Ϊ0,��CH378�ڲ����Զ������ļ����� 
* Input          : buf---���ݻ�����
*                  ReqCount------׼��д���������
*                  RealCount-----ʵ��д���������
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378SecWrite( PUINT8 buf, UINT8 ReqCount, PUINT8 RealCount ) 
{
	UINT8  s, count;
	UINT8  maxnum;
	UINT16 len, l;

	if( RealCount ) 
	{
		*RealCount = 0;
	}	
	maxnum = MAX_BYTE_PER_OPERATE / SectorSize; 				 /* ���㵱�ο��Զ�ȡ�����������Ŀ */
	
	/* ѭ��д���������� */
	do
	{
		/* ���㵥��д��������� */
		if( ReqCount >= maxnum )
		{
			count = maxnum;
			ReqCount = ReqCount - count;			
		}
		else
		{
			count = ReqCount;
			ReqCount = 0;			
		}

		/* ��Ҫд�������Ԥ���͵��ڲ��������� */
		len = count * SectorSize;

		/* ���Ż��˲��ִ�������߶��ٶ� */
//		CH378WriteOfsBlock( buf, 0x00, len );						 /* ����������滻������ٶ� */
		CH37x_WR_CMD_PORT( CMD40_WR_HOST_OFS_DATA );					 /* д��1���ֽ������� */		
		CH37x_WR_DAT_PORT( 0x00 );									 /* д��2���ֽ�ƫ�Ƶ�ַ */			
		CH37x_WR_DAT_PORT( 0x00 );							
		CH37x_WR_DAT_PORT( ( UINT8 )len );							 /* д��2���ֽ����ݳ��� */
		CH37x_WR_DAT_PORT( len >> 8 );							
		l = len;
		while( l )
		{
			CH37x_WR_DAT_PORT( *buf++ );
			CH37x_WR_DAT_PORT( *buf++ );
			CH37x_WR_DAT_PORT( *buf++ );
			CH37x_WR_DAT_PORT( *buf++ );
			CH37x_WR_DAT_PORT( *buf++ );
			CH37x_WR_DAT_PORT( *buf++ );
			CH37x_WR_DAT_PORT( *buf++ );
			CH37x_WR_DAT_PORT( *buf++ );
			CH37x_WR_DAT_PORT( *buf++ );
			CH37x_WR_DAT_PORT( *buf++ );
			CH37x_WR_DAT_PORT( *buf++ );
			CH37x_WR_DAT_PORT( *buf++ );
			CH37x_WR_DAT_PORT( *buf++ );
			CH37x_WR_DAT_PORT( *buf++ );
			CH37x_WR_DAT_PORT( *buf++ );
			CH37x_WR_DAT_PORT( *buf++ );
			l = l - 16;
		}
		xEndCH37xCmd( );
		buf	= buf + len;

		/* ����д�������� */
		CH37x_WR_CMD_PORT( CMD1H_SEC_WRITE );
		CH37x_WR_DAT_PORT( count );
		xEndCH37xCmd( );
		s = Wait378Interrupt( );  								 /* �ȴ��жϲ���ȡ״̬ */
		if( s != ERR_SUCCESS )
		{
			return( s );
		}
		if( RealCount ) 
		{
			*RealCount += count;
		}		
	}while( ReqCount );
	return( s );
}

/*******************************************************************************
* Function Name  : CH378DiskReadSec
* Description    : ��U�̶�ȡ������������ݿ鵽������
* Input          : buf---���ݻ�����
*                  iLbaStart------׼����ȡ��������ʼ������
*                  iSectorCount---׼����ȡ��������
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378DiskReadSec( PUINT8 buf, UINT32 iLbaStart, UINT8 iSectorCount ) 
{	
	UINT8  s, count;
	UINT8  maxnum;
	UINT16 len;
	UINT32 seclba;	

	s = 0;
	seclba = iLbaStart;
	maxnum = MAX_BYTE_PER_OPERATE / SectorSize; 				 /* ���㵱�ο��Զ�ȡ�����������Ŀ */

	/* ѭ����ȡ�������� */
	while( iSectorCount )
	{
		/* ���㵥�ζ�ȡ�������� */
		if( iSectorCount >= maxnum )
		{
			count = maxnum;
			iSectorCount = iSectorCount - count;			
		}
		else
		{
			count = iSectorCount;
			iSectorCount = 0;			
		}
	
		/* ���Ͷ�ȡ�������� */		
		CH37x_WR_CMD_PORT( CMD5H_DISK_READ );  					 /* ��USB�洢�������� */
		CH37x_WR_DAT_PORT( (UINT8)seclba );  						 /* LBA�����8λ */
		CH37x_WR_DAT_PORT( (UINT8)( (UINT16)seclba >> 8 ) );
		CH37x_WR_DAT_PORT( (UINT8)( seclba >> 16 ) );
		CH37x_WR_DAT_PORT( (UINT8)( seclba >> 24 ) ); 	 		 /* LBA�����8λ */
		CH37x_WR_DAT_PORT( count );  							 	 /* ������ */
		xEndCH37xCmd( );
		s = Wait378Interrupt( );  								 /* �ȴ��жϲ���ȡ״̬ */
		if( s != ERR_SUCCESS )
		{
			return( s );
		}					
		
		/* ��ȡ�������ݵ�ָ�������� */
		len = count * SectorSize;
		CH378ReadBlock( buf, len );
		buf	= buf + len;		
		seclba = seclba + count;	
	}
	return( s );
}

/*******************************************************************************
* Function Name  : CH378DiskWriteSec
* Description    : ���������еĶ�����������ݿ�д��U��
* Input          : buf---���ݻ�����
*                  iLbaStart------д�������ʼ��������
*                  iSectorCount---׼��д���������
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378DiskWriteSec( PUINT8 buf, UINT32 iLbaStart, UINT8 iSectorCount )  
{	
	UINT8  s, count;
	UINT8  maxnum;
	UINT16 len;
	UINT32 seclba;
	
	s = 0;
	seclba = iLbaStart;
	maxnum = MAX_BYTE_PER_OPERATE / SectorSize; 				 /* ���㵱�ο��Զ�ȡ�����������Ŀ */

	/* ѭ��д���������� */
	while( iSectorCount )
	{
		/* ���㵥��д��������� */
		if( iSectorCount >= maxnum )
		{
			count = maxnum;
			iSectorCount = iSectorCount - count;			
		}
		else
		{
			count = iSectorCount;
			iSectorCount = 0;			
		}	

		/* ��Ҫд�������Ԥ���͵��ڲ��������� */
		len = count * SectorSize;
		CH378WriteOfsBlock( buf, 0x00, len );
		buf	= buf + len;
				
		/* ����д�������� */
		CH37x_WR_CMD_PORT( CMD5H_DISK_WRITE );  					 /* ��USB�洢��д���� */
		CH37x_WR_DAT_PORT( (UINT8)seclba );  						 /* LBA�����8λ */
		CH37x_WR_DAT_PORT( (UINT8)( (UINT16)seclba >> 8 ) );
		CH37x_WR_DAT_PORT( (UINT8)( seclba >> 16 ) );
		CH37x_WR_DAT_PORT( (UINT8)( seclba >> 24 ) );  			 /* LBA�����8λ */
		CH37x_WR_DAT_PORT( count );  								 /* ������ */
		xEndCH37xCmd( );		
		s = Wait378Interrupt( );  								 /* �ȴ��жϲ���ȡ״̬ */
		if( s != ERR_SUCCESS )
		{
			return( s );
		}
		seclba = seclba + count;		
	}
	return( s );
}

#endif

/**********************************************************************************************************/
/* ����Ϊ���̲�ѯ�����������������,����ͨ����EN_DISK_QUERY�رջ��� */
/**********************************************************************************************************/

#ifdef	EN_DISK_QUERY

/*******************************************************************************
* Function Name  : CH378DiskCapacity
* Description    : ��ѯ������������(������)
* Input          : DiskCap---������
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378DiskCapacity( PUINT32 DiskCap )  
{
	UINT8  s;
	
	s = CH378SendCmdWaitInt( CMD0H_DISK_CAPACITY );
	if ( s == ERR_SUCCESS ) 
	{  
		CH37x_WR_CMD_PORT( CMD00_RD_HOST_REQ_DATA );				 /* д��1���ֽ������� */		
		CH37x_RD_DAT_PORT( );							
		CH37x_RD_DAT_PORT( );						 	
		*DiskCap = CH378Read32bitDat( );  						 /* ��CH378оƬ��ȡ32λ�����ݲ��������� */
	}
	else 
	{
		*DiskCap = 0;
	}
	return( s );
}

/*******************************************************************************
* Function Name  : CH378DiskQuery
* Description    : ��ѯ����ʣ��ռ���Ϣ(������)
* Input          : DiskFre---������
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378DiskQuery( PUINT32 DiskFre, PUINT32 DiskCap )  
{
	UINT8  s;
	UINT8  c0, c1, c2, c3;
	UINT8  c4, c5, c6, c7;

	s = CH378SendCmdWaitInt( CMD0H_DISK_QUERY );
	if( s == ERR_SUCCESS ) 
	{  		
		CH37x_WR_CMD_PORT( CMD00_RD_HOST_REQ_DATA );				 /* д��1���ֽ������� */		
		CH37x_RD_DAT_PORT( );							
		CH37x_RD_DAT_PORT( );						 		 	     /* ��ȡ2���ֽ����ݳ��� */		
				
		c4 = CH37x_RD_DAT_PORT( );  									 /* CH378_CMD_DATA.DiskQuery.mTotalSector */
		c5 = CH37x_RD_DAT_PORT( );
		c6 = CH37x_RD_DAT_PORT( );
		c7 = CH37x_RD_DAT_PORT( );

		*DiskCap = c4 | (UINT16)c5 << 8 | (UINT32)c6 << 16 | (UINT32)c7 << 24;
		
		c0 = CH37x_RD_DAT_PORT( );  								 /* CH378_CMD_DATA.DiskQuery.mFreeSector */
		c1 = CH37x_RD_DAT_PORT( );
		c2 = CH37x_RD_DAT_PORT( );
		c3 = CH37x_RD_DAT_PORT( );
		*DiskFre = c0 | (UINT16)c1 << 8 | (UINT32)c2 << 16 | (UINT32)c3 << 24;
		CH37x_RD_DAT_PORT( );  									 /* CH378_CMD_DATA.DiskQuery.mDiskFat */
		xEndCH37xCmd( );
	}
	else 
	{
		*DiskFre = 0;
	}
	return( s );
}

#endif

/**********************************************************************************************************/
/* ����ΪĿ¼�����������������,����ͨ����EN_DIR_CREATE�رջ��� */
/**********************************************************************************************************/

#ifdef	EN_DIR_CREATE

/*******************************************************************************
* Function Name  : CH378DirCreate
* Description    : �ڸ�Ŀ¼���½�Ŀ¼(�ļ���)����,���Ŀ¼�Ѿ�������ôֱ�Ӵ�
* Input          : PathName---Ҫ�������ļ��е�·�����ļ�����
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378DirCreate( PUINT8 PathName ) 
{
	CH378SetFileName( PathName );  								 /* ���ý�Ҫ�������ļ��е��ļ����� */
	return( CH378SendCmdWaitInt( CMD0H_DIR_CREATE ) );
}
#endif

/**********************************************************************************************************/
/* ����Ϊ���ļ��������������������,����ͨ����EN_LONG_NAME�رջ��� */
/**********************************************************************************************************/

#ifdef	EN_LONG_NAME

/*******************************************************************************
* Function Name  : CH378GetLongName
* Description    : �ɶ��ļ�������Ŀ¼(�ļ���)�������Ӧ�ĳ��ļ���
*                  ��Ҫ������ļ���������·��PathName
* Input          : PathName---���ļ���������·��
* Output         : LongName---���صĳ��ļ���,��UNICODEС�˱���,��˫0����
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378GetLongName( PUINT8 PathName, PUINT8 LongName )
{
	UINT8  status;	
	
	CH378SetFileName( PathName );  								 /* ���ý�Ҫ�������ļ�������·���� */
	status = CH378SendCmdWaitInt( CMD10_GET_LONG_FILE_NAME );
	if( status == ERR_SUCCESS )
	{
		/* ��������������ݷ���,���ȡ������ */
		CH378ReadReqBlock( LongName );
	}
	return( status );		
}

/*******************************************************************************
* Function Name  : CH378GetShortName
* Description    : �ɶ��ļ�������Ŀ¼(�ļ���)����·�����������ļ��������Ӧ�Ķ��ļ���
* Input          : LongName---�����ĳ��ļ���,��UNICODEС�˱���,��˫0����
* Output         : PathName---���ļ���������·��
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378GetShortName( PUINT8 PathName, PUINT8 LongName )
{
	UINT8  status;	
	UINT16	count;

	/* ���ý�Ҫ�����Ķ��ļ�������·���� */	
	CH378SetFileName( PathName );  								 /* ���ý�Ҫ�������ļ��Ĳ���·���� */

	/* ���ó��ļ��� */
	for( count = 0; count < LONG_NAME_BUF_LEN; count += 2 ) 
	{
		if ( *(PUINT16)( &LongName[ count ] ) == 0 ) 
		{
			break;  											 /* ������λ�� */
		}
	}
	if( ( count == 0 ) || ( count >= LONG_NAME_BUF_LEN ) || ( count > LONE_NAME_MAX_CHAR ) ) 
	{
		return( ERR_LONG_NAME_ERR );  							 /* ���ļ�����Ч */
	}	
	count = count + 2;											 /* ���ļ�����������ֽ�Ϊ0x00 */		
	CH37x_WR_CMD_PORT( CMD10_SET_LONG_FILE_NAME );					 /* д��1���ֽ������� */		
	CH37x_WR_DAT_PORT( ( UINT8 )count );							 /* д��2���ֽ����ݳ��� */
	CH37x_WR_DAT_PORT( count >> 8 );							
	do
	{
		CH37x_WR_DAT_PORT( *LongName++ );
	}while( --count );		
	xEndCH37xCmd( );

	/* ��ȡ��Ӧ�Ķ��ļ��� */
	status = CH378SendCmdWaitInt( CMD1H_GET_SHORT_FILE_NAME );
	if( status == ERR_SUCCESS )
	{
		/* ��������������ݷ���,���ȡ������ */
		CH378ReadReqBlock( PathName );
	}
	return( status );		
}

/*******************************************************************************
* Function Name  : CH378CreateLongName
* Description    : �½����г��ļ������ļ�
*                  ��Ҫ������ļ���������·��PathName(�����Ȳο�FAT�淶�ɳ��ļ������в���),
*                  ��Ҫ������UNICODEС�˱������˫0�����ĳ��ļ���LongName
* Input          : PathName---���ļ���������·��
*                  LongName---��UNICODEС�˱������˫0�����ĳ��ļ���
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378CreateLongName( PUINT8 PathName, PUINT8 LongName )  
{
	UINT16	count;			
	
	/* ���ý�Ҫ�����Ķ��ļ�������·���� */
	CH378SetFileName( PathName );  								 
	
	/* ���ó��ļ��� */
	for( count = 0; count < LONG_NAME_BUF_LEN; count += 2 ) 
	{
		if ( *(PUINT16)( &LongName[ count ] ) == 0 ) 
		{
			break;  											 /* ������λ�� */
		}
	}
	if( ( count == 0 ) || ( count >= LONG_NAME_BUF_LEN ) || ( count > LONE_NAME_MAX_CHAR ) ) 
	{
		return( ERR_LONG_NAME_ERR );  							 /* ���ļ�����Ч */
	}	
	count = count + 2;											 /* ���ļ�����������ֽ�Ϊ0x00 */		
	CH37x_WR_CMD_PORT( CMD10_SET_LONG_FILE_NAME );					 /* д��1���ֽ������� */		
	CH37x_WR_DAT_PORT( ( UINT8 )count );							 /* д��2���ֽ����ݳ��� */
	CH37x_WR_DAT_PORT( count >> 8 );							
	do
	{
		CH37x_WR_DAT_PORT( *LongName++ );
	}while( --count );		
	xEndCH37xCmd( );	
	
	/* ִ�д������ļ������� */
	return( CH378SendCmdWaitInt( CMD0H_LONG_FILE_CREATE ) );
}

/*******************************************************************************
* Function Name  : CH378CreateLongDir
* Description    : �½����г��ļ�����Ŀ¼
* Input          : PathName---���ļ���������·��
*                  LongName---��UNICODEС�˱������˫0�����ĳ��ļ���
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378CreateLongDir( PUINT8 PathName, PUINT8 LongName )  
{
	UINT16	count;			

	/* ���ý�Ҫ�����Ķ��ļ�������·���� */	
	CH378SetFileName( PathName );  								
	
	/* ���ó��ļ��� */
	for( count = 0; count < LONG_NAME_BUF_LEN; count += 2 ) 
	{
		if ( *(PUINT16)( &LongName[ count ] ) == 0 ) 
		{
			break;  											 /* ������λ�� */
		}
	}
	if( ( count == 0 ) || ( count >= LONG_NAME_BUF_LEN ) || ( count > LONE_NAME_MAX_CHAR ) ) 
	{
		return( ERR_LONG_NAME_ERR );  							 /* ���ļ�����Ч */
	}	
	count = count + 2;											 /* ���ļ�����������ֽ�Ϊ0x00 */		
	CH37x_WR_CMD_PORT( CMD10_SET_LONG_FILE_NAME );					 /* д��1���ֽ������� */		
	CH37x_WR_DAT_PORT( ( UINT8 )count );							 /* д��2���ֽ����ݳ��� */
	CH37x_WR_DAT_PORT( count >> 8 );							
	do
	{
		CH37x_WR_DAT_PORT( *LongName++ );
	}while( --count );		
	xEndCH37xCmd( );	
	
	/* ִ�д�����Ŀ¼������ */
	return( CH378SendCmdWaitInt( CMD0H_LONG_DIR_CREATE ) );
}

/*******************************************************************************
* Function Name  : CH378OpenLongName
* Description    : ͨ�����ļ������ļ�
* Input          : PathName---���ļ�����Ŀ¼��·����
*                  LongName---��UNICODEС�˱������˫0�����ĳ��ļ���
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378OpenLongName( PUINT8 PathName, PUINT8 LongName ) 
{
	UINT8  status;	
	UINT16	count;

	/* ���ý�Ҫ�����Ķ��ļ���·���� */	
	CH378SetFileName( PathName );  								 /* ���ý�Ҫ�������ļ��Ĳ���·���� */

	/* ���ó��ļ��� */
	for( count = 0; count < LONG_NAME_BUF_LEN; count += 2 ) 
	{
		if ( *(PUINT16)( &LongName[ count ] ) == 0 ) 
		{
			break;  											 /* ������λ�� */
		}
	}
	if( ( count == 0 ) || ( count >= LONG_NAME_BUF_LEN ) || ( count > LONE_NAME_MAX_CHAR ) ) 
	{
		return( ERR_LONG_NAME_ERR );  							 /* ���ļ�����Ч */
	}	
	count = count + 2;											 /* ���ļ�����������ֽ�Ϊ0x00 */		
	CH37x_WR_CMD_PORT( CMD10_SET_LONG_FILE_NAME );					 /* д��1���ֽ������� */		
	CH37x_WR_DAT_PORT( ( UINT8 )count );							 /* д��2���ֽ����ݳ��� */
	CH37x_WR_DAT_PORT( count >> 8 );							
	do
	{
		CH37x_WR_DAT_PORT( *LongName++ );
	}while( --count );		
	xEndCH37xCmd( );

	/* �򿪳��ļ�����Ӧ���ļ� */
	status = CH378SendCmdWaitInt( CMD0H_LONG_FILE_OPEN );
	return( status );	
}

/*******************************************************************************
* Function Name  : CH378EraseLongName
* Description    : ͨ�����ļ���ɾ���ļ�
* Input          : PathName---���ļ�����Ŀ¼��·����
*                  LongName---��UNICODEС�˱������˫0�����ĳ��ļ���
* Output         : None
* Return         : �����ж�״̬
*******************************************************************************/
UINT8 CH378EraseLongName( PUINT8 PathName, PUINT8 LongName ) 
{
	UINT8  status;	
	UINT16	count;

	/* ���ý�Ҫ�����Ķ��ļ���·���� */	
	CH378SetFileName( PathName );  								 /* ���ý�Ҫ�������ļ��Ĳ���·���� */

	/* ���ó��ļ��� */
	for( count = 0; count < LONG_NAME_BUF_LEN; count += 2 ) 
	{
		if ( *(PUINT16)( &LongName[ count ] ) == 0 ) 
		{
			break;  											 /* ������λ�� */
		}
	}
	if( ( count == 0 ) || ( count >= LONG_NAME_BUF_LEN ) || ( count > LONE_NAME_MAX_CHAR ) ) 
	{
		return( ERR_LONG_NAME_ERR );  							 /* ���ļ�����Ч */
	}	
	count = count + 2;											 /* ���ļ�����������ֽ�Ϊ0x00 */		
	CH37x_WR_CMD_PORT( CMD10_SET_LONG_FILE_NAME );					 /* д��1���ֽ������� */		
	CH37x_WR_DAT_PORT( ( UINT8 )count );							 /* д��2���ֽ����ݳ��� */
	CH37x_WR_DAT_PORT( count >> 8 );							
	do
	{
		CH37x_WR_DAT_PORT( *LongName++ );
	}while( --count );		
	xEndCH37xCmd( );

	/* ɾ�����ļ�����Ӧ���ļ� */
	status = CH378SendCmdWaitInt( CMD0H_LONG_FILE_ERASE );
	return( status );	
}


#endif

/**********************************************************************************************************/
/* ����Ϊ�ǳ�������������,����ͨ����EN_OTHER_FUNCTION�رջ���,Ҳ����ѡ���ԵĿ��� */
/**********************************************************************************************************/

#ifdef	EN_OTHER_FUNCTION

/*******************************************************************************
* Function Name  : CH378GetICVer
* Description    : ��ȡCH378оƬ�汾��
* Input          : None
* Output         : None
* Return         : ����оƬ�汾��
*******************************************************************************/
UINT8 CH378GetICVer( void ) 
{
	UINT8  s;

	CH37x_WR_CMD_PORT( CMD01_GET_IC_VER );
	s = CH37x_RD_DAT_PORT( );
	xEndCH37xCmd( );
	return( s );
}

/*******************************************************************************
* Function Name  : CH378HardwareReset
* Description    : ִ��CH378Ӳ����λ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH378HardwareReset( void ) 
{
	CH37x_WR_CMD_PORT( CMD00_RESET_ALL );
	xEndCH37xCmd( );
	delay_ms( 50 );												 /* �ȴ���λ��� */
}

/*******************************************************************************
* Function Name  : CH378EnterHalfSleep
* Description    : ʹ��CH378�����˯��״̬
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH378EnterHalfSleep( void ) 
{
	CH37x_WR_CMD_PORT( CMD10_ENTER_SLEEP ); 
	CH37x_WR_DAT_PORT( HALF_SLEEP_MODE ); 						 /* ��˯��ģʽ */	
	xEndCH37xCmd( );
	delay_ms( 50 );												 /* �ȴ�������� */
}

/*******************************************************************************
* Function Name  : CH378EnterFullSleep
* Description    : ʹ��CH378�������˯��״̬
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH378EnterFullSleep( void ) 
{
	CH37x_WR_CMD_PORT( CMD10_ENTER_SLEEP ); 
	CH37x_WR_DAT_PORT( FULL_SLEEP_MODE ); 						 /* ���˯��ģʽ */	
	xEndCH37xCmd( );
	delay_ms( 50 );												 /* �ȴ�������� */
}

/*******************************************************************************
* Function Name  : CH378CheckExist
* Description    : ���CH378�����Լ�ͨѶ�Ƿ�����
* Input          : None
* Output         : None
* Return         : ����ERR_SUCCESS��ʾCH378�����Լ�ͨѶ����;
*                  ����ERR_USB_UNKNOWN��ʾ������,��Ҫ������
*******************************************************************************/
UINT8 CH378CheckExist( void ) 
{
	UINT8  dat;

	CH37x_WR_CMD_PORT( CMD11_CHECK_EXIST );  						 /* ���Ե�Ƭ����CH378֮���ͨѶ�ӿ� */
	CH37x_WR_DAT_PORT( 0x78 );
	dat = CH37x_RD_DAT_PORT( );
	xEndCH37xCmd( );
	if( dat != 0x87 ) 
	{
		/* ͨѶ�ӿڲ�����,����ԭ����:�ӿ������쳣,�����豸Ӱ��(Ƭѡ��Ψһ),���ڲ�����,һֱ�ڸ�λ,���񲻹��� */
		return( ERR_USB_UNKNOWN );  
	}
	else
	{
		return( ERR_SUCCESS );
	}	
}

/*******************************************************************************
* Function Name  : CH378ClearStall
* Description    : ����˵����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CH378ClearStall( UINT8 endp ) 
{
	return( CH378SendCmdDatWaitInt( CMD1H_CLR_STALL, endp ) );	 /* ������ʽ: ���ƴ���-����˵���� */	
}

/*******************************************************************************
* Function Name  : CH378EnumDevice
* Description    : CH378�ֶ�ö��USB�豸
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CH378SelfEnumDevice( PUINT8 buf ) 
{
	UINT8  status;
	
	/* ��ȡ�豸������ */
	status = CH378SendCmdDatWaitInt( CMD1H_GET_DESCR, 0x01 );
	if( status != ERR_SUCCESS )
	{
		return( status );
	}
	status = CH378ReadReqBlock( buf );
	
	/* �ɽ������������� */

	/* ���õ�ַ */
	status = CH378SendCmdDatWaitInt( CMD1H_SET_ADDRESS, 0x05 );
	if( status != ERR_SUCCESS )
	{
		return( status );
	}
	
	/* ��ȡ���������� */
	status = CH378SendCmdDatWaitInt( CMD1H_GET_DESCR, 0x02 );
	if( status != ERR_SUCCESS )
	{
		return( status );
	}
	status = CH378ReadReqBlock( buf );

	/* �ɽ������������� */
	
	/* ��������ֵ */
	status = CH378SendCmdDatWaitInt( CMD1H_SET_CONFIG, buf[ 5 ] );
	if( status != ERR_SUCCESS )
	{
		return( status );
	}
	return( status );		
}

/*******************************************************************************
* Function Name  : CH378AutoEnumDevice
* Description    : CH378�Զ�ö���豸
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CH378AutoEnumDevice( void ) 
{ 
	return( CH378SendCmdWaitInt( CMD0H_AUTO_SETUP ) );		
}

/*******************************************************************************
* Function Name  : CH378AutoInitDisk
* Description    : CH378�Զ���ʼ��USB�洢��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CH378AutoInitDisk( void ) 
{
	return( CH378SendCmdWaitInt( CMD0H_DISK_INIT ) );		
}

/*******************************************************************************
* Function Name  : CH378AutoResetDisk
* Description    : CH378��λUSB�洢��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CH378AutoResetDisk( void ) 
{
	return( CH378SendCmdWaitInt( CMD0H_DISK_RESET ) );		
}

/*******************************************************************************
* Function Name  : CH378GetDiskSize
* Description    : CH378��ȡUSB�洢����������
* Input          : None
* Output         : buf---�������ݰ�
*				   len---�������ݳ���(һ��Ϊ8���ֽ�)
* Return         : None
*******************************************************************************/
UINT8 CH378GetDiskSize( PUINT8 buf, PUINT8 len ) 
{
	UINT8  status;
	
	status = CH378SendCmdWaitInt( CMD0H_DISK_SIZE );
	if( status != ERR_SUCCESS )
	{
		return( status );
	}
	*len = CH378ReadReqBlock( buf );
	return( status );
}	

/*******************************************************************************
* Function Name  : CH378GetDiskInquiry
* Description    : CH378��ȡUSB�洢������
* Input          : None
* Output         : buf---�������ݰ�
*				   len---�������ݳ���(һ��Ϊ36���ֽ�)
* Return         : None
*******************************************************************************/
UINT8 CH378GetDiskInquiry( PUINT8 buf, PUINT8 len ) 
{
	UINT8  status;
	
	status = CH378SendCmdWaitInt( CMD0H_DISK_INQUIRY );
	if( status != ERR_SUCCESS )
	{
		return( status );
	}
	*len = CH378ReadReqBlock( buf );
	return( status );	
}	

/*******************************************************************************
* Function Name  : CH378GetDiskReady
* Description    : CH378���USB�洢���Ƿ�׼����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CH378GetDiskReady( void ) 
{
	UINT8  status;
	
	status = CH378SendCmdWaitInt( CMD0H_DISK_READY );
	if( status != ERR_SUCCESS )
	{
		return( status );
	}
	return( status );	
}	

/*******************************************************************************
* Function Name  : CH378GetDiskReqSense
* Description    : CH378���USB�洢������
* Input          : None
* Output         : buf---�������ݰ�
*				   len---�������ݳ���
* Return         : None
*******************************************************************************/
UINT8 CH378GetDiskReqSense( PUINT8 buf, PUINT8 len ) 
{
	UINT8  status;
	
	status = CH378SendCmdWaitInt( CMD0H_DISK_R_SENSE );
	if( status != ERR_SUCCESS )
	{
		return( status );
	}
	*len = CH378ReadReqBlock( buf );
	return( status );
}	

/*******************************************************************************
* Function Name  : CH378BlockOnlyCMD
* Description    : CH378ִ��BulkOnly����Э�������
* Input          : buf---��Ҫִ�е�BulkOnly�����; 
* Output         : buf---ִ��BulkOnly��������ݰ�
*                  len---�������ݰ�����  
* Return         : None
*******************************************************************************/
UINT8 CH378BlockOnlyCMD( PUINT8 buf, PUINT8 len ) 
{
	UINT8  status;

	/* ��ͨ��CMD40_WR_HOST_OFS_DATA����д��CBW�� */	
	CH378WriteOfsBlock( buf, 0x00, 31 );
	
	/* �ٷ��������� */
	status = CH378SendCmdWaitInt( CMD0H_DISK_BOC_CMD );
	if( status != ERR_SUCCESS )
	{
		return( status );
	}

	/* ��������������ݷ���,���ȡ������ */
	*len = CH378ReadReqBlock( buf );
	return( status );
}		



#endif


/*******************************************************************************
* Function Name  : CH378EraseAll
* Description    : 
* Input          : 
* Output         :  
* Return         : None
*******************************************************************************/
UINT8 CH378EraseAll( void ) 
{
		UINT8  status;
	
		unsigned char  buf[ 64 ];												 /* ���ݻ����� */
		memset(buf, 0, sizeof(buf));
	
		strcpy( (char*)buf, "\\*" );  							 /* C51��Ŀ¼���������г���Ŀ¼�µ��ļ� */

		status = CH378FileOpen( buf );  					 /* ö�ٶ༶Ŀ¼�µ��ļ�����Ŀ¼ */
		while( status == USB_INT_DISK_READ ) 
		{  
				/* ö�ٵ�ƥ����ļ� */
				CH378ReadReqBlock( buf );  						 /* ��ȡö�ٵ����ļ�������·���� */
				
				unsigned char  fileName[ 64 ];
				memset(fileName, 0, sizeof(fileName));
				memcpy( fileName, buf, 8); 
				memcpy( fileName + 8, ".TXT", 4); 
				
				status = CH378FileErase(fileName);  				/* ɾ���ļ� */

				rt_thread_delay(10);
			
				if( status != ERR_SUCCESS ) 
				{
						//return 3;
				}
		
		
			//usart4.printf( "***EnumName: %s\n",buf );				 /* ��ӡ����,ԭʼ8+3��ʽ,δ����ɺ�С����ָ��� */
			CH37x_WR_CMD_PORT( CMD0H_FILE_ENUM_GO );  			 /* ����ö���ļ���Ŀ¼ */
			xEndCH37xCmd( );
			status = Wait378Interrupt( );
		}
		//if( status != ERR_MISS_FILE ) 
		//
			//return status;  						 /* �������� */
		//}
		return status;
}		

void CH378HardwareReset_MINE( void ) 
{
	CH37x_WR_CMD_PORT( CMD00_RESET_ALL );
	xEndCH37xCmd( );
	delay_ms( 50 );												 /* �ȴ���λ��� */
}
/*******************************************************************************
* Function Name  : CH378EnterHalfSleep
* Description    : ʹ��CH378�����˯��״̬
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH378EnterHalfSleep_MINE( void ) 
{
	CH37x_WR_CMD_PORT( CMD10_ENTER_SLEEP ); 
	CH37x_WR_DAT_PORT( HALF_SLEEP_MODE ); 						 /* ��˯��ģʽ */	
	xEndCH37xCmd( );
	delay_ms( 50 );												 /* �ȴ�������� */
}

/*******************************************************************************
* Function Name  : CH378EnterFullSleep
* Description    : ʹ��CH378�������˯��״̬
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH378EnterFullSleep_MINE( void ) 
{
	CH37x_WR_CMD_PORT( CMD10_ENTER_SLEEP ); 
	CH37x_WR_DAT_PORT( FULL_SLEEP_MODE ); 						 /* ���˯��ģʽ */	
	xEndCH37xCmd( );
	delay_ms( 50 );												 /* �ȴ�������� */
}