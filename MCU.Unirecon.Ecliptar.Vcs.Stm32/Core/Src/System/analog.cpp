
#include "System/analog.h"

AdcData::AdcData(ADC_HandleTypeDef *adc, uint32_t channel)
{
	AdcData(adc, channel, 2);
}
AdcData::AdcData(ADC_HandleTypeDef *adc, uint32_t channel, size_t dataSize)
{
	uint32_t arr[]
	{ channel };
	AdcData(adc, 1, arr, dataSize);
}
AdcData::AdcData(ADC_HandleTypeDef *adc, uint8_t channelsCount,
		uint32_t channels[])
{
	AdcData(adc, channelsCount, channels, channelsCount * 2);
}
AdcData::AdcData(ADC_HandleTypeDef *adc, uint8_t channelsCount,
		uint32_t channels[], size_t dataSize)
{
	this->adc = adc;
	this->dma = adc->DMA_Handle;
	this->channelsCount = channelsCount;
	this->channels = channels;
	conversionsLength = dataSize;
	conversions = new Ring(dataSize * 2);
}

void 				AnalogConverters::Add(AnalogConverter *converter)
{
	int id = hashtableId(converter->ConverterAddress(), 17);
	if (InitedAnalogConverters.Table[id] == NULL)
		InitedAnalogConverters.Table[id] = converter;
	else
	{
		for (int i = 0; i < InitedAnalogConverters.Count; i++)
		{
			if (InitedAnalogConverters.Table[i] == NULL)
				InitedAnalogConverters.Table[i] = converter;
		}
	}
	InitedAnalogConverters.Count++;
}
AnalogConverter* AnalogConverters::Get(uint32_t converterAddress)
{
	int id = hashtableId(converterAddress, 17);
	AnalogConverter *result = InitedAnalogConverters.Table[id];

	if (result->ConverterAddress() == converterAddress)
		return result;

	for (int i = 0; i < InitedAnalogConverters.Count; i++)
	{
		result = InitedAnalogConverters.Table[i];
		if (result->ConverterAddress() == converterAddress)
			return result;
	}

	return NULL;
}

uint32_t AnalogConverters::Remove(uint32_t converterAddress)
{
	int id = hashtableId(converterAddress, 17);
	AnalogConverter *result = InitedAnalogConverters.Table[id];

	if (result->ConverterAddress() == converterAddress)
	{
		InitedAnalogConverters.Table[id] = NULL;
		return 1;
	}
	for (int i = 0; i < InitedAnalogConverters.Count; i++)
	{
		result = InitedAnalogConverters.Table[i];
		if (result->ConverterAddress() == converterAddress)
		{
			InitedAnalogConverters.Table[i] = NULL;
			return 1;
		}
	}
	return 0;
}

AnalogConverters InitedAnalogConverters;

inline void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	AnalogConverter *ac = InitedAnalogConverters.Get((uint32_t) hadc->Instance);
	AdcData *conv = ac->Converter;
	uint32_t len = 0;
	if (conv->dma != NULL)
	{
		len = DMA_DATA_LENGTH(conv->conversionsLength, conv->dma) - conv->conversionsCounter;
		conv->conversionsCounter = 0;
	}
	else
	{
		len = (conv->currentChannelId + 1) % conv->channelsCount;
		conv->conversionsCounter = len;
	}
	ac->Converter->conversions->Step(len * 2);
	ac->ConversionComplete(ac->Converter, len);
}

inline void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
	AnalogConverter *ac = InitedAnalogConverters.Get((uint32_t) hadc->Instance);
	AdcData *conv = ac->Converter;
	uint32_t len = DMA_DATA_LENGTH(conv->conversionsLength, conv->dma);
	conv->conversionsCounter = len;
	ac->Converter->conversions->Step(len * 2);
	ac->ConversionHalfComplete(ac->Converter, len);
}

