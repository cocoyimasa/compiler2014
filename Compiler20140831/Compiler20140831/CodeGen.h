#pragma once
#include "PascalAst.h"
using namespace swd;
namespace compiler{
	//enum class OperandType
	//{
	//	OpConst,OpVar,OpPtr,OpLabel,OpNone,OpProc
	//};
	
	enum OperationType
	{
		MOV,ADD,SUB,MUL,DIV,
		CMP,
		JMP,JMPF,JE,JMPT,
		STORE,LOAD,
		PUSH,POP,
		EQ, UNEQ,
		LT,GT,
		LE,GE,
		AND,OR,
		LABEL,FUNC,PARAM,RET,CALL,
		IConst,FConst
	};
	class CodeGenInfo
	{
	public:
		string labelNext;
		string labelEnd;
	};
	class IRCode
	{
	public:
		OperationType _opType;
		string _op1, _op2; //目标代码生成时使用
		int Count;
		IRCode()
		{
			Count = 0;
			_op1 = "";
			_op2 = "";
		}
		IRCode(OperationType op, string op1, string op2)
		{
			_opType = op;
			_op1 = op1;
			_op2 = op2;
			Count = 2;
		}
		IRCode(OperationType op, string op1)
		{
			_opType = op;
			_op1 = op1;
			_op2 = "";
			Count = 1;
		}
		IRCode(OperationType op)
		{
			_opType = op;
			_op1 = "";
			_op2 = "";
			Count = 0;
		}
		void checkCount();
		string toString();
	};

	OperationType convertTag(Tag t);
	bool isOp(Tag t);


	class IRCodeGen
	{
	public:
		int id;
		map<OperationType, string> opStringDict;
		vector<shared_ptr<IRCode>> IRCodeFile;
		IRCodeGen();
		~IRCodeGen();
		void genCode(Node* node);
		void genCode(Statement* node);
		void genCode(Expression* node);
		void genCode(ComparisonExp* node);//
		void genCode(AssignStmt* node);
		void genCode(FuncCall* node);
		void genCode(IfStmt* node);
		void genCode(ElseStmt* node);//
		void genCode(WhileStmt* node);
		void genCode(RepeatStmt* node);
		void genCode(ForStmt* node);
		void genCode(CaseStmt* node);
		void genCode(FunctionStmt* node);
		void genCode(FunctionDecl* node);
		void genCode(VariableStmt* node);
		void genCode(TypeStmt* node);
		void genCode(ConstantStmt* node);
	};
}
