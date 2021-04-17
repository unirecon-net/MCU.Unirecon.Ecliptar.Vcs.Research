/*
  EcliptarSteeringSystem.h - EcliptarSteeringSystem control library - ver. 0.3 
  Copyright (c) 2020 Unirecon Dariusz Hanc.  All right reserved.
*/

#ifndef EcliptarSteeringSystem_h
#define EcliptarSteeringSystem_h

#include <inttypes.h>
#include <EcliptarRemoteControllerPPM.h>
#include <EcliptarDriveSystem.h>

#define TRUNC_DECIMAL(f, c) (((float)((int)((f) * (c))) / (c)))

class ControlHandler
{
  public:

  virtual void handleControl(uint8_t controlSetId) = 0;
  virtual void handleControlStop() = 0;
  void         attachController(uint8_t orderId);

};

typedef enum
{
  left = -1, right = 1
} MixDirection;

class MotorController : ControlHandler
{

public: 

                          MotorController();

void                      assign(int controlSetId, int motorChannelNo);     
void                      assign(int controlSetId, ControlChannel* motorChannel);         

virtual void              handleControl(uint8_t controlSetId);
virtual void              handleControlStop();

ControlChannel*             MotorChannel[3];

MotorDrive*               Control;

};

class EcliptorController : ControlHandler
{

public: 

                          EcliptorController(Ecliptor* ecliptor, int orderId);

void                      assign(int controlSetId, int eccntrChannelNo,  int eccntrMixChannelNo,  MixDirection eccntrMix, int rotatorChannelNo, int rotatorMixChannelNo, MixDirection rotatorMix);     
void                      assign(int controlSetId, ControlChannel* eccntrChannel,  ControlChannel* eccntrMixChannel,  MixDirection eccntrMix, ControlChannel* rotatorChannel, ControlChannel* rotatorMixChannel, MixDirection rotatorMix);                      

virtual void              handleControl(uint8_t controlSetId);          
virtual void              handleControlStop();                                         

ControlChannel*             EccntrChannel[3];
ControlChannel*             EccntrMixChannel[3];
MixDirection              eccntrMixDirection;

ControlChannel*             RotatorChannel[3];
ControlChannel*             RotatorMixChannel[3];
MixDirection              rotatorMixDirection;

Ecliptor*                 Control;
RotatorProperties*        RotatorControl;
EccentratorProperties*    EccentratorControl;

uint8_t                   orderId;

private:

void                      handleEccntrControl(uint8_t controlSetId);
void                      handleRotatorControl(uint8_t controlSetId);        

};

typedef struct
{
  EcliptarPin                LightPin;
  int                        powerLevel;
  uint8_t                    powerOn :1;
} LightingProperties;

class LightingController : ControlHandler
{

public: 

                          LightingController();

void                      assign(int controlSetId, int frontLightPin, int frontLightChannelNo, int rearLightPin, int rearLightChannelNo);     
void                      assign(int controlSetId, int frontLightPin, ControlChannel* frontLightChannelN, int rearLightPin, ControlChannel* rearLightChannel);                             

virtual void              handleControl(uint8_t controlSetId);
virtual void              handleControlStop();

void                      lightSwitch(LightingProperties* light);

ControlChannel*           FrontLightChannel[3];
LightingProperties        FrontLight;

ControlChannel*           RearLightChannel[3];
LightingProperties        RearLight;

};

class EcliptarControlSystem : ControlHandler
{

public: 
                          EcliptarControlSystem();

void                      assign(int controlSetId, int mainPowerChannelNo);     
void                      assign(int controlSetId, ControlChannel* mainPowerChannel);

bool                      initialize(int startChannelNo, int controlSetChannelNo);
bool                      initialize(ControlChannel* startChannel, ControlChannel* controlSetChannel);

bool                      determine(ControlChannel* channels); 

virtual void              handleControl(uint8_t controlSetId);            
virtual void              handleControlStop();            

ControlChannel*             MainPowerChannel[3];
ControlChannel*             StartChannel;
ControlChannel*             ControlSetChannel;

EcliptorController        LeftEcliptor = EcliptorController(&EclipticalDrive.LeftEcliptor, 1);
EcliptorController        RightEcliptor = EcliptorController(&EclipticalDrive.RightEcliptor, 2);

MotorController           Motor;
LightingController        Lighting;

ControlHandler*           Controllers[5];  

int                       mainPowerPercents;
uint8_t                   started : 1;
uint8_t                   stopped : 1;
uint8_t                   controlSet : 2;

};

extern EcliptarControlSystem     EcliptarControl;

#endif




