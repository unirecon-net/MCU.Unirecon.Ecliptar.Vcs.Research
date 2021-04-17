#include "System/Serial/spi.h"

SerialDevice_SPI::SerialDevice_SPI(SPI_HandleTypeDef *spi)
{
	this->spi = spi;
		DMAReceiver = spi->hdmarx;
		DMATransmiter = spi->hdmatx;
		__HAL_SPI_ENABLE_IT(spi, SPI_IT_RXNE);
}

void SerialDevice_SPI::Receive(void* data, size_t length, uint16_t remoteAddress)
{
	HAL_SPI_Receive_DMA(spi, (uint8_t*)data, length);
}
void SerialDevice_SPI::Transmit(void* data, size_t length, uint16_t remoteAddress)
{
	HAL_SPI_Transmit_DMA(spi, (uint8_t*)data, length);
}
uint32_t SerialDevice_SPI::DeviceAddress()
{
	return	(uint32_t)spi->Instance;
}
