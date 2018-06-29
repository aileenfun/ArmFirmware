#ifndef __KEY_SCAN_H__
#define __KEY_SCAN_H__

#include <rtthread.h>

#define KEY_SET                     0
#define KEY_RESET                   1

#define NUM_OF_KEYS                 9

#define KEY_EVENT_TAG               0                   //PI.8	�¼� 
#define KEY_POWER                   1                   //PI.5  ��Դ��
#define KEY_START                   2                   //PI.11	��ʼ/ֹͣ
#define KEY_CONFIG                  3                   //PA.2	���� SET
#define KEY_FORMAT                  4                   //PH.3	��ʽ�� CANCELL
#define KEY_START_AND_CONFIG    	5                   //PI.11 & PA.2
#define KEY_START_AND_FORMAT    	6                  	//PI.11 & PH.3
#define KEY_CONFIG_AND_FORMAT   	7               	//PA.2 & PH.3
#define KEY_EVENT_AND_CONFIG   		8               	//PI.8 & PA.2


#define KEY_DEBOUNCE_COUNT      	1           		//��������key_press_counter������ʼֵ
#define KEY_HOLD_FIRST_COUNT    	40          		//��ʼ����key_press_counter������ʼֵ
#define KEY_HOLD_INTERVAL           10          		//����key_press_counter��������
#define KEY_HOLD_1S_COUNT           1           		//����1s key_hold_counter����ֵ
#define KEY_HOLD_3S_COUNT           11          		//����3s key_hold_counter����ֵ
														//(KEY_HOLD_1S_COUNT + 1000 / (KEY_HOLD_INTERVAL    *20) * (3 - 1))
#define KEY_RELEASE_1S_COUNT    	50              	//�����ͷ�1s key_release_counter����ֵ

struct key_handler                              		//��������
{
    rt_uint8_t key_last_status;                 		//��һ��ɨ�谴��ֵ
    rt_uint8_t key_status;                      		//��ǰɨ�谴��ֵ
    rt_uint32_t key_press_counter;              		//�������¼�����
    rt_uint32_t key_hold_counter;               		//��������������
    rt_uint32_t key_release_counter;            		//�����ͷż�����
    void (*key_press_event)(void);              		//�����̰�ִ������
    void (*key_hold_event)(void);               		//��������ִ�е�����
    void (*key_hold_1s_event)(void);            		//��������1sִ�е�����
    void (*key_hold_3s_event)(void);            		//��������3sִ�е�����
    void (*key_release_1s_event)(void);     			//�����ͷ�3sִ�е�����
};


void power_on(void);
void power_off(void);
void key_scan_init(void);
void key_handler_init(void);
void dynamic_acquisition_disable(void);

#endif
