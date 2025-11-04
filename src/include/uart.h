#pragma once

typedef void (*UartRxCallback)(char);

void UartA0_init();
void UartA0_setCallback(UartRxCallback cb);
unsigned char UartA0_busy();
void UartA0_writeChar(char c);
void UartA0_write(const char* str);
void UartA0_writeSync(const char* str);
