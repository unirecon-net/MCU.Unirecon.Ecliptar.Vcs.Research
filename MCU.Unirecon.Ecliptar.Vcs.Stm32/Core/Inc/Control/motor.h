/*
 * ecliptar_steering.c
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */


#ifndef _CONTROL_MOTOR_H_
#define _CONTROL_MOTOR_H_

#include "Control/base.h"

class MotorController : ControlHandler // @suppress("Class has a virtual method and non-virtual destructor")
{
public:
							MotorController(MotorDriver* motorDriver, int id);

void                      	assign(int controlSetId, int motorChannelNo);
void                      	assign(int controlSetId, ControlChannel* motorChannel);

virtual void             	handleControl(uint8_t controlSetId);
virtual void             	handleControlStop();

ControlChannel*          	MotorChannel[3];

MotorDriver*				Driver;

};



#endif /* _CONTROL_MOTOR_H_ */

