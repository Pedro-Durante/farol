#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

class Timer{
    public:
    virtual void init() = 0;
    virtual void toggle(int estado);
    virtual int timeout() = 0;
};
#endif