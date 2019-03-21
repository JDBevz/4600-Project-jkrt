#ifndef SCANNER_H
#define SCANNER_H

//#include "pch.h"

#include <iostream>
#include <fstream>
#include <string>
#include "Token.h"
#include "SymbolTable.h"
#include "Symbol.h"
#include "Administration.h"

class Administration;

//class to define a scanner. Scans for tokens in an ifstream, then creates and returns tokens.
class Scanner {
public:
	//Destructor!
	~Scanner();

	//Constructor that takes in an ifstream and a symboltable to create and run the scanner.
	Scanner(std::ifstream &in ,SymbolTable &symtable);

	//GetToken, modified to return token* due to turning tokens into an inherited class structure rather than the provided one class with a struct.
	Token* getToken();

	void setAdmin(Administration &ap){admin = &ap;}

private:
	//Pointer to the inputfile!
	std::ifstream *inputfileptr;

	//Pointer to that symboltable!
	SymbolTable *symtableptr;

	//pointer to admin
	Administration *admin;

	//Look ahead character
	char laChar;

	//Counting the lines for error reporting.
	int lineCount;

	//Keeping track of how many errors I have.
	int errorCount;

	//Checks if a provided character is white space, also increases linecount if character it \n or \r
	bool isWhiteSpace(char achar);

	//Checks to see if provided character is alphabetical, that's all
	bool isAlpha(char achar);

	//Checks to see if provided character is numeric, that's all
	bool isNumeric(char achar);

	//Checks to see if provided character is "special" that's all.
	bool isSpecial(char achar);

	//recognizes a name from the input file and turns it into a token, and returns it.
	Token* recognizeName();

	//recognizes a set of special characters resolving into a symbol turns it into a token and returns it.
	Token* recognizeSpecial();

	//recognizes a numeral from the input file, turns it into a token, returns that.
	Token* recognizeNumeral();

	//recognizes and ignores comments
	void recognizeComment();
};
#endif
