// reading a text file
#include <iostream>
#include <fstream>
#include <string>
#include "Administration.h"
#include "Token.h"
#include "Scanner.h"
#include "Symbol.h"

using namespace std;

Administration::Administration(ifstream& in, ofstream &out, Scanner &sc)
{
	outputfileptr = &out;
	inputfileptr = &in;
	scannerptr = &sc;

	lineNo = 1;
	errorCount = 0;
	string str;
};

Administration::~Administration(){
   delete inputfileptr;
   delete outputfileptr;
   delete scannerptr;
}

int Administration::scan()
{
   if(inputfileptr->peek() == EOF){lineNo = 0; return 0;}

   while(inputfileptr->peek() == '\n'){
      NewLine();
      inputfileptr->get();
   }

	Token* tok;
	int x;
	while(inputfileptr->good()){
		tok = scannerptr->getToken();

		if(tok != nullptr){

         x = tok->getSymbolName();

         if(x >= 297 && x <= 300){
            switch(x){
         case 297:
            error("BAD_NUMERAL");
            break;
         case 298:
            error("BAD_ID");
            break;
         case 299:
            error("BAD_SYM");
            break;
         default:
            break;
            }
         }

         cout << Symbol(x) << "\n";
         *outputfileptr << x << "\n";
		}
      while(inputfileptr->peek() == '\n'){
         NewLine();
         inputfileptr->get();
      }
      delete tok;
	}

	outputfileptr->close();
   delete inputfileptr;
	return 0;
};

void Administration::error(string text)
{
   string str = text;
   cout << "On line " << lineNo << " Error: " << str << "\n";
   errorCount++;
};
