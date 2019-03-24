#ifndef TOKEN_H
#define TOKEN_H
#include <iostream>
#include "Symbol.h"
#include <string>
//Definition of base class Token, doesn't do anything and can't be instantiated on its own due to pure virtual function, however still needed to implement that as a function so that each child could call it with minimum code repition.
class Token
{
public:
	//Destructor
	virtual ~Token();

	//Constructor
	Token(Symbol sn);

	//name Getter
	Symbol getSymbolName() const;

	std::string getTokenString() const;

	//name Setter
	void setSymbolName(Symbol snts);

	//Insert function for writing Token to provided ostream
	virtual void insert(std::ostream &os) const =0;

	//Overload of the << operator
	friend std::ostream & operator << (std::ostream&, Token&);
private:
	//Enumerator Symbol type, terminal symbol name
	Symbol sname;

	char SymbolTypeString[47][20] = {
	"ID",
	"KW_BEGIN",
	"KW_END",
	"KW_CONST",
	"KW_ARRAY",
	"KW_INTEGER",
	"KW_BOOLEAN",
	"KW_PROC",
	"KW_SKIP",
	"KW_READ",
	"KW_WRITE",
	"KW_CALL",
	"KW_IF",
	"KW_DO",
	"KW_FI",
	"KW_OD",
	"KW_FALSE",
	"KW_TRUE",
	"NUMERAL",
	"SYM_PERIOD",
	"SYM_COMMA",
	"SYM_SEMICOLON",
	"SYM_RIGHTSQUARE",
	"SYM_LEFTSQUARE",
	"SYM_AND",
	"SYM_OR",
	"SYM_NOT",
	"SYM_LESSTHAN",
	"SYM_EQUAL",
	"SYM_GREATERTHAN",
	"SYM_PLUS",
	"SYM_MINUS",
	"SYM_MULTIPLY",
	"SYM_DIVIDE",
	"SYM_MODULO",
	"SYM_RIGHTBRACKET",
	"SYM_LEFTBRACKET",
	"SYM_ASSIGNMENT",
	"SYM_GUARD",
	"SYM_RIGHTARROW",
	"SYM_COMMENT",
	"BAD_NUMERAL",
	"BAD_ID",
	"BAD_SYM",
	"BAD_SCAN",
	"NONAME",
	"EOF"
	};
};


#endif //TOKEN_H

