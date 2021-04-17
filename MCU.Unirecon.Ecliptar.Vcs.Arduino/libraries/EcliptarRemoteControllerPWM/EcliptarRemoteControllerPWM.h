/*
  EcliptarRemoteControllerPWM.h - EcliptarRemoteControllerPWM, PPM signal read library  - ver. 0.4.5 
  Copyright (c) 2020 Unirecon Dariusz Hanc.  All right reserved.
*/

#include <inttypes.h>

#ifndef EcliptarRemoteControllerPWM_h
#define EcliptarRemoteControllerPWM_h

#define DEFAULT_MIN_PULSE_US             1000
#define DEFAULT_MID_PULSE_US             1500
#define DEFAULT_MAX_PULSE_US             2000
#define DEFAULT_SIGNAL_LOST              0.00F
#define MIN_TIME_BETWEEN_CHANNELS_US     400UL
#define DEFAULT_PULSE_PERIOD_US          20000UL
#define PULSE_TOLERANCE_US               50

typedef struct
{
  uint8_t                 channelPin :6;    
  byte                    channelRegistry;
  uint8_t                 channelPort :2;
  uint16_t                minPulseWidth;
  uint16_t                maxPulseWidth;
  uint16_t                midPulseWidth;
  float                   signalLostPosition;
  volatile uint8_t        pinState :1;
  volatile uint16_t       innerPulseWidth;
  volatile uint32_t       innerPulsePeriod;
  volatile uint64_t       pulseStartTime;
  volatile uint16_t       pulseWidth;
  volatile uint32_t       pulsePeriod;
  volatile float          pulseFraction;
  volatile uint8_t        newDataAvailable :1;

} RadioChannel;


class EcliptarRemoteControllerPWM
{
public: 
                EcliptarRemoteControllerPWM();

void            attach(uint8_t channelsCount, uint8_t pinsArray[]);
void            attach(uint8_t channelsCount, uint8_t pinsArray[], uint16_t minPulseArray[], uint16_t midPulseArray[], uint16_t maxPulseArray[]);
void            attach(uint8_t channelsCount, uint8_t pinsArray[], uint16_t minPulseArray[], uint16_t midPulseArray[], uint16_t maxPulseArray[], float signalLostArray[]);
void            attach(uint8_t channelsCount, RadioChannel channelArray[]);
bool            attached();

bool            dataAvailable();

bool            timerAllowed();

RadioChannel*   readChannels();
bool            readChannel(RadioChannel* channel);

RadioChannel*   readFractions();
float           readFraction(RadioChannel* channel, uint64_t timestamp);
float           readFraction(RadioChannel* channel);

uint16_t        readPulse(RadioChannel* channel);

float           pulseToFraction(RadioChannel* channel);

void            printPulseReport();    
void            printFractionReport();            

void            handleRadioInterrupt(uint8_t port, uint64_t timestamp, byte registryValue);

void            resetRadioInterrupts();



RadioChannel*    Channels;  
uint8_t          channelsCount :6;  

RadioChannel**   PortChannels[2];
uint8_t          portChannelsCount[2];

volatile uint8_t interruptsCount;

volatile uint64_t lastPulseEndTime;

volatile uint8_t newDataAvailable :1;
volatile uint8_t resetModeState :1;

private:

void             attachPortChannels();
bool             signalLostHandler(RadioChannel* channel, uint64_t timestamp);
void             attachInterrupts();
void             attachInterrupt(byte pin);
void             disableTimer2();
void             enableTimer2();

};

extern EcliptarRemoteControllerPWM RadioController;

#endif
