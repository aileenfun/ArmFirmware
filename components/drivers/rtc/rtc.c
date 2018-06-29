/*
 * File      : rtc.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-01-29     aozima       first version.
 * 2012-04-12     aozima       optimization: find rtc device only first.
 * 2012-04-16     aozima       add scheduler lock for set_date and set_time.
 */

#include <time.h>
#include <string.h>
#include <rtthread.h>


#include "rtc.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_rcc.h"



/** \brief returns the current time.
 *
 * \param time_t * t the timestamp pointer, if not used, keep NULL.
 * \return time_t return timestamp current.
 *
 */
/* for IAR 6.2 later Compiler */
#if defined (__IAR_SYSTEMS_ICC__) &&  (__VER__) >= 6020000
#pragma module_name = "?time"
time_t (__time32)(time_t *t) /* Only supports 32-bit timestamp */
#else
time_t time(time_t *t)
#endif
{
    static rt_device_t device = RT_NULL;
    time_t time_now = 0;

    /* optimization: find rtc device only first. */
    //if (device == RT_NULL)
    {
        device = rt_device_find("rtc");
    }

    /* read timestamp from RTC device. */
    if (device != RT_NULL)
    {
        if (rt_device_open(device, 0) == RT_EOK)
        {
            rt_device_control(device, RT_DEVICE_CTRL_RTC_GET_TIME, &time_now);
            rt_device_close(device);
        }
    }

    /* if t is not NULL, write timestamp to *t */
    if (t != RT_NULL)
    {
        *t = time_now;
    }

    return time_now;
}

/** \brief set system date(time not modify).
 *
 * \param rt_uint32_t year  e.g: 2012.
 * \param rt_uint32_t month e.g: 12 (1~12).
 * \param rt_uint32_t day   e.g: e.g: 31.
 * \return rt_err_t if set success, return RT_EOK.
 *
 */
rt_err_t set_date(rt_uint32_t year, rt_uint32_t month, rt_uint32_t day)
{
    time_t now;
    struct tm *p_tm;
    struct tm tm_new;
    rt_device_t device;
    rt_err_t ret = -RT_ERROR;

    /* get current time */
    now = time(RT_NULL);

    /* lock scheduler. */
    rt_enter_critical();
    /* converts calendar time time into local time. */
    p_tm = localtime(&now);
    /* copy the statically located variable */
    memcpy(&tm_new, p_tm, sizeof(struct tm));
    /* unlock scheduler. */
    rt_exit_critical();

    /* update date. */
    tm_new.tm_year = year - 1900;
    tm_new.tm_mon  = month - 1; /* tm_mon: 0~11 */
    tm_new.tm_mday = day;

    /* converts the local time in time to calendar time. */
    now = mktime(&tm_new);

    device = rt_device_find("rtc");
    if (device == RT_NULL)
    {
        return -RT_ERROR;
    }

    /* update to RTC device. */
    ret = rt_device_control(device, RT_DEVICE_CTRL_RTC_SET_TIME, &now);

    return ret;
}

/** \brief set system time(date not modify).
 *
 * \param rt_uint32_t hour   e.g: 0~23.
 * \param rt_uint32_t minute e.g: 0~59.
 * \param rt_uint32_t second e.g: 0~59.
 * \return rt_err_t if set success, return RT_EOK.
 *
 */
rt_err_t set_time(rt_uint32_t hour, rt_uint32_t minute, rt_uint32_t second)
{
    time_t now;
    struct tm *p_tm;
    struct tm tm_new;
    rt_device_t device;
    rt_err_t ret = -RT_ERROR;

    /* get current time */
    now = time(RT_NULL);

    /* lock scheduler. */
    rt_enter_critical();
    /* converts calendar time time into local time. */
    p_tm = localtime(&now);
    /* copy the statically located variable */
    memcpy(&tm_new, p_tm, sizeof(struct tm));
    /* unlock scheduler. */
    rt_exit_critical();

    /* update time. */
    tm_new.tm_hour = hour;
    tm_new.tm_min  = minute;
    tm_new.tm_sec  = second;

    /* converts the local time in time to calendar time. */
    now = mktime(&tm_new);

    device = rt_device_find("rtc");
    if (device == RT_NULL)
    {
        return -RT_ERROR;
    }

    /* update to RTC device. */
    ret = rt_device_control(device, RT_DEVICE_CTRL_RTC_SET_TIME, &now);

    return ret;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
#include <rtdevice.h>

void list_date(void)
{
    time_t now;

    now = time(RT_NULL);
    rt_kprintf("%s\n", ctime(&now));
}
FINSH_FUNCTION_EXPORT(list_date, show date and time.)

FINSH_FUNCTION_EXPORT(set_date, set date. e.g: set_date(2010,2,28))
FINSH_FUNCTION_EXPORT(set_time, set time. e.g: set_time(23,59,59))
#endif


/**************************/
/*
 * Name               : initialize
 * Description        : ---
 * Author             : ysloveivy.
 *
 * History
 * -------------------
 * Rev                : 0.00
 * Date               : 11/23/2015
 * 
 * create.
 * -------------------
 */
int my_rtc_set_time(unsigned char hour,unsigned char min,unsigned char sec,unsigned char ampm)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);              //使能PWR时钟
	PWR_BackupAccessCmd(ENABLE);                                     //使能备份寄存器访问
	RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);

	RCC_LSEConfig(RCC_LSE_ON);                                       //LSE 开启    
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);              //检查指定的RCC标志位设置与否,等待低速晶振就绪
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);                          //设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
	RCC_RTCCLKCmd(ENABLE);                                           //使能RTC时钟 

	RTC_WaitForSynchro();                                            //等待APB1和RTC等时钟同步
	
	//设置时间
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;

	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12=ampm;

	RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);

	return 0;
}
/*
 * Name               : initialize
 * Description        : ---
 * Author             : ysloveivy.
 *
 * History
 * -------------------
 * Rev                : 0.00
 * Date               : 11/23/2015
 * 
 * create.
 * -------------------
 */
int my_rtc_set_date(unsigned char year,unsigned char month,unsigned char date,unsigned char week)
{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);              //使能PWR时钟
		PWR_BackupAccessCmd(ENABLE);                                     //使能备份寄存器访问
		RCC_BackupResetCmd(ENABLE);
		RCC_BackupResetCmd(DISABLE);

		RCC_LSEConfig(RCC_LSE_ON);                                       //LSE 开启    
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);              //检查指定的RCC标志位设置与否,等待低速晶振就绪
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);                          //设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);                                           //使能RTC时钟 

		RTC_WaitForSynchro();                                            //等待APB1和RTC等时钟同步
	//设置日期
	RTC_DateTypeDef RTC_DateTypeInitStructure;

	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;

	RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
	
	

	return 0;
}
int my_initialize(void)
{
	//判断读到的备份寄存器的值与写入的值是否一样
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);              //使能PWR时钟
		PWR_BackupAccessCmd(ENABLE);                                     //使能备份寄存器访问
		RCC_BackupResetCmd(ENABLE);
		RCC_BackupResetCmd(DISABLE);

		RCC_LSEConfig(RCC_LSE_ON);                                       //LSE 开启    
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);              //检查指定的RCC标志位设置与否,等待低速晶振就绪
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);                          //设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);                                           //使能RTC时钟 

		RTC_WaitForSynchro();                                            //等待APB1和RTC等时钟同步

		my_rtc_set_time(13,57,12,RTC_H12_PM);                               //设置时间
		my_rtc_set_date(16,10,23,6);                                        //设置日期

		RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);                    //向备份寄存器中写入数值
	}

	return 0;
}

