//Author: Kevin Watchuk
//Date: January 18, 2019
#include <limits>
#include <iostream>
#include "Scanner.h"
#include "Token.h"
#include "NameToken.h"
#include "SymToken.h"
#include "NumberToken.h"
#include "SymbolTable.h"
#include <cctype>
#include <sstream>
using namespace std;

//Implementation of Scanner.h

//Constructor that takes in an ifstream and a symboltable to create and run the scanner. Sets the inputfileptr to the provided ifstream and the symtableptr to the provided symboltable
Scanner::Scanner(ifstream& in, SymbolTable &sym)
{
    inputfileptr = &in;

    symtableptr = &sym;
    errorCount = 0;
}

Scanner::~Scanner() {}

//GetToken, modified to return token* due to turning tokens into an inherited class structure rather than the provided one class with a struct. Checks the first char of the ifstream for type, then determines which recognizer to call.
Token* Scanner::getToken()
{

    Token* tokenToReturn;

    if (inputfileptr->eof())
    {
        Symbol sym = SYM_EOF;
        tokenToReturn = new SymToken(sym);
        return tokenToReturn;
    }

    char current = (char)inputfileptr->peek();

    laChar = (char)inputfileptr->peek();

    if (isWhiteSpace(current))
    {
        *inputfileptr >> noskipws >> current;
        laChar = (char)inputfileptr->peek();
        if(current == '\n')
        {
            admin->NewLine();
        }
        while (isspace(current))
        {
            if (inputfileptr->eof())
            {
                Symbol sym = SYM_EOF;
                tokenToReturn = new SymToken(sym);
                return tokenToReturn;
            }

            //isWhiteSpace(laChar);

            if (isspace(laChar))
            {
                if(inputfileptr->peek() == '\n'){
                    return nullptr;
                }
                *inputfileptr >> noskipws >> current;
            }
            else
            {
                current = (char)inputfileptr->peek();
            }

            laChar = (char)inputfileptr->peek();
        }
    }

    if (current == '$')
    {
        recognizeComment();
        return nullptr;
    }

    if (isAlpha(current) || current == '_')
    {

        tokenToReturn = recognizeName();
        return tokenToReturn;
    }
    else if (isNumeric(current))
    {
        tokenToReturn = recognizeNumeral();
        return tokenToReturn;
    }
    else if (isSpecial(current))
    {
        tokenToReturn = recognizeSpecial();
        return tokenToReturn;
    }
    else
    {
        tokenToReturn = new SymToken(NONAME);
        errorCount++;
        return tokenToReturn;
    }

}

//Checks if a provided character is white space, also increases linecount if character it \n or \r
bool Scanner::isWhiteSpace(char achar)
{
    if (isspace(achar))
    {
        return true;
    }
    return false;
}

//Checks to see if provided character is alphabetical, that's all
bool Scanner::isAlpha(char achar)
{
    if (isalpha(achar))
    {
        return true;
    }
    return false;
}

//Checks to see if provided character is numeric, that's all
bool Scanner::isNumeric(char achar)
{
    if (isdigit(achar))
    {
        return true;
    }
    return false;
}

//Checks to see if provided character is "special" that's all.
bool Scanner::isSpecial(char achar)
{
    if (!isalnum(achar))
    {
        return true;

    }
    return false;
}

//recognizes a name from the input file and turns it into a token, and returns it. Also recognizes keywords
//If there is a bad name i.e. a name with a "_" on the front of it then it notes the error to the user increments errorcount and returns a BAD_ID token to the caller.
//Lexemes of length > 10 are cut down to a length of ten. the user is warned of this.
Token* Scanner::recognizeName()
{
    bool lookingForName = true;
    char current;
    string lexeme;
    Token* t;
    while (lookingForName)
    {
        *inputfileptr >> noskipws >>current;

        laChar = (char)inputfileptr->peek();
        lexeme += current;
        if ((!isalpha(laChar) && laChar != '_' && !isdigit(laChar)) )
        {
            lookingForName = false;
        }
    }

    Symbol sym = ID;
    if (lexeme.length() > 10)
    {
        lexeme.resize(10);
    }

    if (lexeme.at(0) == '_')
    {
        sym = BAD_ID;
        t = new NameToken(sym, -1, lexeme);
        admin->error("Scanner error, BAD_ID", 2);
        return t;
    }

    t = symtableptr->search(lexeme);

    if(t == nullptr)
    {
        t = new NameToken(sym, -1, lexeme);
        symtableptr->insert(dynamic_cast<NameToken*> (t));
    }


    if(t == nullptr)
    {
        admin->error("Scanner error, BAD_SCAN", 2);
        return new SymToken(BAD_SCAN);
    }
    else
    {
        return t;
    }
}

//recognizes a set of special characters resolving into a symbol turns it into a token and returns it.
//if the symbol isn't alphabetical or numerical and isn't a recognized symbol a BAD_SYM token is created, error count increased and user informed.
Token* Scanner::recognizeSpecial()
{

    char current;
    Symbol sym;
    *inputfileptr >> noskipws >> current;
    laChar = (char)inputfileptr->peek();
    if (current == '[' && laChar != ']')
    {
        sym = SYM_LEFTSQUARE;
    }
    else if (current == '[' && laChar == ']')
    {
        sym = SYM_GUARD;
        *inputfileptr >> noskipws >> current;
    }
    else if (current == ']')
    {
        sym = SYM_RIGHTSQUARE;
    }
    else if (current == '.')
    {
        sym = SYM_PERIOD;
    }
    else if (current == ',')
    {
        sym = SYM_COMMA;
    }
    else if (current == ';')
    {
        sym = SYM_SEMICOLON;
    }
    else if (current == '&')
    {
        sym = SYM_AND;
    }
    else if (current == '|')
    {
        sym = SYM_OR;
    }
    else if (current == '~')
    {
        sym = SYM_NOT;
    }
    else if (current == '<')
    {
        sym = SYM_LESSTHAN;
    }
    else if (current == '>')
    {
        sym = SYM_GREATERTHAN;
    }
    else if (current == '=')
    {
        sym = SYM_EQUAL;
    }
    else if (current == '+')
    {
        sym = SYM_PLUS;
    }
    else if (current == '-' && laChar != '>')
    {
        sym = SYM_MINUS;
    }
    else if (current == '-' && laChar == '>')
    {
        sym = SYM_RIGHTARROW;
        *inputfileptr >> noskipws >> current;
    }
    else if (current == '*')
    {
        sym = SYM_MULTIPLY;
    }
    else if (current == '/')
    {
        sym = SYM_DIVIDE;
    }
    else if (current == '\\')
    {
        sym = SYM_MODULO;
    }
    else if (current == '(')
    {
        sym = SYM_LEFTPAREN;
    }
    else if (current == ')')
    {
        sym = SYM_RIGHTPAREN;
    }
    else if (current == ':' && laChar == '=')
    {
        sym = SYM_ASSIGNMENT;
        *inputfileptr >> noskipws >> current;
    }
    else
    {
        sym = BAD_SYM;
        admin->error("Scanner error BAD_SYM", 2);
    }

    Token* t = new SymToken(sym);
    return t;
}

//recognizes a numeral from the input file, turns it into a token, returns that.
//if number is bigger or smaller than possible then a BAD_NUMERAL token is created, error count increased, and user informed.
Token* Scanner::recognizeNumeral()
{
    bool lookingForNumber = true;
    char current;
    string number;
    Symbol sym;
    while (lookingForNumber)
    {
        *inputfileptr >> noskipws >> current;
        laChar = (char)inputfileptr->peek();
        number += current;
        if (!isdigit(laChar))
        {
            lookingForNumber = false;
        }
    }

    istringstream st(number);
    int resultingNumber;
    st >> resultingNumber;
    if (!st)
    {
        if (resultingNumber == std::numeric_limits<int>::max())
        {
            sym = BAD_NUMERAL;
            admin->error("Scanner error numeral too big.", 2);
        }
        else if (resultingNumber == std::numeric_limits<int>::min())
        {
            admin->error("Scanner error numeral too small", 2);
            sym = BAD_NUMERAL;
        }
        else
        {
            admin->error("Scanner error BAD_NUM", 2);
            sym = BAD_NUMERAL;
        }
    }
    else
    {
        sym = NUMERAL;
    }
    Token* t = new NumberToken(sym, resultingNumber);
    return t;
}

//recognizes and ignores comments
void Scanner::recognizeComment()
{
    char current;
    *inputfileptr >> noskipws >> current;
    while (inputfileptr->peek() != '\n' && inputfileptr->peek() != '\r')
    {
        *inputfileptr >> noskipws >> current;
    }

}
