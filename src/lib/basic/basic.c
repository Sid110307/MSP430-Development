#include "../../include/uart.h"
#include "basic.h"
#include "tokenizer.h"

struct ForState
{
	int lineAfterFor, forVariable, to;
};

struct LineIndexEntry
{
	int lineNumber;
	const char* programTextPos;
};

static const char* programPtr;
static char buffer[CLI_MAX];

static int gosubStack[MAX_GOSUB_STACK_DEPTH];
static unsigned int gosubStackPtr;

static struct ForState forStack[MAX_FOR_STACK_DEPTH];
static unsigned int forStackPtr;

static struct LineIndexEntry lineIndexArr[MAX_LINE_INDEX];
static unsigned int lineIndexCount;

static unsigned char variables[MAX_VAR_NUM];
static int ended;

static int expr();
static void lineStatement();
static void statement();

peekFunc peekFunction;
pokeFunc pokeFunction;

static char* itoa(const int value, char* buf)
{
	char* p = buf;
	unsigned int u;

	if (value < 0)
	{
		*p++ = '-';
		u = 0u - (unsigned int)value;
	}
	else u = (unsigned int)value;

	char* start = p;
	do
	{
		const unsigned int q = u / 10u, r = u - q * 10u;

		*p++ = (char)('0' + r);
		u = q;
	}
	while (u);

	*p-- = '\0';
	while (start < p)
	{
		const char t = *p;

		*p-- = *start;
		*start++ = t;
	}

	return buf;
}

static void printStr(const char* s) { UartA0_write(s); }

static void printInt(const int v)
{
	char buf[12];
	itoa(v, buf);
	printStr(buf);
}

static void newline() { printStr("\r\n"); }

static void basicAbort()
{
	printStr("BASIC ABORT\r\n");
	ended = 1;
}

void basicInit(const char* program)
{
	programPtr = program;
	forStackPtr = gosubStackPtr = 0;

	lineIndexCount = 0;
	tokenizerInit(program);
	ended = 0;

	peekFunction = (void*)0;
	pokeFunction = (void*)0;
}

void basicInitPeekPoke(const char* program, const peekFunc peek, const pokeFunc poke)
{
	programPtr = program;
	forStackPtr = gosubStackPtr = 0;

	lineIndexCount = 0;
	peekFunction = peek;
	pokeFunction = poke;

	tokenizerInit(program);
	ended = 0;
}

static void accept(const int token)
{
	if (token != tokenizerToken())
	{
		basicAbort();
		return;
	}
	tokenizerNext();
}

static int varFactor()
{
	const int r = (unsigned char)basicGetVariable(tokenizerVariableNum());
	accept(TOKEN_VARIABLE);

	return r;
}

static int factor()
{
	int r;
	switch (tokenizerToken())
	{
		case TOKEN_NUMBER:
			r = tokenizerNum();
			accept(TOKEN_NUMBER);

			break;
		case TOKEN_LEFTPAREN:
			accept(TOKEN_LEFTPAREN);
			r = expr();
			accept(TOKEN_RIGHTPAREN);

			break;
		default:
			r = varFactor();
			break;
	}
	return r;
}

static int term()
{
	int f1 = factor();
	int op = tokenizerToken();

	while (op == TOKEN_STAR || op == TOKEN_SLASH || op == TOKEN_MOD)
	{
		tokenizerNext();
		const int f2 = factor();

		switch (op)
		{
			case TOKEN_STAR: f1 = f1 * f2;
				break;
			case TOKEN_SLASH: f1 = f2 != 0 ? f1 / f2 : 0;
				break;
			case TOKEN_MOD: f1 = f2 != 0 ? f1 % f2 : 0;
				break;
			default:
				break;
		}
		op = tokenizerToken();
	}
	return f1;
}

static int expr()
{
	int t1 = term();
	int op = tokenizerToken();

	while (op == TOKEN_PLUS || op == TOKEN_MINUS || op == TOKEN_AND || op == TOKEN_OR)
	{
		tokenizerNext();
		const int t2 = term();

		switch (op)
		{
			case TOKEN_PLUS:
				t1 += t2;
				break;
			case TOKEN_MINUS:
				t1 -= t2;
				break;
			case TOKEN_AND:
				t1 &= t2;
				break;
			case TOKEN_OR:
				t1 |= t2;
				break;
			default:
				break;
		}
		op = tokenizerToken();
	}
	return t1;
}

static int relation()
{
	int r1 = expr();
	int op = tokenizerToken();

	while (op == TOKEN_LT || op == TOKEN_GT || op == TOKEN_EQ)
	{
		tokenizerNext();
		const int r2 = expr();

		switch (op)
		{
			case TOKEN_LT:
				r1 = r1 < r2;
				break;
			case TOKEN_GT:
				r1 = r1 > r2;
				break;
			case TOKEN_EQ:
				r1 = r1 == r2;
				break;
			default:
				break;
		}
		op = tokenizerToken();
	}
	return r1;
}

static const char* indexFind(const int lineNum)
{
	unsigned int i = lineIndexCount;
	while (i--) if (lineIndexArr[i].lineNumber == lineNum) return lineIndexArr[i].programTextPos;

	return (void*)0;
}

static void indexAdd(const int lineNum, const char* sourcePos)
{
	unsigned int n = lineIndexCount;
	while (n--) if (lineIndexArr[n].lineNumber == lineNum) return;

	if (lineIndexCount < MAX_LINE_INDEX)
	{
		lineIndexArr[lineIndexCount].lineNumber = lineNum;
		lineIndexArr[lineIndexCount].programTextPos = sourcePos;
		++lineIndexCount;
	}
}

static void jumpLineNumSlow(const int lineNum)
{
	tokenizerInit(programPtr);
	while (tokenizerNum() != lineNum)
	{
		do
		{
			do tokenizerNext();
			while (tokenizerToken() != TOKEN_CR && tokenizerToken() != TOKEN_EOL);
			if (tokenizerToken() == TOKEN_CR) tokenizerNext();
		}
		while (tokenizerToken() != TOKEN_NUMBER && !tokenizerFinished());
		if (tokenizerFinished()) return;
	}
}

static void jumpLineNum(const int lineNum)
{
	const char* pos = indexFind(lineNum);

	if (pos) tokenizerGoto(pos);
	else jumpLineNumSlow(lineNum);
}

static void gotoStatement()
{
	accept(TOKEN_GOTO);
	jumpLineNum(tokenizerNum());
}

static void printStatement()
{
	accept(TOKEN_PRINT);
	while (1)
	{
		if (tokenizerToken() == TOKEN_STRING)
		{
			tokenizerString(buffer, sizeof(buffer));
			printStr(buffer);
			tokenizerNext();
		}
		else if (tokenizerToken() == TOKEN_COMMA)
		{
			printStr(" ");
			tokenizerNext();
		}
		else if (tokenizerToken() == TOKEN_SEMICOLON) tokenizerNext();
		else if (tokenizerToken() == TOKEN_VARIABLE || tokenizerToken() == TOKEN_NUMBER) printInt(expr());
		else break;

		if (tokenizerToken() == TOKEN_CR || tokenizerToken() == TOKEN_EOL) break;
	}

	newline();
	tokenizerNext();
}

static void ifStatement()
{
	accept(TOKEN_IF);
	const int r = relation();
	accept(TOKEN_THEN);

	if (r) statement();
	else
	{
		do tokenizerNext();
		while (tokenizerToken() != TOKEN_ELSE && tokenizerToken() != TOKEN_CR && tokenizerToken() !=
			TOKEN_EOL);

		if (tokenizerToken() == TOKEN_ELSE)
		{
			tokenizerNext();
			statement();
		}
		else if (tokenizerToken() == TOKEN_CR) tokenizerNext();
	}
}

static void letStatement()
{
	const int var = tokenizerVariableNum();

	accept(TOKEN_VARIABLE);
	accept(TOKEN_EQ);
	basicSetVariable(var, (char)expr());
	accept(TOKEN_CR);
}

static void gosubStatement()
{
	accept(TOKEN_GOSUB);
	const int lineNum = tokenizerNum();
	accept(TOKEN_NUMBER);
	accept(TOKEN_CR);

	if (gosubStackPtr < MAX_GOSUB_STACK_DEPTH)
	{
		gosubStack[gosubStackPtr] = tokenizerNum();
		gosubStackPtr++;
		jumpLineNum(lineNum);
	}
}

static void returnStatement()
{
	accept(TOKEN_RETURN);
	if (gosubStackPtr > 0)
	{
		gosubStackPtr--;
		jumpLineNum(gosubStack[gosubStackPtr]);
	}
}

static void nextStatement()
{
	accept(TOKEN_NEXT);
	const int var = tokenizerVariableNum();
	accept(TOKEN_VARIABLE);

	if (forStackPtr > 0 && var == forStack[forStackPtr - 1].forVariable)
	{
		basicSetVariable(var, (char)(basicGetVariable(var) + 1));
		if (basicGetVariable(var) <= forStack[forStackPtr - 1].to) jumpLineNum(forStack[forStackPtr - 1].lineAfterFor);
		else
		{
			forStackPtr--;
			accept(TOKEN_CR);
		}
	}
	else accept(TOKEN_CR);
}

static void forStatement()
{
	accept(TOKEN_FOR);
	const int forVariable = tokenizerVariableNum();
	accept(TOKEN_VARIABLE);
	accept(TOKEN_EQ);
	basicSetVariable(forVariable, (char)expr());
	accept(TOKEN_TO);
	const int to = expr();
	accept(TOKEN_CR);

	if (forStackPtr < MAX_FOR_STACK_DEPTH)
	{
		forStack[forStackPtr].lineAfterFor = tokenizerNum();
		forStack[forStackPtr].forVariable = forVariable;
		forStack[forStackPtr].to = to;
		forStackPtr++;
	}
}

static void peekStatement()
{
	accept(TOKEN_PEEK);
	const char peekAddr = (char)expr();
	accept(TOKEN_COMMA);
	const int var = tokenizerVariableNum();
	accept(TOKEN_VARIABLE);
	accept(TOKEN_CR);

	if (peekFunction) basicSetVariable(var, peekFunction(peekAddr));
}

static void pokeStatement()
{
	accept(TOKEN_POKE);
	const char pokeAddr = (char)expr();
	accept(TOKEN_COMMA);
	const char value = (char)expr();
	accept(TOKEN_CR);

	if (pokeFunction) pokeFunction(pokeAddr, value);
}

static void endStatement()
{
	accept(TOKEN_END);
	ended = 1;
}

static void statement()
{
	const int token = tokenizerToken();
	switch (token)
	{
		case TOKEN_PRINT:
			printStatement();
			break;
		case TOKEN_IF:
			ifStatement();
			break;
		case TOKEN_GOTO:
			gotoStatement();
			break;
		case TOKEN_GOSUB:
			gosubStatement();
			break;
		case TOKEN_RETURN:
			returnStatement();
			break;
		case TOKEN_FOR:
			forStatement();
			break;
		case TOKEN_PEEK:
			peekStatement();
			break;
		case TOKEN_POKE:
			pokeStatement();
			break;
		case TOKEN_NEXT:
			nextStatement();
			break;
		case TOKEN_END:
			endStatement();
			break;
		case TOKEN_LET:
			accept(TOKEN_LET);
		case TOKEN_VARIABLE:
			letStatement();
			break;
		default: basicAbort();
			break;
	}
}

static void lineStatement()
{
	indexAdd(tokenizerNum(), tokenizerPos());
	accept(TOKEN_NUMBER);
	statement();
}

void basicRun()
{
	if (tokenizerFinished()) return;
	lineStatement();
}

int basicFinished() { return ended || tokenizerFinished(); }

void basicSetVariable(const unsigned int varNum, const char value)
{
	if (varNum < MAX_VAR_NUM) variables[varNum] = (unsigned char)value;
}

char basicGetVariable(const unsigned int varNum)
{
	if (varNum < MAX_VAR_NUM) return (char)variables[varNum];
	return 0;
}
