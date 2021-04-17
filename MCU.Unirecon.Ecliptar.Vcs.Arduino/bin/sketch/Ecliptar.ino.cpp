#include <Arduino.h>
#line 1 "e:\\_DEVELING\\_PROJECTS\\MCU\\Unirecon\\Ecliptar\\Research\\Ecliptar\\Ecliptar.ino"

 #include <EcliptarRadioController.h>
 #include <EcliptarDriveSystem.h>


#line 6 "e:\\_DEVELING\\_PROJECTS\\MCU\\Unirecon\\Ecliptar\\Research\\Ecliptar\\Ecliptar.ino"
void setup();
#line 17 "e:\\_DEVELING\\_PROJECTS\\MCU\\Unirecon\\Ecliptar\\Research\\Ecliptar\\Ecliptar.ino"
void loop();
#line 6 "e:\\_DEVELING\\_PROJECTS\\MCU\\Unirecon\\Ecliptar\\Research\\Ecliptar\\Ecliptar.ino"
void setup() {
   Serial.begin(115200);
   pinMode(3, OUTPUT);
   pinMode(7, OUTPUT);
   RadioController.attach(8, 2);
   EclipticalDrive.LeftEcliptor.attach(5, 9, A0);
   EclipticalDrive.RightEcliptor.attach(6, 10, A1);
////   EclipticalDrive.Motor.attach(11, 0, 200);
   EclipticalDrive.initialize();
}

void loop() {
  RadioController.printInfoReport();
  EclipticalDrive.printInfoReport();
}

