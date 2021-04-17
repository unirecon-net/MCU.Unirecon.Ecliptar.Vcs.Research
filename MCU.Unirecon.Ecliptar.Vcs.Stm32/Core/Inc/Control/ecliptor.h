/*
 * ecliptar_steering.c
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */
#ifndef ___CONTROL_ECLIPTOR_H
#define ___CONTROL_ECLIPTOR_H

#include "Control/base.h"

class EcliptorController : ControlHandler // @suppress("Class has a virtual method and non-virtual destructor")
{

public:

                          EcliptorController(EcliptorDriver* ecliptor, int orderId);

void                      assign(int controlSetId, int eccntrChannelNo,  int eccntrMixChannelNo,  MixDirection eccntrMix, int rotatorChannelNo, int rotatorMixChannelNo, MixDirection rotatorMix);
void                      assign(int controlSetId, ControlChannel* eccntrChannel,  ControlChannel* eccntrMixChannel,  MixDirection eccntrMix, ControlChannel* rotatorChannel, ControlChannel* rotatorMixChannel, MixDirection rotatorMix);

virtual void              handleControl(uint8_t controlSetId);
virtual void              handleControlStop();

ControlChannel*           EccntrChannel[3];
ControlChannel*           EccntrMixChannel[3];
MixDirection              eccntrMixDirection;

ControlChannel*           RotatorChannel[3];
ControlChannel*           RotatorMixChannel[3];
MixDirection              rotatorMixDirection;

EcliptorDriver*           Driver;
RotatorProperties*        RotatorControl;
EccentratorProperties*   EccentratorControl;

private:

void                      handleEccntrControl(uint8_t controlSetId);
void                      handleRotatorControl(uint8_t controlSetId);

};

#endif /* ___CONTROL_ECLIPTOR_H */
