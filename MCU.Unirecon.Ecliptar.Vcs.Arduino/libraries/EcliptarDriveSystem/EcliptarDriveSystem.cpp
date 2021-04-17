/*
EcliptarDriveSystem.cpp - EcliptarDriveSystem control library - ver. 0.3
Copyright (c) 2020 Unirecon Dariusz Hanc.  All right reserved.
*/

#include <Arduino.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include <EcliptarRemoteControllerPPM.h>
#include "EcliptarDriveSystem.h"

Ecliptor::Ecliptor()
{
  pullLow = MIN_HALLSENSOR_THRESHOLD_VALUE;
  pullHigh = MAX_HALLSENSOR_THRESHOLD_VALUE;
  pushLow = MIN_HALLSENSOR_THRESHOLD_VALUE;
  pushHigh = MAX_HALLSENSOR_THRESHOLD_VALUE;
  Rotator.pulseWidthMin = MIN_SERVO_SIGNAL_US;
  Rotator.pulseWidthMax = MAX_SERVO_SIGNAL_US;
  Rotator.pulseWidthMid = (uint16_t)((float)(MAX_SERVO_SIGNAL_US - MIN_SERVO_SIGNAL_US) / 2) + MIN_SERVO_SIGNAL_US;
  Rotator.pulseDeltaMax = (Rotator.pulseWidthMid - MIN_SERVO_SIGNAL_US) / 2;
  Rotator.pulsePeriod = SERVO_FRAMERATE_MS;
  Rotator.rotationAngleRange = SERVO_ANGLE_RANGE_DEG;
  Rotator.rotationAngleMin = (SERVO_ANGLE_RANGE_DEG / 2) * -1;
  Rotator.rotationAngleMax = (SERVO_ANGLE_RANGE_DEG / 2);
  Rotator.trimByDecimalPart = 0;
  Rotator.direction = normal;
  Rotator.rotationPercents = -2;
  Rotator.rotationAngle = -180;
  Eccentrator.overThreshold = false;
  Eccentrator.running = false;
  Eccentrator.powerOn = false;
  Eccentrator.powerPwmOn = false;
  Eccentrator.powerLevel = 0;
  Eccentrator.innerPowerLevel = 0;
}

void                    Ecliptor::attach(int powerPin, int rotatorPin, int sensorPin)
{
  this->attach(powerPin, rotatorPin, sensorPin, MIN_HALLSENSOR_THRESHOLD_VALUE, MAX_HALLSENSOR_THRESHOLD_VALUE, MIN_HALLSENSOR_THRESHOLD_VALUE, MAX_HALLSENSOR_THRESHOLD_VALUE);
}
void                    Ecliptor::attach(int powerPin, int rotatorPin, int sensorPin, int pullLowThreshold, int pullHighThreshold)
{
  this->attach(powerPin, rotatorPin, sensorPin, pullLowThreshold, pullHighThreshold, MIN_HALLSENSOR_THRESHOLD_VALUE, MAX_HALLSENSOR_THRESHOLD_VALUE);
}
void                    Ecliptor::attach(int powerPin, int rotatorPin, int sensorPin, int pullLowThreshold, int pullHighThreshold, int pushLowThreshold, int pushHighThreshold)
{
  this->attach(powerPin, rotatorPin, sensorPin, pullLowThreshold, pullHighThreshold, pushLowThreshold, pushHighThreshold, MIN_ECCNTR_POWER, MAX_ECCNTR_POWER);
}
void                    Ecliptor::attach(int powerPin, int rotatorPin, int sensorPin, int pullLowThreshold, int pullHighThreshold, int pushLowThreshold, int pushHighThreshold, int minPower, int maxPower)
{
  SensorPin.number = convertAnalogPinNumber(sensorPin);
  SensorPin.isActive = 1;
  Rotator.RotatorPin.number = rotatorPin;
  Rotator.RotatorPin.isActive = 1;
  pinMode(rotatorPin, OUTPUT);
  RotatorPinPtr = &Rotator.RotatorPin;
  Rotator.Drive.attach(rotatorPin, Rotator.pulseWidthMin, Rotator.pulseWidthMax);
  Eccentrator.PowerPin.number = powerPin;
  Eccentrator.PowerPin.isActive = 1;
  PowerPinPtr = &Eccentrator.PowerPin;
  pullLow = pullLowThreshold;
  pullHigh = pullHighThreshold;
  pushLow = pushLowThreshold;
  pushHigh = pushHighThreshold;
  lowThreshold = pullLowThreshold;
  highThreshold = pullHighThreshold;
  Eccentrator.minPowerLevel = minPower;
  Eccentrator.maxPowerLevel = maxPower;
  Eccentrator.powerBit = 1 << (powerPin % 8);
  powerPortPtr = portOutputRegister(digitalPinToPort(powerPin));
  pinMode(powerPin, OUTPUT);
  setupTimer0_PWM_channel();

}

void                    Ecliptor::detach()
{
  SensorPin.isActive = 0;
  Eccentrator.PowerPin.isActive = 0;
  digitalWrite(Eccentrator.PowerPin.number, LOW);
  Rotator.RotatorPin.isActive = 0;
  if (Rotator.Drive.attached())
    Rotator.Drive.detach();
}
bool                    Ecliptor::attached()
{
  return SensorPin.isActive;
}

void                    Ecliptor::definePositions(int lowHrz, int lowDgl, int lowVrt, int pullDgl, int highVrt, int pushDgl)
{
  PositionValues* pos = &(DefinedPositions);
  pos->lowVertical = lowVrt;
  pos->lowDiagonal = lowDgl;
  pos->lowHorizontal = lowHrz;
  pos->highVertical = highVrt;
  pos->pullDiagonal = pullDgl;
  pos->pushDiagonal = pushDgl;
}
void                    Ecliptor::defineRotator(uint16_t minWidth, uint16_t maxWidth, uint16_t midWidth, uint16_t period, float angleRange, RotatorDirection direction)
{
  Rotator.rotationAngleMin = (angleRange / 2) * -1;
  Rotator.rotationAngleMax = (angleRange / 2);

  uint16_t center = ((maxWidth - minWidth) / 2) + minWidth;
  uint16_t subTrim = (midWidth - center);
  uint16_t stdDeviation = ((maxWidth - minWidth) / 2);

  if (subTrim != 0)
  {
    if (subTrim < 0)
      subTrim *= -1;

    Rotator.trimByDecimalPart = (float)subTrim / (float)(stdDeviation);
    float trimAngle = Rotator.rotationAngleMin * Rotator.trimByDecimalPart;
    Rotator.rotationAngleMin -= trimAngle;
    Rotator.rotationAngleMax += trimAngle;

    stdDeviation = stdDeviation - (uint16_t)((float)stdDeviation * Rotator.trimByDecimalPart);
  }
  Rotator.pulseDeltaMax = stdDeviation;

  Rotator.pulseWidthMin = minWidth;
  Rotator.pulseWidthMax = maxWidth;
  Rotator.pulseWidthMid = midWidth;

  Rotator.pulsePeriod = period;
  Rotator.rotationAngleRange = Rotator.rotationAngleMax - Rotator.rotationAngleMin;
  Rotator.direction = direction;

  if (Rotator.Drive.attached())
  {
    Rotator.Drive.detach();
    Rotator.Drive.attach(Rotator.RotatorPin.number, minWidth, maxWidth);
  }
}

inline float            Ecliptor::angleToRadians(int angle)
{
  return (angle / 360) * (2 * PI);
}
inline int              Ecliptor::radiansToAngle(float radians)
{
  return (radians / (2 * PI)) * 360;
}

inline int              Ecliptor::radiansToMicroseconds(float radians)
{

  if (radians != 0)
  {
    if (radians < 0)
      return Rotator.pulseWidthMid - ((radians / angleToRadians(Rotator.rotationAngleMin)) * (Rotator.pulseDeltaMax));
    return Rotator.pulseWidthMid + ((radians / angleToRadians(Rotator.rotationAngleMax)) * (Rotator.pulseDeltaMax));
  }
  return Rotator.pulseWidthMid;
}
inline int              Ecliptor::angleToMicroseconds(int angle)
{

  if (angle != 0)
  {
    return Rotator.pulseWidthMid + (((float)angle / (Rotator.rotationAngleRange / 2)) * Rotator.pulseDeltaMax);
  }
  return Rotator.pulseWidthMid;
}
inline int              Ecliptor::percentsToMicroseconds(int percents)
{
  if (percents != 0)
  {

    return Rotator.pulseWidthMid + (((float)percents / 100) * Rotator.pulseDeltaMax);
  }
  return Rotator.pulseWidthMid;
}

void                    Ecliptor::rotateAngle(int angle)
{
  angle *= Rotator.direction;
  if (angle < Rotator.rotationAngleMin) angle = Rotator.rotationAngleMin;
  else if (angle > Rotator.rotationAngleMax) angle = Rotator.rotationAngleMax;
  if (Rotator.rotationAngle != angle)
  {
    Rotator.rotationAngle = angle;
    Rotator.pulseWidth = angleToMicroseconds(angle);
    Rotator.Drive.writeMicroseconds(Rotator.pulseWidth);
  }
}
void                    Ecliptor::rotateRadians(float radians)
{
  rotateAngle(radiansToAngle(radians));
}
void                    Ecliptor::rotatePercents(int percents)
{
  percents *= Rotator.direction;
  if (Rotator.rotationPercents != percents)
  {
    Rotator.rotationPercents = percents;
    Rotator.pulseWidth = percentsToMicroseconds(percents);
    Rotator.Drive.writeMicroseconds(Rotator.pulseWidth);
  }
}

inline bool             Ecliptor::thresholdPowerOn(uint16_t value)
{
  if (Eccentrator.running)
  {
    if (Eccentrator.powerOn)
    {
      if (value > highThreshold ||
        value < lowThreshold)
      {
        if (!Eccentrator.overThreshold)
        {
          Eccentrator.overThreshold = true;
          innerPowerLevel(0);
        }
        return false;
      }
    }
    else
    {
      if (value <= highThreshold &&
        value >= lowThreshold)
      {
        if (Eccentrator.overThreshold)
          Eccentrator.overThreshold = false;
        innerPowerLevel(Eccentrator.powerLevel);
        return true;
      }
    }
  }
  return false;
}
bool                    Ecliptor::powerLevel(int pwmValue)
{
  if (Eccentrator.powerLevel != pwmValue)
  {
    uint8_t running = pwmValue > 4;
    if (running)
      pwmValue = map(pwmValue, 5, 255, Eccentrator.minPowerLevel, Eccentrator.maxPowerLevel);
    else
      pwmValue = 0;

    cli();
    Eccentrator.running = running;
    Eccentrator.powerLevel = pwmValue;
    sei();

    if (!running)
      innerPowerLevel(0);

    // if (!Eccentrator.overThreshold)
    // {
    //   innerPowerLevel(pwmValue);
    // }
    // else
    // {
    //   innerPowerLevel(0);
    // }

    return true;
  }
  return false;
}
inline void             Ecliptor::innerPowerLevel(int pwmValue)
{
  if (Eccentrator.innerPowerLevel != pwmValue)
  {
    Eccentrator.innerPowerLevel = pwmValue;
    Eccentrator.powerOn = pwmValue > 0;
    if (Eccentrator.powerOn)
    {
      if (pwmValue < 255)
      {
        pwmPowerLevel(pwmValue);
      }
      else
      {
        fullPowerLevel();
      }
    }
    else
    {
      offPowerLevel();
    }
  }
}
inline void             Ecliptor::pwmPowerLevel(int pwmValue)
{
  if (!Eccentrator.powerPwmOn)
    enableTimer0_PWM_channel();

  *powerPwmValuePtr = pwmValue;
}
inline void             Ecliptor::fullPowerLevel()
{

  if (Eccentrator.powerPwmOn)
    disableTimer0_PWM_channel();

  uint8_t regbackup = SREG;
  cli();
  SET_SFR_PTR_BITS(powerPortPtr, Eccentrator.powerBit);
  SREG = regbackup;

}
inline void             Ecliptor::offPowerLevel()
{
  if (Eccentrator.powerPwmOn)
    disableTimer0_PWM_channel();

  uint8_t regbackup = SREG;
  cli();
  CLEAR_SFR_PTR_BITS(powerPortPtr, Eccentrator.powerBit);
  SREG = regbackup;
}

DynamicsData            Ecliptor::readSensorDynamics()
{
  if (newDynamicsData)
  {
    DynamicsData data = Dynamics;
    newDynamicsData = false;
    return data;
  }
  return;
}
MeasureData             Ecliptor::readSensorMeasure()
{
  if (newMeasureData)
  {
    MeasureData data = Measure;
    newMeasureData = false;
    return data;
  }
  return;
}
PositionData            Ecliptor::readSensorPosition()
{
  if (newPositionData)
  {
    PositionData position = Position;
    newPositionData = false;
    return position;
  }
  return;
}
EclipticMotion* Ecliptor::readEclipticMotion()
{
  if (newMotionData)
  {
    newMotionData = false;
  }
  return &Motion;
}

int                     Ecliptor::convertAnalogPinNumber(uint8_t sensorPin)
{

#if defined(analogPinToChannel)
#if defined(__AVR_ATmega32U4__)
  if (sensorPin >= 18) sensorPin -= 18; // allow for channel or pin numbers
#endif
  return analogPinToChannel(sensorPin);
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  if (sensorPin >= 54) return sensorPin -= 54; // allow for channel or pin numbers
#elif defined(__AVR_ATmega32U4__)
  if (sensorPin >= 18) return sensorPin -= 18; // allow for channel or pin numbers
#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
  if (sensorPin >= 24) return sensorPin -= 24; // allow for channel or pin numbers
#else
  if (sensorPin >= 14) return sensorPin -= 14; // allow for channel or pin numbers
#endif
  return sensorPin;

}

inline EcliptorPosition Ecliptor::readPosition(int16_t sensorValue, int16_t sensorDelta)
{
  EcliptorPosition position = checkPosition(sensorValue);
  if (Position.currentPosition != position)
  {
    if (sensorDelta > 0)
    {
      Position.secondQuarter = false;
      lowThreshold = pullLow;
      highThreshold = pullHigh;
    }
    else
    {
      Position.secondQuarter = true;
      lowThreshold = pushLow;
      highThreshold = pushHigh;
    }

    EcliptorPosition lastposition = Position.currentPosition;
    Position.currentPosition = position;
    newPositionData = true;

    switch (position)
    {
    case PushDiagonal:
      if (!Dynamics.pushComputing)
      {
        Dynamics.pushAggregating = true;
        Measure.aggregating = true;
        Position.positionMicros = micros();
      }
      return position;
    case PullDiagonal:
      if (!Dynamics.pullComputing)
      {
        Dynamics.pullAggregating = true;
        Measure.aggregating = true;
        Position.positionMicros = micros();
      }
      return position;
    default:
      break;
    }

    switch (lastposition)
    {
    case PushDiagonal:
      if (!Dynamics.pullComputing)
      {
        Dynamics.pushAggregating = false;
        Position.positionPeriodMicros = micros() - Position.positionMicros;
      }
      break;
    case PullDiagonal:
      if (!Dynamics.pullComputing)
      {
        Dynamics.pullAggregating = false;
        Position.positionPeriodMicros = micros() - Position.positionMicros;
      }
      break;
    case LowHorizontal:
      Position.secondHalf = (Position.secondHalf) ? false : true;
      uint64_t time = micros();
      if (!Position.newHalf)
      {
        Position.halfPeriodSeconds = (float)(time - Position.halfStartMicros) / 1000000.00F;
        Position.newHalf = true;
      }
      Position.halfStartMicros = time;
      break;
    default:
      break;
    }

    return position;
  }
  return NULL;
}
inline EcliptorPosition Ecliptor::checkPosition(int16_t sensorValue)
{
  PositionValues* pos = &DefinedPositions;

  if (sensorValue > pos->lowHorizontal)
  {
    if (sensorValue > pos->lowDiagonal)
    {
      if (sensorValue > pos->lowVertical)
      {
        if (sensorValue > pos->highVertical)
        {
          return HighVertical;
        }
        else
        {
          if (!Position.secondQuarter &&
            sensorValue > pos->pullDiagonal)
          {
            return PullDiagonal;
          }
          else if (Position.secondQuarter &&
            sensorValue > pos->pushDiagonal)
          {
            return PushDiagonal;
          }
        }
        return LowVertical;
      }
      return LowDiagonal;
    }
    return LowHorizontal;
  }
  return LowHorizontal;
}

void                    Ecliptor::deltaMotion(int16_t delta)
{
  if (Position.secondQuarter)
    deltaPush(delta);
  else
    deltaPull(delta);
}
void                    Ecliptor::deltaPull(int16_t delta)
{
  Dynamics.pullDeltaSum += delta;
  Dynamics.pullCounter++;

  if (!Dynamics.pullAggregating && Dynamics.pullCounter > 0)
  {
    Dynamics.pullComputing = true;
    Measure.aggregating = false;
    Dynamics.pullPeriodMillis = (float)Position.positionPeriodMicros / 1000.00F;
    Measure.lastAggregator = 0;
  }
}
void                    Ecliptor::deltaPush(int16_t delta)
{
  Dynamics.pushDeltaSum += delta;
  Dynamics.pushCounter++;

  if (!Dynamics.pushAggregating && Dynamics.pushCounter > 0)
  {
    Dynamics.pushComputing = true;
    Measure.aggregating = false;
    Dynamics.pushPeriodMillis = (float)Position.positionPeriodMicros / 1000.00F;
    Measure.lastAggregator = 0;
  }
}

void                    Ecliptor::computeMotion()
{
  if (Position.newHalf)
  {
    angularVelocity();
  }
  if (Dynamics.pushComputing)
  {
    computePush();
  }
  if (Dynamics.pullComputing)
  {
    computePull();
  }
}
void                    Ecliptor::computePull()
{
  float deltasum = (float)Dynamics.pullDeltaSum;
  float period_ms = Dynamics.pullPeriodMillis;
  float counter = (float)Dynamics.pullCounter;
  float velocity = deltasum / period_ms;
  float accelleration = (velocity / counter) / (period_ms / counter);
  Dynamics.pullDeltaSum = 0;
  Dynamics.pullCounter = 0;
  Motion.pullVelocity = velocity;
  Motion.pullAcceleration = accelleration;
  Dynamics.pullComputing = false;
  newDynamicsData = true;
}
void                    Ecliptor::computePush()
{
  float deltasum = (float)Dynamics.pushDeltaSum;
  float period_ms = Dynamics.pushPeriodMillis;
  float counter = (float)Dynamics.pushCounter;
  float velocity = deltasum / period_ms;
  float accelleration = (velocity / counter) / (period_ms / counter);
  Dynamics.pushDeltaSum = 0;
  Dynamics.pushCounter = 0;
  Motion.pushVelocity = velocity;
  Motion.pushAcceleration = accelleration;
  Dynamics.pushComputing = false;
  newDynamicsData = true;
}
void                    Ecliptor::angularVelocity()
{
  float period_s = Position.halfPeriodSeconds;
  float angularVelocity = PI / period_s;
  Motion.angularAcceleration = (angularVelocity - Motion.angularVelocity) / period_s;
  Motion.angularVelocity = angularVelocity;
  Position.newHalf = false;
}

inline void             Ecliptor::handleTimer0_COMP_Interrupt()
{
  if (interruptEnabled)
  {
    disableTimer0_COMP_Interrupt();
    *powerPwmValuePtr = Eccentrator.innerPowerLevel;
  }
}

inline void             Ecliptor::enableTimer0_COMP_Interrupt()
{
  interruptEnabled = true;
  SET_SFR_BIT(TIMSK0, Eccentrator.compareInterruptBit);   // enable timer compare interrupt 
}
inline void             Ecliptor::disableTimer0_COMP_Interrupt()
{
  interruptEnabled = false;
  CLEAR_SFR_BIT(TIMSK0, Eccentrator.compareInterruptBit);   // disable timer compare interrupt
}

void                    Ecliptor::setupTimer0_PWM_channel()
{
  if (PowerPinPtr->number == 6)
  {
    Eccentrator.compareInterruptBit = OCIE0A;
    powerPwmValuePtr = &OCR0A;
    powerPwmModeBit = COM0A1;
  }
  else if (PowerPinPtr->number == 5)
  {
    Eccentrator.compareInterruptBit = OCIE0B;
    powerPwmValuePtr = &OCR0B;
    powerPwmModeBit = COM0B1;
  }
}

inline void             Ecliptor::enableTimer0_PWM_channel()
{
  Eccentrator.powerPwmOn = true;
  SET_SFR_BIT(TCCR0A, powerPwmModeBit);

}
inline void             Ecliptor::disableTimer0_PWM_channel()
{
  Eccentrator.powerPwmOn = false;
  CLEAR_SFR_BIT(TCCR0A, powerPwmModeBit);
}

MotorDrive::MotorDrive()
{
  Properties.powerLocked = false;
  Properties.running = false;
  Properties.powerOn = false;
  Properties.powerPwmOn = false;
  Properties.powerLevel = 0;
  Properties.innerPowerLevel = 0;
}
void                    MotorDrive::attach(int motorPin)
{
  attach(motorPin, 50, 200);
}
void                    MotorDrive::attach(int motorPin, int minPower, int maxPower)
{
  Properties.MotorPin.number = motorPin;
  Properties.minPowerLevel = minPower;
  Properties.maxPowerLevel = maxPower;
  Properties.MotorPin.isActive = true;
  MotorPinPtr = &Properties.MotorPin;
  Properties.powerBit = 1 << (motorPin % 8);
  powerPortPtr = portOutputRegister(digitalPinToPort(motorPin));
  pinMode(motorPin, OUTPUT);
  setupTimer2_PWM_channel();
}

bool                    MotorDrive::attached()
{
  return  Properties.MotorPin.isActive ? true : false;
}

void                    MotorDrive::powerLevel(int pwmValue)
{
  if (Properties.powerLevel != pwmValue)
  {
    bool running = pwmValue > 3;
    if (running)
      pwmValue = map(pwmValue, 4, 255, Properties.minPowerLevel, Properties.maxPowerLevel);
    else
      pwmValue = 0;

    Properties.running = running;
    Properties.powerLevel = pwmValue;
    innerPowerLevel(pwmValue);
  }
}
inline void             MotorDrive::innerPowerLevel(int pwmValue)
{
  if (Properties.innerPowerLevel != pwmValue)
  {
    Properties.innerPowerLevel = pwmValue;
    Properties.powerOn = pwmValue > 0;

    if (Properties.powerOn)
    {
      if (pwmValue < 255)
      {
        pwmPowerLevel(pwmValue);
      }
      else
      {
        fullPowerLevel();
      }
    }
    else
    {
      offPowerLevel();
    }
  }
}
inline void             MotorDrive::pwmPowerLevel(int pwmValue)
{
  if (!Properties.powerPwmOn)
    enableTimer2_PWM_channel();

  *powerPwmValuePtr = pwmValue;
}
inline void             MotorDrive::fullPowerLevel()
{
  if (Properties.powerPwmOn)
    disableTimer2_PWM_channel();

  uint8_t regbackup = SREG;
  cli();
  SET_SFR_PTR_BITS(powerPortPtr, Properties.powerBit);
  SREG = regbackup;
}
inline void             MotorDrive::offPowerLevel()
{
  if (Properties.powerPwmOn)
    disableTimer2_PWM_channel();

  uint8_t regbackup = SREG;
  cli();
  CLEAR_SFR_PTR_BITS(powerPortPtr, Properties.powerBit);
  SREG = regbackup;
}

void                    MotorDrive::revolutionSpeed(uint16_t rpm)
{

}

inline void             MotorDrive::handleTimer2_COMP_Interrupt()
{
  if (interruptEnabled)
  {
    disableTimer2_COMP_Interrupt();
    *powerPwmValuePtr = Properties.innerPowerLevel;
  }
}

void                    MotorDrive::setupTimer2_PWM_channel()
{
  cli(); //stop interrupts

  if (MotorPinPtr->number == 11)
  {
    Properties.compareInterruptBit = OCIE2A;
    powerPwmValuePtr = &OCR2A;
    powerPwmModeBit = COM2A1;
  }
  else if (MotorPinPtr->number == 3)
  {
    Properties.compareInterruptBit = OCIE2B;
    powerPwmValuePtr = &OCR2B;
    powerPwmModeBit = COM2B1;
  }

  CLEAR_SFR_BITS(TCCR2A, _BV(COM2A1) | _BV(COM2B1));
  SET_SFR_BITS(TCCR2B, _BV(CS20)); // set prescaler to 1 31.37 kHz / 2 ~ 15.675 kHz (dual-slope) // TCCR2B |= (1 << CS21) | (1 << CS20);  // Set for 32 prescaler // (1 << CS22) Sets 64 // (1 << CS21) Sets 8 // (1 << CS22) | (1 << CS20) Sets 128 // (1 << CS20) Sets 1
  SET_SFR_BITS(TCCR2A, _BV(WGM21) | _BV(WGM20)); // set dual-slope pwm //TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20); // set pin 11 and 3 to fast pwm single slope

  //SET_SFR_BIT(TIFR2, OCF2A); // clear any pending interrupts
  //TIMSK2 |= _BV(TOIE2);   // enable timer overflow interrupt
  sei(); //allow interrupts

  // other example options
  // TCCR2A |= (1 << WGM21);   // turn on CTC mode
  // TIMSK2 |= (1 << OCIE2B);  // enable timer compare interrupt
  // OCR2A = 199;// = (16*10^6) / (2500*32) - 1 (must be <256) set compare match register for 2.5khz increments
  // TIMSK2 &= ~(1 << OCIE2A);  // disable timer compare interrupt
}

inline void             MotorDrive::enableTimer2_PWM_channel()
{
  Properties.powerPwmOn = true;
  SET_SFR_BIT(TCCR2A, powerPwmModeBit);
}
inline void             MotorDrive::disableTimer2_PWM_channel()
{
  Properties.powerPwmOn = false;
  CLEAR_SFR_BIT(TCCR2A, powerPwmModeBit);
}

inline void             MotorDrive::enableTimer2_COMP_Interrupt()
{
  //SET_SFR_BIT(TIFR2, TOV2); // clear any pending interrupts
  if (!Properties.powerPwmOn)
    enableTimer2_PWM_channel();

  interruptEnabled = true;
  SET_SFR_BIT(TIMSK2, Properties.compareInterruptBit);
  //SET_SFR_BIT(TIMSK2, TOIE2);   // enable timer overflow interrupt  
}
inline void             MotorDrive::disableTimer2_COMP_Interrupt()
{
  interruptEnabled = false;
  CLEAR_SFR_BIT(TIMSK2, Properties.compareInterruptBit);
  //CLEAR_SFR_BIT(TIMSK2, TOIE2);   // disable timer overflow interrupt
  //SET_SFR_BIT(TIFR2, TOV2); // clear any pending interrupts  
}

EcliptarDriveSystem::EcliptarDriveSystem()
{
  Sensors[0] = &LeftEcliptor;
  Sensors[1] = &RightEcliptor;
}

bool                    EcliptarDriveSystem::initialize()
{
  if (LeftEcliptor.SensorPin.isActive &&
    RightEcliptor.SensorPin.isActive &&
    Motor.Properties.MotorPin.isActive)
  {
    LeftEcliptor.rotatePercents(0);
    RightEcliptor.rotatePercents(0);
    setupTimer0_PWM_mode();
    setupAnalogInterrupt();
    isActive = 1;
    return true;
  }
  return false;
}
bool                    EcliptarDriveSystem::initialized()
{
  return isActive;
}

bool                    EcliptarDriveSystem::dataAvailable()
{
  if (LeftEcliptor.newMotionData || RightEcliptor.newMotionData)
  {
    return true;
  }
  return false;
}

void                    EcliptarDriveSystem::printMotionReport()
{
  bool newline = false;

  if (LeftEcliptor.newMotionData)
  {
    EclipticMotion* left = LeftEcliptor.readEclipticMotion();
    newline = true;
    Serial.println("");

    Serial.print(" L_^_V: " + String(left->pullVelocity, 6));
    Serial.print(" L_^_A: " + String(left->pullAcceleration, 6));
    Serial.print(" L_v_V: " + String(left->pushVelocity, 6));
    Serial.print(" L_v_A: " + String(left->pushAcceleration, 6));
    Serial.print(" L_o_V: " + String(left->angularVelocity, 6));
    Serial.print(" L_o_A: " + String(left->angularAcceleration, 6));

  }

  if (RightEcliptor.newMotionData)
  {
    EclipticMotion* right = RightEcliptor.readEclipticMotion();

    if (!newline)
      Serial.println("");

    Serial.print(" R_^_V: " + String(right->pullVelocity, 2));
    Serial.print(" R_^_A: " + String(right->pullAcceleration, 2));
    Serial.print(" R_v_V: " + String(right->pushVelocity, 2));
    Serial.print(" R_v_A: " + String(right->pushAcceleration, 2));
    Serial.print(" R_o_V: " + String(right->angularVelocity, 2));
    Serial.print(" R_o_A: " + String(right->angularAcceleration, 2));
  }
}
void                    EcliptarDriveSystem::printMeasureReport()
{

  bool printable = false;
  MeasureData left;
  MeasureData right;
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
    Serial.println("");

    Serial.print(" L_~_M: " + String(left.result));

    Serial.print(" R_~_M: " + String(right.result));
  }

}
void                    EcliptarDriveSystem::printDynamicsReport()
{
  bool newline;

  if (LeftEcliptor.newDynamicsData)
  {
    DynamicsData left = LeftEcliptor.readSensorDynamics();
    newline = true;
    Serial.println("");

    Serial.print(" L_>_V: " + String(left.pushDeltaSum));
    Serial.print(" L_>_A: " + String(left.pushDeltaSum));
  }
  if (RightEcliptor.newDynamicsData)
  {
    DynamicsData right = RightEcliptor.readSensorDynamics();

    if (!newline)
      Serial.println("");

    Serial.print(" R_>_V: " + String(right.pushDeltaSum));
    Serial.print(" R_>_A: " + String(right.pullDeltaSum));
  }
}
void                    EcliptarDriveSystem::printPositionReport()
{
  bool newline = false;

  if (LeftEcliptor.newPositionData)
  {
    PositionData left = LeftEcliptor.readSensorPosition();
    newline = true;
    Serial.println("");

    Serial.print(" L_*_C: " + String(left.currentPosition));
    Serial.print(" L_*_T: " + String(left.positionPeriodMicros));

    Serial.print(" L_/_F: " + String(left.secondQuarter));
    Serial.print(" L_|_F: " + String(left.secondHalf));
    Serial.print(" L_|_T: " + String(left.halfPeriodSeconds, 6));

  }

  if (RightEcliptor.newPositionData)
  {
    PositionData right = RightEcliptor.readSensorPosition();

    if (!newline)
      Serial.println("");

    Serial.print(" R_*_C: " + String(right.currentPosition));
    Serial.print(" R_*_T: " + String(right.positionPeriodMicros));

    Serial.print(" R_/_F: " + String(right.secondQuarter));
    Serial.print(" R_|_F: " + String(right.secondHalf));
    Serial.print(" R_|_T: " + String(right.halfPeriodSeconds, 2));
  }
}
void                    EcliptarDriveSystem::printInfoReport()
{
  bool newline = false;

  EclipticMotion* left = LeftEcliptor.readEclipticMotion();
  newline = true;
  Serial.println("");

  Serial.print(" L_^_V: " + String(left->pullVelocity, 6));
  Serial.print(" L_^_A: " + String(left->pullAcceleration, 6));
  Serial.print(" L_v_V: " + String(left->pushVelocity, 6));
  Serial.print(" L_v_A: " + String(left->pushAcceleration, 6));
  Serial.print(" L_o_V: " + String(left->angularVelocity, 6));
  Serial.print(" L_o_A: " + String(left->angularAcceleration, 6));

  EclipticMotion* right = RightEcliptor.readEclipticMotion();

  if (!newline)
    Serial.println("");

  Serial.print(" R_^_V: " + String(right->pullVelocity, 2));
  Serial.print(" R_^_A: " + String(right->pullAcceleration, 2));
  Serial.print(" R_v_V: " + String(right->pushVelocity, 2));
  Serial.print(" R_v_A: " + String(right->pushAcceleration, 2));
  Serial.print(" R_o_V: " + String(right->angularVelocity, 2));
  Serial.print(" R_o_A: " + String(right->angularAcceleration, 2));

}

void                    EcliptarDriveSystem::setupAnalogInterrupt()
{
  cli();
  ADCSRA = 0;             // clear ADCSRA register
  ADCSRB = 0;             // clear ADCSRB register
  ADMUX = 0;              // clear ADMUX register

  SET_SFR_BITS(ADMUX, LeftEcliptor.SensorPin.number & 0x07);  // set A0 analog input pin - left ecliptor
  SET_SFR_BIT(ADMUX, REFS0);  // set reference voltage  AVCC with external capacitor at AREF pin !!
  CLEAR_SFR_BIT(ADMUX, ADLAR);  // Conversion Result = ADCL | ADCH << 8

  // sampling rate is [ADC clock] / [prescaler] / [conversion clock cycles]
  // for ADC clock 16 MHz and a conversion takes 13 clock cycles
  SET_SFR_BITS(ADCSRA, _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0));     // 128 prescaler for 8 KHz
  //SET_SFR_BITS(ADCSRA, _BV(ADPS2) | _BV(ADPS1));    // 64 prescaler for 16.2 KHz
  //SET_SFR_BITS(ADCSRA, _BV(ADPS2) | _BV(ADPS0));    // 32 prescaler for 38.5 KHz
  //SET_SFR_BIT(ADCSRA, ADPS2);                     // 16 prescaler for 76.9 KHz  *****
  //SET_SFR_BITS(ADCSRA, _BV(ADPS1) | _BV(ADPS0));    // 8 prescaler for 153.8 KHz

  SET_SFR_BIT(ADCSRA, ADATE); // enable auto trigger
  SET_SFR_BIT(ADCSRA, ADIE);  // enable interrupt when a measurement is completed  
  SET_SFR_BIT(ADCSRA, ADEN);  // enable ADC
  SET_SFR_BIT(ADCSRA, ADIF);  // Clear Interrupt;
  SET_SFR_BIT(ADCSRA, ADSC);  // start ADC measurements
  sei();
}

void                    EcliptarDriveSystem::setupTimer0_PWM_mode()
{
  cli(); //stop interrupts
  CLEAR_SFR_BITS(TCCR0A, _BV(COM0A1) | _BV(COM0B1));
  SET_SFR_BITS(TCCR0A, _BV(CS01)); // 7.8KHz // CS00 //62.5 KHz   
  SET_SFR_BITS(TCCR0A, _BV(WGM00));  // _BV(WGM01) | _BV(WGM00) Fast PWM (256) // _BV(WGM00) // Dual Slope (510)
  sei(); //allow interrupts

// TCCR0B = _BV(CS00);              1
// TCCR0B = _BV(CS01);              8
// TCCR0B = _BV(CS00) | _BV(CS01);  64
// TCCR0B = _BV(CS02);              256
// TCCR0B = _BV(CS00) | _BV(CS02);  1024
}

inline void             EcliptarDriveSystem::handleAnalogInterrupt()
{
  if (!skipFlag)
  {
    uint8_t low = ADCL;
    uint8_t high = ADCH;

    int16_t result = (high << 8 | low);

    Ecliptor* sensor = Sensors[sensorsCounter];

    byte admuxtemp = ADMUX;
    admuxtemp &= ~(sensor->SensorPin.number & 0x07);
    if (++sensorsCounter == analogSensorsCount)
      sensorsCounter = 0;
    admuxtemp |= Sensors[sensorsCounter]->SensorPin.number & 0x07;
    ADMUX = admuxtemp;

    

    MeasureData* measure = &sensor->Measure;

    int16_t delta = result - measure->result;

    if (delta > DROP_MEASURES_BELOW_US || delta < DROP_MEASURES_ABOVE_US)
    {
      measure->result = result;
      sensor->newMeasureData = true;
      sensor->readPosition(result, delta);
      sensor->thresholdPowerOn(result);

      if (measure->aggregating)
      {
        measure->aggregatorCounter++;
        measure->aggregator += result;

        if (measure->aggregatorCounter == HALLSENSOR_MEASURES_COUNT)
        {

          if (measure->lastAggregator != 0)
          {
            int16_t deltasum = measure->lastAggregator - measure->aggregator;
            measure->lastAggregator = measure->aggregator;
            sensor->deltaMotion(deltasum);
          }
          else
            measure->lastAggregator = measure->aggregator;

          measure->aggregator = 0;
          measure->aggregatorCounter = 0;
        }
      }
    }

    SET_SFR_BIT(ADCSRA, ADIF);
    skipFlag = true;
  }
  else
  {
    skipFlag = false;
  }
}

EcliptarDriveSystem EclipticalDrive;

ISR(ADC_vect)
{
  EclipticalDrive.handleAnalogInterrupt();
}

// ISR(TIMER2_OVF_vect)
// {
//   EclipticalDrive.Motor.handleTimer2_OVF_Interrupt();
// }

// ISR(TIMER2_COMPA_vect)
// {
//   EclipticalDrive.Motor.handleTimer2_COMP_Interrupt();
// }

// ISR(TIMER2_COMPB_vect)
// {
//   EclipticalDrive.Motor.handleTimer2_COMP_Interrupt();
// }

// ISR(TIMER0_COMPA_vect)
// {
//   EclipticalDrive.RightEcliptor.handleTimer0_COMP_Interrupt();
// }

// ISR(TIMER0_COMPB_vect)
// {
//   EclipticalDrive.LeftEcliptor.handleTimer0_COMP_Interrupt();
// }


//inline bool             Ecliptor::computeEclipticMotion(EcliptorPosition position)
// {
//   if (Position.currentPosition != position)
//   {
//     uint64_t time = Measure.sensorMeasureTime;
//     EcliptorPosition lastposition = Position.currentPosition;
//     Position.positionsPeriod = time - Position.currentPositionTime;
//     Position.lastPosition = lastposition;
//     Position.currentPosition = position;
//     Position.currentPositionTime = time;
//
//     switch (lastposition)
//     {
//     case PushDiagonal:
//       Dynamics.pushAggregating = false;
//       break;
//     case HighVertical:
//       if (position == PushDiagonal)
//         Dynamics.pushAggregating = true;
//       Position.highPeriodSeconds = (float)(time - Position.highStartTime) / 1000000.00F;
//       break;
//     case PullDiagonal:
//       Position.secondQuarter = true;
//       Dynamics.pullAggregating = false;
//       Position.highStartTime = time;
//       break;
//     case LowVertical:
//       if (position == PullDiagonal)
//         Dynamics.pullAggregating = true;
//       if (position == LowDiagonal)
//         Position.secondQuarter = true;
//       break;
//     case LowHorizontal:
//       Position.secondQuarter = false;
//       Position.secondHalf = (Position.secondHalf) ? false : true;
//       float period_s = (float)(time - Position.halfStartTime) / 1000000.00F;
//       Position.halfPeriodSeconds = period_s;
//       float angularVelocity = PI / period_s;
//       InnerMotion.angularAcceleration = (angularVelocity - InnerMotion.angularVelocity) / period_s;
//       InnerMotion.angularVelocity = angularVelocity;
//       Position.halfStartTime = time;
//       break;
//     default:
//       break;
//     }
//     newPositionData = true;
//     newMotionData = true;
//     return true;
//   }
//   return false;
// }

 // int16_t lastvalue = measure->sensorValue;
      //
      // if (deltaValue > DROP_AGGREGATES_BELOW_US || deltaValue < DROP_AGGREGATES_ABOVE_US)
      // {
      //}
      //
      //   measure->sensorMeasurePeriod = time - measure->sensorMeasureTime;
      //   measure->sensorMeasureTime = time;          
      //
      //   sensor->computeEclipticMotion(sensor->checkPosition(value));
      //
      //   //sensor->computeMeasureDynamics(deltaValue);
      //
      //   measure->sensorLastValue = lastvalue;
      //   measure->sensorValue = value;
      //
      //   sensor->newMeasureData = true;
      // }
      // else
      // {
      //   value = lastvalue;
      //   if (measure->sensorMeasurePeriod != 0)
      //   {
      //     if (measure->sensorLastValue == lastvalue)
      //     {
      //       measure->sensorMeasurePeriod = 0;
      //       measure->sensorMeasureTime = time;
      //     }
      //     measure->sensorLastValue = lastvalue;
      //     measure->sensorValue = value;
      //   }
      //   else
      //   {
      //     measure->sensorMeasureTime = time;
      //   }
      //
      // }
    //
    // }

// {
//   if (Dynamics.computing)
//   {
//     Dynamics.sensorDeltaSum += delta;
//     Dynamics.dynamicsCounter++;
//   }
//   else if (Dynamics.dynamicsCounter > 0)
//   {
//     float deltasum = (float)Dynamics.sensorDeltaSum;
//     float period_ms = (float)Position.positionsPeriod / 1000.00F;
//     float counter = (float)Dynamics.dynamicsCounter;
//     float velocity = deltasum / period_ms;
//     float accelleration = (velocity / counter) / (period_ms / counter);
//     //Dynamics.sensorVelocity = velocity;
//     //Dynamics.sensorAcceleration = accelleration;
//     Dynamics.sensorDeltaSum = 0;
//     Dynamics.dynamicsCounter = 0;
//     if (deltasum > 0)
//     {
//       InnerMotion.pushVelocity = velocity;
//       InnerMotion.pushAcceleration = accelleration;
//     }
//     else
//     {
//       InnerMotion.pullVelocity = velocity;
//       InnerMotion.pullAcceleration = accelleration;
//     }
//     newDynamicsData = true;
//   }
// }