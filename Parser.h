#ifndef PARSER_H
#define PARSER_H

#include "Symbol.h"
#include "Scanner.h"
#include "Administration.h"
#include "TypeKindEnum.h"
#include "BlockTable.h"
class Administration;
class Scanner;

class Parser
{
public:
    Parser(Scanner &sp);
    virtual ~Parser();
    int parse();
    void setAdmin(Administration &ap){admin = &ap;}

protected:

private:

    void Program(vector <Symbol> SynchSet); //start symbol
    void Block(vector <Symbol> SynchSet,int sLabel,int vLabel);
    int DefinitionPart(vector <Symbol> SynchSet,int& nextVarStart);
    void StatementPart(vector <Symbol> SynchSet);
    int Definition(vector <Symbol> SynchSet,int& nextVarStart);
    void ConstantDefinition(vector <Symbol> SynchSet);
    int VariableDefinition(vector <Symbol> SynchSet,int& nextVarStart);
    void VariableDefinitionA(vector <Symbol> SynchSet, myType TempType);
    myType TypeSymbol(vector <Symbol> SynchSet);
    vector<int> VariableList(vector <Symbol> SynchSet);
    vector<int> VariableListA(vector <Symbol> SynchSet);
    void ProcedureDefinition(vector <Symbol> SynchSet);
    void Statement(vector <Symbol> SynchSet);
    void EmptyStatement(vector <Symbol> SynchSet);
    void ReadStatement(vector <Symbol> SynchSet);
    void WriteStatement(vector <Symbol> SynchSet);
    void AssignmentStatement(vector <Symbol> SynchSet);
    void ProcedureStatement(vector <Symbol> SynchSet);
    void IfStatement(vector <Symbol> SynchSet);
    void DoStatement(vector <Symbol> SynchSet);
    vector<myType> VariableAccessList(vector <Symbol> SynchSet);
    vector<myType> VariableAccessListA(vector <Symbol> SynchSet);
    myType VariableAccess(vector <Symbol> SynchSet);
    vector<myType> ExpressionList(vector <Symbol> SynchSet);
    vector<myType> ExpressionListA(vector <Symbol> SynchSet);
    void GuardedCommand(vector <Symbol> SynchSet,int& startLabel,int GoTo);
    void GuardedCommmandList(vector <Symbol> SynchSet,int& startLabel,int GoTo);
    void GuardedCommmandListA(vector <Symbol> SynchSet,int& startLabel,int GoTo);
    myType Expression(vector <Symbol> SynchSet);
    vector<myType> ExpressionA(vector <Symbol> SynchSet);
    void PrimaryOperator(vector <Symbol> SynchSet);
    myType PrimaryExpression(vector <Symbol> SynchSet);
    vector<myType> PrimaryExpressionA(vector <Symbol> SynchSet);
    void RelationalOperator(vector <Symbol> SynchSet);
    myType SimpleExpression(vector <Symbol> SynchSet);
    myType SimpleExpressionA(vector <Symbol> SynchSet);
    vector<myType> SimpleExpressionB(vector <Symbol> SynchSet);
    void AddingOperator(vector <Symbol> SynchSet);
    myType Term(vector <Symbol> SynchSet);
    vector<myType> TermA(vector <Symbol> SynchSet);
    myType Factor(vector <Symbol> SynchSet);
    void MultiplyingOperator(vector <Symbol> SynchSet);
    void VariableName(vector <Symbol> SynchSet);
    void VariableNameA(vector <Symbol> SynchSet);
    int IndexedSelector(vector <Symbol> SynchSet);
    int Constant(vector <Symbol> SynchSet);
    int BooleanSymbol(vector <Symbol> SynchSet);
    int Numeral(vector <Symbol> SynchSet);
    void NumeralA(vector <Symbol> SynchSet);
    void ConstantName(vector <Symbol> SynchSet);
    int ProcedureName(vector <Symbol> SynchSet);
    void Name(vector <Symbol> SynchSet);

    void Error();
    void Error(const char funcName[], Symbol expected);
    void Error(const char funcName[], string errMessage);
    void Error(const char funcName[], string errMessage, vector<Symbol> &synchSet);
    void stError(const char funcName[], string errMessage, vector<Symbol> &synchSet);

    void match(Symbol sym);
    void match(Symbol sym, const char funcname[]);

    void addSymbol(vector<Symbol> &symSet, Symbol sym);

	//checks for symbol membership in a given set. Used for checking synchsets
    bool isMember(vector<Symbol> &checkset, Symbol sym);

	void getNextToken();
	string laSymbolName();

    Token *laToken;

    Symbol laSymbol;

    Scanner *scptr;

	Administration *admin;

    bool panic;
    int panicCount, errorCount;
    BlockTable bt;
};

#endif // PARSER_H
