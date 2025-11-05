#pragma once

#define PRINT_BUF_SIZE 32
#define BASIC_BUF_SIZE 256
#define MAX_LINE_INDEX 6
#define MAX_FOR_STACK_DEPTH 3
#define MAX_VAR_NUM 26

void basicInit(const char* program);
void basicRun();
void basicList();
int basicFinished();
int basicHandleCli(const char* line);

char basicGetVariable(unsigned int varNum);
void basicSetVariable(unsigned int varNum, char value);
