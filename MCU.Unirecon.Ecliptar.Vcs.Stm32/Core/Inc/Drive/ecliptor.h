/*
 * ecliptar_drive.h
 *
 *  Created on: Mar 15, 2021
 *      Author: merlin
 */

#ifndef __DRIVE_ECLIPTOR_H
#define __DRIVE_ECLIPTOR_H

#include <inttypes.h>

#include "Drive/base.h"

#define MIN_HALLSENSOR_VALUE            		670
#define MAX_HALLSENSOR_VALUE            		4095
#define MIN_HALLSENSOR_THRESHOLD_VALUE  	720
#define MAX_HALLSENSOR_THRESHOLD_VALUE  	920
#define MIN_DISTANCE_MM                 		58
#define MAX_DISTANCE_MM                 		96
#define MIN_SERVO_SIGNAL_US             		1200
#define MAX_SERVO_SIGNAL_US             		4800
#define MIN_ECCNTR_POWER                		100
#define MAX_ECCNTR_POWER                		2047
#define SERVO_ANGLE_RANGE_DEG           		180
#define SERVO_TIMER_COUNTER          		39999
#define BLADE_WEIGHT                    		50
#define COMPUTATIVE_CYCLE_TIME_US  	    	33
#define HALLSENSOR_MEASURES_COUNT       	10

typedef enum
{
	normal = 1, reverse = -1
} RotatorDirection;

typedef enum
{
	None = -1,
	LowHorizontal = 0,
	LowDiagonal = 1,
	LowVertical = 2,
	HighHorizontal = 3,
	RiseDiagonal = 4,
	HighVertical = 5,
	FallDiagonal = 6
} EcliptorArea;

typedef struct
{
	int16_t lowHorizontal, lowDiagonal, lowVertical, highHorizontal, riseDiagonal, highVertical,
			fallDiagonal;

} AreaValues;

typedef struct
{
	inline float angleToRadians(float angle)
	{
		return (angle / 360.00F) * (2.00F * M_PI);
	}
	inline int radiansToAngle(float radians)
	{
		return (radians / (2.00F * M_PI)) * 360.00F;
	}

	inline int radiansToMicroseconds(float radians)
	{

		if (radians != 0)
		{
			if (radians < 0)
				return pulseWidthMid
						- ((radians / angleToRadians(rotationAngleMin))
								* (pulseDeltaMax));
			return pulseWidthMid
					+ ((radians / angleToRadians(rotationAngleMax))
							* (pulseDeltaMax));
		}
		return pulseWidthMid;
	}
	inline int angleToMicroseconds(float angle)
	{

		if (angle != 0)
		{
			return pulseWidthMid
					+ (((float) angle / (rotationAngleRange / 2))
							* pulseDeltaMax);
		}
		return pulseWidthMid;
	}
	inline int percentsToMicroseconds(float percents)
	{
		if (percents != 0)
		{

			return pulseWidthMid
					+ (((float) percents / 100) * pulseDeltaMax);
		}
		return pulseWidthMid;
	}

	PwmTimer* RotatorPwm;
	volatile uint16_t pulseWidth;
	volatile uint16_t pulseWidthMin;
	volatile uint16_t pulseWidthMax;
	volatile uint16_t pulseWidthMid;
	volatile uint16_t pulseDeltaMax;
	volatile uint16_t pulsePeriod;
	volatile float rotationPercents;
	volatile float rotationAngle;
	int16_t rotationAngleRange;
	int16_t rotationAngleMin;
	int16_t rotationAngleMax;
	float trimByDecimalPart;
	RotatorDirection direction;
} RotatorProperties;

typedef struct
{
	PwmTimer* 			PowerPwm;
	volatile int 		powerLevel;
	volatile int 		innerPowerLevel;
	volatile float 	dutyLevel;
	volatile float 	innerDutyLevel;
	int 				maxPowerLevel;
	int 				minPowerLevel;
	volatile uint8_t 	running :1;
	volatile uint8_t 	powerOn :1;
	volatile uint8_t 	powerPwmOn :1;
	volatile uint8_t 	overThold :1;
	volatile uint8_t 	powerLocked :1;
} EccentratorProperties;

typedef struct
{
	uint16_t 			operativeBuffer[10];
	uint16_t 			computativeBuffer[3200];
	uint64_t 			computativeTicksBuffer[64];

	Ring* 				operative = new Ring(operativeBuffer, sizeof(operativeBuffer));
	Ring* 				computative = new Ring(computativeBuffer, sizeof(computativeBuffer));
	Ring* 				computativeTicks = new Ring(computativeTicksBuffer, sizeof(computativeTicksBuffer));

	volatile uint32_t operativeDelta = 0;
	volatile uint64_t operativeMicros = 0;
	volatile uint32_t operativePeriodMicros = 0;

	volatile uint64_t computativeMicros = 0;
	volatile uint32_t computativePeriodMicros = 0;

} MeasureData;

typedef struct
{
	uint8_t 	SetValue(uint16_t value)
	{
		if(value >= sensorMin && value <= sensorMax)
		{
			radialVector = (float)(value - sensorMin) / (float)(sensorMax - sensorMin);
			this->sensorValue = value;
			return 1;
		}
		return 0;
	}
	void		SetRange(uint16_t min, uint16_t max)
	{
		sensorMin = min;
		sensorMax = max;
	}
	uint16_t  	sensorValue = 0;
	uint16_t  	radialVector = 0;
	uint16_t  	arcLength = 0;
	uint16_t  	sensorMin = 0;
	uint16_t  	sensorMax = 0;

} AngularValues;

typedef struct
{
	volatile uint16_t  		riseStartId= 0;
	volatile uint16_t 		riseEndId = 0;
	volatile uint8_t 			riseCounter = 0;
	volatile int16_t 			riseDelta = 0;
	volatile uint8_t 			riseCollecting :1;
	volatile uint8_t 			riseComputing :1;
	volatile uint64_t 		riseStartMicros = 0;
	volatile uint32_t 		risePeriodMicros = 0;

	volatile uint16_t  		fallStartId= 0;
	volatile uint16_t 		fallEndId = 0;
	volatile uint8_t 			fallCounter = 0;
	volatile int16_t 			fallDelta = 0;
	volatile uint8_t 			fallCollecting :1;
	volatile uint8_t 			fallComputing :1;
	volatile uint32_t 		fallStartMicros = 0;
	volatile uint32_t 		fallPeriodMicros = 0;

	volatile AngularValues* 	riseVectors[314];

	volatile AngularValues* 	fallVectors[314];

} DynamicsData;

typedef struct
{
	volatile uint32_t 		currentPositionId = 0;

	volatile EcliptorArea	    currentArea = None;
	volatile uint64_t 		areaStartMicros = 0;
	volatile uint32_t 		areaPeriodMicros = 0;

	volatile uint8_t 			secondQuarter :1;
	volatile uint8_t 			secondHalf :1;
	volatile uint8_t 			newHalf :1;

	volatile uint64_t  		halfStartMicros = 0;
	volatile uint32_t  		halfPeriodMicros = 0;

	volatile uint64_t  		roundStartMicros = 0;
	volatile uint32_t  		roundPeriodMicros = 0;

	AreaValues 				DefinedAreas;

	AngularValues  			AngularPositions[628];

} PositionData;

typedef struct
{
	volatile float angularVelocity;
	volatile float angularAcceleration;

	volatile float riseVelocity;
	volatile float riseAcceleration;

	volatile float fallVelocity;
	volatile float fallAcceleration;

} MotionData;

class EcliptorProperties : public AnalogConverter
{

public:

	uint8_t readyToCompute :1;

	uint8_t newMeasureData :1;
	uint8_t newMotionData :1;
	uint8_t newDynamicsData :1;
	uint8_t newPositionData :1;

	MeasureData 	Measure;
	DynamicsData 	Dynamics;
	PositionData 	Position;
	MotionData 	Motion;

	uint16_t fallLowThold;
	uint16_t fallHighThold;

	uint16_t riseLowThold;
	uint16_t riseHighThold;

	volatile uint16_t curLowThold;
	volatile uint16_t curHighThold;

};

class EcliptorDriver : public EcliptorProperties
{
public:

							EcliptorDriver();
  void                    	Attach(PwmTimer* powerPwm, PwmTimer* rotatorPwm, AdcData* hallsensor);
  void                    	Attach(PwmTimer* powerPwm, PwmTimer* rotatorPwm, AdcData* hallsensor, int riseLowThold, int riseHighThold);
  void                    	Attach(PwmTimer* powerPwm, PwmTimer* rotatorPwm, AdcData* hallsensor, int riseLowThold, int riseHighThold, int fallLowThold, int fallHighThold);
  void                    	Attach(PwmTimer* powerPwm, PwmTimer* rotatorPwm, AdcData* hallsensor, int riseLowThold, int riseHighThold, int fallLowThold, int fallHighThold, int minPower, int maxPower);
  void                    	Detach();

  bool                    	Attached();

  void                    	DefineAreas(int lowHrz, int lowDgl, int lowVrt, int highHrz, int riseDgl, int highVrt, int fallDgl);
  void                    	DefineRotator(uint16_t min, uint16_t max, uint16_t mid, uint16_t frameRate, float angleRange, RotatorDirection direction);

  virtual void 		  	ConversionComplete(AdcData* stream, size_t size);
  virtual void 		  	ConversionHalfComplete(AdcData* stream, size_t size);

  bool                    	PowerLevel(int pwmValue);
  bool                    	PowerLevel(float dutyValue);

  bool                    	tholdPowerOn(uint16_t sensorvalue);

  void                    	RotateAngle(float angle);
  void                    	RotateRadians(float radians);
  void                    	RotatePercents(float percents);

  MeasureData*    			readSensorMeasure();
  DynamicsData*         	readSensorDynamics();
  PositionData*         	readSensorPosition();
  MotionData*         		readEclipticMotion();

  EccentratorProperties  Eccentrator;
  RotatorProperties	    Rotator;

  PwmTimer*            	PowerPwm = NULL;
  PwmTimer*            	RotatorPwm = NULL;

  void 				  		OperativeMeasurements(AdcData *stream, size_t size);
  void 				  		ComputativeMeasurements(AdcData *stream, size_t size);

private:

  inline EcliptorArea		computeArea(int16_t sensorValue, int16_t sensorDelta);
  inline EcliptorArea 	establishArea(int16_t sensorValue);

  void                    	innerPowerLevel(int pwmValue);
  void                    	innerPowerLevel(float dutyValue);

  void                    	computeMotion();
  void                    	computeMotionDelta(int16_t sensorDelta);

  void                    	computeRise();
  void                    	computeRiseDelta(int16_t sensorDelta);

  void                    	computeFall();
  void                    	computeFallDelta(int16_t sensorDelta);

  void                    	computeAngularVelocity();
};

#endif /* __DRIVE_ECLIPTOR_H */
