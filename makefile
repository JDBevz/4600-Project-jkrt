#Makefile for CPSC-4600 Term Project
#Author: Thomas Richardson
#
#
#*******************************************************************************
#Variables

CC	= g++ -std=c++11
CFLAGS	= -Wall -g

default:	plc

plc:		NameToken.o NumberToken.o Scanner.o SymToken.o Token.o Administration.o SymbolTable.o main.o
		$(CC) $(CFLAGS) main.o NameToken.o NumberToken.o Scanner.o SymToken.o Token.o Administration.o SymbolTable.o -o plc

main.o:		main.cpp Token.h Scanner.h Administration.h
		$(CC) $(CFLAGS) -c main.cpp

NameToken.o:	NameToken.h NameToken.cpp Symbol.h
		$(CC) $(CFLAGS) -c NameToken.cpp

NumberToken.o:	NumberToken.h NumberToken.cpp Symbol.h
		$(CC) $(CFLAGS) -c NumberToken.cpp

Scanner.o:	Scanner.cpp Scanner.h Token.h NameToken.h SymToken.h NumberToken.h SymbolTable.h
		$(CC) $(CFLAGS) -c Scanner.cpp

SymToken.o:	SymToken.cpp SymToken.h Symbol.h
		$(CC) $(CFLAGS) -c SymToken.cpp

Token.o:	Token.h Token.cpp
		$(CC) $(CFLAGS) -c Token.cpp

Administration.o:	Administration.h Administration.cpp Token.h Scanner.h
		$(CC) $(CFLAGS) -c Administration.cpp

SymbolTable.o:	SymbolTable.h SymbolTable.cpp
		$(CC) $(CFLAGS) -c SymbolTable.cpp

clean:
		$(RM) *.o *~ plc

