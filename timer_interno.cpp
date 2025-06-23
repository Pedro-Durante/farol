#include <Arduino.h>

#include "timer.h"
#include "timer_interno.h"
#define tempo 500
TimerInterno::TimerInterno()
{}

void TimerInterno::init()
{
    horaInicio = millis();
}

int TimerInterno::timeout()
{
    if (millis() - horaInicio > tempo){
      horaInicio = millis();
        return true;
    }
    return false;

}