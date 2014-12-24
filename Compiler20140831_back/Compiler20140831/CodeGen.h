#pragma once
#include "PascalAst.h"
namespace swd{
	enum class OperandType
	{
		OpConst,OpVar,OpPtr,OpLabel,OpNone,OpProc
	};
	enum class OperationType
	{
		MOV,ADD,SUB,MUL,DIV,

		EQU, EQU_1, EQU_S, EQU_A, EQU_4, 
		EQU_8, EQU_8F, EQU_4F, EQU_2, EQU_32,

		NEQU, NEQU_1, NEQU_S, NEQU_4, NEQU_8,
		NEQU_8F, NEQU_4F, NEQU_2, NEQU_32, NEQU_A,

		LS, LS_1, LS_S, LS_4, LS_8, LS_8F,
		LS_4F, LS_2, LS_1U, LS_2U, LS_4U,

		MR,	MR_1, MR_S, MR_4, MR_8, MR_8F,
		MR_4F, MR_2, MR_1U, MR_2U, MR_4U,

		LE, LE_1, LE_S, LE_4, LE_8, LE_8F,
		LE_4F, LE_2, LE_1U, LE_2U, LE_4U,

		ME,
		ME_1,
		ME_S,
		ME_4,
		ME_8,
		ME_8F,
		ME_4F,
		ME_2,
		ME_1U,
		ME_2U,
		ME_4U,

		IN1,
		IN_C,
		IN_I,
		IN_B,
		IN_SH,
		IN_SM,
		IN_W,
		IN_LW,
		IN_E,

		SUBI,
		SUB_4,
		SUB_8F,
		SUB_8 ,
		SUB_SET,
		SUB_2,

		ADDI,
		ADD_S,
		ADD_4,
		ADD_8F,
		ADD_SET,
		ADD_2,

		OR,
		OR_0,
		OR_4,
		OR_1,
		OR_2,

		XOR,
		XOR_0,
		XOR_4,
		XOR_1,
		XOR_2,

		MULI,
		MUL_4,
		MUL_8F,
		MUL_SET,

		DIVI,
		DIV_8F,
		DIV_4,

		MOD,
		MOD_4,

		SHR,
		SHR_4,
		SHR_1U,
		SHR_1,
		SHR_2,
		SHR_2U,
		SHR_4U,

		AND,
		AND_0,
		AND_4,
		AND_1,
		AND_2,

		SHL,
		SHL_4,
		SHL_1U,
		SHL_1,
		SHL_2,
		SHL_2U,
		SHL_4U,

		NEG,
		NEG_4,
		NEG_1,
		NEG_2,
		NEG_8F,
		NEG_4F,

		NOT,
		NOT_0,
		NOT_4,
		NOT_1,
		NOT_2,

		ASSIGN,
		ASSIGN_1,
		ASSIGN_S,
		ASSIGN_4,
		ASSIGN_2,
		ASSIGN_8,
		ASSIGN_N,
		ASSIGN_32,

		JMP,
		LABEL ,
		PARA,
		RETV,
		CALL,
		JT,
		JNT,
		JE_1,
		JE_2,
		JE_4,
		CALLPTR,
		ASM ,
		GETADDR,

		SET_ADDITEM ,
		GETPROCADDR,

		None = 0,
		ChrToStr = -1,

		ByteToInt = -2,
		ShortToInt = -3,
		SmallToInt = -4,
		WordToInt = -5,
		LongToInt = -27,

		IntToReal = -6,
		ByteToReal = -11,
		ShortToReal = -14,
		SmallToReal = -16,
		WordToReal = -19,
		LongToReal = -21,
		SingleToReal = -23,

		IntToSingle = -7,
		ByteToSingle = -12,
		ShortToSingle = -15,
		SmallToSingle = -17,
		WordToSingle = -20,
		LongToSingle = -22,
		RealToSingle = -41,

		ByteToSmall = -8,
		ShortToSmall = -13,
		IntToSmall = -34,
		LongToSmall = -39,

		ByteToWord = -9,
		ShortToWord = -25,
		IntToWord = -35,
		LongToWord = -40,

		ByteToLong = -10,
		WordToLong = -18,
		ShortToLong = -26,
		SmallToLong = -28,

		ByteToShort = -24,
		IntToShort = -29,
		SmallToShort = -30,
		WordToShort = -31,
		LongToShort = -32,

		IntToByte = -33,
		SmallToByte = -36,
		WordToByte = -37,
		LongToByte = -38,

		ByteToLong8 = -42,
		ShortToLong8 = -43,
		SmallToLong8 = -44,
		IntToLong8 = -45,
		WordToLong8 = -46,
		LongToLong8 = -47,

		TypeCast = -50
	};
	class OperandInfo
	{
	public:
		OperandInfo(OperandType oprdType, string oprdName, bool ref);
		OperandType _operandType;
		string _operandName;
		bool _isRef;
	};
	class IRCode
	{
	public:
		OperationType _opType;
		shared_ptr<OperandInfo> _op1, _op2, _result;
		string _Operation, _Op1Str, _Op2Str, _RsltStr; //目标代码生成时使用
		void initByOperand(OperationType opType,
							shared_ptr<OperandInfo> op1,
							shared_ptr<OperandInfo> op2, 
							shared_ptr<OperandInfo> result);
		void initByString(string Operation, string Op1Str, 
						   string Op2Str, string RsltStr);
	};
	class IRCodeGen
	{
	public:
		vector<shared_ptr<IRCode>> IRCodeFile;
		int labelId;
		IRCodeGen();
		~IRCodeGen();
		void genCode(Node *node);
		void genCode(Statement *node);
		shared_ptr<OperandInfo> genCode(Expression *node);
		void genCode(AssignStmt *node);
		shared_ptr<OperandInfo> genCode(ComparisonExp *node);
		void genCode(FuncCall *node);
		void genCode(IfStmt *node);
		void genCode(ElseStmt *node);
		void genCode(WhileStmt *node);
		void genCode(RepeatStmt *node);
		void genCode(ForStmt *node);
		void genCode(CaseStmt *node);
		void genCode(FunctionStmt *node);
		void genCode(VariableStmt *node);
		void genCode(TypeStmt *node);
		void genCode(ConstantStmt *node);
	};
}
