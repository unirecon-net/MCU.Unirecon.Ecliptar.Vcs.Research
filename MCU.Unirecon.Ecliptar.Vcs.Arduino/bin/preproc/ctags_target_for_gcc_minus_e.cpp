# 1 "e:\\_DEVELING\\_PROJECTS\\MCU\\Unirecon\\Ecliptar\\Research\\Ecliptar\\Ecliptar.ino"

# 3 "e:\\_DEVELING\\_PROJECTS\\MCU\\Unirecon\\Ecliptar\\Research\\Ecliptar\\Ecliptar.ino" 2
# 4 "e:\\_DEVELING\\_PROJECTS\\MCU\\Unirecon\\Ecliptar\\Research\\Ecliptar\\Ecliptar.ino" 2


void setup() {
   Serial.begin(115200);
   pinMode(3, 0x1);
   pinMode(7, 0x1);
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
