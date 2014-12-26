#pragma once
#include "SemanticAnalyzerBase.h"

namespace swd
{
	class SymbolTable;
	class Error;
	class SemanticAnalyzer :public SemanticAnalyzerBase
	{
	public:
		SymbolTable *symTable;
		SymbolTable *currentTable;
		std::vector<Error> errList;
		SemanticAnalyzer();
		~SemanticAnalyzer();
		void visit(Node *node);
		void visit(Statement* node);
		void visit(Expression* node);
		void visit(ComparisonExp* node);
		void visit(AssignStmt* node);
		void visit(FuncCall* node);
		void visit(IfStmt* node);
		void visit(ElseStmt* node);
		void visit(WhileStmt* node);
		void visit(RepeatStmt* node);
		void visit(ForStmt* node);
		void visit(CaseStmt* node);
		void visit(FunctionStmt* node);
		void visit(VariableStmt* node);
		void visit(TypeStmt* node);
		void visit(ConstantStmt* node);
	};
}

