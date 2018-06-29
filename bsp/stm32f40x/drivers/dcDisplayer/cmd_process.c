#include "cmd_process.h"
#include "stdio.h"
#include "hmi_user_uart.h"
#include <rtthread.h>
#include "usart4.h"
static uint16 current_screen_id = 0;//��ǰ����ID
static int32 test_value = 0;//����ֵ
static uint8 update_en = 0;//���±��
uint8 cmd_buffer[CMD_MAX_SIZE];//ָ��buffer
uint32 timer_tick_last_update = 0; //��һ�θ��µ�ʱ��
extern volatile uint32  timer_tick_count; //��ʱ������
enum enum_DISPSTAT{DISPSTAT_MA,DISPSTAT_PROC} dispstat;
int rangeMax=500;
int rangeMin=0;
float slop=1;
float offset=2;
rt_event_t evt_record;
rt_event_t evt_readhistory;
struct rt_messagequeue mq_record;
char msg_pool[2048];
char unitBuf[10]={0x6d,0x67,0x2f,0x6d,0x5e,0x33,0x00};
void SetTextValueInt32(uint16 screen_id, uint16 control_id,int32 value)
{
	uchar buffer[12] = {0};
	sprintf(buffer,"%ld",value); //������ת��Ϊ�ַ���
	SetTextValue(screen_id,control_id,buffer);
}

void SetTextValueFloat(uint16 screen_id, uint16 control_id,float value)
{
	uchar buffer[12] = {0};
	sprintf(buffer,"%.1f",value);//�Ѹ�����ת��Ϊ�ַ���(����һλС��)
	SetTextValue(screen_id,control_id,buffer);
}
void AddDataToChannel(uint8 *buf)
{
	char tempbuf[4];
	memcpy(tempbuf,buf,4);
	rt_uint32_t event;
	if(rt_event_recv(evt_record,
									btn_DISPMA|btn_DISPPRC,
									RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,
									RT_WAITING_NO,
									&event)==RT_EOK)
	{

		//if(event&btn_DISPMA)
//		{
//			dispstat=DISPSTAT_MA;
//		}
//		if(event&btn_DISPPRC)
//		{
//			dispstat=DISPSTAT_PROC;
//		}
	}

		float mAval_float;
		sscanf(tempbuf,"%f",&mAval_float);
	mAval_float=slop*mAval_float+0;
		uint8 out[4];
		float pAval_float;
		switch(dispstat)
		{
			case DISPSTAT_MA:
				SetTextValueFloat(recordscreen,15,mAval_float);
			HistoAddFloat(historyscreen,7,mAval_float);
			usart4.printf("V:%f",mAval_float);
			//GraphChannelDataAdd(recordscreen,1,0,tempbuf,4);
				break;
			case DISPSTAT_PROC:
				pAval_float = (mAval_float-4)*(rangeMax-rangeMin)/16+rangeMin;
				SetTextValueFloat(recordscreen,15,pAval_float);
			HistoAddFloat(recordscreen,1,pAval_float);
				break;
			
		}
		
		//GraphChannelDataAdd(recordscreen,1,0,out,2);
		//HistoDataAdd(recordscreen,1,out,4);
		
}

void th_ScreenProcessFunc()
{
	rt_mq_init(&mq_record,"mqt",&msg_pool[0],128-sizeof(void*),sizeof(msg_pool),RT_IPC_FLAG_FIFO);
	InitDisplayer();
	rt_err_t rst=0;
	while(1)
	{
		unsigned char buf[4];
		GetMessage();
	/****************************************************************************************************************
		�ر�ע��
		MCU��ҪƵ���򴮿����������ݣ����򴮿������ڲ��������������Ӷ��������ݶ�ʧ(��������С����׼��8K��������4.7K)
		1) һ������£�����MCU�򴮿����������ݵ����ڴ���100ms���Ϳ��Ա������ݶ�ʧ�����⣻
		2) �����Ȼ�����ݶ�ʧ�����⣬���жϴ�������BUSY���ţ�Ϊ��ʱ���ܷ������ݸ���������
		******************************************************************************************************************/

		//TODO: ����û�����
		//�����и���ʱ��ÿ100����ˢ��һ��
	/*
		if(update_en&&timer_tick_count-timer_tick_last_update>=TIME_100MS)
		{
			update_en = 0;
			timer_tick_last_update = timer_tick_count;			

			UpdateUI();
		}
*/
		//����Ϊ����ʾ���㣬ÿ100�����Զ�����һ��
		if(timer_tick_count-timer_tick_last_update>=TIME_100MS)
		{
			timer_tick_last_update = timer_tick_count;
			rst=rt_mq_recv(&mq_record,&buf[0],4,RT_WAITING_NO);
			if(rst==RT_EOK)
			{
				AddDataToChannel(buf);
				//������ݵ�ͨ��0
				//write to tf card
				//ad->tfcard->displayer
				//          ->network
			}
			UpdateUI();
			
		}
			rt_thread_delay(50);
	}
		
}

void starttest()
{
	uint16 i = 0;
		uint8 dat[100] = {0};

		//���ɷ���
		for (i=0;i<100;++i)
		{
			if((i%20)>=10)
				dat[i] = 200;
			else
				dat[i] = 20;
		}
		GraphChannelDataAdd(recordscreen,1,0,dat,100);//������ݵ�ͨ��0
}

void GetMessage()
{
	unsigned short size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE); //�ӻ������л�ȡһ��ָ��        
		if(size>0)//���յ�ָ��
		{
			ProcessMessage((PCTRL_MSG)cmd_buffer, size);//ָ���
		}
	}		
/*! 
 *  \brief  ��Ϣ��������
 *  \param msg ��������Ϣ
 *  \param size ��Ϣ����
 */
void ProcessMessage( PCTRL_MSG msg, uint16 size )
{
	uint8 cmd_type = msg->cmd_type;//ָ������
	uint8 ctrl_msg = msg->ctrl_msg;   //��Ϣ������
	uint8 control_type = msg->control_type;//�ؼ�����
	uint16 screen_id = PTR2U16(&msg->screen_id);//����ID
	uint16 control_id = PTR2U16(&msg->control_id);//�ؼ�ID
	uint32 value = PTR2U32(msg->param);//��ֵ

	switch(cmd_type)
	{		
	case NOTIFY_TOUCH_PRESS://����������
	case NOTIFY_TOUCH_RELEASE://�������ɿ�
		NotifyTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4));
		break;	
	case NOTIFY_WRITE_FLASH_OK://дFLASH�ɹ�
		NotifyWriteFlash(1);
		break;
	case NOTIFY_WRITE_FLASH_FAILD://дFLASHʧ��
		NotifyWriteFlash(0);
		break;
	case NOTIFY_READ_FLASH_OK://��ȡFLASH�ɹ�
		NotifyReadFlash(1,cmd_buffer+2,size-6);//ȥ��֡ͷ֡β
		break;
	case NOTIFY_READ_FLASH_FAILD://��ȡFLASHʧ��
		NotifyReadFlash(0,0,0);
		break;
	case NOTIFY_READ_RTC://��ȡRTCʱ��
		NotifyReadRTC(cmd_buffer[1],cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7]);
		break;
	case NOTIFY_CONTROL:
		{
			if(ctrl_msg==MSG_GET_CURRENT_SCREEN)//����ID�仯֪ͨ
			{
				NotifyScreen(screen_id);
			}
			else
			{
				switch(control_type)
				{
				case kCtrlButton: //��ť�ؼ�
					NotifyButton(screen_id,control_id,msg->param[1]);
					break;
				case kCtrlText://�ı��ؼ�
					NotifyText(screen_id,control_id,msg->param);
					break;
				case kCtrlProgress: //�������ؼ�
					NotifyProgress(screen_id,control_id,value);
					break;
				case kCtrlSlider: //�������ؼ�
					NotifySlider(screen_id,control_id,value);
					break;
				case kCtrlMeter: //�Ǳ�ؼ�
					NotifyMeter(screen_id,control_id,value);
					break;
				case kCtrlMenu://�˵��ؼ�
					NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);
					break;
				case kCtrlSelector://ѡ��ؼ�
					NotifySelector(screen_id,control_id,msg->param[0]);
					break;
				case kCtrlRTC://����ʱ�ؼ�
					NotifyTimer(screen_id,control_id);
					break;
				default:
					break;
				}
			}			
		}
		break;
	default:
		break;
	}
}

/*! 
 *  \brief  �����л�֪ͨ
 *  \details  ��ǰ����ı�ʱ(�����GetScreen)��ִ�д˺���
 *  \param screen_id ��ǰ����ID
 */
void NotifyScreen(uint16 screen_id)
{
	//TODO: ����û�����
	
	current_screen_id = screen_id;//�ڹ��������п��������л�֪ͨ����¼��ǰ����ID
/*
	if(current_screen_id==4)//�¶�����
	{
		uint16 i = 0;
		uint8 dat[100] = {0};

		//���ɷ���
		for (i=0;i<100;++i)
		{
			if((i%20)>=10)
				dat[i] = 200;
			else
				dat[i] = 20;
		}
		GraphChannelDataAdd(4,1,0,dat,100);//������ݵ�ͨ��0

		//���ɾ�ݲ�
		for (i=0;i<100;++i)
		{
			dat[i] = 16*(i%15);
		}
		GraphChannelDataAdd(4,1,1,dat,100);//������ݵ�ͨ��1
	}
	else if(current_screen_id==9)//��ά��
	{
		//��ά��ؼ���ʾ�����ַ�ʱ����Ҫת��ΪUTF8���룬
		//ͨ����ָ�����֡���ת�������ݴ��123�� ���õ��ַ�����������
		uint8 dat[] = {0xE5,0xB9,0xBF,0xE5,0xB7,0x9E,0xE5,0xA4,0xA7,0xE5,0xBD,0xA9,0x31,0x32,0x33};
		SetTextValue(9,1,dat);
	}
	*/
}

/*! 
 *  \brief  ���������¼���Ӧ
 *  \param press 1���´�������3�ɿ�������
 *  \param x x����
 *  \param y y����
 */
void NotifyTouchXY(uint8 press,uint16 x,uint16 y)
{
	//TODO: ����û�����
}



void UpdateUI()
{
	/*
	if(current_screen_id==2)//�ı����ú���ʾ
	{
		//��ǰ�������¶ȴ�0��100ѭ����ʾ�������ִ�0.0-99.9ѭ����ʾ
		SetTextValueInt32(2,5,test_value%100);//��ǰ����
		SetTextValueInt32(2,6,test_value%100);//�¶�
		SetTextValueFloat(2,7,(test_value%1000)/10.0);//������

		++test_value;
	}
	else if(current_screen_id==5)//�������ͻ������
	{
		SetProgressValue(5,1,test_value%100);

		++test_value;
	}
	else if(current_screen_id==6)//�Ǳ�ؼ�
	{
		SetMeterValue(6,1,test_value%360);
		SetMeterValue(6,2,test_value%360);

		++test_value;
	}
	*/
}

/*! 
 *  \brief  ��ť�ؼ�֪ͨ
 *  \details  ����ť״̬�ı�(�����GetControlValue)ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param state ��ť״̬��0����1����
 */
void NotifyButton(uint16 screen_id, uint16 control_id, uint8  state)
{
	//TODO: ����û�����
	rt_uint32_t event;
	switch(screen_id)
	{
		case mainscreen:
			
			break;
		case recordscreen:
		{
			switch(control_id)
			{
				case 2:
					//save button
				rt_event_send(evt_record,btn_SAVE);
				
					break;
				case 3:
					rt_event_send(evt_record,btn_START);
						ResetScreen();
				SetScreen(recordscreen);
					dispstat=DISPSTAT_MA;
				rt_event_send(evt_record,btn_DISPMA);
						ResetScreen();
				rt_thread_delay(20);
				SetScreen(recordscreen);
				SetTextValue(recordscreen,14,"mA");
				HistoRangeSet(recordscreen,1,20,0);
				HistoEnable(recordscreen,1);
					break;
				case 4:
					rt_event_send(evt_record,btn_START);
				ResetScreen();
				rt_thread_delay(20);
				SetScreen(recordscreen);
				HistoEnable(recordscreen,1);
				dispstat=DISPSTAT_PROC;
				SetTextValue(recordscreen,14,unitBuf);
				rt_event_send(evt_record,btn_DISPPRC);
				HistoRangeSet(recordscreen,1,rangeMax,rangeMin);

					break;
				case 5:
				//starttest();
				rt_event_send(evt_record,btn_START);
						ResetScreen();
				rt_thread_delay(20);
				SetScreen(recordscreen);
				HistoEnable(recordscreen,1);
				break;
				case 6:
					rt_event_recv(evt_record,
									btn_START,
									RT_EVENT_FLAG_AND|RT_EVENT_FLAG_CLEAR,
									RT_WAITING_NO,
									&event);
					rt_event_send(evt_record,btn_STOP);
				HistoDisable(recordscreen,1);
				
				break;
				case 7:
					if(rt_event_recv(evt_record,
									btn_START,
									RT_EVENT_FLAG_AND,
									RT_WAITING_NO,
									&event)==RT_EOK)
					{
						//please stop frist
					}
				//	stoptest();
				break;
				default:
					break;
			}
			break;
			}
		case historyscreen:
			break;
		default:
			break;
	}
	/*
	if(screen_id==3)//��ť��ͼ�ꡢ��������
	{
		if(control_id==3)//���а�ť
		{
			if(state==0)//ֹͣ����
			{				
				AnimationPlayFrame(3,1,1);//��ʾֹͣͼ��
				AnimationStop(3,2);//����ֹͣ����
			}
			else//��ʼ����
			{				
				SetControlVisiable(3,1,1);//��ʾͼ��
				SetControlVisiable(3,2,1);//��ʾ����

				AnimationPlayFrame(3,1,0);//��ʾ����ͼ��
				AnimationStart(3,2);//������ʼ����
			}		
		}
		else if(control_id==4)//��λ��ť
		{
			SetControlVisiable(3,1,0);//����ͼ��
			SetControlVisiable(3,2,0);//���ض���
			SetButtonValue(3,3,0);//��ʾ��ʼ����
		}
	}
	*/
}
	

/*! 
 *  \brief  �ı��ؼ�֪ͨ
 *  \details  ���ı�ͨ�����̸���(�����GetControlValue)ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param str �ı��ؼ�����
 */
void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str)
{
	//TODO: ����û�����
	int32 value = 0; 

	if(screen_id==recordscreen)
	{
		
		switch(control_id)
		{
			case 8:
				//locA
				break;
			case 9:
				//loc b
				break;
			case 10:
				//loc c
				break;
			case 11:
				sscanf(str,"%s",unitBuf);
				//unit
				break;
			case 12:
				//range max
			sscanf(str,"%ld",&rangeMax);//���ַ���ת��Ϊ����
				break;
			case 13:
				//range min
			sscanf(str,"%ld",&rangeMin);//���ַ���ת��Ϊ����
				break;
		}
		
		/*
		if(control_id==1)
		{
			//�޶���ֵ��Χ��Ҳ�������ı��ؼ����������ã�
			if(value<0)
				value = 0;
			else if(value>380)
				value = 380;

			SetTextValueInt32(2,1,value);  //������ߵ�ѹ
			SetTextValueInt32(2,4,value/2);  //������ߵ�ѹ/2
		}*/
	}
}

/*! 
 *  \brief  �������ؼ�֪ͨ
 *  \details  ����GetControlValueʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param value ֵ
 */
void NotifyProgress(uint16 screen_id, uint16 control_id, uint32 value)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  �������ؼ�֪ͨ
 *  \details  ���������ı�(�����GetControlValue)ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param value ֵ
 */
void NotifySlider(uint16 screen_id, uint16 control_id, uint32 value)
{
	//TODO: ����û�����
	if(screen_id==5&&control_id==2)//�������
	{
		test_value = value;

		SetProgressValue(5,1,test_value); //���½�������ֵ
	}
}

/*! 
 *  \brief  �Ǳ�ؼ�֪ͨ
 *  \details  ����GetControlValueʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param value ֵ
 */
void NotifyMeter(uint16 screen_id, uint16 control_id, uint32 value)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  �˵��ؼ�֪ͨ
 *  \details  ���˵���»��ɿ�ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param item �˵�������
 *  \param state ��ť״̬��0�ɿ���1����
 */
void NotifyMenu(uint16 screen_id, uint16 control_id, uint8  item, uint8  state)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  ѡ��ؼ�֪ͨ
 *  \details  ��ѡ��ؼ��仯ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param item ��ǰѡ��
 */
void NotifySelector(uint16 screen_id, uint16 control_id, uint8  item)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  ��ʱ����ʱ֪ͨ����
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 */
void NotifyTimer(uint16 screen_id, uint16 control_id)
{
	//TODO: ����û�����
}

	/*! 
	 *  \brief  ��ȡ�û�FLASH״̬����
	 *  \param status 0ʧ�ܣ�1�ɹ�
	 *  \param _data ��������
	 *  \param length ���ݳ���
	 */
	void NotifyReadFlash(uint8 status,uint8 *_data,uint16 length)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  д�û�FLASH״̬����
 *  \param status 0ʧ�ܣ�1�ɹ�
 */
void NotifyWriteFlash(uint8 status)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  ��ȡRTCʱ�䣬ע�ⷵ�ص���BCD��
 *  \param year �꣨BCD��
 *  \param month �£�BCD��
 *  \param week ���ڣ�BCD��
 *  \param day �գ�BCD��
 *  \param hour ʱ��BCD��
 *  \param minute �֣�BCD��
 *  \param second �루BCD��
 */
void NotifyReadRTC(uint8 year,uint8 month,uint8 week,uint8 day,uint8 hour,uint8 minute,uint8 second)
{
}
