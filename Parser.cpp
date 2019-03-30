/****

Parser with improved function and error detection and recovery.

Author : Jesse Bevans, Kevin Watchuk

****/


#include <algorithm>
#include <vector>
#include "Parser.h"
#include <iostream>
#include "NumberToken.h"


using namespace std;


Parser::Parser(Scanner& sc)
{
//ctor
    scptr = &sc;
    laToken = nullptr;
    panic = false;
    panicCount = 0;
    errorCount = 0;
    bt=BlockTable();
}

Parser::~Parser()
{
//dtor
}

int Parser::parse()
{
    vector <Symbol> SynchSet;

    do
    {
        laToken = scptr->getToken();
    }
    while(laToken == nullptr);

    laSymbol = laToken->getSymbolName();

    //cout << "First token : " << laSymbolName() << endl;
    if(laSymbol == KW_BEGIN)
    {
        addSymbol(SynchSet, SYM_EOF); //end of any possible input
        Program(SynchSet);
    }

    if(laSymbol != SYM_EOF)
    {
		Error(__func__, "Expected EOF not found, ignoring remaining input until EOF", SynchSet);
    }

	if(laSymbol == SYM_EOF)
	{
		cout << "EOF reached.\n";
        return errorCount;
	}

    return -1;
}

//first set :  begin
//follow set :  E
//synch set : .
void Parser::Program(vector <Symbol> SynchSet)
{
    cout << "Program\n";
    int varLabel, startLabel;
    //
    if(laSymbol != KW_BEGIN)
    {
        Error(__func__, "Missing 'begin' at start of program", SynchSet);
    }

    if(!(bt.newBlock()))
    {
        admin->error("fatal error", 1);
    }
    varLabel = NewLabel();
    startLabel = NewLabel();
    admin->emit3("PROG",varLabel,startLabel);
    Block(SynchSet,startLabel,varLabel);

    bt.endBlock();
    if(laSymbol == SYM_PERIOD)
    {
        match(laSymbol, __func__);
        admin->emit1("ENDPROG");
    }
    else
    {
        Error(__func__, "Missing '.' at end of program", SynchSet);
    }

}

//first set :  begin
//follow set :  ; .
void Parser::Block(vector <Symbol> SynchSet,int sLabel,int vLabel)
{
    cout << "Block\n";

    //add follow set to synch set
    addSymbol(SynchSet, SYM_PERIOD);
    addSymbol(SynchSet, SYM_SEMICOLON);

    if(laSymbol == KW_BEGIN)
    {
        match(laSymbol, __func__);
    }
    else
    {
        Error(__func__, "Missing 'begin' at start of block", SynchSet);
    }
    int varLength = 0;
    int nextVarStart = 3;
    varLength =DefinitionPart(SynchSet,nextVarStart);
    admin->emit3("DEFARG", vLabel, varLength);
    admin->emit2("DEFADDR",sLabel);
    StatementPart(SynchSet);

//match end
    if(laSymbol == KW_END)
    {
        match(laSymbol, __func__);
    }
    else
    {
        Error(__func__, "Missing 'end' at end of block", SynchSet);
    }

}

//first set :  const proc integer Boolean
//follow set :  skip read write call if do name end
int Parser::DefinitionPart(vector <Symbol> SynchSet, int& nextVarStart)
{
    cout << "DefinitionPart\n";
    int varLength = 0;
    //add follow set to synch set
    addSymbol(SynchSet, KW_SKIP);
    addSymbol(SynchSet, KW_READ);
    addSymbol(SynchSet, KW_WRITE);
    addSymbol(SynchSet, KW_CALL);
    addSymbol(SynchSet, KW_IF);
    addSymbol(SynchSet, KW_DO);
    addSymbol(SynchSet, ID);
    addSymbol(SynchSet, KW_END);
    //
    addSymbol(SynchSet, KW_CONST);
    addSymbol(SynchSet, KW_PROC);
    addSymbol(SynchSet, KW_INTEGER);
    addSymbol(SynchSet, KW_BOOLEAN);

    switch(laSymbol)
    {
    //proper definition part beginning / first set
    case KW_CONST:
    case KW_PROC:
    case KW_INTEGER:
    case KW_BOOLEAN:
        varLength += Definition(SynchSet, nextVarStart);
        //all definitions must be followed by a semicolon
        if(laSymbol == SYM_SEMICOLON)
        {
            match(laSymbol, __func__);
            varLength += DefinitionPart(SynchSet,nextVarStart);
        }
        else
        {
            Error(__func__, "Missing ';' at end of Definition", SynchSet);
            varLength += DefinitionPart(SynchSet,nextVarStart);
        }
        break;
    //empty definition part / follow set
    case KW_SKIP:
    case KW_READ:
    case KW_WRITE:
    case KW_CALL:
    case KW_IF:
    case KW_DO:
    case ID:
    case KW_END:
	case SYM_EOF:
        return 0;
	case SYM_SEMICOLON:
		Error(__func__, "Blank statement", SynchSet);
	//any unexpected or incorrect symbols
    default:
        if(panic)
        {
            return 0;
        }
        Error(__func__, "Unexpected symbol", SynchSet);
        varLength =DefinitionPart(SynchSet,nextVarStart);
    }
    return varLength;

}

//first set :  skip read write call if do letter
//follow set :  [] fi od end
void Parser::StatementPart(vector <Symbol> SynchSet)
{
    cout << "StatementPart\n";

    //add follow set to synch set
    addSymbol(SynchSet, SYM_GUARD);
    addSymbol(SynchSet, KW_FI);
    addSymbol(SynchSet, KW_OD);
    addSymbol(SynchSet, KW_END);
    //first set
    addSymbol(SynchSet, KW_SKIP);
    addSymbol(SynchSet, KW_READ);
    addSymbol(SynchSet, KW_WRITE);
    addSymbol(SynchSet, KW_CALL);
    addSymbol(SynchSet, KW_IF);
    addSymbol(SynchSet, KW_DO);
    addSymbol(SynchSet, ID);

    switch(laSymbol)
    {
	//correct statement part begin
    case KW_SKIP:
    case KW_READ:
    case KW_WRITE:
    case KW_CALL:
    case KW_IF:
    case KW_DO:
    case ID:
        Statement(SynchSet);
        if(laSymbol == SYM_SEMICOLON)
        {
            match(laSymbol, __func__);
            StatementPart(SynchSet);
        }
        else
        {
            Error(__func__, "Missing ';' at end of Statement", SynchSet);
        }
        break;
	//follow set
    case SYM_GUARD:
    case KW_FI:
    case KW_OD:
    case KW_END:
	case SYM_EOF:
        return;
	case SYM_SEMICOLON:
		Error(__func__, "Blank statement", SynchSet);
		match(laSymbol,__func__);
		break;
	//unexpected symbols
    default:
        if(panic)
        {
            return;
        }
        Error(__func__, "Unexpected symbol", SynchSet);
    }

}

//first set :  const proc integer Boolean
//follow set :  ;
int Parser::Definition(vector <Symbol> SynchSet,int& nextVarStart)
{
    cout << "Definition\n";
    varLength = 0;
    addSymbol(SynchSet, SYM_SEMICOLON);

    switch(laSymbol)
    {
    case KW_CONST:
        ConstantDefinition(SynchSet);
        break;
    case KW_INTEGER:
    case KW_BOOLEAN:
        varLength =VariableDefinition(SynchSet,nextVarStart);
        break;
    case KW_PROC:
        ProcedureDefinition(SynchSet);
        break;
    default:
        Error(__func__, "Unexpected symbol", SynchSet);
        return 0;
        break;
    }
    return varLength;
}

//first set :  const
//follow set :  ;
void Parser::ConstantDefinition(vector <Symbol> SynchSet)
{
    cout << "ConstantDefinition\n";

    bool isGood = true;

    addSymbol(SynchSet, SYM_SEMICOLON);
    addSymbol(SynchSet, ID);
    addSymbol(SynchSet, SYM_EQUAL);
    addSymbol(SynchSet, NUMERAL);
    addSymbol(SynchSet, KW_TRUE);
    addSymbol(SynchSet, KW_FALSE);
    int position = 0;
    myType temptype;
    int tempvalue = 0;
//const
    if(laSymbol == KW_CONST)
    {
        match(laSymbol, __func__);
    }
    else
    {	//this would be a very difficult error to encounter
    	if(isGood)
			{Error(__func__, "Missing 'const'", SynchSet); isGood = false;}
    }
//const name
    if(laSymbol == ID)
    {
        NameToken *nt = (NameToken*)laToken;
        position = nt->getPosition();
        nt = nullptr;
        ///
        cout << position << endl;

        ConstantName(SynchSet);
    }
    else
    {
    	if(isGood)
			{Error(__func__, "Missing constant name", SynchSet); isGood = false;}
    }

//equals
    if(laSymbol == SYM_EQUAL)
    {
        match(laSymbol, __func__);
    }
    else
    {
    	if(isGood)
			{Error(__func__, "Missing '='", SynchSet); isGood = false;}
    }

NumberToken *numt;
NameToken *nt;
TableEntry te;

//constant
    switch(laSymbol)
    {
    case NUMERAL:
        temptype = INT;
        numt= (NumberToken*) laToken;
        tempvalue = numt->getValue();
        numt = nullptr;
        Constant(SynchSet);
        if(!bt.define(position,CONSTANT,temptype,1,tempvalue,bt.currentLevel(),0,0))
        {
            admin->error("This is an error. Ambiguous definition of constant", 3);
        }
        break;
    case KW_FALSE:
        temptype = BOOL;
        tempvalue = 0;
        Constant(SynchSet);
        if(!bt.define(position,CONSTANT,temptype,1,tempvalue,bt.currentLevel(),0,0))
        {
            admin->error("This is an error. Ambiguous definition of constant", 3);
        }
        break;
    case KW_TRUE:
        temptype = BOOL;
        tempvalue=1;
        Constant(SynchSet);
        if(!bt.define(position,CONSTANT,temptype,1,tempvalue,bt.currentLevel(),0,0))
        {
            admin->error("This is an error. Ambiguous definition of constant", 3);
        }
        break;
    case ID:

        nt = (NameToken*) laToken;
        bool error;
        te = bt.find(nt->getPosition(),error );
        if(!error){
            temptype = te.type;
            tempvalue = te.value;
            Constant(SynchSet);
            if(!bt.define(position,CONSTANT,temptype,1,tempvalue,bt.currentLevel(),0,0))
            {
                admin->error("Unable to define constant", 3);
            }
        }
        else{
            admin->error("Unable to define constant",3);
            Constant(SynchSet);
        }
        break;
    default:
    	if(isGood)
			{Error(__func__, "Missing constant", SynchSet); isGood = false;}
    }

}

//first set :  integer boolean
//follow set :  ;
int Parser::VariableDefinition(vector <Symbol> SynchSet, int& nextVarStart)
{
    cout << "VariableDefinition\n";
    varLength = 0;
    addSymbol(SynchSet, SYM_SEMICOLON);
    myType temptype;
//type symbol
    switch(laSymbol)
    {
    case KW_INTEGER:
    case KW_BOOLEAN:
        temptype =TypeSymbol(SynchSet);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
    }

//variable definition A
    if(laSymbol == ID | laSymbol == KW_ARRAY)
    {
       varLength = VariableDefinitionA(SynchSet,temptype, nextVarStart);
    }
    else
    {
        Error(__func__, "Incorrect variable definition", SynchSet);
        return;
    }
    return varLength;
}

//first set :  array letter
//follow set :  ;
int Parser::VariableDefinitionA(vector <Symbol> SynchSet,myType TempType,int& nextVarStart)
{
    cout << "VariableDefinitionA\n";
    int varLength = 0;
    addSymbol(SynchSet, SYM_SEMICOLON);
    int numberInArray =0;
    vector<int> varlist;
    if(laSymbol == ID)
    {
        varlist = VariableList(SynchSet);
        varLength = varlist.size();
        for(int i = 0; i<varlist.size();i++)
        {
            if(!bt.define(varlist[i],VAR,TempType,1,0,bt.currentLevel(),nextVarStart,0))
            {
                admin->error("Ambiguous definition of variable", 3);
            }
            nextVarStart += 1;
        }
        return varLength;
    }
    else if(laSymbol == KW_ARRAY)
    {
        bool isProperArray = false;

        match(laSymbol, __func__);
        if(laSymbol == ID)
        {
           varlist= VariableList(SynchSet);
        }
        else
        {
            Error(__func__, "", SynchSet);
            return;
        }

        if(laSymbol == SYM_LEFTSQUARE)
        {
            match(laSymbol, __func__);
        }
        else
        {
            Error(__func__, "", SynchSet);
            return;
        }
        NumberToken *nt;
        NameToken *namet;
        TableEntry te;
        myType typeCheck;
        bool error;
        switch(laSymbol)
        {
        case NUMERAL:
            nt = (NumberToken*) laToken;
            numberInArray =nt->getValue();
            varLength = numberInArray;
            Constant(SynchSet);
            isProperArray = true;
            break;
        case KW_TRUE:
            isProperArray= false;
            admin->error("An array should have an int as the amount of variables.",3);;
            Constant(SynchSet);
            break;
        case KW_FALSE:
            isProperArray= false;
            admin->error("An array should have an int as the amount of variables.",3);
            numberInArray = 0;
            Constant(SynchSet);
            break;
        case ID:
            namet = (NameToken*) laToken;
            te = bt.find(namet->getPosition(),error);

            if(!error){
                typeCheck = te.type;
                if(typeCheck == INT)
                {
                    numberInArray = te.value;
                    varLength = numberInArray;
                    isProperArray = true;
                }
                else
                {
                    isProperArray= false;
                    admin->error("An array should have an int as the amount of variables.",3);
                }
            }
            else{
                admin->error("Const or variable in array size not defined",3);
            }
            Constant(SynchSet);
            break;
        default:
            Error(__func__, "", SynchSet);
            return varLength;
            break;
        }

        if(laSymbol == SYM_RIGHTSQUARE)
        {
            match(laSymbol, __func__);

            if(isProperArray)
            {
                for(int i = 0; i<varlist.size();i++)
                {
                    if(!bt.define(varlist[i],ARR,TempType,numberInArray,0,bt.currentLevel(),nextVarStart,0))
                    {
                        admin->error("Ambiguous definition of array member",3);
                    }
                    nextVarStart += numberInArray;
                }
                return varLength;
            }


        }
        else
        {
            Error(__func__, "", SynchSet);
            return 0;
        }
        return 0;
    }
    else
    {
        Error(__func__, "", SynchSet);
        return varLength;
    }


}

myType Parser::TypeSymbol(vector <Symbol> SynchSet)
{
    switch(laSymbol)
    {
    case KW_INTEGER:
        match(laSymbol, __func__);
        return INT;
        break;
    case KW_BOOLEAN:
        match(laSymbol, __func__);
        return BOOL;
        break;
    default:
        Error(__func__, "", SynchSet);
        return UNIVERSAL;
    }
}

//first set :  letter
//follow set :   := [ ;
vector<int> Parser::VariableList(vector <Symbol> SynchSet)
{
    cout << "VariableList\n";

    vector<int> positionVec;
    vector<int> Vec;
    addSymbol(SynchSet, SYM_SEMICOLON);
    addSymbol(SynchSet, SYM_ASSIGNMENT);
    addSymbol(SynchSet, SYM_LEFTSQUARE);
    NameToken *nt;
    int position;
    vector<int> newVec;
    switch(laSymbol)
    {
    case ID:
        nt = (NameToken*) laToken;
        position =nt->getPosition();
        positionVec.push_back(position);
        VariableName(SynchSet);
        newVec=VariableListA(SynchSet);
        positionVec.insert(positionVec.end(),newVec.begin(),newVec.end());
        return positionVec;
        break;
    default:
        Error(__func__, "Error in VariableList", SynchSet);
        return Vec;
    }
}

//first set :  ,
//follow set :  := ;
vector<int> Parser::VariableListA(vector <Symbol> SynchSet)
{
    vector<int> intVec;


    switch(laSymbol)
    {
    case SYM_COMMA:
        match(laSymbol, __func__);
        return VariableList(SynchSet);
        break;
    case SYM_ASSIGNMENT:
    case SYM_LEFTSQUARE:
    case SYM_SEMICOLON:
        return intVec;
    default:
        Error(__func__, "", SynchSet);
        return intVec;
    }

}

//first set :  proc
//follow set :  ;
void Parser::ProcedureDefinition(vector <Symbol> SynchSet)
{
    if(laSymbol == KW_PROC)
    {
        match(laSymbol, __func__);
        int position = ProcedureName(SynchSet);
        int procLabel = NewLabel();
        if(position != -1)
        {


        if(!bt.define(position,PROC,UNIVERSAL,0,0,bt.currentLevel(),0, procLabel))
        {
            admin->error("Ambiguous definition of procedure",3);
        }
        }
        else
        {
            admin->error("That Identifier token doesn't exist",3);
        }
        if(!(bt.newBlock()))
        {
            admin->error("Fatal error",1);
        }
        int varLabel = NewLabel();
        int startLabel = NewLabel();
        admin->emit2("DEFADDR",procLabel);
        admin->emit3("PROC",varLabel,startLabel);
        Block(SynchSet, startLabel,varLabel);
        bt.endBlock();
        admin->emit1("ENDPROC");
    }
    else
    {
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : skip read write call if do letter
//follow set : ;
void Parser::Statement(vector <Symbol> SynchSet)
{
    cout << "Statement\n";

    switch(laSymbol)
    {
    case KW_SKIP:
        EmptyStatement(SynchSet);
        break;
    case KW_CALL:
        ProcedureStatement(SynchSet);
        break;
    case KW_READ:
        ReadStatement(SynchSet);
        break;
    case KW_WRITE:
        WriteStatement(SynchSet);
        break;
    case KW_IF:
        IfStatement(SynchSet);
        break;
    case KW_DO:
        DoStatement(SynchSet);
        break;
    case ID: //assignment statement
        AssignmentStatement(SynchSet);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : skip
//follow set : ;
void Parser::EmptyStatement(vector <Symbol> SynchSet)
{
    cout << "EmptyStatement\n";
    SynchSet.push_back(SYM_SEMICOLON);

    if(laSymbol == KW_SKIP)
    {
        match(laSymbol, __func__);
    }
    else
    {
        Error(__func__, "", SynchSet);
        return;
    }

    SynchSet.pop_back();

}

//first set : read
//follow set : ;
void Parser::ReadStatement(vector <Symbol> SynchSet)
{
    cout << "ReadStatement\n";

    if(laSymbol == KW_READ)
    {
        match(laSymbol, __func__);
        vector<myType> tempVect = VariableAccessList(SynchSet);
        admin->emit2("READ",tempVect.size());
    }
    else
    {
        Error(__func__, "", SynchSet);
        return;
    }

}

//first set : write
//follow set : ;
void Parser::WriteStatement(vector <Symbol> SynchSet)
{
    cout << "WriteStatement \n";

    if(laSymbol == KW_WRITE)
    {
        match(laSymbol, __func__);
        vector<myType> typeVec=ExpressionList(SynchSet);
        admin->emit2("WRITE", typeVec.size());
    }
    else
    {
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : letter
//follow set : ;
void Parser::AssignmentStatement(vector <Symbol> SynchSet)
{
    cout << "AssignmentStatement  \n";
//variable access list
    vector<myType> typeList1;
    vector<myType> typeList2;
    if(laSymbol == ID)
    {
        typeList1 = VariableAccessList(SynchSet);
        admin->emit1("ASSIGN",typeList1.size());
    }
    else
    {
        Error(__func__, "", SynchSet);
        return;
    }
//assignment symbol
    if(laSymbol == SYM_ASSIGNMENT)
    {
        match(laSymbol, __func__);
    }
    else
    {
        Error(__func__, "expected ':=' in assignment statement", SynchSet);
        return;
    }
//Expression - ( ~ false true number letter
    switch(laSymbol)
    {
    case SYM_MINUS:
    case SYM_LEFTPAREN:
    case KW_FALSE:
    case KW_TRUE:
    case NUMERAL:
    case ID:
        typeList2 =ExpressionList(SynchSet);
        break;
    default:
        Error(__func__, "Error in assignment statement list", SynchSet);
        return;
    }
    if(typeList1.size()== typeList2.size())
    {
        for(int i = 0; i<typeList1.size();i++)
        {
            if(typeList1[i] != typeList2[i])
            {
                admin->error("Type mismatch between variable list and expression list ",3);
            }
        }

    }
    else
    {
        admin->error("There was a number of items mismatch between variables and expressions",3);
    }

}

//first set : call
//follow set : ;
void Parser::ProcedureStatement(vector <Symbol> SynchSet)
{
    cout << "ProcedureStatement\n";
    if(laSymbol == KW_CALL)
    {
        match(laSymbol, __func__);
    }
    else
    {
        Error(__func__, "", SynchSet);
        return;
    }

    if(laSymbol == ID)
    {

       int position = ProcedureName(SynchSet);
       bool error;
       TableEntry te = bt.find(position,error);
       if(!error){

           int tempindex = te.index;
           Kind tempkind = te.kind;
           admin->emit3("CALL",bt.currentLevel()-te.level,te.startAddress);
                if(tempkind!= PROC)
                {
                    admin->error("Cannot make a procedure call to something that isn't a procedure.",3);
                }
       }
       else{
        admin->error("The procedure was not defined",3);
       }
    }
    else
    {
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : if
//follow set : ;
void Parser::IfStatement(vector <Symbol> SynchSet)
{
    cout << "IfStatement\n";
    if(laSymbol == KW_IF)
    {
        match(laSymbol, __func__);
    }
    else
    {
        Error(__func__, "", SynchSet);
        return;
    }
    int startLabel = NewLabel();
    int doneLabel = NewLabel();
    GuardedCommmandList(SynchSet,startLabel,stopLabel);
    admin->emit2("DEFADDR",startLabel);
    admin->emit2("FI",admin->getLinecount());
    admin->emit2("DEFADDR",doneLabel);
    if(laSymbol == KW_FI)
    {
        match(laSymbol, __func__);
    }
    else
    {
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : do
//follow set : ;
void Parser::DoStatement(vector <Symbol> SynchSet)
{
    cout << "DoStatement\n";

    if(laSymbol == KW_DO)
    {
        match(laSymbol, __func__);
    }
    else
    {
        Error(__func__, "", SynchSet);
        return;
    }
    int startLabel = NewLabel();
    int loopLabel =NewLabel();
    admin->emit2("DEFADDR",loopLabel);
    GuardedCommmandList(SynchSet,startLabel,loopLabel);
    admin->emit2("DEFADDR",startLabel);
    if(laSymbol == KW_OD)
    {
        match(laSymbol, __func__);
    }
    else
    {
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : letter
//follow set : := ;
vector<myType> Parser::VariableAccessList(vector <Symbol> SynchSet)
{
    cout << "VariableAccessList\n";
    vector<myType> typeVec;
    vector<myType> newVec;
    myType temptype;

    if(laSymbol == ID)
    {
        temptype = VariableAccess(SynchSet);
        typeVec.push_back(temptype);
        newVec =VariableAccessListA(SynchSet);
        typeVec.insert(typeVec.end(),newVec.begin(),newVec.end());
        return typeVec;
    }
    else
    {
        Error(__func__, "Unexpected symbol", SynchSet);
        return newVec;
    }

}

//first set : ,
//follow set : := ;
vector<myType> Parser::VariableAccessListA(vector <Symbol> SynchSet)
{
    cout << "VariableAccessListA\n";
    vector<myType> typeVec;
    switch(laSymbol)
    {
    case SYM_COMMA:
        match(laSymbol, __func__);
        typeVec=VariableAccessList(SynchSet);
        return typeVec;
        break;
    case SYM_ASSIGNMENT:
        return typeVec;
    default:
        if(isMember(SynchSet, laSymbol)){
            return typeVec;
        }
        Error(__func__, "expected ',' or ':='", SynchSet);
        return typeVec;
    }
}

//first set : letter
//follow set : * / \ + - < > = ^ | := ( [ , ;
myType Parser::VariableAccess(vector <Symbol> SynchSet)
{
    cout << "VariableAccess \n";
    myType type;
    if(laSymbol == ID)
    {
        NameToken *nt = (NameToken*) laToken;
        bool error;
        TableEntry te = bt.find(nt->getPosition(),error);
        if(!error){

                int tempindex = te.index;
                myType temptype;
                Kind tempKind = te.kind;
                VariableName(SynchSet);
                int present = IndexedSelector(SynchSet);
                temptype = te.type;
                admin->emit3("VARIABLE",bt.currentLevel()-te.level,te.displacement);
            if(present == 1)
            {
                if(tempKind != ARR)
                {
                    admin->error("A variable with an indexed selector must be an array",3);
                }
                else{
                    admin->emit3("INDEX",te.size,admin->getLinecount());
                }
            }
            else
            {
                if(tempKind != VAR)
                {
                    admin->error( "A variable without an indexed selector must be a var",3);
                }
            }
            return temptype;
        }
        else{
            admin->error("The variable was not defined", 3);
            Name(SynchSet);//why is this here??....? What the fuck?
            return UNIVERSAL;
        }
    }
}

//first set : - ( ~ false true number letter
//follow set : ;
vector<myType> Parser::ExpressionList(vector <Symbol> SynchSet)
{
    cout << "ExpressionList \n";

    vector<myType> typeVec;
    myType type;
    vector<myType> newVec;

    switch(laSymbol)
    {
    case SYM_MINUS:
    case SYM_LEFTPAREN:
    case SYM_NOT:
    case KW_FALSE:
    case KW_TRUE:
    case NUMERAL:
    case ID:
        type =Expression(SynchSet);
        typeVec.push_back(type);
        newVec =ExpressionListA(SynchSet);
        typeVec.insert(typeVec.end(),newVec.begin(),newVec.end());
        return typeVec;
        break;
    default:
        Error(__func__, "Expected expression", SynchSet);
        return typeVec;
    }
}

//first set : ,
//follow set : ;
vector<myType> Parser::ExpressionListA(vector <Symbol> SynchSet)
{
    cout << "ExpressionListA \n";
    vector<myType> newVec;
    switch(laSymbol)
    {
    case SYM_COMMA:
        match(laSymbol, __func__);
        return ExpressionList(SynchSet);
        break;
    case SYM_SEMICOLON:
        return newVec;
    default:
        Error(__func__, "Expected ',' or ';'", SynchSet);
        return newVec;
    }
}

//first set: - ( ~ false true name
//follow set: guard fi od
void Parser::GuardedCommand(vector <Symbol> SynchSet,int& startLabel,int GoTo)
{
    cout << "GuardedCommand\n";
    admin->emit2("DEFADDR",startLabel);
    myType type;
    switch(laSymbol)
    {
    case SYM_MINUS:
    case SYM_LEFTPAREN:
    case SYM_NOT:
    case KW_TRUE:
    case KW_FALSE:
    case ID:
        type =Expression(SynchSet);
        if(type != BOOL)
        {
            admin->error("A guarded command must be of type bool",3);
        }
        startLabel = NewLabel();
        if(laSymbol == SYM_RIGHTARROW)
        {
            match(laSymbol, __func__);
            admin->emit2("ARROW",startLabel);
            StatementPart(SynchSet);
            admin->emit2("BAR",GoTo);
        }
        else
        {
            Error(__func__, "Expected '->'", SynchSet);
            return;
        }
        break;
    default:
        Error(__func__, "Unexpected symbol", SynchSet);
        return;
        break;
    }
}

//first set : - ( ~ false true name
//follow set : fi od
void Parser::GuardedCommmandList(vector <Symbol> SynchSet,int& startLabel, int GoTo)
{
    cout << "GuardedCommandList \n";

    GuardedCommand(SynchSet,startLabel,GoTo);

    switch(laSymbol)
    {
    case SYM_GUARD:
        GuardedCommmandListA(SynchSet, startLabel,GoTo);
        break;
    case KW_FI:
    case KW_OD:
        return;
    default:
        Error(__func__, "Expected end of statement or '[]'", SynchSet);
        return;
    }

}

//first set : []
//follow set : fi od
void Parser::GuardedCommmandListA(vector <Symbol> SynchSet,int& startLabel, int GoTo)
{
    cout << "GuardedCommandListA \n";

    switch(laSymbol)
    {
    case SYM_GUARD:
        match(laSymbol, __func__);
        GuardedCommmandList(SynchSet,startLabel,GoTo);
        break;
    case KW_FI:
    case KW_OD:
        return;
    default:
        Error(__func__, "Expected end of statement or '[]'", SynchSet);
        return;
    }
}

//first set : - ( ~ false true number letter
//follow set : , ) ] ;
myType Parser::Expression(vector <Symbol> SynchSet)
{
    cout << "Expression \n";

    myType type;
    vector<myType> typeVec;
    switch(laSymbol)
    {
    case SYM_MINUS:
    case SYM_LEFTPAREN:
    case SYM_NOT:
    case KW_TRUE:
    case KW_FALSE:
    case ID:
    case NUMERAL:
        type = PrimaryExpression(SynchSet);
        typeVec =ExpressionA(SynchSet);
        if(typeVec.size()> 0 && type!= BOOL)
        {
            admin->error("An access of multiple expressions must be a boolean value, not a mathematical one.",3);
        }
        else if(typeVec.size()>0 && type == BOOL)
        {
            for(int i = 0; i<typeVec.size();i++)
            {
                if(typeVec[i] != BOOL)
                {
                    admin->error("The constituent parts of multiple expressions must be a boolean value, not a mathematical one.",3);
                }
            }
        }
        return type;
        break;
    default:
        Error(__func__, "Bad expression", SynchSet);
        return UNIVERSAL;
        break;
    }
}

//first set : and or
//follow set : , -> ) ] ;
vector<myType> Parser::ExpressionA(vector <Symbol> SynchSet)
{
    cout << "ExpressionA \n";
    vector<myType> typeVec;
    myType type;
    switch(laSymbol)
    {
    case SYM_AND:
    case SYM_OR:
        Symbol tempSym =PrimaryOperator(SynchSet);
        type = PrimaryExpression(SynchSet);
        typeVec.push_back(type);
        return typeVec;
        break;
    case SYM_COMMA:
    case SYM_RIGHTPAREN:
    case SYM_RIGHTARROW:
    case SYM_RIGHTSQUARE:
    case SYM_SEMICOLON:
        return typeVec;
    default:
        Error(__func__, "Expected primary expression or expression", SynchSet);
        return typeVec;
    }
}

//first set : and or
//follow set : - ( ~ false true number letter
void Parser::PrimaryOperator(vector <Symbol> SynchSet)
{
    cout << "PrimaryOperator \n";
    switch(laSymbol)
    {
    case SYM_AND:
        admin->emit1("AND");
        match(laSymbol, __func__);
        break;
    case SYM_OR:
        admin->emit1("OR");
        match(laSymbol, __func__);
        break;
    default:
        Error(__func__, "Expected '^' or '|'", SynchSet);
        return;
    }
}

//first set : - ( ~ false true number letter
//follow set : ^ | , ) ] ;
myType Parser::PrimaryExpression(vector <Symbol> SynchSet)
{
    cout << "PrimaryExpression \n";
    myType type;
    vector<myType> typeVec;
    switch(laSymbol)
    {
    case SYM_MINUS:
    case SYM_LEFTPAREN:
    case SYM_NOT:
    case KW_TRUE:
    case KW_FALSE:
    case ID:
    case NUMERAL:
        type=SimpleExpression(SynchSet);
        typeVec =PrimaryExpressionA(SynchSet);
        if(typeVec.size() > 0)
        {
            return BOOL;
        }
        return type;
        break;
    default:
        Error(__func__, "", SynchSet);
        return UNIVERSAL;
    }
}

//first set : < > =
//follow set : ^ | , ) ] ; ->
vector<myType> Parser::PrimaryExpressionA(vector <Symbol> SynchSet)
{
    cout << "PrimaryExpressionA \n";
    vector<myType> typeVec;
    myType type;
    switch(laSymbol)
    {
    case SYM_LESSTHAN:
    case SYM_GREATERTHAN:
    case SYM_EQUAL:
        RelationalOperator(SynchSet);
        type = SimpleExpression(SynchSet);
        typeVec.push_back(type);
        return typeVec;
        break;
    case SYM_AND:
    case SYM_OR:
    case SYM_COMMA:
    case SYM_RIGHTPAREN:
    case SYM_RIGHTSQUARE:
    case SYM_SEMICOLON:
    case SYM_RIGHTARROW:
        return  typeVec;
    default:
        Error(__func__, "", SynchSet);
        return typeVec;
        break;
    }
}

//first set : < > =
//follow set : - ( ~ false true number letter
void Parser::RelationalOperator(vector <Symbol> SynchSet)
{
    cout << "RelationalOperator \n";

    switch(laSymbol)
    {
    case SYM_LESSTHAN:
        admin->emit1("LESS");
        match(laSymbol, __func__);
        break;
    case SYM_GREATERTHAN:
        admin->emit1("GREATER");
        match(laSymbol, __func__);
        break;
    case SYM_EQUAL:
        admin->emit1("EQUAL");
        match(laSymbol, __func__);
        break;
    default:
        Error(__func__, "Expected '<' , '>' , '='", SynchSet);
        return;
        break;
    }
}

//first set : - ( ~ false true number letter
//follow set : < > = ^ | , ) ] ;
myType Parser::SimpleExpression(vector <Symbol> SynchSet)
{
    cout << "SimpleExpression \n";

    if(laSymbol == SYM_MINUS)
    {
        match(laSymbol, __func__);
    }
    return SimpleExpressionA(SynchSet);

}

//first set : ( ~ false true number letter
//follow set : < > = ^ | , ) ] ; ->
myType Parser::SimpleExpressionA(vector <Symbol> SynchSet)
{
    cout << "SimpleExpressionA \n";
    myType type;
    vector<myType> typeVec;
    switch(laSymbol)
    {
    case SYM_LEFTPAREN:
    case SYM_NOT:
    case KW_FALSE:
    case KW_TRUE:
    case NUMERAL:
    case ID:
        type=Term(SynchSet);
        typeVec =SimpleExpressionB(SynchSet);
        if(typeVec.size()> 0 && type!= INT)
        {
            admin->error("A simple expression of multiple terms must be an int",3);
        }
        else if(typeVec.size()>0 && type == INT)
        {
            for(int i = 0; i<typeVec.size();i++)
            {
                if(typeVec[i] != INT)
                {
                    admin->error("The subsequent parts of a simple expression with multiple terms must be of type int.",3);
                }
            }
        }
        return type;
        break;
    case SYM_LESSTHAN:
    case SYM_GREATERTHAN:
    case SYM_EQUAL:
    case SYM_AND:
    case SYM_OR:
    case SYM_COMMA:
    case SYM_RIGHTPAREN:
    case SYM_RIGHTSQUARE:
    case SYM_SEMICOLON:
    case SYM_RIGHTARROW:
        return UNIVERSAL;//I have no idea what to do here, check with jesse. I'm working on it.
    default:
        Error(__func__, "", SynchSet);
        return UNIVERSAL;
    }
}

//first set : + -
//follow set : < > = ^ | , ) ] ; ->
vector<myType> Parser::SimpleExpressionB(vector <Symbol> SynchSet)
{
    cout << "SimpleExpressionB \n";

    vector<myType> typeVec;
    myType type;
    switch(laSymbol)
    {
    case SYM_PLUS:
    case SYM_MINUS:
        AddingOperator(SynchSet);
        type = SimpleExpressionA(SynchSet);
        typeVec.push_back(type);
        return typeVec;
        break;
    case SYM_LESSTHAN:
    case SYM_GREATERTHAN:
    case SYM_EQUAL:
    case SYM_AND:
    case SYM_OR:
    case SYM_COMMA:
    case SYM_RIGHTPAREN:
    case SYM_RIGHTARROW:
    case SYM_RIGHTSQUARE:
    case SYM_LEFTPAREN:
    case SYM_SEMICOLON:
        return typeVec;
    default:
        Error(__func__, "", SynchSet);
        return typeVec;
        break;
    }
}

//first set : + -
//follow set : + - < > = ^ | , ) ] ;
void Parser::AddingOperator(vector <Symbol> SynchSet)
{
    cout << "AddingOperator \n";

    switch(laSymbol)
    {
    case SYM_PLUS:
        admin->emit1("ADD");
        match(laSymbol, __func__);
        break;
    case SYM_MINUS:
        admin->emit1("SUBTRACT");
        match(laSymbol, __func__);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : ( ~ false true number letter
//follow set : + - < > = ^ | , ) ] ;
myType Parser::Term(vector <Symbol> SynchSet)
{
    cout << "Term \n";
    myType type;
    vector<myType> typeVec;

    switch(laSymbol)
    {
    case SYM_LEFTPAREN:
    case SYM_NOT:
    case KW_FALSE:
    case KW_TRUE:
    case NUMERAL:
    case ID:
        type =Factor(SynchSet);

        typeVec = TermA(SynchSet);
        if(typeVec.size()!=0)
        {
            if(type != INT)
            {
                admin->error("An occurence of a factor in a Term must be of type int.",3);
            }
            for(int i = 0; i < typeVec.size(); ++i)
            {
                if(typeVec[i] != INT)
                {
                    admin->error("An occurance of a factor in a Term must be of type int.",3);
                }
            }
        }
        return type;
        break;
    default:
        Error(__func__, "", SynchSet);
        return UNIVERSAL;
    }
}

//first set : * / \
//follow set : + - < > = ^ | , ) ] ;
vector <myType> Parser::TermA(vector <Symbol> SynchSet)
{
    cout << "TermA \n";
    vector<myType> typeVec;
    myType type;
    switch(laSymbol)
    {
    case SYM_MULTIPLY:
    case SYM_DIVIDE:
    case SYM_MODULO:
        MultiplyingOperator(SynchSet);
        type = Term(SynchSet);
        typeVec.push_back(type);
        return typeVec;
        break;
    case SYM_PLUS:
    case SYM_MINUS:
    case SYM_LESSTHAN:
    case SYM_GREATERTHAN:
    case SYM_EQUAL:
    case SYM_AND:
    case SYM_OR:
    case SYM_COMMA:
    case SYM_RIGHTARROW:
    case SYM_RIGHTPAREN:
    case SYM_RIGHTSQUARE:
    case SYM_SEMICOLON:
        return typeVec;
    default:
        Error(__func__, "", SynchSet);
        return typeVec;
    }
}

//first set : ( ^ false true number letter
//follow set : * / \ + - < > = ^ | , ) ] ;
myType Parser::Factor(vector <Symbol> SynchSet)
{
    cout << "Factor \n";
    NameToken *nt;
    bool error;
    TableEntry te;
    Kind tempkind;
    myType temptype;
    int constValue;
    switch(laSymbol)
    {
    case NUMERAL:
        constValue =Constant(SynchSet);
        return INT;
        break;
    case KW_TRUE:
        constValue=Constant(SynchSet);
        return BOOL;
        break;
    case KW_FALSE:
        constValue =Constant(SynchSet);
        return BOOL;
        break;
    case ID:
        nt = (NameToken*) laToken;
        te = bt.find(nt->getPosition(), error);
        if(!error){
            tempkind = te.kind;
            temptype = te.type;
            if(tempkind == CONSTANT)
            {

                Constant(SynchSet);
                admin->emit2("CONSTANT",te.value);

            }
            else if(tempkind == VAR | tempkind == ARR)
            {
                VariableAccess(SynchSet);
                admin->emit1("VALUE");
            }
            return temptype;
//            else
//            {
//                admin->error("The id being accessed is undefined.",3);
//            }
        }
        else
        {
            admin->error("The id being accessed is undefined.",3);
            VariableAccess(SynchSet);
        }

        break;
    case SYM_LEFTPAREN:
        match(laSymbol, __func__);
        temptype =Expression(SynchSet);
        if(laSymbol == SYM_RIGHTPAREN)
            match(laSymbol, __func__);
        else
        {
            Error(__func__, "Missing ')'", SynchSet);
        }
        return temptype;
        break;
    case SYM_NOT:
        match(laSymbol, __func__);
        admin->emit1("NOT");
        return Factor(SynchSet);
        break;
    default:
        Error(__func__, "", SynchSet);
        return UNIVERSAL;
    }
}

//first set : * / \
//follow set : ( ~ false true number letter
void Parser::MultiplyingOperator(vector <Symbol> SynchSet)
{
    cout << "MultiplyingOperator \n";

    switch(laSymbol)
    {
    case SYM_MULTIPLY:
        admin->emit1("MULTIPLY");
        match(laSymbol, __func__);
        break;
    case SYM_DIVIDE:
        admin->emit("DIVIDE");
        match(laSymbol, __func__);
        break;
    case SYM_MODULO:
        admin->emit("MODULO");
        match(laSymbol, __func__);
        break;
    default:
        Error(__func__, "Expected '*' , '/' , or '\'", SynchSet);
        return;
    }
}

//first set : [
//follow set : * / \ + - < > = ^ | := -> ) ] , ;
int Parser::IndexedSelector(vector <Symbol> SynchSet)
{
    cout << "IndexedSelector \n";
    myType type;
    switch(laSymbol)
    {
    case SYM_LEFTSQUARE:
        match(laSymbol, __func__);
        type =Expression(SynchSet);
        if(type != INT)
        {
            admin->error("An indexed selector must have an int as its enclosed value",3);
        }
        if(laSymbol == SYM_RIGHTSQUARE)
        {
            match(laSymbol, __func__);
            return 1;
        }
        else
        {
            Error(__func__, "Missing closing bracket", SynchSet);
            return 0;
        }
        break;
    default:
        return 0;
        break;
    }
}

//first set : false true number letter
//follow set : * / \ + - < > = ^ | , ) ] ;
int Parser::Constant(vector <Symbol> SynchSet)
{
    cout << "Constant\n";
    int constVal = 0;
    switch(laSymbol)
    {
    case NUMERAL:
        constVal =Numeral(SynchSet);
        break;
    case KW_TRUE:
    case KW_FALSE:
        constVal =BooleanSymbol(SynchSet);
        break;
    case ID:
        ConstantName(SynchSet);
        break;
    default:
        Error(__func__, "Expected constant", SynchSet);
        return 0;
        break;
    }
    return constVal;
}

//first set : false true
//follow set : * / \ + - < > = ^ | , ) ] ;
int Parser::BooleanSymbol(vector <Symbol> SynchSet)
{
    cout << "BooleanSymbol \n";

    switch(laSymbol)
    {
    case KW_TRUE:
        match(laSymbol, __func__);
        return 1;
        break;
    case KW_FALSE:
        match(laSymbol, __func__);
        return 0;
        break;
    default:
        Error(__func__, "Expected 'true' or 'false'", SynchSet);
        return;
    }
}

//first set : number
//follow set : * / \ + - < > = ^ | , ) ] ;
int Parser::Numeral(vector <Symbol> SynchSet)
{
    cout << "Numeral \n";
    NumberToken *nt = (NumberToken*) laToken;
    int val = nt->getValue();
    if(laSymbol == NUMERAL)
    {
        match(laSymbol, __func__);
        return val;
    }
    else
    {
        Error(__func__, "Numeral error", SynchSet);
    }
}

//first set : letter
//follow set : * / \ + - < > = ^ | :=  , ) ] ;
void Parser::VariableName(vector <Symbol> SynchSet)
{
    cout << "VariableName \n";
    if(laSymbol == ID)
    {
        match(laSymbol, __func__);
    }
    else
    {
        Error(__func__, "Variable name error", SynchSet);
    }
}

//first set : letter
//follow set : * / \ + - < > = ^ | , ) ] = ;
void Parser::ConstantName(vector <Symbol> SynchSet)
{
    cout << "ConstantName \n";
    if(laSymbol == ID)
    {
        match(laSymbol, __func__);
    }
    else
    {
        Error(__func__, "Constant name error", SynchSet);
    }
}

//first set : letter
//follow set : ; begin
int Parser::ProcedureName(vector <Symbol> SynchSet)
{
    cout << "ProcedureName \n";
    if(laSymbol == ID)
    {
        NameToken *nt = (NameToken*) laToken;
        int position = nt->getPosition();
        match(laSymbol, __func__);
        cout << position << endl << endl;
        return position;
    }
    else
    {
        Error(__func__, "Procedure name error", SynchSet);
        return -1;
    }
}

//first set : letter
//follow set : * / \ + - < > = ^ | :=  , ) ] ;
void Parser::Name(vector <Symbol> SynchSet)
{
    cout << "Name \n";
    if(laSymbol == ID)
    {
        match(laSymbol, __func__);
    }
    else
    {
        Error(__func__, "Name error", SynchSet);
    }
}

void Parser::Error(const char funcname[], string errMessage, vector<Symbol> &SynchSet)
{
	if(!panic)
	{
		panic = true;
		cout << "\nBegin error recovery.\n";
		errorCount++;

		admin->error(errMessage + " : " + laSymbolName() + " in " + funcname, 2);

		//cout << "Error " << errMessage << " : " << laSymbolName() << " in " << funcname << ".\n\n";
	}
		//get a new symbol while
		while(!isMember(SynchSet, laSymbol))
		{
			//cout << "getting new token in error\n";
			getNextToken();
		};

}

void Parser::match(Symbol sym, const char funcname[])
{

    cout << "match terminal : " << laSymbolName() << " in " << funcname << endl;
//    --panicCount;
	if(panic)//panicCount == 0)
    {
        cout << "Panic recovered.\n\n";
		panic = false;
    }
	getNextToken();
    cout << "Got new token : " << laSymbolName() << endl;

}


//helper funtions

void Parser::addSymbol(vector<Symbol> &symSet, Symbol sym)
{
    if(!isMember(symSet, sym))
    {
        symSet.push_back(sym);
    }
}

bool Parser::isMember(vector<Symbol> &checkset, Symbol sym)
{

    if(std::find(checkset.begin(), checkset.end(), sym) != checkset.end())
    {
        return true;
    }
    else
    {
        return false;
    }

}

string Parser::laSymbolName()
{
	return laToken->getTokenString();
}

void Parser::getNextToken()
{

	do
    {
        laToken = scptr->getToken();
    }
    while(laToken == nullptr);

    laSymbol = laToken->getSymbolName();

	switch(laSymbol)
	{
	//these are all scanning errors, so they should be reported by the scanner and just ignored here.
	case BAD_SCAN:
	case BAD_SYM:
	case BAD_ID:
	case BAD_NUMERAL:
		cout << "scanner error : " << laSymbolName() << endl;
		getNextToken();
	default:
		break;
	}

}
