//Author: Thomas Richardson

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include "Administration.h"
#include "Token.h"
#include "Scanner.h"
#include "Parser.h"

using namespace std;

int main(int argc, char* argv[])
{
    ifstream ifs;
    ofstream ofs;

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

    admin.compile();

    cout << "\n\nDONE";
	return 0;
}
