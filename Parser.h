#ifndef PARSER_H
#define PARSER_H

#include "Symbol.h"
#include "Scanner.h"
#include "Administration.h"

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
    void Block(vector <Symbol> SynchSet);
    void DefinitionPart(vector <Symbol> SynchSet);
    void StatementPart(vector <Symbol> SynchSet);
    void Definition(vector <Symbol> SynchSet);
    void ConstantDefinition(vector <Symbol> SynchSet);
    void VariableDefinition(vector <Symbol> SynchSet);
    void VariableDefinitionA(vector <Symbol> SynchSet);
    void TypeSymbol(vector <Symbol> SynchSet);
    void VariableList(vector <Symbol> SynchSet);
    void VariableListA(vector <Symbol> SynchSet);
    void ProcedureDefinition(vector <Symbol> SynchSet);
    void Statement(vector <Symbol> SynchSet);
    void EmptyStatement(vector <Symbol> SynchSet);
    void ReadStatement(vector <Symbol> SynchSet);
    void WriteStatement(vector <Symbol> SynchSet);
    void AssignmentStatement(vector <Symbol> SynchSet);
    void ProcedureStatement(vector <Symbol> SynchSet);
    void IfStatement(vector <Symbol> SynchSet);
    void DoStatement(vector <Symbol> SynchSet);
    void VariableAccessList(vector <Symbol> SynchSet);
    void VariableAccessListA(vector <Symbol> SynchSet);
    void VariableAccess(vector <Symbol> SynchSet);
    void ExpressionList(vector <Symbol> SynchSet);
    void ExpressionListA(vector <Symbol> SynchSet);
    void GuardedCommand(vector <Symbol> SynchSet);
    void GuardedCommmandList(vector <Symbol> SynchSet);
    void GuardedCommmandListA(vector <Symbol> SynchSet);
    void Expression(vector <Symbol> SynchSet);
    void ExpressionA(vector <Symbol> SynchSet);
    void PrimaryOperator(vector <Symbol> SynchSet);
    void PrimaryExpression(vector <Symbol> SynchSet);
    void PrimaryExpressionA(vector <Symbol> SynchSet);
    void RelationalOperator(vector <Symbol> SynchSet);
    void SimpleExpression(vector <Symbol> SynchSet);
    void SimpleExpressionA(vector <Symbol> SynchSet);
    void SimpleExpressionB(vector <Symbol> SynchSet);
    void AddingOperator(vector <Symbol> SynchSet);
    void Term(vector <Symbol> SynchSet);
    void TermA(vector <Symbol> SynchSet);
    void Factor(vector <Symbol> SynchSet);
    void MultiplyingOperator(vector <Symbol> SynchSet);
    void VariableName(vector <Symbol> SynchSet);
    void VariableNameA(vector <Symbol> SynchSet);
    void IndexedSelector(vector <Symbol> SynchSet);
    void Constant(vector <Symbol> SynchSet);
    void BooleanSymbol(vector <Symbol> SynchSet);
    void Numeral(vector <Symbol> SynchSet);
    void NumeralA(vector <Symbol> SynchSet);
    void ConstantName(vector <Symbol> SynchSet);
    void ProcedureName(vector <Symbol> SynchSet);
    void Name(vector <Symbol> SynchSet);

    void Error();
    void Error(const char funcName[], Symbol expected);
    void Error(const char funcName[], string errMessage);
    void Error(const char funcName[], string errMessage, vector<Symbol> &synchSet);


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
};

#endif // PARSER_H
