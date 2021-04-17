#include "System/Serial/i2c.h"

SerialDevice_I2C::SerialDevice_I2C(I2C_HandleTypeDef *i2c, uint8_t peripheralAddress)
{
	this->i2c = i2c;
		DMAReceiver = i2c->hdmarx;
		DMATransmiter = i2c->hdmatx;
		this->peripheralAddress = peripheralAddress;
}

void SerialDevice_I2C::Receive(void* data, size_t length, uint16_t remoteAddress)
{
	HAL_I2C_Mem_Read_DMA(i2c, peripheralAddress, remoteAddress, 1, (uint8_t*)data, length);
}
void SerialDevice_I2C::Transmit(void* data, size_t length, uint16_t remoteAddress)
{
	HAL_I2C_Mem_Read_DMA(i2c, peripheralAddress, remoteAddress, 1, (uint8_t*)data, length);
}

uint32_t SerialDevice_I2C::DeviceAddress()
{
	return	(uint32_t)i2c->Instance;
}

