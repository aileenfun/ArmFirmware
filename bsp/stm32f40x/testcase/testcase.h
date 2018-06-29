#ifndef __TESTCASE_H__
#define __TESTCASE_H__

/*����ģʽ*/
//#define TEST_DEBUG	
//#define SRAM_DEBUG								
//#define WIFI_DEBUG								
//#define DATA_PROCESS_DEBUG				
//#define ADS1299_DEBUG							
//#define CPU_USAGE_MONITOR
//#define SDIO_DEBUG
//#define TEST_SIGNAL					//256�����Ҳ������ź�

#ifdef TEST_DEBUG
#include <stm32f10x.h>
#define	TEST_IO(n)	(n ? (GPIO_SetBits(GPIOG, GPIO_Pin_0)) : (GPIO_ResetBits(GPIOG, GPIO_Pin_0)))

void test_init(void);
#endif

#endif
