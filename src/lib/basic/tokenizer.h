#pragma once

enum
{
	TOKEN_ERROR,
	TOKEN_EOL,
	TOKEN_NUMBER,
	TOKEN_STRING,
	TOKEN_VARIABLE,
	TOKEN_LET,
	TOKEN_PRINT,
	TOKEN_IF,
	TOKEN_THEN,
	TOKEN_ELSE,
	TOKEN_FOR,
	TOKEN_TO,
	TOKEN_NEXT,
	TOKEN_GOTO,
	TOKEN_REM,
	TOKEN_PEEK,
	TOKEN_POKE,
	TOKEN_END,
	TOKEN_COMMA,
	TOKEN_SEMICOLON,
	TOKEN_PLUS,
	TOKEN_MINUS,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_STAR,
	TOKEN_SLASH,
	TOKEN_MOD,
	TOKEN_LEFTPAREN,
	TOKEN_RIGHTPAREN,
	TOKEN_LT,
	TOKEN_GT,
	TOKEN_EQ,
	TOKEN_CR,
};

void tokenizerGoto(const char* program);
void tokenizerInit(const char* program);
void tokenizerNext();
int tokenizerToken();
int tokenizerNum();
int tokenizerVariableNum();
void tokenizerString(char* dest, int len);
int tokenizerFinished();
char const* tokenizerPos();
