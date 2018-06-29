/************************************版权申明********************************************
**                             广州大彩光电科技有限公司
**                             http://www.gz-dc.com
**-----------------------------------文件信息--------------------------------------------
** 文件名称:   hmi_user_uart.c
** 修改时间:   2011-05-18
** 文件说明:   用户MCU串口驱动函数库
** 技术支持：  Tel: 020-82186683  Email: hmi@gz-dc.com Web:www.gz-dc.com
--------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------
                                  使用必读
   hmi_user_uart.c中的串口发送接收函数共3个函数：串口初始化Uartinti()、发送1个字节SendChar()、
   发送字符串SendStrings().若移植到其他平台，需要修改底层寄
   存器设置,但禁止修改函数名称，否则无法与HMI驱动库(hmi_driver.c)匹配。
--------------------------------------------------------------------------------------



----------------------------------------------------------------------------------------
                          1. 基于STM32平台串口驱动
----------------------------------------------------------------------------------------*/
#include <rtthread.h>
#include "hmi_user_uart.h"
#include "usart4.h"
/****************************************************************************
* 名    称： UartInit()
* 功    能： 串口初始化
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
volatile uint32  timer_tick_count; //定时器节拍
static void timeout1(void* parameter)
{
	timer_tick_count++;
}
void InitDisplayer()
{
	static rt_timer_t timer1;
	  timer1 = rt_timer_create("timer1",  
                            timeout1, 
                            RT_NULL, 
                            1, 
                            RT_TIMER_FLAG_PERIODIC); 

    if (timer1 != RT_NULL)
        rt_timer_start(timer1);
		

		
}

void UartInit(uint32 BaudRate)
{

}


/*****************************************************************
* 名    称： SendChar()
* 功    能： 发送1个字节
* 入口参数： t  发送的字节
* 出口参数： 无
 *****************************************************************/
void  SendChar(uchar t)
{
  usart4.send_byte(t);
}
