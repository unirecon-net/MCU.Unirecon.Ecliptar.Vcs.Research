/*
EcliptarSteeringSystem.cpp - EcliptarSteeringSystem control library - ver. 0.3
Copyright (c) 2020 Unirecon Dariusz Hanc.  All right reserved.
*/

#include <Arduino.h>
#include <inttypes.h>
#include <EcliptarRemoteControllerPPM.h>
#include <EcliptarDriveSystem.h>
#include "EcliptarSteeringSystem.h"


void        ControlHandler::attachController(uint8_t orderId)
{
    EcliptarControl.Controllers[orderId] = this;
}

MotorController::MotorController()
{
    Control = &EclipticalDrive.Motor;
    attachController(3);
}

void        MotorController::assign(int controlSetId, int motorChannelNo)
{
    assign(controlSetId, &RemoteController.Channels[motorChannelNo - 1]);
}
void        MotorController::assign(int controlSetId, ControlChannel* motorChannel)
{
    MotorChannel[controlSetId] = motorChannel;
}

void        MotorController::handleControl(uint8_t controlSetId)
{
    ControlChannel* channel = MotorChannel[controlSetId];
    if (channel != NULL)
    {
        if (channel->newDataAvailable)
            Control->powerLevel((channel->controlValue * 255) / 100);
    }
}
void        MotorController::handleControlStop()
{
    Control->powerLevel(0);
}

EcliptorController::EcliptorController(Ecliptor* ecliptor, int orderId)
{
    Control = ecliptor;
    EccentratorControl = &ecliptor->Eccentrator;
    RotatorControl = &ecliptor->Rotator;

    this->orderId = orderId;
    attachController(orderId);
}

void        EcliptorController::assign(int controlSetId, int eccntrChannelNo, int eccntrMixChannelNo, MixDirection eccntrMix, int rotatorChannelNo, int rotatorMixChannelNo, MixDirection rotatorMix)
{
    assign(controlSetId, &RemoteController.Channels[eccntrChannelNo - 1], &RemoteController.Channels[eccntrMixChannelNo - 1], eccntrMix,
        &RemoteController.Channels[rotatorChannelNo - 1], &RemoteController.Channels[rotatorMixChannelNo - 1], rotatorMix);
}
void        EcliptorController::assign(int controlSetId, ControlChannel* eccntrChannel, ControlChannel* eccntrMixChannel, MixDirection eccntrMix, ControlChannel* rotatorChannel, ControlChannel* rotatorMixChannel, MixDirection rotatorMix)
{
    EccntrChannel[controlSetId] = eccntrChannel;
    EccntrMixChannel[controlSetId] = eccntrMixChannel;
    eccntrMixDirection = eccntrMix;

    RotatorChannel[controlSetId] = rotatorChannel;
    RotatorMixChannel[controlSetId] = rotatorMixChannel;
    rotatorMixDirection = rotatorMix;
}

void        EcliptorController::handleControl(uint8_t controlSetId)
{
    handleEccntrControl(controlSetId);

    handleRotatorControl(controlSetId);

    //Control->computeMotion();
}
void        EcliptorController::handleControlStop()
{
    Control->powerLevel(0);
    Control->rotatePercents(0);
}

void        EcliptorController::handleEccntrControl(uint8_t controlSetId)
{
    uint8_t dataChanged = 0;
    ControlChannel* channel = EccntrChannel[controlSetId];
    EccentratorProperties* eccentr = &Control->Eccentrator;
    if (channel != NULL)
    {
        dataChanged += channel->newDataAvailable;
        int percents = channel->controlValue;
        ControlChannel* mixchannel = EccntrMixChannel[controlSetId];
        if (mixchannel != NULL)
        {
            dataChanged += mixchannel->newDataAvailable;
            if (dataChanged > 0)
            {
                percents = percents - (mixchannel->controlValue * eccntrMixDirection);
                percents = constrain(percents, 0, 100);
            }
        }
        if (dataChanged > 0)
        {
            Control->powerLevel((percents * 255) / 100);
        }
    }
}

void        EcliptorController::handleRotatorControl(uint8_t controlSetId)
{
    uint8_t dataChanged = 0;
    ControlChannel* channel = RotatorChannel[controlSetId];
    if (channel != NULL)
    {
        dataChanged += channel->newDataAvailable;
        int percents = channel->controlValue;
        ControlChannel* mixchannel = RotatorMixChannel[controlSetId];
        if (mixchannel != NULL)
        {
            dataChanged += mixchannel->newDataAvailable;
            if (dataChanged > 0)
            {
                percents = percents - (mixchannel->controlValue * rotatorMixDirection);
                percents = constrain(percents, -100, 100);
            }
        }
        if (dataChanged > 0)
        {
            Control->rotatePercents(percents);
        }
    }
}

LightingController::LightingController()
{
    attachController(4);
}

void        LightingController::assign(int controlSetId, int frontLightPin, int frontLightChannelNo, int rearLightPin, int rearLightChannelNo)
{
    assign(controlSetId, frontLightPin, &RemoteController.Channels[frontLightChannelNo - 1], rearLightPin, &RemoteController.Channels[rearLightChannelNo - 1]);
}
void        LightingController::assign(int controlSetId, int frontLightPin, ControlChannel* frontLightChannel, int rearLightPin, ControlChannel* rearLightChannel)
{
    FrontLight.LightPin.number = frontLightPin;
    FrontLight.LightPin.isActive = true;
    pinMode(frontLightPin, OUTPUT);
    FrontLightChannel[controlSetId] = frontLightChannel;

    RearLight.LightPin.number = rearLightPin;
    RearLight.LightPin.isActive = true;
    pinMode(rearLightPin, OUTPUT);
    RearLightChannel[controlSetId] = rearLightChannel;
}

void        LightingController::handleControl(uint8_t controlSetId)
{
    ControlChannel* front = FrontLightChannel[controlSetId];
    if (front != NULL)
    {
        if (front->newDataAvailable)
        {
            int state = front->controlValue;
            if (state < 0 && FrontLight.powerOn ||
                state > 0 && !FrontLight.powerOn)
                lightSwitch(&FrontLight);
        }
    }

    ControlChannel* rear = RearLightChannel[controlSetId];
    if (rear != NULL)
    {
        if (rear->newDataAvailable)
        {
            int state = rear->controlValue;
            if (state < 0 && RearLight.powerOn ||
                state > 0 && !RearLight.powerOn)
                lightSwitch(&RearLight);
        }
    }
}
void        LightingController::handleControlStop()
{
    if (FrontLight.powerOn)
        lightSwitch(&FrontLight);
    if (RearLight.powerOn)
        lightSwitch(&RearLight);
}

void        LightingController::lightSwitch(LightingProperties* light)
{
    if (light->powerOn)
    {
        light->powerOn = false;
        digitalWrite(light->LightPin.number, LOW);
    }
    else
    {
        light->powerOn = true;
        digitalWrite(light->LightPin.number, HIGH);
    }
}

EcliptarControlSystem::EcliptarControlSystem()
{
    started = false;
    stopped = true;
    controlSet = 0;
    attachController(0);
}

void        EcliptarControlSystem::assign(int controlSetId, int mainPowerChannelNo)
{
    assign(controlSetId, &RemoteController.Channels[mainPowerChannelNo - 1]);
}
void        EcliptarControlSystem::assign(int controlSetId, ControlChannel* mainPowerChannel)
{
    MainPowerChannel[controlSetId] = mainPowerChannel;
}

bool        EcliptarControlSystem::initialize(int startChannelNo, int controlSetChannelNo)
{
    initialize(&RemoteController.Channels[startChannelNo - 1], &RemoteController.Channels[controlSetChannelNo - 1]);
}
bool        EcliptarControlSystem::initialize(ControlChannel* startChannel, ControlChannel* controlSetChannel)
{
    StartChannel = startChannel;
    ControlSetChannel = controlSetChannel;
}

void        EcliptarControlSystem::handleControl(uint8_t controlSetId)
{
    if (MainPowerChannel[controlSetId] != NULL)
        mainPowerPercents = MainPowerChannel[controlSetId]->controlValue;

    Motor.Control->Properties.revolutionSpeed = LeftEcliptor.Control->Motion.angularVelocity / 60;
}
void        EcliptarControlSystem::handleControlStop()
{
    mainPowerPercents = 0;
}

bool        EcliptarControlSystem::determine(ControlChannel* channels)
{

    if (channels != NULL)
    {
        if (ControlSetChannel->newDataAvailable)
        {
           uint8_t _controlSet = (uint8_t)(((ControlSetChannel->controlValue + 10) * 2) / 100);
              if (controlSet != _controlSet)
               controlSet = _controlSet;
        }
        if (StartChannel->newDataAvailable)
        {
            uint8_t _started = (uint8_t)((StartChannel->controlValue + 10) / 100);
            if (started != _started)
            {
                if (_started)
                    stopped = false;
                started = _started;
            }
        }
    }
    if (started)
    {
        for (int i = 0; i < 5; i++)
            Controllers[i]->handleControl(controlSet);

        return true;
    }
    // else if (!stopped)
    // {
    //     for (int i = 0; i < 5; i++)
    //         Controllers[i]->handleControlStop();
    //     stopped = true;
    // }


    return false;
}

EcliptarControlSystem     EcliptarControl;

