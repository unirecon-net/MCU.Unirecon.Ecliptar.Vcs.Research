#ifndef SYSTEM_SERIAL_SPI_H
#define SYSTEM_SERIAL_SPI_H

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "spi.h"

#include "System/Serial/adapter.h"

class SerialDevice_SPI : public SerialAdapter
{
public:
	virtual ~SerialDevice_SPI() {}
	SerialDevice_SPI(SPI_HandleTypeDef* spiInstance);

	virtual void Receive(void* data, size_t length, uint16_t remoteAddress = 0);
	virtual void Transmit(void* data, size_t length, uint16_t remoteAddress = 0);

	virtual	 uint32_t 	DeviceAddress();

private:
	SPI_HandleTypeDef* spi;
};

#endif /* SYSTEM_SERIAL_SPI_H */
