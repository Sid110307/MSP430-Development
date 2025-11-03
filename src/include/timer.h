#pragma once

typedef void (*TimerA0Callback)();

void TimerA0_init(unsigned int interval, unsigned int divider, TimerA0Callback cb);
void TimerA0_start();
void TimerA0_stop();
void TimerA0_setInterval(unsigned int interval);
void TimerA0_setCallback(TimerA0Callback cb);
