#ifndef TIMER_INTERNO_H_INCLUDED
#define TIMER_INTERNO_H_INCLUDED

#include "timer.h"

class TimerInterno: public Timer {
    public:
    TimerInterno();
    void init();
    int timeout();

    private:
    unsigned long horaInicio;
};

#endif