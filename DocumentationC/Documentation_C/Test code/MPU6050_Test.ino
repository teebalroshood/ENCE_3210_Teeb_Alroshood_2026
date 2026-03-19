// https://github.com/ElectronicCats/mpu6050/blob/master/examples/MPU6050_raw/MPU6050_raw.ino

#include "MPU6050.h"

MPU6050 gMPU;
int16_t gAx, gAy, gAz;

void setup() {
  Serial.begin(115200);

  // Init IMU
  gMPU.initialize();

}

void loop() {

  gMPU.getAcceleration(&gAx, &gAy, &gAz);
  Serial.println(gAx);

}
