/****

Parser with improved function and error detection and recovery.

Author : Jesse Bevans

****/


#include <algorithm>
#include <vector>
#include "Parser.h"
#include <iostream>

using namespace std;

Parser::Parser(Scanner& sc)
{
//ctor
    scptr = &sc;
    laToken = nullptr;
    panic = false;
    panicCount = 0;
    errorCount = 0;
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

    cout << "First token : " << SymbolTypeString[laSymbol - 256] << endl;
    if(laSymbol == KW_BEGIN)
    {
        addSymbol(SynchSet, SYM_EOF); //end of any possible input
        Program(SynchSet);
    }

    if(laSymbol != SYM_EOF)
    {
		Error(__func__, "Expected EOF not found", SynchSet);
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

    //
    if(laSymbol != KW_BEGIN)
    {
        Error(__func__, "Missing 'begin' at start of program", SynchSet);
    }
    Block(SynchSet);

    if(laSymbol == SYM_PERIOD)
    {
        match(laSymbol, __func__);
    }
    else
    {
        Error(__func__, "Missing '.' at end of program", SynchSet);
    }

}

//first set :  begin
//follow set :  ; .
void Parser::Block(vector <Symbol> SynchSet)
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

    DefinitionPart(SynchSet);

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
void Parser::DefinitionPart(vector <Symbol> SynchSet)
{
    cout << "DefinitionPart\n";

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
        Definition(SynchSet);
        //all definitions must be followed by a semicolon
        if(laSymbol == SYM_SEMICOLON)
        {
            match(laSymbol, __func__);
            DefinitionPart(SynchSet);
        }
        else
        {
            Error(__func__, "Missing ';' at end of Definition", SynchSet);
//			if(isMember(SynchSet, laSymbol))
//			{
//				cout << SymbolTypeString[laSymbol-256] << " in " << __func__ << " synch set." << endl;
//				DefinitionPart(SynchSet);
//			}
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
        return;
	//any unexpected or incorrect symbols
    default:
        Error(__func__, "Unexpected symbol", SynchSet);
        DefinitionPart(SynchSet);
    }

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
			if(isMember(SynchSet, laSymbol))
			{
				cout << SymbolTypeString[laSymbol-256] << " in " << __func__ << " synch set." << endl;
				StatementPart(SynchSet);
			}
        }
        break;
	//follow set
    case SYM_GUARD:
    case KW_FI:
    case KW_OD:
    case KW_END:
        return;
	case SYM_SEMICOLON:
		Error(__func__, "Blank statement", SynchSet);
		match(laSymbol,__func__);
		break;
	//unexpected symbols
    default:
        Error(__func__, "Unexpected symbol", SynchSet);
        if(isMember(SynchSet, laSymbol))
        {
            cout << SymbolTypeString[laSymbol-256] << " in " << __func__ << " synch set." << endl;
            StatementPart(SynchSet);
        }
    }

}

//first set :  const proc integer Boolean
//follow set :  ;
void Parser::Definition(vector <Symbol> SynchSet)
{
    cout << "Definition\n";

    addSymbol(SynchSet, SYM_SEMICOLON);

    switch(laSymbol)
    {
    case KW_CONST:
        ConstantDefinition(SynchSet);
        break;
    case KW_INTEGER:
    case KW_BOOLEAN:
        VariableDefinition(SynchSet);
        break;
    case KW_PROC:
        ProcedureDefinition(SynchSet);
        break;
    default:
        Error(__func__, "Unexpected symbol", SynchSet);
        return;
        break;
    }
}

//first set :  const
//follow set :  ;
void Parser::ConstantDefinition(vector <Symbol> SynchSet)
{
    cout << "ConstantDefinition\n";

    addSymbol(SynchSet, SYM_SEMICOLON);

//const
    if(laSymbol == KW_CONST)
    {
        match(laSymbol, __func__);
    }
    else
    {
        Error(__func__, "Missing 'const'", SynchSet);
    }
//const name
    if(laSymbol == ID)
    {
        ConstantName(SynchSet);
    }
    else
    {
        Error(__func__, "Missing ID", SynchSet);
    }

//equals
    if(laSymbol == SYM_EQUAL)
    {
        match(laSymbol, __func__);
    }
    else
    {
        Error(__func__, "Missing '='", SynchSet);
    }

//constant
    switch(laSymbol)
    {
    case NUMERAL:
    case KW_FALSE:
    case KW_TRUE:
    case ID:
        Constant(SynchSet);
        break;
    default:
        Error(__func__, "Missing constant", SynchSet);
    }

}

//first set :  integer boolean
//follow set :  ;
void Parser::VariableDefinition(vector <Symbol> SynchSet)
{
    cout << "VariableDefinition\n";

    addSymbol(SynchSet, SYM_SEMICOLON);

//type symbol
    switch(laSymbol)
    {
    case KW_INTEGER:
    case KW_BOOLEAN:
        TypeSymbol(SynchSet);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
    }

//variable definition A
    if(laSymbol == ID | laSymbol == KW_ARRAY)
    {
        VariableDefinitionA(SynchSet);
    }
    else
    {
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set :  array letter
//follow set :  ;
void Parser::VariableDefinitionA(vector <Symbol> SynchSet)
{
    cout << "VariableDefinitionA\n";

    addSymbol(SynchSet, SYM_SEMICOLON);

    if(laSymbol == ID)
    {
        VariableList(SynchSet);
        return;
    }
    else if(laSymbol == KW_ARRAY)
    {
        match(laSymbol, __func__);
        if(laSymbol == ID)
        {
            VariableList(SynchSet);
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

        switch(laSymbol)
        {
        case NUMERAL:
        case KW_TRUE:
        case KW_FALSE:
        case ID:
            Constant(SynchSet);
            break;
        default:
            Error(__func__, "", SynchSet);
            return;
            break;
        }

        if(laSymbol == SYM_RIGHTSQUARE)
        {
            match(laSymbol, __func__);
        }
        else
        {
            Error(__func__, "", SynchSet);
            return;
        }
    }
    else
    {
        Error(__func__, "", SynchSet);
        return;
    }


}

void Parser::TypeSymbol(vector <Symbol> SynchSet)
{
    switch(laSymbol)
    {
    case KW_INTEGER:
    case KW_BOOLEAN:
        match(laSymbol, __func__);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set :  letter
//follow set :   := [ ;
void Parser::VariableList(vector <Symbol> SynchSet)
{
    cout << "VariableList\n";

    addSymbol(SynchSet, SYM_SEMICOLON);
    addSymbol(SynchSet, SYM_ASSIGNMENT);
    addSymbol(SynchSet, SYM_LEFTSQUARE);

    switch(laSymbol)
    {
    case ID:
        VariableName(SynchSet);
        VariableListA(SynchSet);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set :  ,
//follow set :  := ;
void Parser::VariableListA(vector <Symbol> SynchSet)
{

    switch(laSymbol)
    {
    case SYM_COMMA:
        match(laSymbol, __func__);
        VariableList(SynchSet);
        break;
    case SYM_ASSIGNMENT:
    case SYM_LEFTSQUARE:
    case SYM_SEMICOLON:
        return;
    default:
        Error(__func__, "", SynchSet);
        return;
    }

}

//first set :  proc
//follow set :  ;
void Parser::ProcedureDefinition(vector <Symbol> SynchSet)
{
    if(laSymbol == KW_PROC)
    {
        match(laSymbol, __func__);
        ProcedureName(SynchSet); ///add in more IF to make sure? redundant
        Block(SynchSet);
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
        VariableAccessList(SynchSet);
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
        ExpressionList(SynchSet);
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
    if(laSymbol == ID)
    {
        VariableAccessList(SynchSet);
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
        Error(__func__, "", SynchSet);
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
        ExpressionList(SynchSet);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
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
        ProcedureName(SynchSet);
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
///add checks for this (expression)
    GuardedCommmandList(SynchSet);

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

    GuardedCommmandList(SynchSet);

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
void Parser::VariableAccessList(vector <Symbol> SynchSet)
{
    cout << "VariableAccessList\n";

    if(laSymbol == ID)
    {
        VariableAccess(SynchSet);
        VariableAccessListA(SynchSet);
    }
    else
    {
        Error(__func__, "", SynchSet);
        return;
    }


}

//first set : ,
//follow set : := ;
void Parser::VariableAccessListA(vector <Symbol> SynchSet)
{
    cout << "VariableAccessListA\n";

    switch(laSymbol)
    {
    case SYM_COMMA:
        match(laSymbol, __func__);
        VariableAccessList(SynchSet);
        break;
    case SYM_ASSIGNMENT:
    case SYM_SEMICOLON:
        return;
    default:
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : letter
//follow set : * / \ + - < > = ^ | := ( [ , ;
void Parser::VariableAccess(vector <Symbol> SynchSet)
{
    cout << "VariableAccess \n";

    if(laSymbol == ID)
    {
        VariableName(SynchSet);
        IndexedSelector(SynchSet);
    }
}

//first set : - ( ~ false true number letter
//follow set : ;
void Parser::ExpressionList(vector <Symbol> SynchSet)
{
    cout << "ExpressionList \n";

    switch(laSymbol)
    {
    case SYM_MINUS:
    case SYM_LEFTPAREN:
    case SYM_NOT:
    case KW_FALSE:
    case KW_TRUE:
    case NUMERAL:
    case ID:
        Expression(SynchSet);
        ExpressionListA(SynchSet);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : ,
//follow set : ;
void Parser::ExpressionListA(vector <Symbol> SynchSet)
{
    cout << "ExpressionListA \n";

    switch(laSymbol)
    {
    case SYM_COMMA:
        match(laSymbol, __func__);
        ExpressionList(SynchSet);
        break;
    case SYM_SEMICOLON:
        return;
    default:
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set: - ( ~ false true name
//follow set: guard fi od
void Parser::GuardedCommand(vector <Symbol> SynchSet)
{
    switch(laSymbol)
    {
    case SYM_MINUS:
    case SYM_LEFTPAREN:
    case SYM_NOT:
    case KW_TRUE:
    case KW_FALSE:
    case ID:
        Expression(SynchSet);
        if(laSymbol == SYM_RIGHTARROW)
        {
            match(laSymbol, __func__);
            StatementPart(SynchSet);
        }
        else
        {
            Error(__func__, "", SynchSet);
            return;
        }
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
        break;
    }
}

//first set : - ( ~ false true name
//follow set : fi od
void Parser::GuardedCommmandList(vector <Symbol> SynchSet)
{
    cout << "GuardedCommandList \n";

    GuardedCommand(SynchSet);
///
    switch(laSymbol)
    {
    case SYM_GUARD:
        GuardedCommmandListA(SynchSet);
        break;
    case KW_FI:
    case KW_OD:
        return;
    default:
        Error(__func__, "", SynchSet);
        return;
    }

}

//first set : []
//follow set : fi od
void Parser::GuardedCommmandListA(vector <Symbol> SynchSet)
{
    cout << "GuardedCommandListA \n";

    switch(laSymbol)
    {
    case SYM_GUARD:
        match(laSymbol, __func__);
        GuardedCommmandList(SynchSet);
        break;
    case KW_FI:
    case KW_OD:
        return;
    default:
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : - ( ~ false true number letter
//follow set : , ) ] ;
void Parser::Expression(vector <Symbol> SynchSet)
{
    cout << "Expression \n";

    switch(laSymbol)
    {
    case SYM_MINUS:
    case SYM_LEFTPAREN:
    case SYM_NOT:
    case KW_TRUE:
    case KW_FALSE:
    case ID:
    case NUMERAL:
        PrimaryExpression(SynchSet);
        ExpressionA(SynchSet);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
        break;
    }
}

//first set : and or
//follow set : , -> ) ] ;
void Parser::ExpressionA(vector <Symbol> SynchSet)
{
    cout << "ExpressionA \n";

    switch(laSymbol)
    {
    case SYM_AND:
    case SYM_OR:
        PrimaryOperator(SynchSet);
        PrimaryExpression(SynchSet);
        break;
    case SYM_COMMA:
    case SYM_RIGHTPAREN:
    case SYM_RIGHTARROW:
    case SYM_RIGHTSQUARE:
    case SYM_SEMICOLON:
        return;
    default:
        Error(__func__, "", SynchSet);
        return;
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
    case SYM_OR:
        match(laSymbol, __func__);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : - ( ~ false true number letter
//follow set : ^ | , ) ] ;
void Parser::PrimaryExpression(vector <Symbol> SynchSet)
{
    cout << "PrimaryExpression \n";

    switch(laSymbol)
    {
    case SYM_MINUS:
    case SYM_LEFTPAREN:
    case SYM_NOT:
    case KW_TRUE:
    case KW_FALSE:
    case ID:
    case NUMERAL:
        SimpleExpression(SynchSet);
        PrimaryExpressionA(SynchSet);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : < > =
//follow set : ^ | , ) ] ; ->
void Parser::PrimaryExpressionA(vector <Symbol> SynchSet)
{
    cout << "PrimaryExpressionA \n";

    switch(laSymbol)
    {
    case SYM_LESSTHAN:
    case SYM_GREATERTHAN:
    case SYM_EQUAL:
        RelationalOperator(SynchSet);
        SimpleExpression(SynchSet);
        break;
    case SYM_AND:
    case SYM_OR:
    case SYM_COMMA:
    case SYM_RIGHTPAREN:
    case SYM_RIGHTSQUARE:
    case SYM_SEMICOLON:
    case SYM_RIGHTARROW:
        return;
    default:
        Error(__func__, "", SynchSet);
        return;
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
    case SYM_GREATERTHAN:
    case SYM_EQUAL:
        match(laSymbol, __func__);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
        break;
    }
}

//first set : - ( ~ false true number letter
//follow set : < > = ^ | , ) ] ;
void Parser::SimpleExpression(vector <Symbol> SynchSet)
{
    cout << "SimpleExpression \n";

    if(laSymbol == SYM_MINUS)
    {
        match(laSymbol, __func__);
    }
    SimpleExpressionA(SynchSet);

}

//first set : ( ~ false true number letter
//follow set : < > = ^ | , ) ] ; ->
void Parser::SimpleExpressionA(vector <Symbol> SynchSet)
{
    cout << "SimpleExpressionA \n";

    switch(laSymbol)
    {
    case SYM_LEFTPAREN:
    case SYM_NOT:
    case KW_FALSE:
    case KW_TRUE:
    case NUMERAL:
    case ID:
        Term(SynchSet);
        SimpleExpressionB(SynchSet);
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
        return;
    default:
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : + -
//follow set : < > = ^ | , ) ] ; ->
void Parser::SimpleExpressionB(vector <Symbol> SynchSet)
{
    cout << "SimpleExpressionB \n";

    switch(laSymbol)
    {
    case SYM_PLUS:
    case SYM_MINUS:
        AddingOperator(SynchSet);
        SimpleExpressionA(SynchSet);
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
        return;
    default:
        Error(__func__, "", SynchSet);
        return;
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
    case SYM_MINUS:
        match(laSymbol, __func__);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : ( ~ false true number letter
//follow set : + - < > = ^ | , ) ] ;
void Parser::Term(vector <Symbol> SynchSet)
{
    cout << "Term \n";

    switch(laSymbol)
    {
    case SYM_LEFTPAREN:
    case SYM_NOT:
    case KW_FALSE:
    case KW_TRUE:
    case NUMERAL:
    case ID:
        Factor(SynchSet);
        TermA(SynchSet);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : * / \
//follow set : + - < > = ^ | , ) ] ;
void Parser::TermA(vector <Symbol> SynchSet)
{
    cout << "TermA \n";

    switch(laSymbol)
    {
    case SYM_MULTIPLY:
    case SYM_DIVIDE:
    case SYM_MODULO:
        MultiplyingOperator(SynchSet);
        Term(SynchSet);
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
        return;
    default:
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : ( ^ false true number letter
//follow set : * / \ + - < > = ^ | , ) ] ;
void Parser::Factor(vector <Symbol> SynchSet)
{
    cout << "Factor \n";

    switch(laSymbol)
    {
    case NUMERAL:
    case KW_TRUE:
    case KW_FALSE:
///case ID: ///AMBIGUITY ALERT !!!! For now we will send it down the variable access path
        Constant(SynchSet);
        break;
    case ID:
        VariableAccess(SynchSet);
        break;
    case SYM_LEFTPAREN:
        match(laSymbol, __func__);
        Expression(SynchSet);
        if(laSymbol == SYM_RIGHTPAREN)
            match(laSymbol, __func__);
        else
        {
            Error(__func__, "", SynchSet);
            return;
        }
        break;
    case SYM_NOT:
        match(laSymbol, __func__);
        Factor(SynchSet);
        break;

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
    case SYM_DIVIDE:
    case SYM_MODULO:
        match(laSymbol, __func__);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : [
//follow set : * / \ + - < > = ^ | := -> ) ] , ;
void Parser::IndexedSelector(vector <Symbol> SynchSet)
{
    cout << "IndexedSelector \n";

    switch(laSymbol)
    {
    case SYM_LEFTSQUARE:
        match(laSymbol, __func__);
        Expression(SynchSet);
        if(laSymbol == SYM_RIGHTSQUARE)
        {
            match(laSymbol, __func__);
        }
        else
        {
            Error(__func__, "", SynchSet);
            return;
        }
        break;
    case SYM_MULTIPLY:
    case SYM_MODULO:
    case SYM_DIVIDE:
    case SYM_PLUS:
    case SYM_MINUS:
    case SYM_LESSTHAN:
    case SYM_GREATERTHAN:
    case SYM_EQUAL:
    case SYM_AND:
    case SYM_OR:
    case SYM_RIGHTARROW:
    case SYM_ASSIGNMENT:
    case SYM_RIGHTPAREN:
    case SYM_RIGHTSQUARE:
    case SYM_COMMA:
    case SYM_SEMICOLON:
        return;
    default:
        Error(__func__, "", SynchSet);
        return;
        break;
    }
}

//first set : false true number letter
//follow set : * / \ + - < > = ^ | , ) ] ;
void Parser::Constant(vector <Symbol> SynchSet)
{
    cout << "Constant\n";
    switch(laSymbol)
    {
    case NUMERAL:
        Numeral(SynchSet);
        break;
    case KW_TRUE:
    case KW_FALSE:
        BooleanSymbol(SynchSet);
        break;
    case ID:
        ConstantName(SynchSet);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
        break;
    }
}

//first set : false true
//follow set : * / \ + - < > = ^ | , ) ] ;
void Parser::BooleanSymbol(vector <Symbol> SynchSet)
{
    cout << "BooleanSymbol \n";

    switch(laSymbol)
    {
    case KW_TRUE:
    case KW_FALSE:
        match(laSymbol, __func__);
        break;
    default:
        Error(__func__, "", SynchSet);
        return;
    }
}

//first set : number
//follow set : * / \ + - < > = ^ | , ) ] ;
void Parser::Numeral(vector <Symbol> SynchSet)
{
    cout << "Numeral \n";
    if(laSymbol == NUMERAL)
        match(laSymbol, __func__);
    else
    {
        Error(__func__, "", SynchSet);
        return;
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
}

//first set : letter
//follow set : ; begin
void Parser::ProcedureName(vector <Symbol> SynchSet)
{
    cout << "ProcedureName \n";
    if(laSymbol == ID)
    {
        match(laSymbol, __func__);
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
}

void Parser::Error(const char funcname[], string errMessage, vector<Symbol> &synchSet)
{
	if(!panic)
	{
		panic = true;
		cout << "\nBegin panic mode error recovery.\n";
		errorCount++;
	}
		cout << "Error " << errMessage << " : " << SymbolTypeString[laSymbol-256] << " found in " << funcname << ".\n\n";

		//if we see an error symbol, get rid of it and get the next token.
		while(!isMember(synchSet, laSymbol))
		{
			cout << "getting new token in error\n";
			laToken = scptr->getToken();
			laSymbol = laToken->getSymbolName();
		};

}

void Parser::match(Symbol sym)
{

    cout << "match terminal : " << SymbolTypeString[sym - 256] << endl;
//    --panicCount;
	if(panic)//panicCount == 0)
    {
        cout << "Error recovered.\n\n";
		panic = false;
    }

    do
    {
        laToken = scptr->getToken();
    }
    while(laToken == nullptr);
	laSymbol = laToken->getSymbolName();
    cout << "Got new token : " << SymbolTypeString[laSymbol - 256] << endl;
}

void Parser::match(Symbol sym, const char funcname[])
{

    cout << "match terminal : " << SymbolTypeString[sym - 256] << " in " << funcname << endl;
//    --panicCount;
	if(panic)//panicCount == 0)
    {
        cout << "Panic recovered.\n\n";
		panic = false;
    }

    do
    {
        laToken = scptr->getToken();
    }
    while(laToken == nullptr);
    laSymbol = laToken->getSymbolName();

    cout << "Got new token : " << SymbolTypeString[laSymbol - 256] << endl;
}

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