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
		
		shared_ptr<Node> root;
	protected:
		shared_ptr<Node> currNode;
		Lexer *lex;
	public:
		Parser(Lexer *l)
		{
			lex = l;
			it = lex->tokenStream.begin();
		}
		bool find(Tag t);
		bool advance(Tag t);
		bool match(Tag t);
		void parseProgram();
		void parseBlock();
		shared_ptr<VariableDecl> parseVariable();
		shared_ptr<VariableStmt> parseVariables();
		shared_ptr<TypeStmt> parseType();
		shared_ptr<ConstantStmt> parseConstant();
		shared_ptr<AssignStmt> parseAssign();
		shared_ptr<ComparisonExp> parseComparison();
		shared_ptr<Expression> parseExpression();
		shared_ptr<Expression> parseExpression1();
		shared_ptr<Expression> parseTerm();
		shared_ptr<Expression> parseFactor();
		shared_ptr<Statement> parseStmtList(Tag teminator);
		shared_ptr<Statement> parseStatement();
		shared_ptr<Statement> parseIf();
		shared_ptr<Statement> parseElse();
		shared_ptr<Statement> parseWhile();
		shared_ptr<Statement> parseRepeat();
		shared_ptr<Statement> parseCase();
		shared_ptr<Statement> parseCaseBranch();
		shared_ptr<Statement> parseFor();
		shared_ptr<Expression> parseCaseConst();
		shared_ptr<FuncCall> parseFunctionCall();
		shared_ptr<Statement> parseFunction();
		shared_ptr<Statement> parseProcedure();
		shared_ptr<Statement> parseProcedureBase(bool isFunction);
	};
}