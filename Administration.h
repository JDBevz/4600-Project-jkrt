//Admin class is concerned with providing services as error reporting,
//and for the overall admin of the scanner;
//its role will change a new phases are added

#ifndef ADMINISTRATION_H
#define ADMINISTRATION_H
#include <string>
#include <iostream>
#include <fstream>
#include "Scanner.h"
#include "Parser.h"

//this is the max number of errors before the compiler bails out
#define MAXERRORS 10

using namespace std;

class Scanner;
class Parser;

class Administration
{
  public:

	//set up input and output files for scanning
//    Administration(ifstream& in, ofstream& out);

//	Administration(ifstream& in, ofstream &out, Scanner &sc);
//
	Administration(ifstream& in, ofstream &out, Scanner &sc, Parser &pa);

	//destructor
	~Administration();

	//Begin a new line of input
	void NewLine(){lineNo++; correctline = true;}

	//Error function for the phases
	void error(string text, int typeFlag);

	//call only the scanner from here
	int scan();
	//call only the parser
	int parse();
	//call all functions
	int compile();

	int getLinecount(){return lineNo;};

	private:
	//output file
	ofstream *outputfileptr;

	//input file
	ifstream *inputfileptr;

	//scanner
	Scanner *scannerptr;

	Parser *parserptr;

	//maintain the current line number
	int lineNo;

	//report error only if correct line is true; prevents multiple/redundant error/line
	bool correctline;

	//count the number of errors
	int errorCount;

};
#endif
