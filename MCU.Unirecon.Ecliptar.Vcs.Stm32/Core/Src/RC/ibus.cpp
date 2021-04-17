/*
 * ecliptar_rc_.c
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */

#include "RC/ibus.h"

RC_IBUS::RC_IBUS() // @suppress("Class members should be properly initialized")
{
	available = false;
	channelsCount = 0;
	sensorsCount = 0;
}

void RC_IBUS::Attach(uint8_t numberOfChannels, UART_HandleTypeDef *rc_uart)
{
	this->Attach(numberOfChannels, rc_uart, NULL, NULL, NULL, NULL);
}
void RC_IBUS::Attach(uint8_t numberOfChannels, UART_HandleTypeDef *rc_uart,
		uint16_t minPulseArray[], uint16_t midPulseArray[],
		uint16_t maxPulseArray[])
{
	this->Attach(numberOfChannels, rc_uart, minPulseArray, midPulseArray,
			maxPulseArray, NULL);
}
void RC_IBUS::Attach(uint8_t numberOfChannels, UART_HandleTypeDef *rc_uart,
		uint16_t minPulseArray[], uint16_t midPulseArray[],
		uint16_t maxPulseArray[], int signalLostArray[])
{
	uint8_t length = numberOfChannels;
	channelsCount = length;
	Channels = new ControlChannel[length];
	Sensors = new SensorInfo[SENSORS_MAX_LENGTH];
	requestBuffer = new uint8_t[REQUEST_BUFFER_SIZE];
	responseBuffer = new uint8_t[RESPONSE_BUFFER_SIZE];
	channelValues = new uint16_t[length];
	innerChannelValues = new uint16_t[length];

	for (uint8_t i = 0; i < length; i++)
	{
		ControlChannel *channel = &(Channels[i]);

		channel->index = i;

		channel->minPulseWidth = (minPulseArray == NULL) ?
		DEFAULT_MIN_PULSE_US :
															minPulseArray[i];
		channel->midPulseWidth = (midPulseArray == NULL) ?
		DEFAULT_MID_PULSE_US :
															midPulseArray[i];
		channel->maxPulseWidth = (maxPulseArray == NULL) ?
		DEFAULT_MAX_PULSE_US :
															maxPulseArray[i];
		channel->signalLostPosition =
				(signalLostArray == NULL) ?
				DEFAULT_SIGNAL_LOST :
											signalLostArray[i];
		channel->innerPulseWidth = minPulseArray[i];
		channel->pulseWidth = minPulseArray[i];
		channel->pulseDelta = 0;
		channel->controlValue = 0;

	}
	Serial = new SerialDevice(rc_uart);
	Serial->SetReceiptEvent(this);
	Serial->Open(128, 32);

}
void RC_IBUS::Attach(uint8_t numberOfChannels, UART_HandleTypeDef *rc_uart,
		ControlChannel channelArray[])
{
	uint8_t length = numberOfChannels;
	channelsCount = length;
	Channels = channelArray;
	Sensors = new SensorInfo[SENSORS_MAX_LENGTH];
	requestBuffer = new uint8_t[REQUEST_BUFFER_SIZE];
	responseBuffer = new uint8_t[RESPONSE_BUFFER_SIZE];
	channelValues = new uint16_t[length];
	innerChannelValues = new uint16_t[length];

	for (uint8_t i = 0; i < length; i++)
	{
		ControlChannel *channel = &(Channels[i]);

		channel->index = i;

		if (channel->minPulseWidth == 0)
			channel->minPulseWidth = DEFAULT_MIN_PULSE_US;
		if (channel->midPulseWidth == 0)
			channel->midPulseWidth = DEFAULT_MID_PULSE_US;
		if (channel->maxPulseWidth == 0)
			channel->maxPulseWidth = DEFAULT_MAX_PULSE_US;
		channel->innerPulseWidth = DEFAULT_MIN_PULSE_US;
		channel->pulseWidth = DEFAULT_MIN_PULSE_US;
		channel->pulseDelta = 0;
		channel->controlValue = 0;
	}

	Serial = new SerialDevice(rc_uart);
	Serial->SetReceiptEvent(this);
	Serial->Open(128, 32);
}

bool RC_IBUS::Attached()
{
	if (this->channelsCount > 0)
		return true;
	else
		return false;
}

bool RC_IBUS::Available()
{
	if (available)
	{
		available = false; // reset the flag
		return true;
	}
	return false;
}

ControlChannel* RC_IBUS::ReadChannels()
{
	if (Available())
	{
		uint8_t length = channelsCount;
		//NVIC_DisableIRQ(UART5_IRQn);
		__disable_irq();
		memcpy(innerChannelValues, channelValues, length * 2);
		__enable_irq();
		//NVIC_EnableIRQ(UART5_IRQn);
		uint8_t changedDataCounter = 0;

		for (uint8_t i = 0; i < length; i++)
		{
			changedDataCounter += readChannel(&Channels[i]);
		}

		if (changedDataCounter > 0)
			return Channels;
	}
	return NULL;
}
uint8_t RC_IBUS::readChannel(ControlChannel *channel)
{
	channel->innerPulseWidth = innerChannelValues[channel->index];

	bool _available = pulseDepreciation(channel);
	channel->available = _available;

	if (_available)
		channel->controlValue = pulseToPercents(channel);
	return _available;
}

uint8_t RC_IBUS::AddSensor(uint8_t type, uint8_t len)
{
	// add a sensor, return sensor number
	if (len != 2 && len != 4)
		len = 2;
	if (sensorsCount < SENSORS_MAX_LENGTH)
	{
		SensorInfo *s = &Sensors[sensorsCount];
		s->sensorType = (SensorType) type;
		s->sensorLength = len;
		s->sensorValue = 0;
		sensorsCount++;
	}
	return sensorsCount;
}
void RC_IBUS::SetSensorMeasurement(uint8_t id, int32_t value)
{
	if (id < sensorsCount)
		Sensors[id].sensorValue = value;
}

uint8_t RC_IBUS::pulseDepreciation(ControlChannel *channel)
{
	int32_t width = channel->pulseWidth;
	int32_t widthDifference = channel->innerPulseWidth - width;
	channel->pulseDelta = widthDifference;

	int32_t sign = 1;
	if (widthDifference < 0)
	{
		widthDifference *= -1;
		sign = -1;
	}

	if (widthDifference > PULSE_MIN_CHANGE_US)
	{

		float accelThold = PULSE_MAX_ACCEL_US
				- ((float) widthDifference
						/ (float) ((channel->maxPulseWidth
								- channel->midPulseWidth)));

		if (accelThold < PULSE_MIN_ACCEL_US)
		{
			accelThold = PULSE_MIN_ACCEL_US;
		}

		if (widthDifference > PULSE_MAX_CHANGE_US)
		{
			widthDifference = PULSE_MAX_CHANGE_US;
		}

		widthDifference = (int32_t) ((float) widthDifference * accelThold);
		channel->pulseWidth = width + (widthDifference * sign);

		return 1;
	}
	return 0;
}
float RC_IBUS::pulseToPercents(ControlChannel *channel)
{
	uint32_t minPulse = channel->minPulseWidth;
	uint32_t midPulse = channel->midPulseWidth;
	uint32_t maxPulse = channel->maxPulseWidth;
	uint32_t pulse = channel->pulseWidth;

	if (pulse <= minPulse)
	{
		if (minPulse == midPulse)
			return 0;
		else
			return -100;
	}
	else if (pulse >= maxPulse)
		return 100;

	return (pulse < midPulse) ?
			(((float) (midPulse - pulse) * 100) / (float) (midPulse - minPulse))
					* -1.00F :
			(((float) (pulse - midPulse) * 100) / (float) (maxPulse - midPulse));
}

void RC_IBUS::PrintInfoReport(ControlChannel *channels)
{
	if (channels != NULL)
	{
		uint8_t length = channelsCount;
		printnln();
		print("WIDTH:");
		for (uint8_t i = 0; i < length; i++)
		{
			print(" CH%d: %d", i + 1, Channels[i].pulseWidth);
		}
		printnln();
		print("FRACTION:");
		for (uint8_t i = 0; i < length; i++)
		{
			print(" CH%d: %d", i + 1, Channels[i].controlValue);
		}
	}
}

inline void RC_IBUS::ReceiptCompleteEvent(Ring *data, size_t size)
{
	ibusReceiver(data, size);
}
inline void RC_IBUS::TransmitCompleteEvent(Ring *data, size_t size)
{
}

inline void RC_IBUS::ibusReceiver(Ring *data, size_t size)
{
	uint8_t _size = size;
	while (!frameStarted && data->Read())
	{
		uint8_t rbyte = data->ReadedByte();
		if (rbyte == CHANNELS_MSG_LENGTH)
		{
			frameStarted = 1;
			requestType = CHANNELS;
			requestLength = CHANNELS_MSG_LENGTH;
			frameLength = 1;
		}
		else if (rbyte == SENSORS_MSG_LENGTH)
		{
			frameStarted = 1;
			requestType = SENSORS;
			requestLength = SENSORS_MSG_LENGTH;
			frameLength = 1;
		}
		_size--;
	}

	if (frameStarted && _size > 0)
	{
		uint8_t tocopy = requestLength - frameLength;
		if (tocopy > _size)
			tocopy = _size;
		data->Read(&requestBuffer[frameLength - 1], tocopy);
		frameLength += tocopy;
		_size -= tocopy;

		if (frameLength == requestLength)
		{
			if (ibusChecksum(requestBuffer))
			{
				switch (requestType)
				{
				case SENSORS:
					ibusSensors(requestBuffer);
					break;
				case CHANNELS:
					ibusChannels(requestBuffer);
					break;
				case NONE:
					break;
				default:
					break;
				}
			}
			ibusReset();
			if (_size > 0)
				ibusReceiver(data, _size);
		}
	}

}
inline bool RC_IBUS::ibusChecksum(uint8_t *buffer)
{
	uint8_t len = requestLength - 3;
	uint16_t chksum = 0xFFFF - requestLength;
	uint8_t lchksum = buffer[len];
	uint8_t hchksum = buffer[len + 1];
	for (int i = 0; i < len; i++)
	{
		chksum -= buffer[i];
	}
	if (chksum == (hchksum << 8) + lchksum)
		return true;
	return false;
}
inline void RC_IBUS::ibusChannels(uint8_t *buffer)
{
	if (buffer[0] == PROTOCOL_COMMAND40)
	{
		uint8_t *chn_buffer = &buffer[1];
		for (uint8_t i = 0; i < channelsCount; i++)
		{
			uint8_t pos = i * 2;
			uint16_t val = chn_buffer[pos] | chn_buffer[pos + 1] << 8;
			channelValues[i] = val;
		}
		available = true;
		return;
	}
}
inline void RC_IBUS::ibusSensors(uint8_t *buffer)
{
	uint8_t adr = buffer[0] & 0x0f;
	if (adr <= sensorsCount && adr > 0)
	{
		// all sensor data commands go here
		// we only process the sens_length == 4 commands (=message length is 4 bytes incl overhead) to prevent the case the
		// return messages from the UART TX port loop back to the RX port and are processed again. This is extra
		// precaution as it will also be prevented by the PROTOCOL_TIMEGAP required

		uint16_t chksum = 0xFFFF;
		SensorInfo *s = &Sensors[adr - 1];
		switch (buffer[0] & 0x0f0)
		{
		case PROTOCOL_COMMAND_DISCOVER: // 0x80, discover sensor
			// echo discover command: 0x04, 0x81, 0x7A, 0xFF
			responseLength = 4;
			responseBuffer[0] = 0x04;
			responseBuffer[1] = (PROTOCOL_COMMAND_DISCOVER + adr);
			chksum -= (0x04 + PROTOCOL_COMMAND_DISCOVER + adr);
			break;
		case PROTOCOL_COMMAND_TYPE: // 0x90, send sensor type
			// echo sensortype command: 0x06 0x91 0x00 0x02 0x66 0xFF
			responseLength = 6;
			responseBuffer[0] = 0x06;
			responseBuffer[1] = PROTOCOL_COMMAND_TYPE + adr;
			responseBuffer[2] = s->sensorType;
			responseBuffer[3] = s->sensorLength;
			chksum -= (0x06 + PROTOCOL_COMMAND_TYPE + adr + s->sensorType
					+ s->sensorLength);
			break;
		case PROTOCOL_COMMAND_VALUE:			// 0xA0, send sensor data
			responseLength = 6;
			uint8_t t;
			// echo sensor value command: 0x06 0x91 0x00 0x02 0x66 0xFF
			responseBuffer[0] = t = 0x04 + s->sensorLength;
			chksum -= t;
			responseBuffer[1] = t = PROTOCOL_COMMAND_VALUE + adr;
			chksum -= t;
			responseBuffer[2] = t = s->sensorValue & 0x0ff;
			chksum -= t;
			responseBuffer[3] = t = (s->sensorValue >> 8) & 0x0ff;
			chksum -= t;
			if (s->sensorLength == 4)
			{
				responseLength += 2;
				responseBuffer[4] = t = (s->sensorValue >> 16) & 0x0ff;
				chksum -= t;
				responseBuffer[5] = t = (s->sensorValue >> 24) & 0x0ff;
				chksum -= t;
			}
			break;
		default:
			adr = 0; // unknown command, prevent sending chksum
			break;
		}
		if (adr > 0)
		{
			responseBuffer[responseLength - 2] = chksum & 0x0ff;
			responseBuffer[responseLength - 1] = chksum >> 8;
			Serial->Transmit(responseBuffer, responseLength, 0);
		}
	}
}
inline void RC_IBUS::ibusReset()
{
	memset(requestBuffer, 0, requestLength);
	requestLength = 0;
	frameStarted = 0;
	frameLength = 0;
	requestType = NONE;
	if (responseLength > 0)
	{
		memset(responseBuffer, 0, responseLength);
		responseLength = 0;
	}

}

RC_IBUS RC;

