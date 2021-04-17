/*
  EcliptarRemoteControllerPPM.h - EcliptarRemoteControllerPPM, PPM signal read library  - ver. 0.4.5 
  Copyright (c) 2020 Unirecon Dariusz Hanc.  All right reserved.
*/

#include <inttypes.h>

#ifndef EcliptarRemoteControllerPPM_h
#define EcliptarRemoteControllerPPM_h

#define DEFAULT_MIN_PULSE_US             1000
#define DEFAULT_MID_PULSE_US             1500
#define DEFAULT_MAX_PULSE_US             2000
#define DEFAULT_SIGNAL_LOST              0
#define HIGH_STATE_TIME_US               600UL
#define END_FRAME_TIME_US                2500UL
#define FRAME_PERIOD_US                  20000UL
#define PULSE_DROP_CHANGE_BELOW_US       2
#define PULSE_DROP_CHANGE_ABOVE_US       1000
#define PULSE_DIVIDE_TOLERANCE_US        40


typedef struct 
{  
  uint16_t                minPulseWidth;
  uint16_t                maxPulseWidth;
  uint16_t                midPulseWidth;
  int                     signalLostPosition;
  volatile uint16_t       innerPulseWidth;
  volatile uint16_t       pulseWidth;
  volatile int            controlValue;
  volatile uint8_t        newDataAvailable :1;
  uint8_t                 isSwitch :1;
} ControlChannel;

class EcliptarInterruptHandler {
public:
  virtual void handleInterrupt(uint8_t interruptNum) = 0;
  void attachInterrupt(uint8_t interruptNum, uint8_t mode);
  void detachInterrupt(uint8_t interruptNum);
};

class EcliptarRemoteControllerPPM : EcliptarInterruptHandler
{
public: 
                EcliptarRemoteControllerPPM();

void            attach(uint8_t channelsCount, uint8_t interruptPin);
void            attach(uint8_t channelsCount, uint8_t interruptPin, uint16_t minPulseArray[], uint16_t midPulseArray[], uint16_t maxPulseArray[]);
void            attach(uint8_t channelsCount, uint8_t interruptPin, uint16_t minPulseArray[], uint16_t midPulseArray[], uint16_t maxPulseArray[], int signalLostArray[]);
void            attach(uint8_t channelsCount, uint8_t interruptPin, ControlChannel channelArray[]);
bool            attached();

void            defineSwitches(uint8_t switchChannels[]);

bool            dataAvailable();

bool            connectionLost(uint64_t timestamp);

ControlChannel*   readChannels(bool withFailSafe);
uint8_t         readChannel(ControlChannel* channel);
uint8_t         readChannelFailSafe(ControlChannel* channel, uint64_t timestamp);
uint8_t         readChannelFailSafe(ControlChannel* channel);

uint8_t         readPulse(ControlChannel* channel);

int             pulseToPercents(ControlChannel* channel);

void            printInfoReport(ControlChannel* channels);           

virtual void    handleInterrupt(uint8_t interruptNum);

void            resetRadioInterrupts();

ControlChannel*     Channels;  
uint8_t           channelsCount :6;  

volatile uint8_t  pulseCounter;
volatile uint64_t pulseEndTime;
volatile uint64_t innerFrameEndTime;
volatile uint64_t frameEndTime;

byte              pinStateRegistry;

volatile uint8_t  newDataAvailable :1;
volatile uint8_t  resetMode :1;

private:

bool             signalLostHandler(ControlChannel* channel, uint64_t timestamp);

};

extern EcliptarRemoteControllerPPM RemoteController;

#endif
