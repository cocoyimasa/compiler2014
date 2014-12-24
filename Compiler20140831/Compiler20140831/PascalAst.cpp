#include "PascalAst.h"
#include "CodeGen.h"
#include "SemanticAnalyzerBase.h"
using namespace compiler;
namespace swd
{
	//semantic analyzer
	void Node::accept(SemanticAnalyzerBase *visitor)
	{
		visitor->visit(this);
	}
	void Statement::accept(SemanticAnalyzerBase *visitor)
	{
		visitor->visit(this);
	}
	void Expression::accept(SemanticAnalyzerBase *visitor)
	{
		visitor->visit(this);
	}
	void AssignStmt::accept(SemanticAnalyzerBase *visitor)
	{
		visitor->visit(this);
	}
	void ComparisonExp::accept(SemanticAnalyzerBase *visitor)
	{
		visitor->visit(this);
	}
	void FuncCall::accept(SemanticAnalyzerBase *visitor)
	{
		visitor->visit(this);
	}
	void IfStmt::accept(SemanticAnalyzerBase *visitor)
	{
		visitor->visit(this);
	}
	void ElseStmt::accept(SemanticAnalyzerBase *visitor)
	{
		visitor->visit(this);
	}
	void WhileStmt::accept(SemanticAnalyzerBase *visitor)
	{
		visitor->visit(this);
	}
	void RepeatStmt::accept(SemanticAnalyzerBase *visitor)
	{
		visitor->visit(this);
	}
	void ForStmt::accept(SemanticAnalyzerBase *visitor)
	{
		visitor->visit(this);
	}
	void CaseStmt::accept(SemanticAnalyzerBase *visitor)
	{
		visitor->visit(this);
	}
	void FunctionStmt::accept(SemanticAnalyzerBase *visitor)
	{
		visitor->visit(this);
	}
	void VariableStmt::accept(SemanticAnalyzerBase *visitor)
	{
		visitor->visit(this);
	}
	void TypeStmt::accept(SemanticAnalyzerBase *visitor)
	{
		visitor->visit(this);
	}
	void ConstantStmt::accept(SemanticAnalyzerBase *visitor)
	{
		visitor->visit(this);
	}
	//Code generator--------------------------------------------------
	void Node::genCode(compiler::IRCodeGen *visitor)
	{
		visitor->genCode(this);
	}
	void Statement::genCode(compiler::IRCodeGen *visitor)
	{
		visitor->genCode(this);
	}
	void Expression::genCode(compiler::IRCodeGen *visitor)
	{
		visitor->genCode(this);
	}
	void AssignStmt::genCode(compiler::IRCodeGen *visitor)
	{
		visitor->genCode(this);
	}
	void ComparisonExp::genCode(compiler::IRCodeGen *visitor)
	{
		visitor->genCode(this);
	}
	void FuncCall::genCode(compiler::IRCodeGen *visitor)
	{
		visitor->genCode(this);
	}
	void IfStmt::genCode(compiler::IRCodeGen *visitor)
	{
		visitor->genCode(this);
	}
	void ElseStmt::genCode(compiler::IRCodeGen *visitor)
	{
		visitor->genCode(this);
	}
	void WhileStmt::genCode(compiler::IRCodeGen *visitor)
	{
		visitor->genCode(this);
	}
	void RepeatStmt::genCode(compiler::IRCodeGen *visitor)
	{
		visitor->genCode(this);
	}
	void ForStmt::genCode(compiler::IRCodeGen *visitor)
	{
		visitor->genCode(this);
	}
	void CaseStmt::genCode(compiler::IRCodeGen *visitor)
	{
		visitor->genCode(this);
	}
	void FunctionStmt::genCode(compiler::IRCodeGen *visitor)
	{
		visitor->genCode(this);
	}
	void VariableStmt::genCode(compiler::IRCodeGen *visitor)
	{
		visitor->genCode(this);
	}
	void TypeStmt::genCode(compiler::IRCodeGen *visitor)
	{
		visitor->genCode(this);
	}
	void ConstantStmt::genCode(compiler::IRCodeGen *visitor)
	{
		visitor->genCode(this);
	}
}