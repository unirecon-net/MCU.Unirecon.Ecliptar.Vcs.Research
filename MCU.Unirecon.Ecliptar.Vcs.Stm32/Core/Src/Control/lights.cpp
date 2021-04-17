/*
 * ecliptar_steering.c
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */

#include "Control/lights.h"

LightsController::LightsController()
{
    attachController(4);
}

void        LightsController::assign(int controlSetId, uint16_t frontLightPin, int frontLightChannelNo, uint16_t rearLightPin, int rearLightChannelNo)
{
    assign(controlSetId, frontLightPin, &RC.Channels[frontLightChannelNo - 1], rearLightPin, &RC.Channels[rearLightChannelNo - 1]);
}
void        LightsController::assign(int controlSetId, uint16_t frontLightPin, ControlChannel* frontLightChannel, uint16_t rearLightPin, ControlChannel* rearLightChannel)
{
    FrontLight.LightPin = frontLightPin;
    FrontLightChannel[controlSetId] = frontLightChannel;

    RearLight.LightPin = rearLightPin;
    RearLightChannel[controlSetId] = rearLightChannel;
}

void        LightsController::handleControl(uint8_t controlSetId)
{
    ControlChannel* front = FrontLightChannel[controlSetId];
    if (front != NULL)
    {
        if (front->available)
        {
            int state = front->controlValue;
            if ((state < 0 && FrontLight.powerOn) ||
                (state > 0 && !FrontLight.powerOn))
                lightSwitch(&FrontLight);
        }
    }

    ControlChannel* rear = RearLightChannel[controlSetId];
    if (rear != NULL)
    {
        if (rear->available)
        {
            int state = rear->controlValue;
            if ((state < 0 && RearLight.powerOn) ||
                (state > 0 && !RearLight.powerOn))
                lightSwitch(&RearLight);
        }
    }
}
void        LightsController::handleControlStop()
{
    if (FrontLight.powerOn)
        lightSwitch(&FrontLight);
    if (RearLight.powerOn)
        lightSwitch(&RearLight);
}

void        LightsController::lightSwitch(LightProperties* light)
{
    if (light->powerOn)
    {
        light->powerOn = false;
        HAL_GPIO_WritePin(light->LightPort, light->LightPin, LOW);
    }
    else
    {
        light->powerOn = true;
        HAL_GPIO_WritePin(light->LightPort, light->LightPin, HIGH);
    }
}




