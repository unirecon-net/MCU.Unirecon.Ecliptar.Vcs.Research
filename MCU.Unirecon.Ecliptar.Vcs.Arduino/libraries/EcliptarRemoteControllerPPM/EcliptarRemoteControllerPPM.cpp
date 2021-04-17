/*
  EcliptarRemoteControllerPPM.cpp - EcliptarRemoteControllerPPM library - ver. 0.4.5
  Copyright (c) 2020 Unirecon Dariusz Hanc.  All right reserved.
*/

#include <Arduino.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <wiring_private.h>

#include "EcliptarRemoteControllerPPM.h"


static EcliptarInterruptHandler* handlers[EXTERNAL_NUM_INTERRUPTS];

void EcliptarInterruptHandler::attachInterrupt(uint8_t interruptNum, uint8_t mode) {
  if (interruptNum < EXTERNAL_NUM_INTERRUPTS) {
    handlers[interruptNum] = this;

    // Configure the interrupt

    switch (interruptNum) {
#if defined(__AVR_ATmega32U4__)

    case 0:
      EICRA = (EICRA & ~((1 << ISC00) | (1 << ISC01))) | (mode << ISC00);
      EIMSK |= (1 << INT0);
      break;
    case 1:
      EICRA = (EICRA & ~((1 << ISC10) | (1 << ISC11))) | (mode << ISC10);
      EIMSK |= (1 << INT1);
      break;
    case 2:
      EICRA = (EICRA & ~((1 << ISC20) | (1 << ISC21))) | (mode << ISC20);
      EIMSK |= (1 << INT2);
      break;
    case 3:
      EICRA = (EICRA & ~((1 << ISC30) | (1 << ISC31))) | (mode << ISC30);
      EIMSK |= (1 << INT3);
      break;
    case 4:
      EICRB = (EICRB & ~((1 << ISC60) | (1 << ISC61))) | (mode << ISC60);
      EIMSK |= (1 << INT6);
      break;
#elif defined(EICRA) && defined(EICRB) && defined(EIMSK)
    case 2:
      EICRA = (EICRA & ~((1 << ISC00) | (1 << ISC01))) | (mode << ISC00);
      EIMSK |= (1 << INT0);
      break;
    case 3:
      EICRA = (EICRA & ~((1 << ISC10) | (1 << ISC11))) | (mode << ISC10);
      EIMSK |= (1 << INT1);
      break;
    case 4:
      EICRA = (EICRA & ~((1 << ISC20) | (1 << ISC21))) | (mode << ISC20);
      EIMSK |= (1 << INT2);
      break;
    case 5:
      EICRA = (EICRA & ~((1 << ISC30) | (1 << ISC31))) | (mode << ISC30);
      EIMSK |= (1 << INT3);
      break;
    case 0:
      EICRB = (EICRB & ~((1 << ISC40) | (1 << ISC41))) | (mode << ISC40);
      EIMSK |= (1 << INT4);
      break;
    case 1:
      EICRB = (EICRB & ~((1 << ISC50) | (1 << ISC51))) | (mode << ISC50);
      EIMSK |= (1 << INT5);
      break;
    case 6:
      EICRB = (EICRB & ~((1 << ISC60) | (1 << ISC61))) | (mode << ISC60);
      EIMSK |= (1 << INT6);
      break;
    case 7:
      EICRB = (EICRB & ~((1 << ISC70) | (1 << ISC71))) | (mode << ISC70);
      EIMSK |= (1 << INT7);
      break;
#else
    case 0:
#if defined(EICRA) && defined(ISC00) && defined(EIMSK)
      EICRA = (EICRA & ~((1 << ISC00) | (1 << ISC01))) | (mode << ISC00);
      EIMSK |= (1 << INT0);
#elif defined(MCUCR) && defined(ISC00) && defined(GICR)
      MCUCR = (MCUCR & ~((1 << ISC00) | (1 << ISC01))) | (mode << ISC00);
      GICR |= (1 << INT0);
#elif defined(MCUCR) && defined(ISC00) && defined(GIMSK)
      MCUCR = (MCUCR & ~((1 << ISC00) | (1 << ISC01))) | (mode << ISC00);
      GIMSK |= (1 << INT0);
#else
      #warning attachInterrupt not finished for this CPU(case 0)
#endif
        break;

      case 1:
#if defined(EICRA) && defined(ISC10) && defined(ISC11) && defined(EIMSK)
        EICRA = (EICRA & ~((1 << ISC10) | (1 << ISC11))) | (mode << ISC10);
        EIMSK |= (1 << INT1);
#elif defined(MCUCR) && defined(ISC10) && defined(ISC11) && defined(GICR)
        MCUCR = (MCUCR & ~((1 << ISC10) | (1 << ISC11))) | (mode << ISC10);
        GICR |= (1 << INT1);
#elif defined(MCUCR) && defined(ISC10) && defined(GIMSK) && defined(GIMSK)
        MCUCR = (MCUCR & ~((1 << ISC10) | (1 << ISC11))) | (mode << ISC10);
        GIMSK |= (1 << INT1);
#else
        #warning attachInterrupt may need some more work for this cpu(case 1)
#endif
          break;

        case 2:
#if defined(EICRA) && defined(ISC20) && defined(ISC21) && defined(EIMSK)
          EICRA = (EICRA & ~((1 << ISC20) | (1 << ISC21))) | (mode << ISC20);
          EIMSK |= (1 << INT2);
#elif defined(MCUCR) && defined(ISC20) && defined(ISC21) && defined(GICR)
          MCUCR = (MCUCR & ~((1 << ISC20) | (1 << ISC21))) | (mode << ISC20);
          GICR |= (1 << INT2);
#elif defined(MCUCR) && defined(ISC20) && defined(GIMSK) && defined(GIMSK)
          MCUCR = (MCUCR & ~((1 << ISC20) | (1 << ISC21))) | (mode << ISC20);
          GIMSK |= (1 << INT2);
#endif
          break;
#endif
    }
  }
}

void EcliptarInterruptHandler::detachInterrupt(uint8_t interruptNum) {
  if (interruptNum < EXTERNAL_NUM_INTERRUPTS) {
    switch (interruptNum) {
#if defined(__AVR_ATmega32U4__)
    case 0:
      EIMSK &= ~(1 << INT0);
      break;
    case 1:
      EIMSK &= ~(1 << INT1);
      break;
    case 2:
      EIMSK &= ~(1 << INT2);
      break;
    case 3:
      EIMSK &= ~(1 << INT3);
      break;
    case 4:
      EIMSK &= ~(1 << INT6);
      break;
#elif defined(EICRA) && defined(EICRB) && defined(EIMSK)
    case 2:
      EIMSK &= ~(1 << INT0);
      break;
    case 3:
      EIMSK &= ~(1 << INT1);
      break;
    case 4:
      EIMSK &= ~(1 << INT2);
      break;
    case 5:
      EIMSK &= ~(1 << INT3);
      break;
    case 0:
      EIMSK &= ~(1 << INT4);
      break;
    case 1:
      EIMSK &= ~(1 << INT5);
      break;
    case 6:
      EIMSK &= ~(1 << INT6);
      break;
    case 7:
      EIMSK &= ~(1 << INT7);
      break;
#else
    case 0:
#if defined(EIMSK) && defined(INT0)
      EIMSK &= ~(1 << INT0);
#elif defined(GICR) && defined(ISC00)
      GICR &= ~(1 << INT0); // atmega32
#elif defined(GIMSK) && defined(INT0)
      GIMSK &= ~(1 << INT0);
#else
      #warning detachInterrupt not finished for this cpu
#endif
        break;

    case 1:
#if defined(EIMSK) && defined(INT1)
      EIMSK &= ~(1 << INT1);
#elif defined(GICR) && defined(INT1)
      GICR &= ~(1 << INT1); // atmega32
#elif defined(GIMSK) && defined(INT1)
      GIMSK &= ~(1 << INT1);
#else
      #warning detachInterrupt may need some more work for this cpu(case 1)
#endif
        break;

      case 2:
#if defined(EIMSK) && defined(INT2)
        EIMSK &= ~(1 << INT2);
#elif defined(GICR) && defined(INT2)
        GICR &= ~(1 << INT2); // atmega32
#elif defined(GIMSK) && defined(INT2)
        GIMSK &= ~(1 << INT2);
#elif defined(INT2)
        #warning detachInterrupt may need some more work for this cpu(case 2)
#endif
          break;
#endif
    }

    handlers[interruptNum] = 0;
  }
}

#if defined(__AVR_ATmega32U4__)
ISR(INT0_vect) {
  if (handlers[EXTERNAL_INT_0])
    handlers[EXTERNAL_INT_0]->handleInterrupt(EXTERNAL_INT_0);
}

ISR(INT1_vect) {
  if (handlers[EXTERNAL_INT_1])
    handlers[EXTERNAL_INT_1]->handleInterrupt(EXTERNAL_INT_1);
}

ISR(INT2_vect) {
  if (handlers[EXTERNAL_INT_2])
    handlers[EXTERNAL_INT_2]->handleInterrupt(EXTERNAL_INT_2);
}

ISR(INT3_vect) {
  if (handlers[EXTERNAL_INT_3])
    handlers[EXTERNAL_INT_3]->handleInterrupt(EXTERNAL_INT_3);
}

ISR(INT6_vect) {
  if (handlers[EXTERNAL_INT_4])
    handlers[EXTERNAL_INT_4]->handleInterrupt(EXTERNAL_INT_4);
}

#elif defined(EICRA) && defined(EICRB)

ISR(INT0_vect) {
  if (handlers[EXTERNAL_INT_2])
    handlers[EXTERNAL_INT_2]->handleInterrupt(EXTERNAL_INT_2);
}

ISR(INT1_vect) {
  if (handlers[EXTERNAL_INT_3])
    handlers[EXTERNAL_INT_3]->handleInterrupt(EXTERNAL_INT_3);
}

ISR(INT2_vect) {
  if (handlers[EXTERNAL_INT_4])
    handlers[EXTERNAL_INT_4]->handleInterrupt(EXTERNAL_INT_4);
}

ISR(INT3_vect) {
  if (handlers[EXTERNAL_INT_5])
    handlers[EXTERNAL_INT_5]->handleInterrupt(EXTERNAL_INT_5);
}

ISR(INT4_vect) {
  if (handlers[EXTERNAL_INT_0])
    handlers[EXTERNAL_INT_0]->handleInterrupt(EXTERNAL_INT_0);
}

ISR(INT5_vect) {
  if (handlers[EXTERNAL_INT_1])
    handlers[EXTERNAL_INT_1]->handleInterrupt(EXTERNAL_INT_1);
}

ISR(INT6_vect) {
  if (handlers[EXTERNAL_INT_6])
    handlers[EXTERNAL_INT_6]->handleInterrupt(EXTERNAL_INT_6);
}

ISR(INT7_vect) {
  if (handlers[EXTERNAL_INT_7])
    handlers[EXTERNAL_INT_7]->handleInterrupt(EXTERNAL_INT_7);
}

#else

ISR(INT0_vect) {
  if (handlers[EXTERNAL_INT_0])
    handlers[EXTERNAL_INT_0]->handleInterrupt(EXTERNAL_INT_0);
}

ISR(INT1_vect) {
  if (handlers[EXTERNAL_INT_1])
    handlers[EXTERNAL_INT_1]->handleInterrupt(EXTERNAL_INT_1);
}

#if defined(EICRA) && defined(ISC20)
ISR(INT2_vect) {
  if (handlers[EXTERNAL_INT_2])
    handlers[EXTERNAL_INT_2]->handleInterrupt(EXTERNAL_INT_2);
}
#endif

#endif

EcliptarRemoteControllerPPM::EcliptarRemoteControllerPPM()
{
  resetMode = true;
  newDataAvailable = false;
  frameEndTime = 0;
  innerFrameEndTime = 0;
  pulseEndTime = 0;
}

void            EcliptarRemoteControllerPPM::attach(uint8_t numberOfChannels, uint8_t interruptPin)
{
  this->attach(numberOfChannels, interruptPin, NULL, NULL, NULL, NULL);
}
void            EcliptarRemoteControllerPPM::attach(uint8_t numberOfChannels, uint8_t interruptPin, uint16_t minPulseArray[], uint16_t midPulseArray[], uint16_t maxPulseArray[])
{
  this->attach(numberOfChannels, interruptPin, minPulseArray, midPulseArray, maxPulseArray, NULL);
}
void            EcliptarRemoteControllerPPM::attach(uint8_t numberOfChannels, uint8_t interruptPin, uint16_t minPulseArray[], uint16_t midPulseArray[], uint16_t maxPulseArray[], int signalLostArray[])
{
  uint8_t length = numberOfChannels;
  channelsCount = length;
  Channels = new ControlChannel[length];
  pinStateRegistry = 1 << (interruptPin % 8);
  for (uint8_t i = 0; i < length; i++)
  {
    ControlChannel* channel = &(Channels[i]);

    channel->minPulseWidth = (minPulseArray == NULL) ? DEFAULT_MIN_PULSE_US : minPulseArray[i];
    channel->midPulseWidth = (midPulseArray == NULL) ? DEFAULT_MID_PULSE_US : midPulseArray[i];
    channel->maxPulseWidth = (maxPulseArray == NULL) ? DEFAULT_MAX_PULSE_US : maxPulseArray[i];
    channel->signalLostPosition = (signalLostArray == NULL) ? DEFAULT_SIGNAL_LOST : signalLostArray[i];
    channel->innerPulseWidth = channel->midPulseWidth;
    channel->pulseWidth = channel->midPulseWidth;
    channel->isSwitch = 0;
    channel->controlValue = 0;
  }

  attachInterrupt(digitalPinToInterrupt(interruptPin), CHANGE);
}
void            EcliptarRemoteControllerPPM::attach(uint8_t numberOfChannels, uint8_t interruptPin, ControlChannel channelArray[])
{
  uint8_t length = numberOfChannels;
  channelsCount = length;
  Channels = channelArray;
  pinStateRegistry = 1 << (interruptPin % 8);
  for (uint8_t i = 0; i < length; i++)
  {
    ControlChannel* channel = &(Channels[i]);

    if (channel->minPulseWidth == 0) channel->minPulseWidth = DEFAULT_MIN_PULSE_US;
    if (channel->midPulseWidth == 0)  channel->midPulseWidth = DEFAULT_MID_PULSE_US;
    if (channel->maxPulseWidth == 0) channel->maxPulseWidth = DEFAULT_MAX_PULSE_US;
    channel->innerPulseWidth = channel->midPulseWidth;
    channel->pulseWidth = channel->midPulseWidth;
    channel->controlValue = 0;
  }

  attachInterrupt(digitalPinToInterrupt(interruptPin), CHANGE);
}

void            EcliptarRemoteControllerPPM::defineSwitches(uint8_t switchChannels[])
{
  for (uint8_t i = 0; i < channelsCount; i++)
  {
    Channels[i].isSwitch = switchChannels[i];
  }
}

bool            EcliptarRemoteControllerPPM::attached()
{
  if (this->channelsCount > 0)
    return true;
  else
    return false;
}

bool            EcliptarRemoteControllerPPM::dataAvailable()
{
  if (newDataAvailable)
  {
    frameEndTime = innerFrameEndTime;
    newDataAvailable = false; // reset the flag
    return true;
  }
  return false;
}

ControlChannel* EcliptarRemoteControllerPPM::readChannels(bool withFailSafe)
{
  if (dataAvailable())
  {
    uint8_t length = channelsCount;
    uint64_t timestamp = micros();
    uint8_t failFlag = false;
    uint8_t changedDataCounter = 0;
    failFlag = connectionLost(timestamp);

    if (!withFailSafe)
    {
      if (!failFlag)
      {
        for (uint8_t i = 0; i < length; i++)
        {
          changedDataCounter += readChannel(&Channels[i]);
        }
      }
    }
    else
    {
      for (uint8_t i = 0; i < length; i++)
      {
        changedDataCounter += readChannelFailSafe(&Channels[i], timestamp);
      }
    }

    if (failFlag)
      resetRadioInterrupts();

    if (changedDataCounter > 0)
      return Channels;
  }
  return NULL;
}

bool            EcliptarRemoteControllerPPM::connectionLost(uint64_t timestamp)
{
  if (timestamp - frameEndTime > 50000)
    return true;
  return false;
}

uint8_t         EcliptarRemoteControllerPPM::readChannel(ControlChannel* channel)
{
  if (channel->newDataAvailable = readPulse(channel))
  {
    channel->controlValue = pulseToPercents(channel);
    return 1;
  }
  return 0;
}
uint8_t         EcliptarRemoteControllerPPM::readChannelFailSafe(ControlChannel* channel, uint64_t timestamp)
{
  if (channel->newDataAvailable = readPulse(channel))
  {
    if (signalLostHandler(channel, timestamp))
      channel->controlValue = channel->signalLostPosition;
    else
      channel->controlValue = pulseToPercents(channel);
  }
  return channel->newDataAvailable;
}

uint8_t         EcliptarRemoteControllerPPM::readChannelFailSafe(ControlChannel* channel)
{
  return readChannelFailSafe(channel, micros());
}

int             EcliptarRemoteControllerPPM::pulseToPercents(ControlChannel* channel)
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
    (((midPulse - pulse) * 100) / (midPulse - minPulse)) * -1 :
    (((pulse - midPulse) * 100) / (maxPulse - midPulse));
}

uint8_t         EcliptarRemoteControllerPPM::readPulse(ControlChannel* channel)
{
  cli();
  int16_t newWidth = channel->innerPulseWidth;
  sei();
  int16_t width = channel->pulseWidth;
  int16_t  widthDifference = width - newWidth;
  if (widthDifference < 0)
    widthDifference *= -1;
  if (widthDifference > PULSE_DROP_CHANGE_BELOW_US)
  {
    // if (!(!channel->isSwitch && widthDifference > PULSE_DROP_CHANGE_ABOVE_US))
    // {
      channel->pulseWidth = (widthDifference < PULSE_DIVIDE_TOLERANCE_US) ? (width + newWidth) / 2 : newWidth;
      return 1;
    // }
  }
  return 0;
}

bool            EcliptarRemoteControllerPPM::signalLostHandler(ControlChannel* channel, uint64_t timestamp)
{
  if (channel->pulseWidth < 500 ||
    channel->pulseWidth > 2500)
    return true;
  return false;
}

void            EcliptarRemoteControllerPPM::printInfoReport(ControlChannel* channels)
{
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
    Serial.print("FRACTION:");
    for (uint8_t i = 0; i < length; i++)
    {
      Serial.print(" CH" + String(i + 1) + ": " + String(Channels[i].controlValue));
    }
  }
}

void            EcliptarRemoteControllerPPM::handleInterrupt(uint8_t interruptNum)
{
  if ((PIND & pinStateRegistry) != 0)
  {
    uint64_t currentTime = micros();
    uint16_t pulse = currentTime - pulseEndTime;
    pulseEndTime = currentTime;

    if (pulse > END_FRAME_TIME_US)
    {
      pulseCounter = 0;
      innerFrameEndTime = currentTime;
      if (resetMode)
      {
        resetMode = false;
        newDataAvailable = false;
      }
      else
      {
        newDataAvailable = true;
      }
    }
    else if (!resetMode && pulseCounter < channelsCount)
    {
      Channels[pulseCounter].innerPulseWidth = pulse;
      pulseCounter++;
    }
  }
}

void            EcliptarRemoteControllerPPM::resetRadioInterrupts()
{
  resetMode = true;
  newDataAvailable = false;
  for (uint8_t i = 0; i < channelsCount; i++)
    Channels[i].newDataAvailable = false;
}

EcliptarRemoteControllerPPM RemoteController;

