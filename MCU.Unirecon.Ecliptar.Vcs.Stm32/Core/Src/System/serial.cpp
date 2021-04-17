/*
 * System/serial.cpp
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */

#include "System/serial.h"

				SerialDevice::SerialDevice(UART_HandleTypeDef *uart)
{
	SerialDevice_UART *c = new SerialDevice_UART(uart);
	adapter = c;
	DMAReceiver = c->DMAReceiver;
	DMATransmiter = c->DMATransmiter;
	HandlerType = UART;
	uint32_t adr = (uint32_t) uart->Instance;
	SerialCode = hashcode32(&adr, 4);
	OpenedSerialDevices.Add(this);
}
				SerialDevice::SerialDevice(SPI_HandleTypeDef *spi)
{
	SerialDevice_SPI *c = new SerialDevice_SPI(spi);
	adapter = c;
	DMAReceiver = c->DMAReceiver;
	DMATransmiter = c->DMATransmiter;
	HandlerType = SPI;
	uint32_t adr = (uint32_t) spi->Instance;
	SerialCode = hashcode32(&adr, 4);
	OpenedSerialDevices.Add(this);
}
				SerialDevice::SerialDevice(I2C_HandleTypeDef *i2c, uint8_t peripheralAddress)
{
	SerialDevice_I2C *c = new SerialDevice_I2C(i2c, peripheralAddress);
	adapter = c;
	DMAReceiver = c->DMAReceiver;
	DMATransmiter = c->DMATransmiter;

	HandlerType = I2C;
	uint32_t adr = (uint32_t) i2c->Instance;
	SerialCode = hashcode32(&adr, 4);
	OpenedSerialDevices.Add(this);
}

void 			SerialDevice::Open()
{
	receiptBuffer = new Ring(SERIAL_RX_STREAM_SIZE);
	maxReceiptLength = SERIAL_RX_MAX_LENGTH;
	receiptBufferSize = SERIAL_RX_STREAM_SIZE;
	transmitBuffer = new Ring(SERIAL_TX_STREAM_SIZE);
	maxTransmitLength = SERIAL_TX_MAX_LENGTH;
	transmitBufferSize = SERIAL_TX_STREAM_SIZE;

	Receive(receiptBuffer->Data.buffer, receiptBuffer->Data.size);
}
void 			SerialDevice::Open(size_t rxsize, size_t txsize)
{
	receiptBuffer = new Ring(rxsize);
	maxReceiptLength = rxsize / 2;
	receiptBufferSize = rxsize;

	transmitBuffer = new Ring(txsize);
	maxTransmitLength = txsize / 2;
	transmitBufferSize = txsize;

	Receive(receiptBuffer->Data.buffer, receiptBuffer->Data.size);
}
void 			SerialDevice::Open(void *rxbuffer, size_t rxsize, void *txbuffer,
		size_t txsize)
{
	receiptBuffer = new Ring(rxbuffer, rxsize);
	maxReceiptLength = rxsize / 2;
	receiptBufferSize = rxsize;

	transmitBuffer = new Ring(txbuffer, txsize);
	maxTransmitLength = txsize / 2;
	transmitBufferSize = txsize;

	Receive(receiptBuffer->Data.buffer, receiptBuffer->Data.size);
}

uint8_t 		SerialDevice::Available()
{
	return receiptBuffer->Available();
}

void 			SerialDevice::Write(const void *data, size_t length)
{
	transmitBuffer->Write(data, length);
	Transmit(NULL, 0);
}
uint8_t 		SerialDevice::Read()
{
	return receiptBuffer->Read();
}
uint8_t 		SerialDevice::ReadedByte()
{
	return receiptBuffer->ReadedByte();
}
size_t 			SerialDevice::Read(void *data, size_t length)
{
	return receiptBuffer->Read(data, length);
}

void 			SerialDevice::Receive(void *data, size_t length, uint16_t remoteAddress)
{
	adapter->Receive(data, length, remoteAddress);
}
void 			SerialDevice::Transmit(void *data, size_t length, uint16_t remoteAddress)
{
	if (data != NULL)
	{
		Write(data, length);
	}
	else
	{
		if (transmitLength)
			return;
		transmitLength = transmitBuffer->ReadLinearlength();
		if (transmitLength)
		{
			void *dataPtr = transmitBuffer->ReadAddress();
			adapter->Transmit(dataPtr, transmitLength, remoteAddress);
		}
		adapter->Transmit(data, length, remoteAddress);
	}
}

uint32_t 		SerialDevice::DeviceAddress()
{
	return adapter->DeviceAddress();
}

inline void	SerialDevice::ReceiptComplete(void)
{
	uint32_t receiptPosition = DMA_DATA_LENGTH(receiptBufferSize, DMAReceiver); //get current write position
	if (receiptPosition != lastReceiptPosition)
	{
		if (receiptPosition > lastReceiptPosition)
		{
			receiptLength = (receiptPosition - lastReceiptPosition);
		}
		else
		{
			receiptLength = (receiptBufferSize - lastReceiptPosition)
					+ receiptPosition;
		}
		receiptBuffer->Step(receiptLength); // Increase Ring reader - if assigned SetReceiptEvent also trigger process pass Buffer as paramter
		if(receiptEvent != NULL)
			receiptEvent->ReceiptCompleteEvent(receiptBuffer, receiptLength);
	}
	lastReceiptPosition = receiptPosition;

	if (lastReceiptPosition == receiptBufferSize)
		lastReceiptPosition = 0;
}
inline void	SerialDevice::TransmitComplete(void)
{
	if (transmitLength)
	{
		transmitBuffer->Skip(transmitLength);
		if(transmitEvent != NULL)
			transmitEvent->TransmitCompleteEvent(transmitBuffer, transmitLength);
		transmitLength = 0;
		Transmit(NULL, 0);
	}
}

void 			SerialDevice::SetReceiptEvent(SerialEvent* event)
{
	receiptEvent = event;
}
void 			SerialDevice::SetTransmitEvent(SerialEvent* event)
{
	transmitEvent = event;
}

void 			SerialDevices::Add(SerialDevice *serialDevice)
{
	int id = hashtableId(serialDevice->DeviceAddress(), 17);

	if (OpenedSerialDevices.Table[id] == NULL)
		OpenedSerialDevices.Table[id] = serialDevice;
	else
	{
		for (int i = 0; i < OpenedSerialDevices.Count; i++)
		{
			if (OpenedSerialDevices.Table[i] == NULL)
				OpenedSerialDevices.Table[i] = serialDevice;
		}
	}

	OpenedSerialDevices.Count++;
}
SerialDevice* 	SerialDevices::Get(uint32_t deviceAddress)
{
	int id = hashtableId(deviceAddress, 17);
	SerialDevice *result = OpenedSerialDevices.Table[id];

	if (result->DeviceAddress() == deviceAddress)
		return result;

	for (int i = 0; i < OpenedSerialDevices.Count; i++)
	{
		result = OpenedSerialDevices.Table[i];
		if (result->DeviceAddress() == deviceAddress)
			return result;
	}

	return NULL;
}
uint8_t 		SerialDevices::Remove(uint32_t deviceAddress)
{
	int id = hashtableId(deviceAddress, 17);
	SerialDevice *result = OpenedSerialDevices.Table[id];

	if (result->DeviceAddress() == deviceAddress)
	{
		OpenedSerialDevices.Table[id] = NULL;
		return 1;
	}

	for (int i = 0; i < OpenedSerialDevices.Count; i++)
	{
		result = OpenedSerialDevices.Table[i];
		if (result->DeviceAddress() == deviceAddress)
		{
			OpenedSerialDevices.Table[i] = NULL;
			return 1;
		}
	}
	return 0;
}

SerialDevices OpenedSerialDevices;

inline void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE)); // Disable TXEIE and TCIE interrupts
	huart->gState = HAL_UART_STATE_READY; // Tx process is ended, restore huart->gState to Ready
	SerialDevice *sc = OpenedSerialDevices.Get((uint32_t)huart->Instance);
	sc->TransmitComplete();
}

inline void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	SerialDevice *sc = OpenedSerialDevices.Get((uint32_t)huart->Instance);
	sc->ReceiptComplete();
}

//
//typedef enum
//{
//	i2cWrite,
//	i2cRead
//} I2cDirection;
//
//
//typedef enum
//{
//	I2C_STATUS_FREE = 0,
//	I2C_STATUS_START,
//	I2C_STATUS_ADDRESS,
//	I2C_STATUS_START_R, when the second start signal is sent // Read Data
//	I2C_STATUS_REDATA,
//	 I2C_STATUS_FINSIH, // operation is completed
//	 I2C_STATUS_ERR, // error
//
//}I2C_STATUS;
//
//typedef struct _I2cMessage
//{
//	uint32_t         Length;
//	uint32_t 		 Index;
//	uint8_t          slaveAddress;
//	I2cDirection     direction;
//	I2C_STATUS        status;
//	uint16_t         regAddress;
//	uint8_t          *buffer;
//} I2cMessage;
//
//
//static I2cMessage Message;
//
//
//void i2cdrvInitBus()
//{
//	I2C_InitTypeDef  I2C_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	GPIO_InitTypeDef GPIO_InitStructure;
//
//
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
//
//
//	GPIO_StructInit(&GPIO_InitStructure);
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; // SCL
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // SDA
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
//	GPIO_PinAFConfig(GPIOB, GPIO_Pin_9Source, GPIO_AF_I2C1);
//
//	I2C_DeInit(I2C1);
//	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
//	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
//	I2C_InitStructure.I2C_OwnAddress1 = 0x30;;
//	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
//	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
//	I2C_InitStructure.I2C_ClockSpeed = 400000;
//	I2C_Init(I2C1, &I2C_InitStructure);
//
//	I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE);
//
//	NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
//	NVIC_Init(&NVIC_InitStructure);
//
//	GPIO_SetBits(GPIOB, GPIO_Pin_8);
//	GPIO_SetBits(GPIOB, GPIO_Pin_9);
//
//}
//
//
//
// // read data
//bool i2cdevRead(uint8_t devAddr, uint8_t ReadAddr, uint8_t DatasLen, uint8_t *DataRead)
//{
//	int waitTime;
//	Message.status = I2C_STATUS_START;
//	Message.slaveAddress = devAddr;
//	Message.Index = 0;
//
//	Message.buffer = DataRead;
//
//	Message.Length = DatasLen;
//	 Message.regAddress = ReadAddr; // address register
//	Message.direction = i2cRead;
//
//	I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);
//	I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE);
//	 I2C1-> CR1 = (I2C_CR1_START | I2C_CR1_PE); // 1 interrupt trigger start
//	waitTime = clock();
//	 while (1) // wait for completion of reception
//	{
//		if (Message.status == I2C_STATUS_FINSIH)
//		{
//			return true;
//		}
//		if (Message.status == I2C_STATUS_ERR)
//		{
//			return false;
//		}
//		if (clock() - waitTime > 10)
//		{
//			I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
//			return false;
//		}
//	}
//}
//
//
// // Send a byte
//bool i2cdevWriteByte(uint8_t devAddr, uint8_t WriteAddr, uint8_t DataToWrite)
//{
//	int waitTime;
//	Message.status = I2C_STATUS_START;
//	Message.slaveAddress = devAddr;
//	Message.Index = 0;
//
//	Message.buffer = &DataToWrite;
//
//	Message.Length = 1;
//	 Message.regAddress = WriteAddr; // address register
//	Message.direction = i2cWrite;
//
//	I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);
//	I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE);
//	 I2C1-> CR1 = (I2C_CR1_START | I2C_CR1_PE); // 1 triggered the interrupt is initiated
//	waitTime = clock();
//	 while (1) // wait for the completion of transmission
//	{
//		if (Message.status == I2C_STATUS_FINSIH)
//		{
//			return true;
//		}
//		if (Message.status == I2C_STATUS_ERR)
//		{
//			return false;
//		}
//		if (clock() - waitTime > 10)
//		{
//			I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
//			return false;
//		}
//	}
//}
//
//void  I2C1_ER_IRQHandler(void)
//{
//	if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF))
//	{
//		I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE);
//
//		I2C_ClearFlag(I2C1, I2C_FLAG_AF);
//
//		Message.status = I2C_STATUS_ERR;
//	}
//	if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BERR))
//	{
//		I2C_ClearFlag(I2C1, I2C_FLAG_BERR);
//	}
//	if (I2C_GetFlagStatus(I2C1, I2C_FLAG_OVR))
//	{
//		I2C_ClearFlag(I2C1, I2C_FLAG_OVR);
//	}
//	if (I2C_GetFlagStatus(I2C1, I2C_FLAG_ARLO))
//	{
//		I2C_ClearFlag(I2C1, I2C_FLAG_ARLO);
//	}
//}
//
//
//
//void I2C1_EV_IRQHandler()
//{
//	uint16_t SR1;
//	uint16_t SR2;
//
//	SR1 = I2C1->SR1;
//
//	// Start bit event
//	if (SR1 & I2C_SR1_SB)
//	{
//		if (Message.status == I2C_STATUS_START)
//		{
//			I2C_Send7bitAddress(I2C1, Message.slaveAddress << 1, I2C_Direction_Transmitter);//2
//			Message.status = I2C_STATUS_ADDRESS;
//		}
//		else if (Message.status == I2C_STATUS_START_R)
//		{
//			 I2C_AcknowledgeConfig (I2C1, ENABLE); // 5 Switch to read data
//			I2C_Send7bitAddress(I2C1, Message.slaveAddress << 1, I2C_Direction_Receiver);
//			Message.status = I2C_STATUS_REDATA;
//		}
//	}
//	 else if (SR1 & I2C_SR1_ADDR) // address transfer is complete
//	{
//		SR2 = I2C1->SR2;
//
//		 if (Message.status == I2C_STATUS_ADDRESS) // send register address
//		{
//			 I2C_SendData (I2C1, Message.regAddress); // 4 transmission address
//		}
//		 if (Message.Length == 1) // only accept one byte
//		{
//			if (Message.status == I2C_STATUS_REDATA)
//			{
//				I2C_AcknowledgeConfig(I2C1, DISABLE);
//			}
//		}
//
//		I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
//	}
//	else if (SR1 & I2C_SR1_BTF)
//	{
//		 // 4 last transfer is complete
//
//		 if (SR1 & I2C_SR1_TXE) // send completed
//		{
//			 if (Message.direction == i2cRead) // read data
//			{
//				 if (Message.status == I2C_STATUS_ADDRESS) // sending the register
//				{
//					I2C1->CR1 = (I2C_CR1_START | I2C_CR1_PE);//
//					 Message.status = I2C_STATUS_START_R; // read data start
//				}
//			}
//			 else // write data
//			{
//				I2C_SendData(I2C1, Message.buffer[Message.Index++]);
//				if (Message.Index == Message.Length)
//				{
//					 I2C_ITConfig (I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE); // close the interruption
//					 I2C1-> CR1 = (I2C_CR1_STOP | I2C_CR1_PE); // add a follow-up test if
//					Message.status = I2C_STATUS_FINSIH;
//				}
//			}
//		}
//	}
//	if (SR1 & I2C_SR1_RXNE)
//	{
//
//		Message.buffer[Message.Index++] = I2C_ReceiveData(I2C1);
//
//		if (Message.Index == Message.Length - 1)
//		{
//			 I2C_AcknowledgeConfig (I2C1, DISABLE); // end data ban response
//		}
//
//		if (Message.Index == Message.Length)
//		{
//			 I2C1-> CR1 = (I2C_CR1_STOP | I2C_CR1_PE); // add a follow-up test if
//			 I2C_ITConfig (I2C1, I2C_IT_EVT | I2C_IT_BUF, DISABLE); // receiving end
//			Message.status = I2C_STATUS_FINSIH;
//		}
//	}
//	else if (SR1 & I2C_SR1_TXE)
//	{
//
//	}
//}
//
//import smbus
//from time import sleep
//
//#some MPU6050 Registers and their Address
//PWR_MGMT_1   = 0x6B
//SMPLRT_DIV   = 0x19
//CONFIG       = 0x1A
//GYRO_CONFIG  = 0x1B
//INT_ENABLE   = 0x38
//ACCEL_XOUT_H = 0x3B
//ACCEL_YOUT_H = 0x3D
//ACCEL_ZOUT_H = 0x3F
//GYRO_XOUT_H  = 0x43
//GYRO_YOUT_H  = 0x45
//GYRO_ZOUT_H  = 0x47
//
//
//def MPU_Init():
//	#write to sample rate register
//	bus.write_byte_data(Device_Address, SMPLRT_DIV, 7)
//
//	#Write to power management register
//	bus.write_byte_data(Device_Address, PWR_MGMT_1, 1)
//
//	#Write to Configuration register
//	bus.write_byte_data(Device_Address, CONFIG, 0)
//
//	#Write to Gyro configuration register
//	bus.write_byte_data(Device_Address, GYRO_CONFIG, 24)
//
//	#Write to interrupt enable register
//	bus.write_byte_data(Device_Address, INT_ENABLE, 1)
//
//def read_raw_data(addr):
//	#Accelero and Gyro value are 16-bit
//        high = bus.read_byte_data(Device_Address, addr)
//        low = bus.read_byte_data(Device_Address, addr+1)
//
//        #concatenate higher and lower value
//        value = ((high << 8) | low)
//
//        #to get signed value from mpu6050
//        if(value > 32768):
//                value = value - 65536
//        return value
//
//
//bus = smbus.SMBus(1) 	# or bus = smbus.SMBus(0) for older version boards
//Device_Address = 0x68   # MPU6050 device address
//
//MPU_Init()
//
//print (" Reading Data of Gyroscope and Accelerometer")
//
//while True:
//
//	#Read Accelerometer raw value
//	acc_x = read_raw_data(ACCEL_XOUT_H)
//	acc_y = read_raw_data(ACCEL_YOUT_H)
//	acc_z = read_raw_data(ACCEL_ZOUT_H)
//
//	#Read Gyroscope raw value
//	gyro_x = read_raw_data(GYRO_XOUT_H)
//	gyro_y = read_raw_data(GYRO_YOUT_H)
//	gyro_z = read_raw_data(GYRO_ZOUT_H)
//
//	#Full scale range +/- 250 degree/C as per sensitivity scale factor
//	Ax = acc_x/16384.0
//	Ay = acc_y/16384.0
//	Az = acc_z/16384.0
//
//	Gx = gyro_x/131.0
//	Gy = gyro_y/131.0
//	Gz = gyro_z/131.0
//
//
//	print ("Gx=%.2f" %Gx, u'\u00b0'+ "/s", "\tGy=%.2f" %Gy, u'\u00b0'+ "/s", "\tGz=%.2f" %Gz, u'\u00b0'+ "/s", "\tAx=%.2f g" %Ax, "\tAy=%.2f g" %Ay, "\tAz=%.2f g" %Az)
//	sleep(1)
