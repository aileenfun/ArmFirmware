#include <time.h>
#include <string.h>
#include "key_scan.h"
#include <stm32f4xx.h>

#define KEY_SCAN_THREAD_STACK_SIZE                      1024*5
#define KEY_SCAN_THREAD_PRIORITY                        25
#define KEY_SCAN_THREAD_TIMESLICE                       5
/*�߳�id*/
static rt_thread_t key_scan_tid = RT_NULL;


struct key_handler key[NUM_OF_KEYS];

/*��������*/
static void key_scan_entry(void *parameter);
static void key_wifi_control_event(void);
static void key_tfcard_init_event(void);
static void key_wifi_reset_event(void);
static rt_bool_t key_start_flag = 0;

static void key_lock_event(void);
static void key_display_waveform_event(void);
static void key_start_event(void);
static void key_config_event(void);
static void key_format_event(void);
static void dynamic_acquisition_enable_event(void);
static void dynamic_acquisition_disable_event(void);
static void cancel_dynamic_acquisition_disable_event(void);
static void switch_total_channel_number_event(void);
static void cancel_switch_total_channel_number_event(void);
static void format_SDCard_event(void);
static void key_cancel_event(void);
static void data_add_event_tag_event(void);


void rt_hw_key_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    /* Enable the GPIO Clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);

    /* Configure the GPIOA.2 pin as input*/
	/*
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	*/

    /* Configure the GPIOH.3 and GPIOH.4 pins as input*/
	/*
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_Init(GPIOH, &GPIO_InitStructure);
*/
    /* Configure the GPIOI.5, GPIOI.8 and GPIOI.11 pins as input*/
		
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_Init(GPIOI, &GPIO_InitStructure);

		/* Configure the GPIOI.4 as output to control power supply*/
		
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN; // DOWN for power off, up for power on
		GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_Init(GPIOI, &GPIO_InitStructure);
}

static rt_uint8_t get_key_status(rt_uint8_t key_num)
{
    switch (key_num)
    {
				case 0:
					//	return (GPIO_ReadInputDataBit(GPIOI, GPIO_Pin_8)); //PI8
				case 1:
						return (GPIO_ReadInputDataBit(GPIOI, GPIO_Pin_5)); //PI5 for power press scan
				case 2:
						//return (GPIO_ReadInputDataBit(GPIOI, GPIO_Pin_11));  //PI11
				case 3:
					//	return (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2));  //PA2
				case 4:
					//	return (GPIO_ReadInputDataBit(GPIOH, GPIO_Pin_3));  //PI11	
#if 0						
				case 5:
						return (GPIO_ReadInputDataBit(GPIOH, GPIO_Pin_4));  //PH4
#endif
				case 5:
					//	return (GPIO_ReadInputDataBit(GPIOI, GPIO_Pin_11) | GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2));   //PI11 & PA2
				case 6:
					//	return (GPIO_ReadInputDataBit(GPIOI, GPIO_Pin_11) | GPIO_ReadInputDataBit(GPIOH, GPIO_Pin_3));   //PI11 & PH3
				case 7:
					//	return (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) | GPIO_ReadInputDataBit(GPIOH, GPIO_Pin_3));   //PA.2 & PH3
				case 8:
					//	return (GPIO_ReadInputDataBit(GPIOI, GPIO_Pin_8) | GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2));   //PI.8 & PA2
				default:
						return 0;
    }
}

void key_handler_init(void)
{
    rt_uint32_t i;

    for (i = 0; i < NUM_OF_KEYS; i++)
    {
        key[i].key_last_status = 0;
        key[i].key_status = 0;
        key[i].key_press_counter = 0;
        key[i].key_hold_counter = 0;
        key[i].key_release_counter = 0;
        key[i].key_press_event = RT_NULL;
        key[i].key_hold_event = RT_NULL;
        key[i].key_hold_1s_event = RT_NULL;
        key[i].key_hold_3s_event = RT_NULL;
        key[i].key_release_1s_event = RT_NULL;
    }
		//key[KEY_EVENT_TAG].key_press_event = one_wire_event;
		key[KEY_EVENT_TAG].key_hold_3s_event = data_add_event_tag_event;  
    key[KEY_POWER].key_hold_1s_event = power_off;
    key[KEY_START_AND_CONFIG].key_press_event = key_lock_event;
		//key[KEY_START_AND_FORMAT].key_press_event = key_display_waveform_event;//��ʱû����ʾ���ι��̣�
    key[KEY_CONFIG_AND_FORMAT].key_press_event = key_wifi_control_event;
		key[KEY_EVENT_AND_CONFIG].key_press_event = key_tfcard_init_event;//�ֶ���ʼ��tfcard
		
#ifdef TEMP_NO_COMPILE
#else
	//if(IS_CARD_READER_DETECTED == RT_FALSE)	//Card Reader Not Detected
#endif //TEMP_NO_COMPILE
	{
		key[KEY_START].key_hold_3s_event = key_start_event;
		key[KEY_CONFIG].key_hold_3s_event = key_config_event;
		key[KEY_FORMAT].key_hold_3s_event = key_format_event;  

	}
}
void power_on(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);  //ʹ��PI�˿�ʱ��

    /* ��Դ���� GPIOI.4*/
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP; // DOWN for power off, up for power on
		GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_Init(GPIOI, &GPIO_InitStructure);

    GPIO_SetBits(GPIOI, GPIO_Pin_4); // up for power on
}


void power_off(void)
{
    GPIO_ResetBits(GPIOI, GPIO_Pin_4);
}
static void key_scan_entry(void *parameter)
{
    rt_uint32_t i;

    while (1)
    {
        for (i = 0; i < NUM_OF_KEYS; i++)
        {
            key[i].key_status = get_key_status(i);
            if ((key[i].key_status == KEY_SET) && (key[i].key_last_status == KEY_RESET))        //��������
            {
								//auto_shutdown_timer_reset();//added by qbc
                key[i].key_press_counter = 0;
                key[i].key_hold_counter = 0;
            }
            else if ((key[i].key_status == KEY_SET) && (key[i].key_last_status == KEY_SET)) //����δ�ſ�
            {
                if (++key[i].key_press_counter >= (KEY_HOLD_FIRST_COUNT + KEY_HOLD_INTERVAL))   //��������
                {
                    key[i].key_press_counter = KEY_HOLD_FIRST_COUNT;
                    key[i].key_hold_counter++;

                    if (key[i].key_hold_event != RT_NULL)
                    {
												//auto_shutdown_timer_reset();
                        key[i].key_hold_event();                        
                    }
                }

                if ((key[i].key_hold_counter == KEY_HOLD_1S_COUNT) &&
																							(key[i].key_press_counter == KEY_HOLD_FIRST_COUNT))       //�������³���1s
                {
                    if (key[i].key_hold_1s_event != RT_NULL)
                    {
												//auto_shutdown_timer_reset();
                        key[i].key_hold_1s_event();                        
                    }
                }

                if ((key[i].key_hold_counter == KEY_HOLD_3S_COUNT) &&
																							(key[i].key_press_counter == KEY_HOLD_FIRST_COUNT))       //�������³���3s
                {
                    if (key[i].key_hold_3s_event != RT_NULL)
                    {
												//auto_shutdown_timer_reset();
                        key[i].key_hold_3s_event();                        
                    }
                }
            }
            else if ((key[i].key_status == KEY_RESET) && (key[i].key_last_status == KEY_SET)) //�����ſ�
            {
                if ((key[i].key_press_counter >= KEY_DEBOUNCE_COUNT) &&
                        (key[i].key_press_counter < (KEY_HOLD_FIRST_COUNT + KEY_HOLD_INTERVAL)) &&
                        (key[i].key_hold_counter == 0))//�����̰�(������1s)
                {
                    if (key[i].key_press_event != RT_NULL)
                    {
												//auto_shutdown_timer_reset();
                        key[i].key_press_event();                        
                    }
                }
				
								key[i].key_press_counter = 0;
                key[i].key_hold_counter = 0;
                key[i].key_release_counter = 0;
            }
            else if ((key[i].key_status == KEY_RESET) && (key[i].key_last_status == KEY_RESET)) //����δ����
            {
                if (++key[i].key_release_counter >= KEY_RELEASE_1S_COUNT)
                {
										//auto_shutdown_timer_reset();
                    if (key[i].key_release_1s_event != RT_NULL)
                    {
                        key[i].key_release_1s_event();                        
                        key[i].key_release_counter = 0;
                    }
                }
            }
            key[i].key_last_status = key[i].key_status;
        }

        rt_thread_delay(2); //ÿ��20msɨ��һ�ΰ���
    }
}

void key_scan_init(void)
{
    rt_hw_key_init();
    key_handler_init();
		//auto_shutdown_timer_start(); //added by qbc
	
    key_scan_tid = rt_thread_create("Key_Scan",
                                key_scan_entry,
                                RT_NULL,
                                KEY_SCAN_THREAD_STACK_SIZE,
                                KEY_SCAN_THREAD_PRIORITY,
                                KEY_SCAN_THREAD_TIMESLICE);
    if (key_scan_tid != RT_NULL)
    {
        rt_thread_startup(key_scan_tid);
    }
}



static void key_wifi_control_event(void)
{
	/*
    static rt_uint32_t key_status = 1;

    if (key_status == 0)
    {
        wifi_reconnect();
        key_status = 1;
    }
    else
    {
        wifi_disconnect();
        key_status = 0;
    }
	*/
}
static void key_tfcard_init_event(void)
{
	/*
		tfcard_init();
		//rt_thread_delay(10);
		//tfcard_write_init();
		key[KEY_FORMAT].key_hold_3s_event = key_format_event;//��ʼ�����֮��ſ��Ը�ʽ��tfcard  
	*/
}
static void key_wifi_reset_event(void)
{
	/*
    buzz(1, 1000, 0);

    wifi_config_reset();
*/
}

static void key_lock_event(void)
{
    static rt_bool_t key_lock_flag = 0;
    static struct key_handler key_backup[NUM_OF_KEYS];
    rt_uint32_t i;

    if (key_lock_flag == 0)
    {
        for (i = 0; i < NUM_OF_KEYS; i++)
        {
            key_backup[i].key_last_status       = key[i].key_last_status;
            key_backup[i].key_status            = key[i].key_status;
            key_backup[i].key_press_counter     = key[i].key_press_counter;
            key_backup[i].key_hold_counter      = key[i].key_hold_counter;
            key_backup[i].key_release_counter 	= key[i].key_release_counter;
            key_backup[i].key_press_event       = key[i].key_press_event;
            key_backup[i].key_hold_event        = key[i].key_hold_event;
            key_backup[i].key_hold_1s_event     = key[i].key_hold_1s_event;
            key_backup[i].key_hold_3s_event     = key[i].key_hold_3s_event;
            key_backup[i].key_release_1s_event  = key[i].key_release_1s_event;
        }

        for (i = 0; i < NUM_OF_KEYS; i++)
        {
            key[i].key_last_status = 0;
            key[i].key_status = 0;
            key[i].key_press_counter = 0;
            key[i].key_hold_counter = 0;
            key[i].key_release_counter = 0;
            key[i].key_press_event = RT_NULL;
            key[i].key_hold_event = RT_NULL;
            key[i].key_hold_1s_event = RT_NULL;
            key[i].key_hold_3s_event = RT_NULL;
            key[i].key_release_1s_event = RT_NULL;
        }

				//key[KEY_POWER].key_release_1s_event = power_off;
				//key[KEY_EVENT_TAG].key_hold_3s_event = data_add_event_tag_event; // �¼���ť��������״̬��Ҫ��˼���Ȼ��Ч�����Բ��Դ˼��������� 
       // key[KEY_START_AND_CONFIG].key_press_event = key_lock_event;
       // key[KEY_CONFIG].key_hold_3s_event = key_wifi_reset_event;
		

        key_lock_flag = 1;
    }
    else
    {
        for (i = 0; i < NUM_OF_KEYS; i++)
        {
            key[i].key_last_status      = key_backup[i].key_last_status;
            key[i].key_status           = key_backup[i].key_status;
            key[i].key_press_counter    = key_backup[i].key_press_counter;
            key[i].key_hold_counter     = key_backup[i].key_hold_counter;
            key[i].key_release_counter  = key_backup[i].key_release_counter;
            key[i].key_press_event      = key_backup[i].key_press_event;
            key[i].key_hold_event       = key_backup[i].key_hold_event;
            key[i].key_hold_1s_event    = key_backup[i].key_hold_1s_event;
            key[i].key_hold_3s_event    = key_backup[i].key_hold_3s_event;
            key[i].key_release_1s_event = key_backup[i].key_release_1s_event;
        }
      //  send_display_state_event(DISPLAY_KEY_UNLOCK);
        key_lock_flag = 0;
    }
}


static void key_start_event(void)
{
	/*
		if(tfcard_ready == RT_FALSE)
				return;//tfcard��δ��ʼ��
		
		if((whocap != NONE)&&(whocap != TFCARD))
				return;//�Ȳ��Ǵ������˲ɼ�״̬Ҳ���Ǵ���tfcard�ɼ�״̬
		
    if (key_start_flag == 0)
    {			
        send_display_state_event(DISPLAY_STRING_CONFIRM_ENABLE_ACQ);
        key[KEY_START].key_press_event = dynamic_acquisition_enable_event;
        key[KEY_FORMAT].key_press_event = key_cancel_event;
    }
    else
    {		
        send_display_state_event(DISPLAY_STRING_CONFIRM_DISABLE_ACQ);
        key[KEY_START].key_press_event = dynamic_acquisition_disable_event;
        key[KEY_FORMAT].key_press_event = cancel_dynamic_acquisition_disable_event;
    }
	*/
//    rt_thread_delay(100);
}

static void key_config_event(void)
{	
	/*
    send_display_state_event(DISPLAY_CHANNEL_NUMBER_BLINK);	
    key[KEY_START].key_press_event = switch_total_channel_number_event;
    key[KEY_FORMAT].key_press_event = cancel_switch_total_channel_number_event;
	*/
}

static void key_format_event(void)
{		
	/*
		if(whocap != NONE)//�ɼ�״̬�£���ֹ��ʽ��
				return;
		if(tfcard_ready != RT_TRUE)//tfcard��δ��ʼ������ֹ��ʽ��
				return;
		
    send_display_state_event(DISPLAY_STRING_CONFIRM_FORMAT); 
		key[KEY_START].key_press_event = format_SDCard_event;
    key[KEY_START].key_hold_3s_event = RT_NULL;
    key[KEY_FORMAT].key_press_event = key_cancel_event;
//    rt_thread_delay(100);
	*/
	
}

static void dynamic_acquisition_enable_event(void)//
{			
	/*
		//tfcard_init();
#if 0		
		if(tfcard_ready == RT_FALSE)
		{
			tfcard_init();
			tfcard_write_init();
			rt_thread_delay(100);
		}
#endif
	
		key[KEY_START].key_press_event = RT_NULL;	
		key[KEY_CONFIG_AND_FORMAT].key_press_event = RT_NULL;
		key[KEY_FORMAT].key_press_event = RT_NULL;    
		key[KEY_FORMAT].key_hold_3s_event = RT_NULL; 
		//key[KEY_EVENT_TAG].key_hold_3s_event = data_add_event_tag_event;
		
		wifi_disconnect();	
		//wifi_hw_powerdown();
#if 0 //�˴�����ɾ������̣߳���Ϊɾ������̻߳ᵼ����Դй¶�����ɾ���ʹ�������Դ����������wifi_send�߳��޷��ٴ���
		if (wifi_send != RT_NULL)
		{
			rt_thread_delete(wifi_send);
			wifi_send = RT_NULL;
		}	
#endif
		//wifi_power_down();//�����������ֱ�ӶϿ�WiFi��Դ����Ϊ�����߳���Ҫ�ȴ�WiFi״̬
		rt_thread_delay(100);
		
		//tfcard_init();
		
		
		dynamic_acq_enable_time = time(RT_NULL);
		send_display_state_event(DISPLAY_STRING_DYNAMIC_ACQ_TIME);	

		rt_enter_critical();
		one_wire_device_num[0] = one_wire_family_search(0, &one_wire_data[0][0], DS2401_FAMILY_CODE, 0);			
		rt_exit_critical();				

		
		//create_initial_EEG_data_file(file_path);	
#if  0
		for (int i = 0; i < sizeof(acq_para.channel_config); i++)
		{
			acq_para.channel_config[i] = 0;
		}	
		dynamic_acquisition_channel_config(&acq_para, current_chan_num);	
#endif	
		process_channel_config(&acq_para);//����ͨ�����������fpga

		//fsmc_write(0x15, 32);//Ĭ��32ͨ��
		ads1299_set_rate(1);	//����Ƶ��256		
		
		
		//�����������ʹ��������ʼ����aa55aa55
		ads1299_fpga_data_ready = RT_FALSE; //ȷ��״̬�ı�Ҫ�����ڶ�ȡfpga����֮ǰ
		rt_thread_delay(100);//��ʱ��ȷ��ads1299_thread_entry���˳�������״̬
		
		
		rt_mutex_take(&ads1299_package.mutex, RT_WAITING_FOREVER);
		//ads1299_package.rbuffer.read_mirror = ads1299_package.rbuffer.write_mirror;
		//ads1299_package.rbuffer.read_index = ads1299_package.rbuffer.write_index;
		ads1299_package.rbuffer.read_mirror = ads1299_package.rbuffer.write_mirror = 0;
		ads1299_package.rbuffer.read_index = ads1299_package.rbuffer.write_index = 0;
		
		rt_mutex_release(&ads1299_package.mutex);
				
		rt_mutex_take(&tfcard_package.mutex, RT_WAITING_FOREVER);
		//tfcard_package.rbuffer.read_mirror = tfcard_package.rbuffer.write_mirror;
		//tfcard_package.rbuffer.read_index = tfcard_package.rbuffer.write_index;
		tfcard_package.rbuffer.read_mirror = tfcard_package.rbuffer.write_mirror = 0;
		tfcard_package.rbuffer.read_index = tfcard_package.rbuffer.write_index = 0;
		rt_mutex_release(&tfcard_package.mutex);	
		
		dynamic_acq_flag = RT_TRUE;//��ʱע��
		
		//ads1299_enable(SAMPLE);//�պ���
		key_start_flag = 1;
		
		
		create_file_flag = RT_TRUE;//ֻ�����ֶ�ʹ�ܶ�̬ʱ����һ���ļ����Ҷ�̬��¼�����в��ٴ����µ��ļ�
		
		whocap = TFCARD;//tfcard�ɼ�״̬
		auto_shutdown_timer_stop();//��ʱ��
		
		temp_ads1299_flag = RT_TRUE;
		rt_thread_delay(1000);
		ads1299_enable(0);//����������
		*/
}

static void dynamic_acquisition_disable_event(void)
{
/*
	key[KEY_START].key_press_event = RT_NULL;
	
#ifdef TEMP_NO_COMPILE
#else				
	if((IS_SD_CARD_DETECTED == RT_TRUE) && (IS_CARD_READER_DETECTED == RT_FALSE))
#endif
	{
		dynamic_acquisition_disable();
		
		send_display_state_event(DISPlAY_STIRNG_COMPANY_NAME);
		//key[KEY_EVENT_TAG].key_hold_3s_event = RT_NULL;
		key[KEY_FORMAT].key_hold_3s_event = key_format_event; 
		key[KEY_CONFIG_AND_FORMAT].key_press_event = key_wifi_control_event;
	}	
	//else//Ĭ��tf���Ѿ�����
	//{
	//	key_cancel_event();
	//}
	key_start_flag = 0;
	
	create_file_flag = RT_FALSE;//ֻ�����ֶ�ʹ�ܶ�̬ʱ����һ���ļ����Ҷ�̬��¼�����в��ٴ����µ��ļ�
	
	whocap = NONE;//�˳�tfcard�ɼ�״̬
	
	//CH378FileClose(1);//������
	
	auto_shutdown_timer_start();//�˳���̬ʱ������ʱ��
	*/
}

static void cancel_dynamic_acquisition_disable_event(void)
{		
    //send_display_state_event(DISPLAY_STRING_DYNAMIC_ACQ_TIME);
}

static void switch_total_channel_number_event(void)
{	
  //  switch_total_channel_number();
}

static void cancel_switch_total_channel_number_event(void)
{
    key_handler_init();
    //send_display_state_event(DISPLAY_CHANNEL_NUMBER_STEADY);
}

static void format_SDCard_event(void)
{
	/*
	key[KEY_START].key_press_event = RT_NULL;
	
	send_display_state_event(DISPlAY_STIRNG_FORMATING);

	CH378EraseAll();
	CH378EraseAll();
	CH378EraseAll();
	
	disp_timer.background = DISPLAY_BACKGROUND_REFRESH_INTERVAL;
	storage_space_display_ctr = 5;//����ˢ��tfcard����
	
	key_cancel_event();
*/
}

static void key_cancel_event(void)
{
	
    key_handler_init();		
  //  send_display_state_event(DISPlAY_STIRNG_COMPANY_NAME);
	
}

void data_add_event_tag_event(void)
{		
	/*
		if(whocap == NONE)
				return;
		
    data_add_event_tag();
    buzz(1, 500, 0);
	*/
}

void dynamic_acquisition_disable(void)
{
		/*
	if(dynamic_acq_flag == RT_TRUE)
	{
#if 0//����߳����Ű�  ��Ӱ������
		if (tfcard_write_tid != RT_NULL)
		{
			rt_thread_delete(tfcard_write_tid);
			tfcard_write_tid = RT_NULL;
		}
#endif 
		rt_thread_delay(100);
		
		//wifi_power_on();
#if 0 //�˴�����ɾ������̣߳���Ϊɾ������̻߳ᵼ����Դй¶�����ɾ���ʹ�������Դ����������wifi_send�߳��޷��ٴ���
		wifi_send_init();	
#endif
		wifi_reconnect();
	
		key_handler_init();
		ads1299_disable(SAMPLE);
		dynamic_acq_flag = RT_FALSE;
		key_start_flag = 0;
		
	}
*/
			
}

static void key_display_waveform_event(void)
{
	/*
    static rt_bool_t display_waveform_flag = 0;
    static struct key_handler key_start_temp[2];

    if (display_waveform_flag == 0)
    {
        key_start_temp[0].key_hold_1s_event = key[KEY_START].key_hold_1s_event;
        key_start_temp[0].key_hold_3s_event = key[KEY_START].key_hold_3s_event;
        key_start_temp[1].key_hold_1s_event = key[KEY_FORMAT].key_hold_1s_event;
        key_start_temp[1].key_hold_3s_event = key[KEY_FORMAT].key_hold_3s_event;

        key[KEY_START].key_press_event   = switch_display_channel;
        key[KEY_START].key_hold_1s_event = RT_NULL;
        key[KEY_START].key_hold_3s_event = RT_NULL;

        key[KEY_FORMAT].key_press_event     = switch_display_gain;
        key[KEY_FORMAT].key_hold_1s_event = RT_NULL;
        key[KEY_FORMAT].key_hold_3s_event = RT_NULL;

        enter_display_waveform_mode();

        display_waveform_flag = 1;
    }
    else
    {
        exit_display_waveform_mode();

        key[KEY_START].key_press_event   = RT_NULL;
        key[KEY_START].key_hold_1s_event = key_start_temp[0].key_hold_1s_event;
        key[KEY_START].key_hold_3s_event = key_start_temp[0].key_hold_3s_event;
        key[KEY_FORMAT].key_press_event   = RT_NULL;
        key[KEY_FORMAT].key_hold_1s_event = key_start_temp[1].key_hold_1s_event;
        key[KEY_FORMAT].key_hold_3s_event = key_start_temp[1].key_hold_3s_event;
        display_waveform_flag = 0;
    }
		*/
}