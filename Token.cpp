//#include "pch.h"
#include "Token.h"


using namespace std;

//Implementation of token, override of pure virtual function

//Destructor
Token::~Token()
{

}

//Constructor
Token::Token(Symbol sn) :sname(sn)
{
}

//Insert function for writing Token to provided ostream
void Token::insert(ostream &os) const
{
	os << SymbolTypeString[sname - 256];
}

//name Getter
Symbol Token::getSymbolName() const
{
	return sname;
}

string Token::getTokenString() const
{
	return SymbolTypeString[sname - 256];
}

//name Setter
void Token::setSymbolName(Symbol snts)
{
	sname = snts;
}

//Overload of the << operator
ostream& operator<<(ostream& out, Token& t)
{
	t.insert(out);
	return out;
}
