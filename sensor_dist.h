#ifndef SENSOR_DIST_H_INCLUDED
#define SENSOR_DIST_H_INCLUDED

#include "sensor.h"

class SensorDist: public Sensor{
    public:
    SensorDist();
    unsigned int leSensor(int trigPin, int echoPIN);
};
#endif