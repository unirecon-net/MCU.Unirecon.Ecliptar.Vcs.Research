/*
 * System/print.cpp
 *
 *  Created on: 24 mar 2021
 *      Author: merlin
 */

#include "System/print.h"

char printBuffer[255];
SerialDevice *PrintDevice;
void setPrinter(SerialDevice *printer)
{
	PrintDevice = printer;
}
uint8_t checkPrinter(SerialDevice *printer)
{
	if (PrintDevice != NULL && printer != NULL)
		return printer->DeviceAddress() == PrintDevice->DeviceAddress();
	return 0;
}
SerialDevice* getPrinter()
{
	return PrintDevice;
}
void uartprint(const char *fmt, va_list argp)
{
	if (PrintDevice != NULL)
	{
		if (0 < vsprintf(printBuffer, fmt, argp)) // build string
		{			
			int len = strlen(printBuffer);
			PrintDevice->Write(printBuffer, len); // send message via UART
			memset(printBuffer, 0, len);
		}
	}
}
void swvprint(const char *fmt, va_list argp)
{
	if (0 < vsprintf(printBuffer, fmt, argp)) // build string
	{
		int len = strlen(printBuffer);
		for (int i = 0; i < len; i++)
			ITM_SendChar((printBuffer[i]));
		memset(printBuffer, 0, len);
	}
}
void print(const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
#ifdef DEBUG
	swvprint(fmt, argp);
#else
	    uartprint(fmt, argp);
#endif
	va_end(argp);
}
void printnln(void)
{
	print("\r\n");
}
void println(const char *fmt, ...)
{
	va_list argp;
	print(fmt, argp);
	printnln();
}
void printxln(const char *fmt, ...)
{
	printnln();
	va_list argp;
	print(fmt, argp);
}
