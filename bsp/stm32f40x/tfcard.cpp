#include "tfcard.hpp"
#include "rtthread.h"
#include "ch378.h"
#include "ch378_file_sys.h"
#include "delay.h"
#define TFCARD_MAX_SIZE_PER_WRITE				1024*16
#define TFCARD_BLOCK_SIZE								512

#define TFCARD_SINGLE_FILE_MAX_LEN			1024*1024*10			//�����ļ���󳤶�  ��λ���ֽ�
rt_bool_t tfcard_ready = RT_FALSE;
/*�̲߳���*/
#define TFCARD_WRITE_THREAD_STACK_SIZE       	TFCARD_MAX_SIZE_PER_WRITE+1024*2
#define TFCARD_WRITE_THREAD_PRIORITY         	25
#define TFCARD_WRITE_THREAD_TIMESLICE        	50
/*�߳�ID*/
rt_thread_t tfcard_write_tid = RT_NULL;
rt_err_t tfcard_init(void)
{
		rt_uint8_t s=0;
		tfcard_ready = RT_FALSE;
	
		s = mInitCH37xHost();	
		if(ERR_SUCCESS!=s)
		{
				return RT_ERROR;
		}
		delay_ms( 100 );
		
		/* ���U�̻���SD���Ƿ�����,�ȴ������ */
		if( CH378DiskConnect( ) != ERR_SUCCESS ) 
				return RT_ERROR;
		
		delay_ms( 200 ); /* ��ʱ,��ѡ����,�е�USB�洢����Ҫ��ʮ�������ʱ */
		
		/* ���ڼ�⵽USB�豸��,���ȴ�100*50mS,��Ҫ�����ЩMP3̫��,���ڼ�⵽USB�豸��������DISK_MOUNTED��,
		���ȴ�5*50mS,��Ҫ���DiskReady������ */
		for( int i = 0; i < 100; i ++ ) 
		{  
				/* ��ȴ�ʱ��,100*50mS */
				delay_ms( 50 );
				//printf( "Ready ?\n" );
				s = CH378DiskReady( );  						 /* ��ʼ�����̲����Դ����Ƿ���� */
				if( s == ERR_SUCCESS ) 
				{
						break;  										 /* ׼���� */
				}
				else if( s == ERR_DISK_DISCON ) 
				{
						return RT_ERROR;//goto UnknownUsbDevice; 							 /* ��⵽�Ͽ�,���¼�Ⲣ��ʱ */
				}
				
				if( CH378GetDiskStatus( ) >= DEF_DISK_MOUNTED && i >= 5 ) 
				{
						break;  										 /* �е�U�����Ƿ���δ׼����,�������Ժ���,ֻҪ�佨������MOUNTED�ҳ���5*50mS */
				}
		}
		if( s == ERR_DISK_DISCON ) 
		{  
				/* ��⵽�Ͽ�,���¼�Ⲣ��ʱ */
				//printf( "Device gone\n" );
				return RT_ERROR;
		}
		if( CH378GetDiskStatus( ) < DEF_DISK_MOUNTED ) 
		{  
				/* δ֪USB�豸,����USB���̡���ӡ���� */
				//printf( "Unknown device\n" );
				return RT_ERROR;
		}	
				
		tfcard_ready = RT_TRUE;

		
		return RT_EOK;
}
int AddTestIndex(const char* TestLoc,long time,float a, float b, const char* unit)
{
	//ReadAllTestIndex()
	//Open 
	//open TestIndex.bin
	//if testindex.bin is not there, create one
	//read all lines
	//add 
	return -1;
}
int AddValue(int TestIndex,long time, unsigned int value)
{
	//open file
	//write value
	//need to create a buffer
	return 0;
}
int EndTestIndex(int TestIndex)
{
	//close file
	//write buffer into file
	return 0;
}
int DeleteTestIndex(int TestIndex)
{
	//open testindex.bin
	//mark 0 on del line
	return 0;
}
int ReadAllTestIndex(struct TestIndexListStc ti)
{
	//open testindex.bin
	//read all lines
	//if del is 0, continue
	//refresh ti
	//�ֽ�д ��ʮk��
	return 0;
}
