/*
 * ecliptar_sys_print.h
 *
 *  Created on: 24 mar 2021
 *      Author: merlin
 */

#ifndef _SYSTEM_PRINT_H_
#define __SYSTEM_PRINT_H_

#include <stdarg.h>
#include "usart.h"
#include "System/serial.h"
#include "System/Serial/usb.h"

void setPrinter(SerialDevice* printer);
uint8_t checkPrinter(SerialDevice* printer);
SerialDevice* getprinter();
void printnln(void);
void uartprint(UART_HandleTypeDef* uart, const char *fmt, va_list argp);
void swvprint(const char *fmt, va_list argp);
void print(const char *fmt, ...);
void println(const char *fmt, ...);
void printxln(const char *fmt, ...);

#endif /* __SYSTEM_PRINT_H_ */
