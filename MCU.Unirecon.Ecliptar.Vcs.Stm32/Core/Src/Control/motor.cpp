/*
 * ecliptar_steering.c
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */

#include "Control/motor.h"

MotorController::MotorController(MotorDriver* motorDriver, int id)
{
    Driver = motorDriver;
    attachController(id);
}

void        MotorController::assign(int controlSetId, int motorChannelNo)
{
    assign(controlSetId, &RC.Channels[motorChannelNo - 1]);
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
        if (channel->available)
            Driver->PowerLevel(channel->controlValue);
    }
}
void        MotorController::handleControlStop()
{
    Driver->PowerLevel(0);
}



