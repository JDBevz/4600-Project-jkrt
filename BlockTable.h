#ifndef BLOCKTABLE_H
#define BLOCKTABLE_H
#include <vector>
#include "TypeKindEnum.h"
#define MAXBLOCK 10

typedef struct TableEntry{
    int index = -1;  //hash table index of the name of the object

    Kind kind = NONE; //array, constant, variable, procedure

    int size = 0;   //only for arrays

    myType type = UNIVERSAL; //Boolean, integer, Universal

    int value = 0;  //value of named constants

    int displacement = 0; //displacement from the base address of the activation record where the variable exists

    int startAddress = 0; //for procedures, the address of the first executable instruction

    int level = 0;
};

class BlockTable {
    public:
        BlockTable();

        ~BlockTable();

        //Returns true if ind(index) is found in the current block
        //False otherwise
        bool search(int ind);

        //inserts a TableEntry with necessary field values
        //if index does not exist and return true
        //return false otherwise
        bool define(int index, Kind kind, myType type, int nsize, int nvalue);

        //define with added fields for code generation
        bool define(int index, Kind kind, myType type, int nsize, int nvalue, int level, int displacement, int startLabel);

        TableEntry find(int index, bool &error);

        //creates an empty block and pushes it onto
        //the stack of blocks
        bool newBlock();

        //pops the current block from the stack
        void endBlock();

        //return the current level
        int currentLevel();

    private:

        std::vector<std::vector<TableEntry> > table;

        int blockLevel;

};
#endif

