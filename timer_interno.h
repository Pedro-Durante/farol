#ifndef TIMER_INTERNO_H_INCLUDED
#define TIMER_INTERNO_H_INCLUDED

#include "timer.h"

class TimerInterno: public Timer {
    public:
    TimerInterno();
    void init();
    void toggle(int estado);
    int timeout();

    private:
    int controle;
    unsigned long horaInicio;
};

#endif