/*
 * System/print.h
 *
 *  Created on: 24 mar 2021
 *      Author: merlin
 */

#ifndef _SYSTEM_SCAN_H_
#define _SYSTEM_SCAN_H_

#include <stdarg.h>
#include "usart.h"

#include "System/serial.h"
#include "System/Serial/usb.h"

void setScanner(SerialDevice* scanner);
uint8_t checkScanner(SerialDevice* scanner);
SerialDevice* getScanner();
int16_t uartscan(uint16_t timeout, const char *fmt, va_list argp);
int16_t scan(uint16_t timeout, const char *fmt, ...);
int16_t scandata(uint16_t timeout, void* buffer, size_t size);
int16_t scanfull(uint16_t timeout, void* buffer);


#endif /* _SYSTEM_SCAN_H_ */
