#pragma once

#include <stdint.h>

#include "uart.h"

namespace MSP
{
	class UartCli : public NonCopyable
	{
	public:
		static UartCli& instance()
		{
			static UartCli instance;
			return instance;
		}

		void init() { UartA0::instance().setCallback(&UartCli::rxCallbackStatic); }

		void setLineHandler(void (*handler)(const char*)) { lineHandler_ = handler; }

		void write(const char* str) { UartA0::instance().write(str); }

	private:
		UartCli() : lineHandler_(0), bufferIndex_(0) { buffer_[0] = '\0'; }

		static void rxCallbackStatic(char c) { UartCli::instance().rxCallback(c); }

		void rxCallback(char c)
		{
			if (c == '\r' || c == '\n')
			{
				if (bufferIndex_ > 0)
				{
					buffer_[bufferIndex_] = '\0';
					if (lineHandler_) lineHandler_(buffer_);
					bufferIndex_ = 0;
				}
				UartA0::instance().write("\r\n");
			}
			else if (bufferIndex_ < (sizeof(buffer_) - 1))
			{
				buffer_[bufferIndex_++] = c;
				UartA0::instance().writeChar(c);
			}
		}

		void (*lineHandler_)(const char*);
		char buffer_[256];
		uint16_t bufferIndex_;
	};
}
