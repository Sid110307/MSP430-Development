#pragma once

#include <msp430.h>
#include <stdint.h>

#include "base.h"

namespace MSP
{
	class TimerA0 : public NonCopyable
	{
	public:
		static TimerA0& instance()
		{
			static TimerA0 timer;
			return timer;
		}

		void init(uint16_t periodTicks, void (*callback)() = 0, uint16_t mode = MC_1)
		{
			callback_ = callback;

			TA0CCR0 = periodTicks - 1;
			TA0CCTL0 = CCIE;
			TA0CTL = TASSEL_2 | ID_0 | mode;
		}

		void setCallback(void (*callback)()) { callback_ = callback; }

		void handleInterrupt() { if (callback_) callback_(); }

	private:
		TimerA0() : callback_(0)
		{
		};
		void (*callback_)();
	};
}
