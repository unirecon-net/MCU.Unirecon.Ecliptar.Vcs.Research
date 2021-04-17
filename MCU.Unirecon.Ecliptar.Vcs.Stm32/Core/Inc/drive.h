/*
 * ecliptar_drive.h
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */

#ifndef _DRIVE_H_
#define _DRIVE_H_

#include "Drive/ecliptor.h"
#include "Drive/motor.h"
#include "system.h"

class DriveSystem: public AnalogConverter
{
public:
	DriveSystem();

	bool Init(AdcData *hallsensors);
	bool Inited();

	bool Available();

	virtual void ConversionComplete(AdcData *stream, size_t size);
	virtual void ConversionHalfComplete(AdcData *stream, size_t size);

	void PrintMotionReport();
	void PrintMeasureReport();
	void PrintDynamicsReport();
	void PrintPositionReport();
	void PrintInfoReport();

	EcliptorDriver LeftEcliptor;
	EcliptorDriver RightEcliptor;
	MotorDriver Motor;
	uint8_t isActive :1;

private:

	EcliptorDriver *ecliptors[2];

};

extern DriveSystem Drive;

#endif /* _DRIVE_H_ */
