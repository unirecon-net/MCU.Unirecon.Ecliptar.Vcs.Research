#include "System/Serial/uart.h"

SerialDevice_UART::SerialDevice_UART(UART_HandleTypeDef *uart)
{
	this->uart = uart;
	DMAReceiver = uart->hdmarx;
	DMATransmiter = uart->hdmatx;
	__HAL_UART_ENABLE_IT(uart, UART_IT_IDLE);
}

void 	SerialDevice_UART::Receive(void* data, size_t length, uint16_t remoteAddress)
{
	HAL_UART_Receive_DMA(uart, (uint8_t*)data, length);
}
void 	SerialDevice_UART::Transmit(void* data, size_t length, uint16_t remoteAddress)
{
	HAL_UART_Transmit_DMA(uart, (uint8_t*)data, length);
}

uint32_t SerialDevice_UART::DeviceAddress()
{
return (uint32_t)uart->Instance;
}

