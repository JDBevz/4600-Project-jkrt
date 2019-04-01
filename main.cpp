#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include "Administration.h"
#include "Token.h"
#include "Scanner.h"
#include "Parser.h"
#include "Assembler.h"
#include "interp.h"

using namespace std;

int main(int argc, char* argv[])
{
    ifstream ifs, ifs2;
    ofstream ofs, ofs2, ofs3;

    if(argc != 3)
    {
        cout << "Correct usage : ./plc inputFile outputFile \nExiting\n.";
        return 0;
    }

    cout << "input from : " << argv[1] << "  output to : " << argv[2] << "\n";
    ofs.open(argv[2], ofstream::out);
    ifs.open(argv[1], ifstream::in);

    SymbolTable symtab;
    Scanner sc(ifs, symtab);
    Parser pa(sc);
    Administration admin(ifs, ofs, sc, pa);

    pa.setAdmin(admin);
    sc.setAdmin(admin);

    int compileStatus = admin.compile();

    //erase open output file on error in input program
    if(compileStatus != 0){
        ofs.close();
        ofs.open(argv[2], ofstream::out);
    }

    ofs.close();
    ifs.close();
//    ifs2.open(argv[2], ifstream::in);
//    ofs2.open("assemblerOutputPass1.txt", ofstream::out);
//    Assembler as(ifs2,ofs2);
//    as.firstPass();
//    ifs2.close();
//    ifs2.open(argv[2], ifstream::in);
//    as.secondPass();

    cout << "\n\nDONE COMPILATION\n\n";
	return 0;
}
