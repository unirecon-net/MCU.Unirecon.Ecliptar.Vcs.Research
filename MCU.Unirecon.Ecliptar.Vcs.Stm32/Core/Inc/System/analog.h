#ifndef __SYSTEM_ANALOG_H
#define __SYSTEM_ANALOG_H

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <adc.h>
#include "stm32f4xx_hal.h"

#include "System/base.h"
#include "System/Data/ring.h"
#include "System/hash.h"

class  AdcData
{
public:
	AdcData(ADC_HandleTypeDef* adc, uint32_t channel);
	AdcData(ADC_HandleTypeDef* adc, uint32_t channel, size_t dataSize);
	AdcData(ADC_HandleTypeDef* adc, uint8_t channelsCount, uint32_t channels[]);
	AdcData(ADC_HandleTypeDef* adc, uint8_t channelsCount, uint32_t channels[], size_t dataSize);

	ADC_HandleTypeDef *adc = NULL;
	uint32_t* channels = NULL;
	uint8_t  channelsCount = 0;
	uint8_t  currentChannelId = 0;
	uint32_t conversionsCounter = 0;
	uint16_t conversionsLength = 0;
	DMA_HandleTypeDef* dma = NULL;
	Ring* conversions = NULL;
};

class AnalogConverter // @suppress("Class has a virtual method and non-virtual destructor")
{
public:
	virtual ~AnalogConverter(){}
	virtual void ConversionComplete(AdcData* stream, size_t size) = 0;
	virtual void ConversionHalfComplete(AdcData* stream, size_t size) = 0;

	uint32_t 	ConverterAddress()
	{
		return (uint32_t)Converter->adc;
	}

	AdcData* 		Converter;
};

class AnalogConverters
{
public:
	AnalogConverter*	Table[17];
	u8			    	Count;
	void			   	Add(AnalogConverter *converter);
	AnalogConverter*	Get(uint32_t key);
	uint32_t	Remove(uint32_t key);
};

extern 	AnalogConverters InitedAnalogConverters;


#endif /* __SYSTEM_ANALOG_H */
