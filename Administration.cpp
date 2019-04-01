// reading a text file
#include <iostream>
#include <fstream>
#include <string>
#include "Administration.h"
#include "Token.h"
#include "Scanner.h"
#include "Symbol.h"

using namespace std;

Administration::Administration(ifstream& in, ofstream &out, Scanner &sc, Parser &pa)
{
    outputfileptr = &out;
    inputfileptr = &in;
    scannerptr = &sc;
    parserptr = &pa;

    lineNo = 1;
    errorCount = 0;
};

Administration::~Administration() {}


int Administration::compile()
{
    cout << "Starting compilation" << endl;
    if(inputfileptr->peek() == EOF)
    {
        lineNo = 0;
        return 0;
    }

    while(inputfileptr->peek() == '\n')
    {
        NewLine();
        inputfileptr->get();
    }

    int parseStatus = parserptr->parse();

    cout << "Parsing complete with " << errorCount << " errors, and " << lineNo << " lines " << endl;

    return parseStatus;

}

int Administration::scan()
{
    if(inputfileptr->peek() == EOF)
    {
        lineNo = 0;
        return 0;
    }

    while(inputfileptr->peek() == '\n')
    {
        NewLine();
        inputfileptr->get();
    }

    Token* tok;
    int x;
    while(inputfileptr->good())
    {

//        tok = scannerptr->getToken();

        if(tok != nullptr)
        {
            x = tok->getSymbolName();

			//scanner errors
            if(correctline && x >= 297 && x <= 300)
            {
                switch(x)
                {
                case BAD_NUMERAL:
                case BAD_ID:
                case BAD_SYM:
                    error( tok->getTokenString(), 2 );
                    break;
                case BAD_SCAN: //symbol table full
                    cout << "\nFATAL ERROR : Symbol table full on line " << ++lineNo << "\n\n";
                    outputfileptr->close();
                    return 1;
                    break;
                default:
                    break;
                }
                correctline = false;
            }
            *outputfileptr << *tok << "\n";
            //delete the token pointer if it is not a name
            if(tok->getSymbolName()  > 273)
            {
                delete tok;
            }
        }

        while(inputfileptr->peek() == '\n')
        {
            NewLine();
            inputfileptr->get();
        }
    }

    if(errorCount >= MAXERRORS)
    {
        cout << "Max errors reached. Stopping.\n";
        return 1;
    }

    outputfileptr->close();

    return 0;
}

void Administration::emit1(string OP_CODE)
{
    if(!errFlag)
    {
        *outputfileptr << OP_CODE << endl;
         cout << "EMIT " << OP_CODE << endl;
    }
}

void Administration::emit2(string OP_CODE, int arg1)
{
    if(!errFlag)
    {
        *outputfileptr << OP_CODE << endl << arg1 << endl;
         cout << "EMIT " << OP_CODE << endl << arg1 << endl;
    }
}

void Administration::emit3(string OP_CODE, int arg1, int arg2)
{
    if(!errFlag)
    {
        *outputfileptr << OP_CODE << endl << arg1 << endl << arg2 << endl;
         cout << "EMIT " << OP_CODE << endl << arg1 << endl << arg2 << endl;
    }
}

void Administration::error(string text, int typeFlag)
{

    errFlag = true;
    if(typeFlag == 1)
    {
        cout << "Fatal error : " << text << "\nStopping Parsing" << endl;
        outputfileptr->close();
        inputfileptr->close();
        exit(EXIT_FAILURE);
    }

    if(errorCount < MAXERRORS){


    cout << "On line " << lineNo << " Error: " << text << "\n";
    //*outputfileptr << "On line " << lineNo << " error : " << text << ".\n";
    errorCount++;
    }
}
