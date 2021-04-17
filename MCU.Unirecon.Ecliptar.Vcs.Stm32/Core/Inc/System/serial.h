#ifndef _SYSTEM_SERIAL_H
#define _SYSTEM_SERIAL_H

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "System/base.h"
#include "System/Data/ring.h"
#include "System/hash.h"
#include "System/Serial/adapter.h"
#include "System/Serial/i2c.h"
#include "System/Serial/spi.h"
#include "System/Serial/uart.h"

#define SERIAL_TX_STREAM_SIZE 256
#define SERIAL_TX_MAX_LENGTH 128

#define SERIAL_RX_STREAM_SIZE 256
#define SERIAL_RX_MAX_LENGTH 128

class SerialDevice : public SerialAdapter
{
public:
	~SerialDevice(){}
	SerialDevice(UART_HandleTypeDef* uart);
	SerialDevice(SPI_HandleTypeDef* spi);
	SerialDevice(I2C_HandleTypeDef* i2c, uint8_t peripheralAddress);

	void 			Open();
	void 			Open(size_t rxsize, size_t txsize);
	void 			Open(void* rxbuffer, size_t rxsize, void* txbuffer, size_t txsize);

	u8		 		Available();

	void    		Write(const void* data, size_t length);
	u8		 		Read();
	u8		 		ReadedByte();
	size_t  		Read(void *data, size_t length);

	virtual void 	Receive(void* data, size_t length, u16 remoteAddress = 0);
	virtual void 	Transmit(void* data, size_t length, u16 remoteAddress = 0);

	virtual	 u32 	DeviceAddress();

	void 			ReceiptComplete(void);
	void 			TransmitComplete(void);

	void 			SetReceiptEvent(SerialEvent* event);
	void 			SetTransmitEvent(SerialEvent* event);

	SerialHandlerType HandlerType;

	i32				SerialCode;

private:

	Ring* 	receiptBuffer;
	Ring* 	transmitBuffer;

	SerialEvent* receiptEvent;
	SerialEvent* transmitEvent;

	u32 		receiptLength = 0;
	u32			transmitLength = 0;

	u32 		lastReceiptPosition = 0;
	u32 		maxReceiptLength;
	u32 		receiptBufferSize;
	u32 		maxTransmitLength;
	u32 		transmitBufferSize;
};

class SerialDevices
{
public:
	SerialDevice* 		Table[17];
	u8			    	Count = 0;
	void			   	Add(SerialDevice *serialCom);
	SerialDevice*		Get(uint32_t key);
	uint8_t				Remove(uint32_t key);
};

extern SerialDevices OpenedSerialDevices;

#endif /* _SYSTEM_SERIAL_H */
