/********************************** (C) COPYRIGHT *******************************
* File Name          : FILE_SYS.H
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
#include "para.h"

#ifndef	__CH378_FS_H__
#define __CH378_FS_H__


#define	STRUCT_OFFSET( s, m )      ( (UINT8)( & ((s *)0) -> m ) )				/* �����ȡ�ṹ��Ա���ƫ�Ƶ�ַ�ĺ� */

#define	MAX_BYTE_PER_OPERATE	   20480						 				/* ��ײ����ÿ�ζ�д����ֽ��� */			

#ifdef	EN_LONG_NAME
#ifndef	LONG_NAME_BUF_LEN
#define	LONG_NAME_BUF_LEN	       ( LONG_NAME_PER_DIR * 20 )			 	    /* ���ж���ĳ��ļ�������������,��СֵΪLONG_NAME_PER_DIR*1 */
#endif
#endif


#define EN_SECTOR_ACCESS 1

#define EN_DISK_QUERY  1


/*******************************************************************************/
/* �������� */
extern UINT16 SectorSize;										 				/* ��ǰU��ÿ��������С */  

/*******************************************************************************/
/* �������� */
extern UINT32 CH378Read32bitDat( void );						 				/* ��CH378оƬ��ȡ32λ�����ݲ��������� */
extern UINT8 CH378ReadVar8( UINT8 var );						 				/* ��CH378оƬ�ڲ���8λ���� */ 
extern void CH378WriteVar8( UINT8 var, UINT8 dat );				 				/* дCH378оƬ�ڲ���8λ���� */
extern UINT32 CH378ReadVar32( UINT8 var );						 				/* ��CH378оƬ�ڲ���32λ���� */
extern void CH378WriteVar32( UINT8 var, UINT32 dat );			 				/* дCH378оƬ�ڲ���32λ���� */
extern UINT32 CH378GetTrueLen( void );							 				/* ���ٷ�����һ������ִ����Ϻ����󳤶�����Ӧ��ʵ�ʳ��� */
extern void CH378SetFileName( PUINT8 PathName ); 				 				/* ���ý�Ҫ�������ļ����ļ�����·���� */
extern UINT8 CH378GetDiskStatus( void );						 				/* ��ȡ���̺��ļ�ϵͳ�Ĺ���״̬ */
extern UINT8 CH378GetIntStatus( void );							 				/* ��ȡ�ж�״̬��ȡ���ж����� */



#ifndef	NO_DEFAULT_CH378_INT
extern UINT8 Wait378Interrupt( void );							 				/* �ȴ�CH378�ж�(INT#�͵�ƽ)�������ж�״̬��, ��ʱ�򷵻�ERR_USB_UNKNOWN */
#endif

extern UINT8 CH378SendCmdWaitInt( UINT8 mCmd );					 				/* �����������,�ȴ��ж� */
extern UINT8 CH378SendCmdDatWaitInt( UINT8 mCmd, UINT8 mDat );	 				/* �����������һ�ֽ����ݺ�,�ȴ��ж� */
extern UINT32 CH378GetFileSize( void );							 				/* ��ȡ��ǰ�ļ����� */
extern void CH378SetFileSize( UINT32 filesize );				 				/* ���õ�ǰ�ļ����� */
extern UINT8 CH378DiskConnect( void );							 				/* ���U��/SD���Ƿ����� */
extern UINT8 CH378DiskReady( void );							 				/* ��ʼ�����̲����Դ����Ƿ���� */
extern UINT16 CH378ReadReqBlock( PUINT8 buf );					 				/* �ӵ�ǰ�����˵�Ľ��ջ�������ȡ�������ݿ� */
extern UINT16 CH378ReadBlock( PUINT8 buf, UINT16 len );			 				/* �ӵ�ǰ�����˵�Ľ��ջ�������ȡ���ݿ� */
extern UINT16 CH378ReadOfsBlock( PUINT8 buf, UINT16 offset, UINT16 len ); 		/* ��ȡ�ڲ�ָ��������ָ��ƫ�Ƶ�ַ���ݿ�(�ڲ�ָ���Զ�����) */
extern UINT16 CH378WriteBlock( PUINT8 buf, UINT16 len );	 	 				/* ���ڲ�ָ����������ǰƫ�Ƶ�ַд�����ݿ�(�ڲ�ָ���Զ�����) */	
extern UINT16 CH378WriteOfsBlock( PUINT8 buf, UINT16 offset, UINT16 len );		/* ���ڲ�ָ��������ָ��ƫ�Ƶ�ַд�����ݿ�(�ڲ�ָ���Զ�����) */ 
extern UINT8 CH378FileOpen( PUINT8 PathName );					 				/* ���ļ�����Ŀ¼(�ļ���) */
extern UINT8 CH378FileCreate( PUINT8 PathName );				 				/* �½��ļ�����,����ļ��Ѿ�������ô��ɾ�������½� */ 
extern UINT8 CH378FileErase( PUINT8 PathName ); 				 				/* ɾ���ļ����ر� */
extern UINT8 CH378FileClose( UINT8 UpdateSz );					 				/* �رյ�ǰ�Ѿ��򿪵��ļ�����Ŀ¼(�ļ���) */
extern UINT8 CH378ByteLocate( UINT32 offset );					 				/* ���ֽ�Ϊ��λ�ƶ���ǰ�ļ�ָ�� */
extern UINT8 CH378ByteRead( PUINT8 buf, UINT16 ReqCount, PUINT16 RealCount );   /* ���ֽ�Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ� */
extern UINT8 CH378ByteReadPrepare( UINT16 ReqCount, PUINT16 RealCount );  	    /* ���ֽ�Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ�׼�� */
extern UINT8 CH378ByteWrite( PUINT8 buf, UINT16 ReqCount, PUINT16 RealCount );  /* ���ֽ�Ϊ��λ��ǰλ��д�����ݿ� */
extern UINT8 CH378ByteWriteExecute( UINT16 ReqCount, PUINT16 RealCount );	    /* ���ֽ�Ϊ��λ��ǰλ��д�����ݿ� */
extern UINT8 CH378FileQuery( PUINT8 buf );						 				/* CH378��ѯ��ǰ�ļ�����Ϣ(���ȡ����ڡ�ʱ�䡢����) */
extern UINT8 CH378FileModify( UINT32 filesize, UINT16 filedate, UINT16 filetime, UINT8 fileattr ); /* CH378�޸ĵ�ǰ�ļ�����Ϣ(���ȡ����ڡ�ʱ�䡢����) */
extern UINT8 CH378DirInfoRead( UINT8 index );							 		/* ��ȡ��ǰ�ļ���Ŀ¼��Ϣ */	
extern UINT8 CH378DirInfoSave( UINT8 index );							 		/* �����ļ���Ŀ¼��Ϣ */

#ifdef	EN_SECTOR_ACCESS
extern UINT8 CH378SecLocate( UINT32 offset );  						 			/* ������Ϊ��λ�ƶ���ǰ�ļ�ָ�� */
extern UINT8 CH378SecRead( PUINT8 buf, UINT8 ReqCount, PUINT8 RealCount );      /* ������Ϊ��λ�ӵ�ǰλ�ö�ȡ���ݿ� */
extern UINT8 CH378SecWrite( PUINT8 buf, UINT8 ReqCount, PUINT8 RealCount );     /* ������Ϊ��λ�ڵ�ǰλ��д�����ݿ� */ 
extern UINT8 CH378DiskReadSec( PUINT8 buf, UINT32 iLbaStart, UINT8 iSectorCount ); /* ��U�̶�ȡ������������ݿ鵽������ */
extern UINT8 CH378DiskWriteSec( PUINT8 buf, UINT32 iLbaStart, UINT8 iSectorCount );/* ���������еĶ�����������ݿ�д��U�� */
#endif
	
#ifdef	EN_DISK_QUERY
extern UINT8 CH378DiskCapacity( PUINT32 DiskCap );  			 				/* ��ѯ������������,������ */
extern UINT8 CH378DiskQuery( PUINT32 DiskFre, PUINT32 DiskCap);  				 				/* ��ѯ����ʣ��ռ���Ϣ,������ */
#endif
	 						 
#ifdef	EN_DIR_CREATE
extern UINT8 CH378DirCreate( PUINT8 PathName );  				 				/* �½��༶Ŀ¼�µ�Ŀ¼(�ļ���)����,֧�ֶ༶Ŀ¼·��,֧��·���ָ���,·�����Ȳ�����255���ַ� */
#endif

#ifdef	EN_LONG_NAME
extern UINT8 CH378GetLongName( PUINT8 PathName, PUINT8 LongName );    			/* �ɶ��ļ�������Ŀ¼(�ļ���)�������Ӧ�ĳ��ļ��� */
extern UINT8 CH378GetShortName( PUINT8 PathName, PUINT8 LongName );				/* �ɳ��ļ�����ȡ��Ӧ�Ķ��ļ��� */
extern UINT8 CH378CreateLongName( PUINT8 PathName, PUINT8 LongName ); 			/* �½����г��ļ������ļ� */
extern UINT8 CH378CreateLongDir( PUINT8 PathName, PUINT8 LongName );  			/* �½����г��ļ�����Ŀ¼ */
#endif


#ifdef	EN_OTHER_FUNCTION
extern UINT8 CH378GetICVer( void );												/* ��ȡCH378оƬ�汾�� */ 
extern void CH378HardwareReset( void );											/* ִ��CH378Ӳ����λ */	
extern UINT8 CH378CheckExist( void );											/* ���CH378�����Լ�ͨѶ�Ƿ����� */
extern UINT8 CH378ClearStall( UINT8 endp );										/* ����˵���� */
extern UINT8 CH378AutoEnumDevice( void ); 										/* CH378�Զ�ö���豸 */	
extern UINT8 CH378AutoInitDisk( void );											/* CH378�Զ���ʼ��USB�洢�� */	
extern UINT8 CH378AutoResetDisk( void );										/* CH378��λUSB�洢�� */	
extern UINT8 CH378GetDiskSize( PUINT8 buf, PUINT8 len );						/* CH378��ȡUSB�洢���������� */
extern UINT8 CH378GetDiskInquiry( PUINT8 buf, PUINT8 len );						/* CH378��ȡUSB�洢������ */
extern UINT8 CH378GetDiskReady( void );											/* CH378���USB�洢���Ƿ�׼���� */
extern UINT8 CH378GetDiskReqSense( PUINT8 buf, PUINT8 len );					/* CH378���USB�洢������ */ 	
extern UINT8 CH378BlockOnlyCMD( PUINT8 buf, PUINT8 len );						/* CH378ִ��BulkOnly����Э������� */
#endif

UINT8 CH378EraseAll( void );
void CH378HardwareReset_MINE( void ) ;
void CH378EnterHalfSleep_MINE( void );
void CH378EnterFullSleep_MINE( void ) ;
#endif
