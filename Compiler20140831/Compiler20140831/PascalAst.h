#pragma once
#include "lexer.h"
namespace compiler
{
	class IRCodeGen;
}
namespace swd{
	class SemanticAnalyzerBase;
	class Declaration;
	class ConstantDecl;
	class VariableDecl;
	class TypeDecl;
	class FunctionDecl;

	class Node
	{
	public:
		Token value;
		shared_ptr<Node> parent;
		std::vector<shared_ptr<Node>> list;
		Node(){}
		virtual bool operator=(Node &node);
		virtual void addNode(shared_ptr<Node> node)
		{
			list.push_back(node);
		}
		virtual void print();
		virtual ~Node(){}
		virtual void accept(SemanticAnalyzerBase *visitor);
		virtual void genCode(compiler::IRCodeGen *visitor);
	};

	class Statement :public Node
	{
	public:
		void print();
		void accept(SemanticAnalyzerBase *visitor);
		void genCode(compiler::IRCodeGen *visitor);
	};

	class Expression :public Node
	{
	public:
		void print();
		void accept(SemanticAnalyzerBase *visitor);
		void genCode(compiler::IRCodeGen *visitor);
	};
	class AssignStmt :public Statement
	{
	public:
		shared_ptr<Expression> left;
		shared_ptr<Expression> right;
		void print();
		void accept(SemanticAnalyzerBase *visitor);
		void genCode(compiler::IRCodeGen *visitor);
	};

	class ComparisonExp :public Expression
	{
	public:
		shared_ptr<Expression> left;
		shared_ptr<Expression> right;
		Token op;
		void print();
		void accept(SemanticAnalyzerBase *visitor);
		void genCode(compiler::IRCodeGen *visitor);
	};

	class FuncCall :public Statement
	{
	public:
		std::string funcName;
		std::vector<Token> constants;
		void print();
		void accept(SemanticAnalyzerBase *visitor);
		void genCode(compiler::IRCodeGen *visitor);
	};

	class IfStmt :public Statement
	{
	public:
		shared_ptr<Expression> condition;
		shared_ptr<Statement> thenBody;
		shared_ptr<Statement> elseBody;
		bool haveElse;
		void print();
		void accept(SemanticAnalyzerBase *visitor);
		void genCode(compiler::IRCodeGen *visitor);
	};

	class ElseStmt :public Statement
	{
	public:
		shared_ptr<Statement> body;
		void accept(SemanticAnalyzerBase *visitor);
		void genCode(compiler::IRCodeGen *visitor);
	};

	class WhileStmt :public Statement
	{
	public:
		shared_ptr<Expression> condition;
		shared_ptr<Statement> body;
		void print();
		void accept(SemanticAnalyzerBase *visitor);
		void genCode(compiler::IRCodeGen *visitor);
	};

	class RepeatStmt :public Statement
	{
	public:
		shared_ptr<Expression> untilCond;
		shared_ptr<Statement> body;
		void print();
		void accept(SemanticAnalyzerBase *visitor);
		void genCode(compiler::IRCodeGen *visitor);
	};

	class ForStmt :public Statement
	{
	public:
		shared_ptr<Statement> startStmt;
		Tag direction;
		shared_ptr<Expression> endValue;
		//shared_ptr<Statement> nextValue;
		shared_ptr<Statement> body;
		void print();
		void accept(SemanticAnalyzerBase *visitor);
		void genCode(compiler::IRCodeGen *visitor);
	};

	class CaseStmt :public Statement
	{
	public:
		shared_ptr<Expression> condition;
		vector<shared_ptr<Statement> > body;
		void print();
		void accept(SemanticAnalyzerBase *visitor);
		void genCode(compiler::IRCodeGen *visitor);
	};

	class VariableStmt :public Statement
	{
	public:
		shared_ptr<VariableDecl> varDeclare;
		vector<shared_ptr<VariableStmt>> varRoot;
		void print();
		void accept(SemanticAnalyzerBase *visitor);
		void genCode(compiler::IRCodeGen *visitor);
	};

	class FunctionStmt :public Statement
	{
	public:
		shared_ptr<FunctionDecl> funcDecl;
		//std::vector<VariableStmt*> decls;//»•nodelist¿Ô’“∞…
		shared_ptr<Statement> body;
		//shared_ptr<Statement> returnVal;
		void print();
		void accept(SemanticAnalyzerBase *visitor);
		void genCode(compiler::IRCodeGen *visitor);
	};

	class TypeStmt :public Statement
	{
	public:
		shared_ptr<TypeDecl> typeDeclare;
		std::vector<shared_ptr<VariableStmt> > varStmts;
		std::vector<shared_ptr<TypeStmt> > typeRoot;
		void print();
		void accept(SemanticAnalyzerBase *visitor);
		void genCode(compiler::IRCodeGen *visitor);
	};

	class ConstantStmt :public Statement
	{
	public:
		shared_ptr<ConstantDecl> constDeclare;
		vector<shared_ptr<ConstantStmt>> constRoot;
		void print();
		void accept(SemanticAnalyzerBase *visitor);
		void genCode(compiler::IRCodeGen *visitor);
	};
}