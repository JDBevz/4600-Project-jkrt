#ifndef SYMBOL_H
#define SYMBOL_H
//Enumerator Symbol, a helpful tool to keep all the possible terminal symbols in a neat order.
// KW = KeyWord, SYM = Symbol, BAD = whatever that is identified as not following the rules.

///KEYWORDS : 297 - 273
///Non Alpha-numeric symbols : 275 - 296
///Error symbols : 297 - 300

enum Symbol
{
	ID = 256, KW_BEGIN, KW_END, KW_CONST, KW_ARRAY, //256 - 260
	KW_INTEGER, KW_BOOLEAN, KW_PROC, KW_SKIP, KW_READ, //261 - 265
	KW_WRITE, KW_CALL, KW_IF, KW_DO, KW_FI, //266 - 270
	KW_OD, KW_FALSE, KW_TRUE, NUMERAL, SYM_PERIOD, //271 - 275
	SYM_COMMA, SYM_SEMICOLON, SYM_RIGHTSQUARE, SYM_LEFTSQUARE, SYM_AND, //276 - 280
	SYM_OR, SYM_NOT, SYM_LESSTHAN, SYM_EQUAL, SYM_GREATERTHAN, //281 - 285
	SYM_PLUS, SYM_MINUS, SYM_MULTIPLY, SYM_DIVIDE, SYM_MODULO, //286 - 290
	SYM_RIGHTPAREN, SYM_LEFTPAREN, SYM_ASSIGNMENT, SYM_GUARD, SYM_RIGHTARROW, //291 - 296
	SYM_COMMENT, BAD_NUMERAL, BAD_ID, BAD_SYM, BAD_SCAN, //296 - 300
	NONAME, SYM_EOF //301, 302
};


#endif //SYMBOL_H
