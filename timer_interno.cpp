#include <Arduino.h>

#include "timer.h"
#include "timer_interno.h"
#define tempo 500
TimerInterno::TimerInterno()
{
    controle = false;
}

void TimerInterno::init()
{
    horaInicio = millis();
    controle = true;
}
void TimerInterno::toggle(int estado){
    controle = estado;
}

int TimerInterno::timeout()
{   if (controle == 0){
        return false;
    }
    else if (millis() - horaInicio > tempo){
      horaInicio = millis();
        return true;
    }
    return false;

}