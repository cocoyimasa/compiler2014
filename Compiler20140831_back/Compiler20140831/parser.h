#pragma once
#include "SymbolTable.h"
#include "PascalAst.h"
namespace swd
{
	class Error
	{
	public:
		std::string errorStr;
		Token errToken;
		int errLine;
		Error(){}
		Error(std::string err, Token t)
		{
			errorStr = err;
			errToken = t;
			errLine = t.line;
		}
		void errorPrint();
		~Error(){}
	};

	class Parser
	{
	public:
		std::vector<Token>::iterator it;
		std::vector<Error> errList;
		//Maybe there should be a attribute called SCOPE
		//--to be implemented
		SymbolTableStack *symStack;
		Node* root;
	protected:
		Node* currNode;
		Lexer *lex;
	public:
		Parser(Lexer *l)
		{
			lex = l;
			it = lex->tokenStream.begin();
			/*root = new Node;
			currNode = root;*/
		}
		bool advance(Tag t);
		bool match(Tag t);
		void parseProgram();
		void parseBlock();
		VariableDecl* parseVariable();
		void parseVariables();
		void parseType();
		void parseConstant();
		AssignStmt* parseAssign();
		ComparisonExp* parseComparison();
		Expression* parseExpression();
		Expression* parseExpression1();
		Expression* parseTerm();
		Expression* parseFactor();
		Statement* parseStmtList(Tag teminator);
		Statement* parseStatement();
		Statement* parseIf();
		Statement* parseElse();
		Statement* parseWhile();
		Statement* parseRepeat();
		Statement* parseCase();
		Statement* parseCaseBranch();
		Statement* parseFor();
		Expression* parseCaseConst();
		Statement* parseFunctionCall();
		Statement* parseFunction();
		Statement* parseProcedure();
		Statement* parseProcedureBase(bool isFunction);
	};
}