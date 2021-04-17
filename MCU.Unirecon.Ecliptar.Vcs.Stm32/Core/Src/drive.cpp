/*
 * ecliptar_drive.c
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */

#include "drive.h"


DriveSystem::DriveSystem() // @suppress("Class members should be properly initialized")
{
	ecliptors[0] = &LeftEcliptor;
	ecliptors[1] = &RightEcliptor;
}

bool DriveSystem::Init(AdcData *hallsensors)
{
	if (LeftEcliptor.Attached() && RightEcliptor.Attached() && Motor.Attached())
	{
		LeftEcliptor.RotatePercents(0);
		RightEcliptor.RotatePercents(0);
		Converter = hallsensors;
		InitedAnalogConverters.Add(this);

		HAL_ADC_Start_DMA(Converter->adc,
				(uint32_t*) Converter->conversions->Data.buffer,
				Converter->conversionsLength);

		isActive = 1;
		return true;
	}
	return false;
}
bool DriveSystem::Inited()
{
	return isActive;
}

bool DriveSystem::Available()
{
	if (LeftEcliptor.newMotionData || RightEcliptor.newMotionData)
	{
		return true;
	}
	return false;
}

void DriveSystem::PrintMotionReport()
{
	if (LeftEcliptor.newMotionData)
	{
		MotionData *left = LeftEcliptor.readEclipticMotion();

		printxln(" L_^_V: %.4f L_^_A: %.4f L_v_V: %.4f L_v_A: %.4f L_o_V: %.4f L_o_A: %.4f",
					left->riseVelocity, left->riseAcceleration,left->fallVelocity, left->fallAcceleration,
					left->angularVelocity, left->angularAcceleration);

	}

	if (RightEcliptor.newMotionData)
	{
		MotionData *right = RightEcliptor.readEclipticMotion();

		printxln(" R_^_V: %.4f R_^_A: %.4f R_v_V: %.4f R_v_A: %.4f R_o_V: %.4f R_o_A: %.4f",
						right->riseVelocity, right->riseAcceleration,right->fallVelocity, right->fallAcceleration,
						right->angularVelocity, right->angularAcceleration);
	}
}
void DriveSystem::PrintMeasureReport()
{

	bool printable = false;
	MeasureData *left;
	MeasureData *right;
	if (LeftEcliptor.newMeasureData)
	{
		left = LeftEcliptor.readSensorMeasure();
		printable = true;
	}
	if (RightEcliptor.newMeasureData)
	{
		right = RightEcliptor.readSensorMeasure();
		printable = true;
	}

	if (printable)
	{
		uint16_t* l = (uint16_t*)left->operative->PeekLast(10);
		uint16_t* r = (uint16_t*)right->operative->PeekLast(10);
		printxln(" L_~_M: %d, %d, %d, %d, %d <-> R_~_M: %d, %d, %d, %d, %d",
					l[4], l[3], l[2], l[1], l[0], r[4], r[3], r[2], r[1], r[0]);

	}
}
void DriveSystem::PrintDynamicsReport()
{
	bool newline;

	if (LeftEcliptor.newDynamicsData)
	{
		DynamicsData *left = LeftEcliptor.readSensorDynamics();
		newline = true;
		printnln();

		print(" L_>_V: %d", left->fallDelta);
		print(" L_>_A: %d", left->riseDelta);
	}
	if (RightEcliptor.newDynamicsData)
	{
		DynamicsData *right = RightEcliptor.readSensorDynamics();

		if (!newline)
			printnln();

		print(" R_>_V: %d", right->fallDelta);
		print(" R_>_A: %d", right->riseDelta);
	}
}
void DriveSystem::PrintPositionReport()
{
	bool newline = false;

	if (LeftEcliptor.newPositionData)
	{
		PositionData *left = LeftEcliptor.readSensorPosition();
		newline = true;
		printnln();

		print(" L_*_C: %d", left->currentArea);
		print(" L_*_T: %.4f", left->areaPeriodMicros);

		print(" L_/_F: %d", left->secondQuarter);
		print(" L_|_F: %d", left->secondHalf);
		print(" L_|_T: %.4f", left->halfPeriodMicros);

	}

	if (RightEcliptor.newPositionData)
	{
		PositionData *right = RightEcliptor.readSensorPosition();

		if (!newline)
			printnln();

		print(" R_*_C: %d", right->currentArea);
		print(" R_*_T: %.4f", right->areaPeriodMicros);

		print(" R_/_F: %d", right->secondQuarter);
		print(" R_|_F: %d", right->secondHalf);
		print(" R_|_T: %.4f", right->halfPeriodMicros);
	}
}
void DriveSystem::PrintInfoReport()
{
	bool newline = false;

	MotionData *left = LeftEcliptor.readEclipticMotion();
	newline = true;
	printnln();

	print(" L_^_V: %.6f", left->riseVelocity);
	print(" L_^_A: %.6f", left->riseAcceleration);
	print(" L_v_V: %.6f", left->fallVelocity);
	print(" L_v_A: %.6f", left->fallAcceleration);
	print(" L_o_V: %.6f", left->angularVelocity);
	print(" L_o_A: %.6f", left->angularAcceleration);

	MotionData *right = RightEcliptor.readEclipticMotion();

	if (!newline)
		printnln();

	print(" R_^_V: %.6f", right->riseVelocity);
	print(" R_^_A: %.6f", right->riseAcceleration);
	print(" R_v_V: %.6f", right->fallVelocity);
	print(" R_v_A: %.6f", right->fallAcceleration);
	print(" R_o_V: %.6f", right->angularVelocity);
	print(" R_o_A: %.6f", right->angularAcceleration);

}

inline void DriveSystem::ConversionComplete(AdcData *stream, size_t size)
{
	 LeftEcliptor.OperativeMeasurements(stream, size / 2);
	RightEcliptor.OperativeMeasurements(stream, size / 2);
}
inline void DriveSystem::ConversionHalfComplete(AdcData *stream, size_t size)
{
	 LeftEcliptor.OperativeMeasurements(stream, size / 2);
	RightEcliptor.OperativeMeasurements(stream, size / 2);
}

DriveSystem Drive;
