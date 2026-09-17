#include <Arduino.h>
#include "BluetoothSerial.h"
#include <AccelStepper.h>

String device_name = "Elec-Hand-v1";

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

int stepsPerRev = 2048;
int stepperMaxSpeed = 250;
int stepperSpeed = 100;

AccelStepper stepper1 = AccelStepper(AccelStepper::FULL4WIRE,2,4,16,17);
AccelStepper stepper2 = AccelStepper(AccelStepper::FULL4WIRE,21,19,18,5);
AccelStepper stepper3 = AccelStepper(AccelStepper::FULL4WIRE,32,33,25,26);
AccelStepper stepperSlide = AccelStepper(AccelStepper::FULL4WIRE,27,15,22,23);

int numOfSteppers = 4;
AccelStepper* steppers[4] = {&stepper1,&stepper2,&stepper3,&stepperSlide};

void setup() {
  Serial.begin(115200);
  SerialBT.begin(device_name);  //Bluetooth device name
  // SerialBT.deleteAllBondedDevices(); // Uncomment this to delete paired devices; Must be called after begin
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  for (int i = 0; i < numOfSteppers; i++) {
    steppers[i]->setMaxSpeed(stepperMaxSpeed);
  }
}

void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    String cmd = SerialBT.readStringUntil('\n');
    parseCommand(cmd);
    SerialBT.println(device_name+": " + cmd);  
  }
  for (int i = 0; i < numOfSteppers; i++) {
    steppers[i]->runSpeedToPosition();
  }
}

// accepts command of the format id:angle
// id is range of 1 to 4 for stepper motors
// angle is in degrees
void parseCommand(String cmd) {
  int colon = cmd.indexOf(':');
  if (colon != -1) {
    int id = cmd.substring(0, colon).toInt();
    int angle = cmd.substring(colon + 1).toInt();
    if (id >= 1 && id <= 4) {
      steppers[id - 1]->moveTo(angle/360.0*stepsPerRev);
      steppers[id - 1]->setSpeed(stepperSpeed);
    }
  }
}
