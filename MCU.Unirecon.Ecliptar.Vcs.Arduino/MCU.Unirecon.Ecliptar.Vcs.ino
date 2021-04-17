
#include <EcliptarRadioController.h>
#include <EcliptarDriveSystem.h>
#include <EcliptarSteeringSystem.h>

EcliptarRemoteControllerPPM* rc = &RemoteController;
EcliptarDriveSystem* drv = &EclipticalDrive;
EcliptarControlSystem* ctrl = &EcliptarControl;

void setup() {
  Serial.begin(115200);


  uint16_t rcMinPulses[] = { 920, 920, 1020, 1050, 1020, 1020, 1100, 1100 };
  uint16_t rcMidPulses[] = { 1500, 1500, 1020, 1500, 1020, 1020, 1100, 1100 };
  uint16_t rcMaxPulses[] = { 2080, 2080, 1980, 1950, 1980, 1980, 1900, 1900 };
  rc->attach(8, 2, rcMinPulses, rcMidPulses, rcMaxPulses);

  drv->LeftEcliptor.definePositions(580, 620, 690, 735, 860, 735);
  drv->LeftEcliptor.defineRotator(600, 2400, 1500, 20, 160, normal);
  drv->LeftEcliptor.attach(5, 9, A0, 625, 870, 660, 760, 20, 255);

  drv->RightEcliptor.definePositions(580, 620, 690, 735, 860, 735);
  drv->RightEcliptor.defineRotator(600, 2400, 1500, 20, 160, reverse);
  drv->RightEcliptor.attach(6, 10, A1, 625, 870, 660, 760, 20, 255);

  drv->Motor.attach(11, 10, 120);

  drv->initialize();

  ctrl->assign(0, 6);
  ctrl->LeftEcliptor.assign(0, 3, 4, left, 2, 1, right);
  ctrl->RightEcliptor.assign(0, 3, 4, right, 2, 1, left);
  ctrl->Motor.assign(0, 5);
  ctrl->Lighting.assign(1, 3, 2, 7, 1);

  ctrl->initialize(7, 8);

  drv->LeftEcliptor.rotateFraction(0);
  drv->RightEcliptor.rotateFraction(0);

}

void loop() {
  ControlChannel* channels;
  if(drv->initialized())
  {
    channels = rc->readChannels(false);
    ctrl->determine(channels);
  
  // rc->printInfoReport(channels);
  // drv->printPositionReport();
  // drv->printMeasureReport();
  // drv->printDynamicsReport();
  // drv->printMotionReport();
  }
}
