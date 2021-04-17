/*
 * System/print.cpp
 *
 *  Created on: 24 mar 2021
 *      Author: merlin
 */

#include "System/scan.h"

char scanBuffer[255];
SerialDevice *ScanDevice;
void setScanner(SerialDevice *scanner)
{
	ScanDevice = scanner;
}
uint8_t checkScanner(SerialDevice *scanner)
{
	if (ScanDevice != NULL && scanner != NULL)
		return scanner->DeviceAddress() == ScanDevice->DeviceAddress();
	return 0;
}
SerialDevice* getScanner()
{
	return ScanDevice;
}
int16_t uartscan(uint16_t timeout, const char *fmt, va_list argp)
{
	if (ScanDevice != NULL)
	{
		while (!ScanDevice->Available())
		{
			HAL_Delay(1);
			timeout--;
			if (timeout < 1)
				return -1;
		}

		uint16_t len = ScanDevice->Read((uint8_t*) scanBuffer,
				sizeof(scanBuffer));
		if (len > 0)
		{
			if (0 < vsscanf(scanBuffer, fmt, argp)) // build string
			{
				memset((uint8_t*) scanBuffer, 0, len);
			}
		}
		return len;
	}
	return -1;
}
int16_t scandata(uint16_t timeout, void *data, size_t size)
{
	if (ScanDevice != NULL && data != NULL && size > 0)
	{
		while (!ScanDevice->Available())
		{
			HAL_Delay(1);
			timeout--;
			if (timeout < 1)
				return -1;
		}
		return ScanDevice->Read((uint8_t*) data, size);
	}
	return -1;
}
int16_t scan(uint16_t timeout, const char *fmt, ...)
{
	int32_t l = -1;
#ifndef DEBUG
	va_list argp;
	va_start(argp, fmt);
	l = uartscan(timeout, fmt, argp);
	va_end(argp);
#endif
	return l;
}
int16_t scanfull(uint16_t timeout, void *data)
{
	if (ScanDevice != NULL && data != NULL)
	{
		size_t size = sizeof(data);
		if (size > 0)
		{
			while (!ScanDevice->Available())
			{
				HAL_Delay(1);
				timeout--;
				if (timeout < 1)
					return -1;
			}
			return ScanDevice->Read((uint8_t*) data, size);
		}
	}
	return -1;
}

