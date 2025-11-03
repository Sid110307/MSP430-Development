#pragma once

#include <msp430.h>
#include <stdint.h>

typedef struct
{
	void (*callback)();
} TimerA0;

static void TimerA0_init(TimerA0* self, const uint16_t periodTicks, const uint16_t divider, void (*callback)())
{
	self->callback = callback;
	TA0CCR0 = periodTicks - 1;
	TA0CCTL0 = CCIE;
	TA0CTL = TASSEL_2 | divider | MC_1;
}

static void TimerA0_setCallback(TimerA0* self, void (*callback)()) { self->callback = callback; }
static void TimerA0_handleInterrupt(const TimerA0* self) { if (self->callback) self->callback(); }
