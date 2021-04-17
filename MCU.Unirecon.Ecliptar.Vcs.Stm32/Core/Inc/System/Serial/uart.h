#ifndef SYSTEM_SERIAL_UART_H
#define SYSTEM_SERIAL_UART_H

#include <inttypes.h>
#include <string.h>
#include <stdint.h>
#include "usart.h"
#include "dma.h"

#include "System/Serial/adapter.h"

class SerialDevice_UART : public SerialAdapter
{
public:
	virtual ~SerialDevice_UART() {}
	SerialDevice_UART(UART_HandleTypeDef* uartInstance);

	virtual void Receive(void* data, size_t length, uint16_t remoteAddress = 0);
	virtual void Transmit(void* data, size_t length, uint16_t remoteAddress = 0);

	virtual	 uint32_t 	DeviceAddress();

private:
	UART_HandleTypeDef* uart;
};

#endif /* SYSTEM_SERIAL_UART_H */
