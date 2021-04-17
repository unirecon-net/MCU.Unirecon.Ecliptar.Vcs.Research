#ifndef SYSTEM_SERIAL_ADAPTER_H
#define SYSTEM_SERIAL_ADAPTER_H

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"

#include "System/Data/ring.h"

typedef enum
{
	UART,
	SPI,
	I2C
}SerialHandlerType;

class SerialEvent // @suppress("Class has a virtual method and non-virtual destructor")
{
public:
	virtual void ReceiptCompleteEvent(Ring* buffer, size_t size) = 0;
	virtual void TransmitCompleteEvent(Ring* buffer, size_t size) = 0;
};

class SerialAdapter // @suppress("Class has a virtual method and non-virtual destructor")
{
public:
	virtual ~SerialAdapter() {}
	virtual void Receive(void* data, size_t length, uint16_t remoteAddress = 0) = 0;
	virtual void Transmit(void* data, size_t length, uint16_t remoteAddress = 0) = 0;

	virtual	 uint32_t 	DeviceAddress() = 0;


	DMA_HandleTypeDef* DMAReceiver;
	DMA_HandleTypeDef* DMATransmiter;

	SerialAdapter* adapter;
};

#endif /* SYSTEM_SERIAL_ADAPTER_H */
