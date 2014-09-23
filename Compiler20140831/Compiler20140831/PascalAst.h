#pragma once
#include "lexer.h"
#include "SymbolTable.h"
#include "CodeGen.h"
namespace swd{
	class Node
	{
	public:
		Token value;
		Node *parent;
		std::vector<Node*> list;
		Node(){}
		virtual bool operator=(Node &node);
		virtual void addNode(Node *node)
		{
			list.push_back(node);
		}
		virtual void print();
		virtual ~Node(){}
		virtual void genCode(IRCodeGen *visitor);
	};

	class Statement :public Node
	{
	public:
		void print();
		void genCode(IRCodeGen *visitor);
	};

	class Expression :public Node
	{
	public:
		void print();
		void genCode(IRCodeGen *visitor);
	};
	class AssignStmt :public Statement
	{
	public:
		Expression *left;
		Expression *right;
		void print();
		void genCode(IRCodeGen *visitor);
	};

	class ComparisonExp :public Expression
	{
	public:
		Expression *left;
		Expression *right;
		Token op;
		void print();
		void genCode(IRCodeGen *visitor);
	};

	class FuncCall :public Statement
	{
	public:
		std::string funcName;
		std::vector<Token> constants;
		void genCode(IRCodeGen *visitor);
	};

	class IfStmt :public Statement
	{
	public:
		Expression* condition;
		Statement* thenBody;
		Statement* elseBody;
		bool haveElse;
		void print();
		void genCode(IRCodeGen *visitor);
	};

	class ElseStmt :public Statement
	{
	public:
		Statement* body;
		void genCode(IRCodeGen *visitor);
	};

	class WhileStmt :public Statement
	{
	public:
		Expression* condition;
		Statement* body;
		void print();
		void genCode(IRCodeGen *visitor);
	};

	class RepeatStmt :public Statement
	{
	public:
		Expression* untilCond;
		Statement* body;
		void print();
		void genCode(IRCodeGen *visitor);
	};

	class ForStmt :public Statement
	{
	public:
		Statement *startValue;
		Tag direction;
		Expression *endValue;
		Statement *nextValue;
		Statement* body;
		void print();
		void genCode(IRCodeGen *visitor);
	};

	class CaseStmt :public Statement
	{
	public:
		Expression *condition;
		vector<Statement*> body;
		void print();
		void genCode(IRCodeGen *visitor);
	};

	class FunctionStmt :public Statement
	{
	public:
		std::string funcName;
		std::vector<VariableStmt*> arguments;
		//std::vector<VariableStmt*> decls;//»•nodelist¿Ô’“∞…
		Token retType;
		Statement *body;
		void print();
		void genCode(IRCodeGen *visitor);
	};

	class VariableStmt :public Statement
	{
	public:
		VariableDecl *varDeclare;
		void genCode(IRCodeGen *visitor);
	};

	class TypeStmt :public Statement
	{
	public:
		TypeDecl *typeDeclare;
		std::vector<VariableStmt*> varStmts;
		void genCode(IRCodeGen *visitor);
	};

	class ConstantStmt :public Statement
	{
	public:
		ConstantDecl *constDeclare;
		void genCode(IRCodeGen *visitor);
	};
}