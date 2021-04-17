/*
 * ecliptar_steering.h
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */

#ifndef _CONTROL_LIGTHS_H_
#define _CONTROL_LIGTHS_H_

#include <gpio.h>

#include "Control/base.h"

typedef struct
{
  uint16_t	                LightPin;
  GPIO_TypeDef*			LightPort;
  uint32_t                 powerLevel;
  float						dutyLevel;
  uint8_t                  powerOn :1;
} LightProperties;


class LightsController : ControlHandler // @suppress("Class has a virtual method and non-virtual destructor")
{
public:
                          LightsController();

void                      assign(int controlSetId, uint16_t frontLightPin, int frontLightChannelNo, uint16_t rearLightPin, int rearLightChannelNo);
void                      assign(int controlSetId, uint16_t frontLightPin, ControlChannel* frontLightChannelN, uint16_t rearLightPin, ControlChannel* rearLightChannel);

virtual void              handleControl(uint8_t controlSetId);
virtual void              handleControlStop();

void                      lightSwitch(LightProperties* light);

ControlChannel*          FrontLightChannel[3];
LightProperties          FrontLight;

ControlChannel*          RearLightChannel[3];
LightProperties          RearLight;

};



#endif /* _CONTROL_LIGTHS_H_ */
