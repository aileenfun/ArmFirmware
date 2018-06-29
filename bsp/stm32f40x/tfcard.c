#include "tfcard.h"

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
char* tibin={"//testindex.bin"};
UINT8 status;
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
int CreatePath(int pos,rt_uint8_t *path)
{
	//rt_uint8_t path[10];
	int bp=0;
	path[bp++]='\\';
	path[bp++]=(pos>>8)&0xff;
	path[bp++]=pos&0xff;
	path[bp++]='.';
	path[bp++]='b';
	path[bp++]='i';
	path[bp++]='n';
	return bp;
}
int AddTestIndex(struct TestIndexListStc til)
{
	int pos=ReadAllTestIndex();
	
	struct TestIndexListStc *ti = NULL;
	ti = &TestIndexListEnd;
	struct TestIndexListStc *pti=NULL;
	pti=malloc(sizeof(TestIndexList));//c0
	ti->next = pti;//c1
	pti = ti;//c2
	ti = ti->next;//c3
	ti->pre = pti;//c4
	ti->next = NULL;//c5
	
	pos++;
	ti->ti=pos;
	ti->del=0;
	ti->time=til.time;
	memcpy(&ti->unit,&til.time,sizeof(til.time));
	memcpy(&ti->TestLocA,&til.TestLocA,sizeof(til.TestLocA));
	memcpy(&ti->TestLocB,&til.TestLocB,sizeof(til.TestLocA));
	memcpy(&ti->TestLocC,&til.TestLocC,sizeof(til.TestLocA));
	ti->a1=til.a1;
	ti->a2=til.a2;
	ti->b1=til.b1;
	ti->b2=til.b2;

	TestIndexListEnd=*ti;
	
	UINT8 tfwrbuf[TFCARD_BLOCK_SIZE];
	memcpy(tfwrbuf,&TestIndexListEnd,sizeof(TestIndexListEnd));
	
	rt_uint8_t idxpath[10];
	CreatePath(TestIndexListEnd.pos,idxpath);
	
	rt_err_t result = rt_mutex_take(&tfio_mutex, RT_WAITING_NO);	
	//update testindex.bin
	CH378FileOpen(tibin);
	CH378SecLocate(SECEND);
	PUINT8 realcnt;
	CH378SecWrite(tfwrbuf,1,realcnt);
	if(*realcnt<1)
	{
		//write error
	}
	//create index.bin
	status=CH378FileCreate(idxpath);
	CH378FileClose(1);
	rt_mutex_release(&tfio_mutex);
	return -1;
}
int AddValue(int TestIndex,long time, unsigned int value)
{
	rt_uint8_t idxpath[10];
	CreatePath(TestIndexListEnd.pos,idxpath);
	status=CH378FileOpen(idxpath);
	if(status!=ERR_SUCCESS)
	{
		//some error
	}
	//move byte pointer or sec pointer?
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
	do{
		
	}while(1);
	return 0;
}
int ReadAllTestIndex()
{
	//open testindex.bin
	//read all lines
	//if del is 0, continue
	//refresh ti
	//�ֽ�д ��ʮk��
	status=0;
	PUINT8 getlen;
	status=CH378FileOpen(tibin);
	UINT8 readbuf[512];
	
	if( status == ERR_SUCCESS )
	{
		int iter=0;
		rt_uint64_t temp;
		int thispos=0;
		struct TestIndexListStc *ti;
		
		do{
				status=CH378SecRead(readbuf,1,getlen);
			if(getlen>0)
			{
				if(thispos==0)
				{
					ti=&TestIndexListBegin;
				}
				else
				{
					struct TestIndexListStc *pti=NULL;
					pti=malloc(sizeof(TestIndexList));//c0
					ti->next=pti;//c1
					pti=ti;//c2
					ti=ti->next;//c3
					ti->pre=pti;//c4
					ti->next=NULL;//c5
					//    1      2        null
					//c0  ti     pti
					//c1         ti->next
					//c2  pti 
					//c3          ti       
					//c4  ti->pre
					//c5                 ti->next
				}
				memcpy(&ti->ti,readbuf+iter,2);
				iter+=2;
				
				ti->del=readbuf[iter];
				iter++;
				
				memcpy(&ti->time,readbuf+iter,8);
				iter+=8;
				
				memcpy(&ti->TestLocA,readbuf+iter,20);
				iter+=20;
				
				memcpy(&ti->TestLocB,readbuf+iter,20);
				iter+=20;
				
				memcpy(&ti->TestLocC,readbuf+iter,20);
				iter+=20;
				
				memcpy(&ti->unit,readbuf+iter,10);
				iter+=10;
				
				memcpy(&ti->a1,readbuf+iter,4);
				iter+=4;
				
				memcpy(&ti->b1,readbuf+iter,4);
				iter+=4;
				
				memcpy(&ti->a2,readbuf+iter,4);
				iter+=4;
				
				memcpy(&ti->b2,readbuf+iter,4);
				 ti->pos=thispos;
			}
				}while(status==ERR_SUCCESS);
			TestIndexListEnd=*ti;
				return TestIndexListEnd.pos;
	}
	else if( status == ERR_MISS_FILE )
	{
		//warnning: new tfcard?
		status=CH378FileCreate(tibin);
		if(status==ERR_SUCCESS)
		{
			//filesys inited;
		}
		else
		{
			//retry
		}
		
	}
	else 
		{
			//open file error
		}
	
	return 0;
}
