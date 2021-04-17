/*
  EcliptarDriveSystem.h - EcliptarDriveSystem control library - ver. 0.3
  Copyright (c) 2020 Unirecon Dariusz Hanc.  All right reserved.
*/

#ifndef EcliptarDriveSystem_h
#define EcliptarDriveSystem_h

#include <inttypes.h>
#include <Servo.h>

#define MIN_HALLSENSOR_VALUE            670
#define MAX_HALLSENSOR_VALUE            1000
#define MIN_HALLSENSOR_THRESHOLD_VALUE  720
#define MAX_HALLSENSOR_THRESHOLD_VALUE  920
#define MIN_DISTANCE_MM                 40
#define MAX_DISTANCE_MM                 80
#define MIN_SERVO_SIGNAL_US             600
#define MAX_SERVO_SIGNAL_US             2400
#define MIN_ECCNTR_POWER                100
#define MAX_ECCNTR_POWER                254
#define MIN_MOTOR_POWER                 70
#define MAX_MOTOR_POWER                 200
#define SERVO_ANGLE_RANGE_DEG           180
#define SERVO_FRAMERATE_MS              20
#define BLADE_WEIGHT                    36
#define READ_INTERRUPT_INTERVAL_US      125
#define HALLSENSOR_MEASURES_COUNT       3
#define DROP_MEASURES_BELOW_US          6
#define DROP_MEASURES_ABOVE_US         -6

#define CLEAR_SFR_BIT(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit)) //macro to clear bit in special function register
#define SET_SFR_BIT(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit) ) //macro to set bit in special function register
#define CLEAR_SFR_BITS(sfr, bits) (_SFR_BYTE(sfr) &= ~(bits)) //macro to clear bits in special function register
#define SET_SFR_BITS(sfr, bits) (_SFR_BYTE(sfr) |= (bits) ) //macro to set bits in special function register
#define CLEAR_SFR_PTR_BIT(sfr, bit) (*(sfr) &= ~_BV(bit)) //macro to clear bit in special function register
#define SET_SFR_PTR_BIT(sfr, bit) (*(sfr) |= _BV(bit) ) //macro to set bit in special function register
#define CLEAR_SFR_PTR_BITS(sfr, bits) (*(sfr) &= ~(bits)) //macro to clear bits in special function register
#define SET_SFR_PTR_BITS(sfr, bits) (*(sfr) |= (bits) ) //macro to set bits in special function register

#define TRUNC_DECIMAL(f, c) (((float)((int)((f) * (c))) / (c)))


typedef struct
{
  uint8_t                    number : 6;
  uint8_t                    isActive : 1;
} EcliptarPin;

typedef enum
{
  normal = 1, reverse = -1
} RotatorDirection;

typedef enum
{
  LowHorizontal = 0, LowDiagonal = 1, LowVertical = 2, PullDiagonal = 3, HighVertical = 4, PushDiagonal = 5
} EcliptorPosition;

typedef struct
{
  int16_t lowHorizontal, lowDiagonal, lowVertical, pullDiagonal, highVertical, pushDiagonal;

} PositionValues;

typedef struct
{
  EcliptarPin                MotorPin;
  uint8_t                    powerBit;
  volatile int               powerLevel;
  volatile int               innerPowerLevel;
  volatile int               maxPowerLevel;
  volatile int               minPowerLevel;
  volatile uint16_t          revolutionSpeed;
  volatile uint8_t           running : 1;
  volatile uint8_t           started : 1;
  volatile uint8_t           powerOn : 1;
  volatile uint8_t           powerPwmOn : 1;
  uint8_t                    compareInterruptBit : 3;
  volatile uint8_t           powerLocked : 1;
} MotorProperties;

typedef struct
{
  EcliptarPin                RotatorPin;
  Servo                      Drive;
  volatile uint16_t          pulseWidth;
  volatile uint16_t          pulseWidthMin;
  volatile uint16_t          pulseWidthMax;
  volatile uint16_t          pulseWidthMid;
  volatile uint16_t          pulseDeltaMax;
  volatile uint8_t           pulsePeriod;
  volatile int               rotationPercents;
  volatile int               rotationAngle;
  int16_t                    rotationAngleRange;
  int16_t                    rotationAngleMin;
  int16_t                    rotationAngleMax;
  float                      trimByDecimalPart;
  RotatorDirection           direction;
} RotatorProperties;

typedef struct
{
  EcliptarPin               PowerPin;
  uint8_t                   powerBit;
  volatile int              powerLevel;
  volatile int              innerPowerLevel;
  int                       maxPowerLevel;
  int                       minPowerLevel;
  uint8_t                   compareInterruptBit : 3;
  volatile uint8_t          running : 1;
  volatile uint8_t          powerOn : 1;
  volatile uint8_t          powerPwmOn : 1;
  volatile uint8_t          overThreshold : 1;
  volatile uint8_t          powerLocked : 1;
} EccentratorProperties;

typedef struct
{
  volatile int16_t          result;
  volatile int16_t          aggregator;
  volatile int16_t          lastAggregator;
  volatile uint8_t          aggregatorCounter;
  volatile uint8_t          aggregating : 1;
  
} MeasureData;

typedef struct
{
  volatile int16_t          pushDeltaSum; 
  volatile int16_t          pullDeltaSum; 
  volatile uint8_t          pushAggregating : 1;
  volatile uint8_t          pullAggregating : 1;
  volatile uint8_t          pushComputing : 1;
  volatile uint8_t          pullComputing : 1;
  volatile uint8_t          pushCounter;
  volatile uint8_t          pullCounter;
  volatile float            pushPeriodMillis;
  volatile float            pullPeriodMillis;

} DynamicsData;

typedef struct
{
  EcliptorPosition          currentPosition;
  volatile uint64_t         positionMicros;
  volatile uint32_t         positionPeriodMicros;

  volatile uint8_t          secondQuarter : 1;
  volatile uint8_t          secondHalf : 1;
  volatile uint8_t          newHalf : 1;

  volatile uint64_t         halfStartMicros;
  volatile float            halfPeriodSeconds;

} PositionData;

typedef struct
{
  volatile float            angularVelocity;
  volatile float            angularAcceleration;

  volatile float            pullVelocity;
  volatile float            pullAcceleration;

  volatile float            pushVelocity;
  volatile float            pushAcceleration;

} EclipticMotion;

class EcliptorProperties
{

public:

  EcliptarPin               SensorPin;
  PositionValues            DefinedPositions;

  uint8_t                   readyToCompute : 1;

  uint8_t                   newMeasureData : 1;
  uint8_t                   newMotionData : 1;
  uint8_t                   newDynamicsData : 1;
  uint8_t                   newPositionData : 1;

  MeasureData               Measure;
  DynamicsData              Dynamics;
  PositionData              Position;
  EclipticMotion            Motion;

  uint16_t                  pullLow;
  uint16_t                  pullHigh;
  
  uint16_t                  pushLow;
  uint16_t                  pushHigh;

  volatile uint16_t         lowThreshold;
  volatile uint16_t         highThreshold;

};

class Ecliptor : public EcliptorProperties
{
public:
  Ecliptor();
  void                    attach(int powerPin, int rotatorPin, int sensorPin);
  void                    attach(int powerPin, int rotatorPin, int sensorPin, int pullLowThreshold, int pullHighThreshold);
  void                    attach(int powerPin, int rotatorPin, int sensorPin, int pullLowThreshold, int pullHighThreshold, int pushLowThreshold, int pushHighThreshold);
  void                    attach(int powerPin, int rotatorPin, int sensorPin, int pullLowThreshold, int pullHighThreshold, int pushLowThreshold, int pushHighThreshold, int minPower, int maxPower);
  void                    detach();

  bool                    attached();

  void                    definePositions(int lowHrz, int lowDgl, int lowVrt, int pullDgl, int highVrt, int pushDgl);
  void                    defineRotator(uint16_t min, uint16_t max, uint16_t mid, uint16_t frameRate, float angleRange, RotatorDirection direction);

  bool                    powerLevel(int pwmValue);

  bool                    thresholdPowerOn(uint16_t sensorvalue);

  int                     angleToMicroseconds(int angle);
  int                     radiansToMicroseconds(float radians);
  int                     percentsToMicroseconds(int percents);

  float                   angleToRadians(int angle);
  int                     radiansToAngle(float radians);

  void                    rotateAngle(int angle);
  void                    rotateRadians(float radians);
  void                    rotatePercents(int percents);

  MeasureData             readSensorMeasure();
  DynamicsData            readSensorDynamics();
  PositionData            readSensorPosition();
  EclipticMotion*         readEclipticMotion();

  EcliptorPosition        readPosition(int16_t sensorValue, int16_t sensorDelta);

  void                    deltaMotion(int16_t sensorDelta);

  void                    computeMotion();

  //bool                  computeEclipticMotion(EcliptorPosition position);

  void                    handleTimer0_COMP_Interrupt();

  void                    enableTimer0_COMP_Interrupt();
  void                    disableTimer0_COMP_Interrupt();

  void                    enableTimer0_PWM_channel();
  void                    disableTimer0_PWM_channel();

  EccentratorProperties   Eccentrator;
  RotatorProperties       Rotator;

  EcliptarPin*            PowerPinPtr = NULL;
  EcliptarPin*            RotatorPinPtr = NULL;

  uint8_t*                powerPortPtr = NULL;
  uint8_t*                powerPwmValuePtr = NULL;

  uint8_t                 powerPwmModeBit;
  
  uint8_t                 interruptEnabled : 1;

private:

  EcliptorPosition        checkPosition(int16_t sensorValue);

  void                    innerPowerLevel(int pwmValue);
  void                    pwmPowerLevel(int pwmValue);
  void                    fullPowerLevel();
  void                    offPowerLevel();

  void                    deltaPull(int16_t sensorDelta);
  void                    deltaPush(int16_t sensorDelta);

  void                    computePull();
  void                    computePush();
  void                    angularVelocity();

  int                     convertAnalogPinNumber(uint8_t sensorPin);

  void                    setupTimer0_PWM_channel();
};

class MotorDrive
{
public:
  MotorDrive();

  void                    attach(int motorPin);
  void                    attach(int motorPin, int minPower, int maxPower);

  bool                    attached();

  void                    powerLevel(int pwmValue);

  void                    revolutionSpeed(uint16_t rpm);

  void                    handleTimer2_COMP_Interrupt();

  void                    setupTimer2_PWM_channel();

  void                    enableTimer2_PWM_channel();
  void                    disableTimer2_PWM_channel();

  void                    enableTimer2_COMP_Interrupt();
  void                    disableTimer2_COMP_Interrupt();

  MotorProperties         Properties;

  EcliptarPin*            MotorPinPtr = NULL;

  volatile uint8_t*       powerPortPtr = NULL;
  volatile uint8_t*       powerPwmValuePtr = NULL;

  uint8_t                 powerPwmModeBit;

private:

  void                    innerPowerLevel(int pwmValue);
  void                    pwmPowerLevel(int pwmValue);
  void                    fullPowerLevel();
  void                    offPowerLevel();

  uint8_t                 interruptEnabled : 1;

};

class EcliptarDriveSystem
{
public:
  EcliptarDriveSystem();
  bool                    initialize();
  bool                    initialized();

  bool                    dataAvailable();

  void                    printMotionReport();
  void                    printMeasureReport();
  void                    printDynamicsReport();
  void                    printPositionReport();
  void                    printInfoReport();

  Ecliptor                LeftEcliptor;
  Ecliptor                RightEcliptor;
  MotorDrive              Motor;
  Ecliptor*               Sensors[2];
  uint8_t                 isActive : 1;

  void                    handleAnalogInterrupt();

private:

  uint8_t                 skipFlag = true;

  uint8_t                 analogSensorsCount = 2;
  uint8_t                 sensorsCounter = 0;
  int8_t                  measuresCounter = 0;

  void                    setupAnalogInterrupt();

  void                    setupTimer0_PWM_mode();
};

extern EcliptarDriveSystem EclipticalDrive;

#endif