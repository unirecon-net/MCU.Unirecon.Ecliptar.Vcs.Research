/*
  EcliptarRemoteControllerPWM.cpp - EcliptarRemoteControllerPWM library - ver. 0.4.5 
  Copyright (c) 2020 Unirecon Dariusz Hanc.  All right reserved.
*/

#include <Arduino.h>
#include <inttypes.h>
#include "EcliptarRemoteControllerPWM.h"

EcliptarRemoteControllerPWM::EcliptarRemoteControllerPWM()
{
}

void          EcliptarRemoteControllerPWM::attachPortChannels()
{
  interruptsCount = 0;
  for (int i = 0; i < 2; i++)
  {
    uint8_t portlength = portChannelsCount[i];
    Serial.print(" Port ");
    Serial.print(i);
    Serial.print(" length ");
    Serial.print(portlength);
    if (portlength > 0)
    {
      PortChannels[i] = new RadioChannel * [portlength];

      uint8_t counter = 0;
      for (uint8_t p = 0; p < channelsCount; p++)
      {
        if (Channels[p].channelPort == i)
        {
          PortChannels[i][counter++] = &(Channels[p]);
          Serial.print("Channel ");
          Serial.print(p);
          Serial.print(" on pin ");
          Serial.print(Channels[p].channelPin);
          Serial.print(" attached to port ");
          Serial.println(i);
        }
      }
    }
  }
}

void          EcliptarRemoteControllerPWM::attach(uint8_t numberOfChannels, uint8_t pinsArray[])
{
  this->attach(numberOfChannels, pinsArray, NULL, NULL, NULL, NULL);
}
void          EcliptarRemoteControllerPWM::attach(uint8_t numberOfChannels, uint8_t pinsArray[], uint16_t minPulseArray[], uint16_t midPulseArray[], uint16_t maxPulseArray[])
{
  this->attach(numberOfChannels, pinsArray, minPulseArray, midPulseArray, maxPulseArray, NULL);
}
void          EcliptarRemoteControllerPWM::attach(uint8_t numberOfChannels, uint8_t pinsArray[], uint16_t minPulseArray[], uint16_t midPulseArray[], uint16_t maxPulseArray[], float signalLostArray[])
{  
  uint8_t length = numberOfChannels;
  channelsCount = length;
  Channels = new RadioChannel[length];
  for (uint8_t i = 0; i < length; i++)
  {
    uint8_t pin = pinsArray[i];
    RadioChannel* channel = &(Channels[i]);
    channel->channelPin = pin;

    channel->channelRegistry = 1 << (pin % 8);
    uint8_t port = pin / 8;
    channel->channelPort = port;
    portChannelsCount[port]++;

    channel->minPulseWidth      = (minPulseArray == NULL)   ? DEFAULT_MIN_PULSE_US : minPulseArray[i];
    channel->midPulseWidth      = (midPulseArray == NULL)   ? DEFAULT_MID_PULSE_US : midPulseArray[i];
    channel->maxPulseWidth      = (maxPulseArray == NULL)   ? DEFAULT_MAX_PULSE_US : maxPulseArray[i];
    channel->signalLostPosition = (signalLostArray == NULL) ? DEFAULT_SIGNAL_LOST   : signalLostArray[i];

    attachInterrupt(pin);
  }

  attachPortChannels();
}
void          EcliptarRemoteControllerPWM::attach(uint8_t numberOfChannels, RadioChannel channelArray[])
{
  uint8_t length = numberOfChannels;
  channelsCount = length;
  Channels = channelArray;
  for (uint8_t i = 0; i < length; i++)
  {
    RadioChannel* channel = &(Channels[i]);
    uint8_t pin = channel->channelPin;

    channel->channelRegistry = 1 << (pin % 8);
    uint8_t port = pin / 8;
    channel->channelPort = port;
    portChannelsCount[port]++;

    if(channel->minPulseWidth  == 0) channel->minPulseWidth = DEFAULT_MIN_PULSE_US;
    if(channel->midPulseWidth == 0)  channel->midPulseWidth = DEFAULT_MID_PULSE_US;    
    if( channel->maxPulseWidth == 0) channel->maxPulseWidth = DEFAULT_MAX_PULSE_US;

    attachInterrupt(pin);
  }

  attachPortChannels();
}

bool          EcliptarRemoteControllerPWM::attached()
{
  if (this->channelsCount > 0)
    return true;
  else
    return false;
}

bool          EcliptarRemoteControllerPWM::dataAvailable()
{
  if (newDataAvailable)
  {
    newDataAvailable = false; // reset the flag
    return true;
  }
  return false;
}

bool          EcliptarRemoteControllerPWM::timerAllowed()
{
  if (interruptsCount > 0)
  {
    return false;
  }
  if ((micros() - lastPulseEndTime) % DEFAULT_PULSE_PERIOD_US > TIME_BETWEEN_MEASUREMENTS_US)
  {
    return false;
  }
  return true;
}

RadioChannel* EcliptarRemoteControllerPWM::readChannels()
{
  if (dataAvailable())
  {

    if (micros() - lastPulseEndTime > 50000)
    {
      resetRadioInterrupts();
      return NULL;
    }

    uint8_t length = channelsCount;
    for (uint8_t i = 0; i < length; i++)
      readChannel(&Channels[i]);
    return Channels;
  }
  return NULL;
}
bool          EcliptarRemoteControllerPWM::readChannel(RadioChannel* channel)
{
  if (channel->newDataAvailable)
  {
    channel->pulsePeriod = channel->innerPulsePeriod;
    readPulse(channel);   
    channel->pulseFraction = pulseToFraction(channel);
    channel->newDataAvailable = false; 
    return true;
  }
  return false;
}

float         EcliptarRemoteControllerPWM::pulseToFraction(RadioChannel* channel)
{
  uint16_t minPulse = channel->minPulseWidth;
  uint16_t midPulse = channel->midPulseWidth;
  uint16_t maxPulse = channel->maxPulseWidth;
  uint16_t pulse = channel->pulseWidth;
  if (pulse < minPulse) pulse = minPulse;
  else if (pulse > maxPulse) pulse = maxPulse;

  return (pulse < midPulse) ? (((float)midPulse - (float)pulse) / ((float)midPulse - (float)minPulse)) * -1.0000 :
                              (((float)pulse - (float)midPulse) / ((float)maxPulse - (float)midPulse));
}

RadioChannel* EcliptarRemoteControllerPWM::readFractions()
{
  if (dataAvailable())
  {
    uint8_t length = channelsCount;
    uint64_t timestamp = micros();

    if (timestamp - lastPulseEndTime > 50000)
      resetRadioInterrupts();

    for (uint8_t i = 0; i < length; i++)
      readFraction(&(Channels[i]), timestamp);

    return Channels;
  }
  return NULL;
}

float         EcliptarRemoteControllerPWM::readFraction(RadioChannel* channel)
{
  return readFraction(channel, micros());
}
float         EcliptarRemoteControllerPWM::readFraction(RadioChannel* channel, uint64_t timestamp)
{
  if (channel->newDataAvailable)
  {
    channel->newDataAvailable = false;
    
    if (signalLostHandler(channel, timestamp))
      return channel->pulseFraction = channel->signalLostPosition;
    
    readPulse(channel);
    
    return channel->pulseFraction = pulseToFraction(channel);
  }
  return channel->pulseFraction;
}

uint16_t      EcliptarRemoteControllerPWM::readPulse(RadioChannel* channel)
{
    uint16_t newWidth        = channel->innerPulseWidth;
    uint16_t width           = channel->pulseWidth;
    int8_t  widthDifference = width - newWidth;
    return channel->pulseWidth = (widthDifference < PLUS_PULSE_TOLERANCE_US && widthDifference > MINUS_PULSE_TOLERANCE_US) ? (width + newWidth) / 2 : newWidth;

}

bool          EcliptarRemoteControllerPWM::signalLostHandler(RadioChannel* channel, uint64_t timestamp)
{
  if (channel->innerPulseWidth < 500 ||
      channel->innerPulseWidth            > 2500 ||
      timestamp - channel->pulseStartTime > 50000)
    return true;
  return false;
}

void          EcliptarRemoteControllerPWM::printPulseReport()
{

  RadioChannel* channels = readChannels();

  if (channels != NULL)
  {
    uint8_t length = channelsCount;
    Serial.println("");
    Serial.print("WIDTH:");
    for (uint8_t i = 0; i < length; i++)
    {
      Serial.print(" CH" + String(i + 1) + ": " + String(Channels[i].pulseWidth));
    }
    Serial.println("");
    Serial.print("PERIOD:");
    for (uint8_t i = 0; i < length; i++)
    {
      Serial.print(" CH" + String(i + 1) + ": " + String(Channels[i].pulsePeriod));
    }
    Serial.println("");
    Serial.print("FRACTION:");
    for (uint8_t i = 0; i < length; i++)
    {
      Serial.print(" CH" + String(i + 1) + ": " + String(Channels[i].pulseFraction));
    }
  }
  // else
  // {
  //   Serial.println("");
  //   Serial.print("NO DATA AVAILABLE");
  // }

}

void          EcliptarRemoteControllerPWM::printFractionReport()
{
  if (dataAvailable())
  {
    uint8_t length = channelsCount;
    Serial.println("");
    Serial.print("FRACTION:");
    for (uint8_t i = 0; i < length; i++)
    {
      Serial.print(" CH" + String(i + 1) + ": " + String(readFraction(&(Channels[i]))));
    }
  }
}

void          EcliptarRemoteControllerPWM::attachInterrupt(byte pin)
{
  *digitalPinToPCMSK(pin) |= bit(digitalPinToPCMSKbit(pin)); // enable pin
  PCIFR |= bit(digitalPinToPCICRbit(pin));                   // clear any outstanding interrupt
  PCICR |= bit(digitalPinToPCICRbit(pin));                   // enable interrupt for the group
}

void          EcliptarRemoteControllerPWM::handleRadioInterrupt(uint8_t port, uint64_t timestamp, byte registryValue)
{
  uint8_t length = portChannelsCount[port];
  RadioChannel** channels = PortChannels[port];
  for (uint8_t c = 0; c < length; c++)
  {
    RadioChannel* channel = channels[c];
    uint8_t state = channel->pinState;
    uint8_t regState = ((registryValue & channel->channelRegistry) > 0);

    if (state != regState)
    {
      if (state < regState)
      {
        channel->pinState = 1;
        if (!resetModeState)
        {
          channel->innerPulsePeriod = timestamp - channel->pulseStartTime;
          channel->pulseStartTime = timestamp;
        }
      }
      else
      {
        channel->pinState = 0;
        if (!resetModeState)
        {
          channel->innerPulseWidth = timestamp - channel->pulseStartTime;
          channel->newDataAvailable = true;
        }
        interruptsCount++;
      }
    }
  }
  if (interruptsCount > channelsCount - 1)
  {
    if (resetModeState)
      resetModeState = false;
    else
      newDataAvailable = true;
    lastPulseEndTime = timestamp;
    interruptsCount = 0;
  }
}


void   EcliptarRemoteControllerPWM::resetRadioInterrupts()
{
  resetModeState = true;
  interruptsCount = 0;
}

EcliptarRemoteControllerPWM RadioController;

ISR(PCINT0_vect)
{
  RadioController.handleRadioInterrupt(1, micros(), PINB);
}

ISR(PCINT2_vect)
{
  RadioController.handleRadioInterrupt(0, micros(), PIND);
}


