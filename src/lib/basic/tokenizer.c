#include "tokenizer.h"
#include <string.h>

#define MAX_NUM_LEN 5
static const char *ptr, *nextPtr;

struct KeywordToken
{
	const char* keyword;
	int token;
	unsigned char len;
};

static int currentToken = TOKEN_ERROR;
#define KW(s, tok) { (s), (tok), (unsigned int)(sizeof(s) - 1) }

static const struct KeywordToken keywords[] = {
	KW("let", TOKEN_LET),
	KW("print", TOKEN_PRINT),
	KW("if", TOKEN_IF),
	KW("then", TOKEN_THEN),
	KW("else", TOKEN_ELSE),
	KW("for", TOKEN_FOR),
	KW("to", TOKEN_TO),
	KW("next", TOKEN_NEXT),
	KW("goto", TOKEN_GOTO),
	KW("gosub", TOKEN_GOSUB),
	KW("return", TOKEN_RETURN),
	KW("call", TOKEN_CALL),
	KW("rem", TOKEN_REM),
	KW("peek", TOKEN_PEEK),
	KW("poke", TOKEN_POKE),
	KW("end", TOKEN_END),
	{0, TOKEN_ERROR, 0}
};

#define KEYWORD_COUNT ((unsigned int)(sizeof(keywords)/sizeof(keywords[0]) - 1))

static int isDigit(const unsigned char c) { return c >= '0' && c <= '9'; }
static int isLower(const unsigned char c) { return c >= 'a' && c <= 'z'; }

static int singleChar()
{
	switch (*ptr)
	{
		case '\r':
		case '\n':
			return TOKEN_CR;
		case ',':
			return TOKEN_COMMA;
		case ';':
			return TOKEN_SEMICOLON;
		case '+':
			return TOKEN_PLUS;
		case '-':
			return TOKEN_MINUS;
		case '&':
			return TOKEN_AND;
		case '|':
			return TOKEN_OR;
		case '*':
			return TOKEN_STAR;
		case '/':
			return TOKEN_SLASH;
		case '%':
			return TOKEN_MOD;
		case '(':
			return TOKEN_LEFTPAREN;
		case '#':
			return TOKEN_HASH;
		case ')':
			return TOKEN_RIGHTPAREN;
		case '<':
			return TOKEN_LT;
		case '>':
			return TOKEN_GT;
		case '=':
			return TOKEN_EQ;
		default:
			return 0;
	}
}

static int getNextToken()
{
	if (*ptr == 0) return TOKEN_EOL;

	if (isDigit(*ptr))
	{
		const char* p = ptr;
		unsigned int n = MAX_NUM_LEN;

		while (n && isDigit(*p))
		{
			++p;
			--n;
		}

		if (p == ptr) return TOKEN_ERROR;
		if (n == 0 && isDigit(*p)) return TOKEN_ERROR;

		nextPtr = p;
		return TOKEN_NUMBER;
	}

	const int sc = singleChar();
	if (sc)
	{
		nextPtr = ptr + 1;
		return sc;
	}

	if (*ptr == '"')
	{
		const char* p = ptr + 1;
		while (*p && *p != '"') ++p;

		if (*p == '"') ++p;
		nextPtr = p;

		return TOKEN_STRING;
	}

	const struct KeywordToken* keywordToken = keywords;
	unsigned int remaining = KEYWORD_COUNT;
	while (remaining--)
	{
		if (strncmp(ptr, keywordToken->keyword, keywordToken->len) == 0)
		{
			const char next = ptr[keywordToken->len];
			if (!isLower(next) && !isDigit(next))
			{
				nextPtr = ptr + keywordToken->len;
				return keywordToken->token;
			}
		}
		++keywordToken;
	}

	if (isLower(*ptr))
	{
		nextPtr = ptr + 1;
		return TOKEN_VARIABLE;
	}
	return TOKEN_ERROR;
}

void tokenizerGoto(const char* program)
{
	ptr = program;
	while (*ptr == ' ' || *ptr == '\t') ptr++;

	currentToken = getNextToken();
}

void tokenizerInit(const char* program) { tokenizerGoto(program); }
int tokenizerToken() { return currentToken; }

void tokenizerNext()
{
	if (tokenizerFinished()) return;

	ptr = nextPtr;
	while (*ptr == ' ' || *ptr == '\t') ptr++;
	currentToken = getNextToken();

	while (currentToken == TOKEN_REM)
	{
		while (*nextPtr && *nextPtr != '\n' && *nextPtr != '\r') nextPtr++;
		if (*nextPtr == '\r')
		{
			nextPtr++;
			if (*nextPtr == '\n') nextPtr++;
		}
		else if (*nextPtr == '\n') nextPtr++;

		ptr = nextPtr;
		while (*ptr == ' ' || *ptr == '\t') ptr++;

		currentToken = getNextToken();
	}
}

int tokenizerNum()
{
	const char* p = ptr;
	int v = 0, digits = 0;

	while (digits < MAX_NUM_LEN && isDigit(*p))
	{
		v = v * 10 + (*p - '0');

		p++;
		digits++;
	}
	return v;
}

void tokenizerString(char* dest, const int len)
{
	if (currentToken != TOKEN_STRING || len <= 0)
	{
		if (len > 0) dest[0] = 0;
		return;
	}

	const char* p = ptr + 1;
	char* d = dest;
	unsigned int rem = (unsigned int)(len - 1);

	while (*p)
	{
		if (*p == '"' || *p == '\n' || *p == '\r' || rem == 0) break;

		*d++ = *p++;
		--rem;
	}
	*d = 0;
}

int tokenizerVariableNum()
{
	const int v = *ptr - 'a';
	return v >= 0 && v < 26 ? v : -1;
}

int tokenizerFinished() { return *ptr == 0 || currentToken == TOKEN_EOL; }
const char* tokenizerPos() { return ptr; }
