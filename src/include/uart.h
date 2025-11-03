#pragma once

#ifndef TX_BUF_SIZE
#define TX_BUF_SIZE 128u
#endif
#if (TX_BUF_SIZE & (TX_BUF_SIZE - 1)) != 0
#error "TX_BUF_SIZE must be a power of two."
#endif

typedef void (*UartRxCallback)(char);

void UartA0_init();
void UartA0_setCallback(UartRxCallback cb);
unsigned char UartA0_busy();
void UartA0_write(const char* str);
void UartA0_writeSync(const char* str);
