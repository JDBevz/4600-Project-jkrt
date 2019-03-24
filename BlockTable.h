#ifndef BLOCKTABLE_H
#define BLOCKTABLE_H
#include <vector>
#include "TypeKindEnum.h"
#define MAXBLOCK 10

typedef struct TableEntry{
    int index = -1;  //hash table index of the name of the object

    Kind kind = NONE; //array, constant, variable, procedure

    int size = 0;   //only for array

    myType type = UNIVERSAL; //Boolean, integer, Universal

    int value = 0;  //value of named constants
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

