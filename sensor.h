#ifndef SENSOR_H_INCLUDED
#define SENSOR_H_INCLUDED

class Sensor{
    public:
    virtual unsigned int leSensor(int trigPin, int echoPIN) = 0;
};
#endif