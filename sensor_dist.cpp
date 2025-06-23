#include <Arduino.h>

#include "sensor_dist.h"

SensorDist::SensorDist()
{}

unsigned int SensorDist::leSensor(int trigPin, int echoPin)
{
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);
  return pulseIn(echoPin,HIGH,50000);
}