#ifndef SYSTEM_SERIAL_I2C_H
#define SYSTEM_SERIAL_I2C_H

#define I2C_DEFAULT_MEMADDSIZE 1

#include <inttypes.h>
#include <string.h>
#include <stdint.h>
#include "i2c.h"
#include "System/Serial/adapter.h"

class SerialDevice_I2C : public SerialAdapter
{
public:
	virtual ~SerialDevice_I2C() {}
	SerialDevice_I2C(I2C_HandleTypeDef* i2cInstance, uint8_t peripheralAddress);

	virtual void Receive(void* data, size_t length, uint16_t remoteAddress = 0);
	virtual void Transmit(void* data, size_t length, uint16_t remoteAddress = 0);

	virtual	 uint32_t 	DeviceAddress();

private:
	I2C_HandleTypeDef* i2c;
	uint8_t				 peripheralAddress;
};

#endif /* SYSTEM_SERIAL_I2C_H */
