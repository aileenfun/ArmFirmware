//#include <stm32f4xx.h>
#include "stm32f4xx.h"
#include "delay.h"

void delay_us_for_oneWire(rt_uint32_t nus)
{
    rt_uint32_t ticks;
    rt_uint32_t tick_old, tick_now, tick_cnt = 0;
    rt_uint32_t reload = SysTick->LOAD;                     //LOAD��ֵ

    tick_cnt = 0;
		//ticks = nus * (SystemCoreClock / 1000000);      //��Ҫ�Ľ�����
		ticks = nus * (SystemCoreClock / 1000000 / 10);      //��Ҫ�Ľ�����
	
    rt_enter_critical();                                                    //��������������ֹ���us��ʱ

    tick_old = SysTick->VAL;                                            //�ս���ʱ�ļ�����ֵ
    while (1)
    {
        tick_now = SysTick->VAL;

        if (tick_now != tick_old)                                       //SYSTICKΪ�ݼ�������
        {
            if (tick_now < tick_old)
            {
                tick_cnt += tick_old - tick_now;
            }
            else
            {
                tick_cnt += reload - tick_now + tick_old;
            }

            tick_old = tick_now;

            if (tick_cnt >= ticks)                                      //�ﵽ��ʱʱ��
            {
                break;
            }
        }
    }

    rt_exit_critical();                                                 //�رյ�������
}

void delay_us(rt_uint32_t nus)
{
    rt_uint32_t ticks;
    rt_uint32_t tick_old, tick_now, tick_cnt = 0;
    rt_uint32_t reload = SysTick->LOAD;                     //LOAD��ֵ

    tick_cnt = 0;
		ticks = nus * (SystemCoreClock / 1000000);      //��Ҫ�Ľ�����

    rt_enter_critical();                                                    //��������������ֹ���us��ʱ

    tick_old = SysTick->VAL;                                            //�ս���ʱ�ļ�����ֵ
    while (1)
    {
        tick_now = SysTick->VAL;

        if (tick_now != tick_old)                                       //SYSTICKΪ�ݼ�������
        {
            if (tick_now < tick_old)
            {
                tick_cnt += tick_old - tick_now;
            }
            else
            {
                tick_cnt += reload - tick_now + tick_old;
            }

            tick_old = tick_now;

            if (tick_cnt >= ticks)                                      //�ﵽ��ʱʱ��
            {
                break;
            }
        }
    }

    rt_exit_critical();                                                 //�رյ�������
}

void delay_ms(rt_uint32_t nms)
{
    rt_uint32_t fac_ms = 1000 / RT_TICK_PER_SECOND;

    if (nms >= fac_ms)
    {
        rt_thread_delay(nms / fac_ms);                      			//RT-Thread��ʱ
    }

    nms %= fac_ms;                                                      //RT-Thread�޷��ṩС��ϵͳʱ�ӽ��ĵ���ʱ

    delay_us((rt_uint32_t)(nms * 1000));
}
