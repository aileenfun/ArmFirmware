
// Automatically generated header file for LINUX
// Generated by scripts/build_platform_config.py

#ifndef _PLATFORM_CONFIG_H
#define _PLATFORM_CONFIG_H
#include <rtthread.h>

#define PC_BOARD_ID "LINUX"
#define PC_BOARD_CHIP "LINUX"
#define PC_BOARD_CHIP_FAMILY "LINUX"



// SYSTICK is the counter that counts up and that we use as the real-time clock
// The smaller this is, the longer we spend in interrupts, but also the more we can sleep!
#define SYSTICK_RANGE 0x1000000 // the Maximum (it is a 24 bit counter) - on Olimexino this is about 0.6 sec
#define SYSTICKS_BEFORE_USB_DISCONNECT 2

#define DEFAULT_BUSY_PIN_INDICATOR (Pin)-1 // no indicator
#define DEFAULT_SLEEP_PIN_INDICATOR (Pin)-1 // no indicator

// When to send the message that the IO buffer is getting full
#define IOBUFFER_XOFF ((TXBUFFERMASK)*6/8)
// When to send the message that we can start receiving again
#define IOBUFFER_XON ((TXBUFFERMASK)*3/8)



#define RAM_TOTAL (-1*1024)
#define FLASH_TOTAL (-1*1024)

#define RESIZABLE_JSVARS // Allocate variables in blocks using malloc

#define USARTS                          0
#define SPIS                            1
#define I2CS                            0
#define ADCS                            0
#define DACS                            0

#define DEFAULT_CONSOLE_DEVICE              EV_USBSERIAL

#define IOBUFFERMASK 31 // (max 255) amount of items in event buffer - events take ~9 bytes each
#define TXBUFFERMASK 31 // (max 255)


// definition to avoid compilation when Pin/platform config is not defined
#define IS_PIN_USED_INTERNALLY(PIN) ((false))
#define IS_PIN_A_LED(PIN) ((false))
#define IS_PIN_A_BUTTON(PIN) ((false))

#endif // _PLATFORM_CONFIG_H

