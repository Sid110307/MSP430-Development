#pragma once

#define CLI_MAX 32
#define BASIC_BUF_SIZE 256
#define MAX_LINE_INDEX 6
#define MAX_FOR_STACK_DEPTH 3
#define MAX_VAR_NUM 26

void basicInit(const char* program);
void basicRun();
int basicFinished();

char basicGetVariable(unsigned int varNum);
void basicSetVariable(unsigned int varNum, char value);
