#ifndef BLOCKTABLE_CPP
#define BLOCKTABLE_CPP
#include <vector>
#include "BlockTable.h"

#include <iostream>

using namespace std;

BlockTable::BlockTable(){
    blockLevel = 0;
    table[blockLevel];
};

BlockTable::~BlockTable(){
    table.clear();
};

//Returns true if ind(index) is found in the current block
//False otherwise
bool BlockTable::search(int ind){
    //cout << "\nSEARCHING IN BLOCK TABLE" <<endl;
    for(int i = 0; i < table[blockLevel].size(); ++i){
        if(table[blockLevel][i].index == ind){
            return true;
        }
    }
    return false;
};
//inserts a TableEntry with necessary field values
//if index does not exist and return true
//return false otherwise
bool BlockTable::define(int index, Kind kind, myType type, int nsize, int nvalue){
    //Going to implement a check for necessary values here before entry creation.
   /// cout << "\n DEFINING IN BLOCK TABLE LEVEL: " << blockLevel << endl;

    TableEntry newEntry;
        newEntry.index = index;
        newEntry.kind = kind;
        newEntry.size = nsize;
        newEntry.type = type;
        newEntry.value = nvalue;

    table[blockLevel-1].push_back(newEntry);

    //std::cout << "\nDEFINE : INDEX : " << newEntry.index << " KIND " << newEntry.kind << " TYPE : " << newEntry.type << " SIZE " << newEntry.size << " VALUE " << newEntry.value << std::endl;

    return true;
};

bool BlockTable::define(int index, Kind kind, myType type, int nsize, int nvalue, int level, int displacement, int startAddress){
    //Going to implement a check for necessary values here before entry creation.
    ///cout << "\n DEFINING IN BLOCK TABLE LEVEL: " << blockLevel << endl;

    TableEntry newEntry;
        newEntry.index = index;
        newEntry.kind = kind;
        newEntry.size = nsize;
        newEntry.type = type;
        newEntry.value = nvalue;
        newEntry.level = level;
        newEntry.displacement = displacement;
        newEntry.startAddress = startAddress;

    table[blockLevel-1].push_back(newEntry);

    //std::cout << "\nDEFINE : INDEX : " << newEntry.index << " KIND " << newEntry.kind << " TYPE : " << newEntry.type << " SIZE " << newEntry.size << " VALUE " << newEntry.value << std::endl;

    return true;
};

TableEntry BlockTable::find(int index, bool &error){

    //cout << "\n FINDING IN BLOCK TABLE" << endl;

    //for(int i = 0; i < table.size(); ++i){
    for(int i = blockLevel-1; i >= 0; --i){
        for(int j = 0; j < table[i].size(); ++j){
            if(table[i][j].index == index){
                error = false;
                //cout << "\n FOUND : INDEX :" << table[i][j].index << " KIND : " << table[i][j].kind << " TYPE : " << table[i][j].type << " SIZE : " << table[i][j].size << " VALUE : " << table[i][j].value << " CURRENT LEVEL : " << currentLevel() << " FIND LEVEL: " << i << endl;
                return table[i][j];
            }
        }
    }

    error = true;
    TableEntry te;
     //cout << "Not found\n";

    return te;
};


//creates an empty block and pushes it onto
//the stack of blocks
bool BlockTable::newBlock(){

    blockLevel = ++blockLevel;

    //cout << "NEW BLOCK, LEVEL : " << blockLevel << endl;

    if(blockLevel > MAXBLOCK){
        //cout << "REACHED MAX BLOCKS" << endl;
        --blockLevel;
        return false;
    }

    table.resize(blockLevel);
    return true;
};
//pops the current block from the stack
void BlockTable::endBlock(){

    table[blockLevel].clear();
    blockLevel = --blockLevel;
    table.resize(blockLevel);

};
//return the current level
int BlockTable::currentLevel(){
    return blockLevel;
};
#endif
