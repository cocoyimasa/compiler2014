#include "CodeGen.h"
namespace swd{

	OperandInfo::OperandInfo(OperandType oprdType, string oprdName, bool ref)
	{
		this->_operandType = oprdType;
		this->_operandName = oprdName;
		this->_isRef = ref;
	}

	void IRCode::initByOperand(OperationType opType,
							   shared_ptr<OperandInfo> op1,
							   shared_ptr<OperandInfo> op2,
							   shared_ptr<OperandInfo> result)
	{
		this->_opType = opType;
		this->_op1 = op1;
		this->_op2 = op2;
		this->_result = result;
	}
	void IRCode::initByString(string Operation, string Op1Str,
							  string Op2Str, string RsltStr)
	{
		this->_Operation = Operation;
		this->_Op1Str = Op1Str;
		this->_Op2Str = Op2Str;
		this->_RsltStr = RsltStr;
	}

	IRCodeGen::IRCodeGen()
	{
		labelId = 0;
	}
	IRCodeGen::~IRCodeGen()
	{
	}
	void IRCodeGen::genCode(Node *node)
	{

	}
	void IRCodeGen::genCode(Statement *node)
	{
	}
	shared_ptr<OperandInfo> IRCodeGen::genCode(Expression *node)
	{
	}
	void IRCodeGen::genCode(AssignStmt *node)
	{
	}
	shared_ptr<OperandInfo> IRCodeGen::genCode(ComparisonExp *node)
	{
	}
	void IRCodeGen::genCode(FuncCall *node)
	{
	}

	void IRCodeGen::genCode(IfStmt *node)
	{
		//表达式
		shared_ptr<OperandInfo> condRes = genCode(node->condition);
		//JNT 表达式结果 NULL _L0
		shared_ptr<IRCode> test1 = make_shared<IRCode>();

		shared_ptr<OperandInfo> test1_result = make_shared<OperandInfo>(OperandType::OpLabel,
																		false,
																		"_L" + (labelId++));

		test1->initByOperand(OperationType::JNT, condRes, NULL, test1_result);
		test1->initByString("JNT", condRes->_operandName, "NULL", "NULL");
		IRCodeFile.push_back(test1);
		//语句
		genCode(node->thenBody);//似乎有错误。多态
		//Else 存在与否
		if (node->haveElse)
		{
			//JMP _L1 NULL NULL
			shared_ptr<IRCode> endIf = make_shared<IRCode>();
			shared_ptr<OperandInfo> endIf_op1 = make_shared<OperandInfo>(OperandType::OpLabel,
																		 false,
																		 "_L" + (labelId++));
			endIf->initByOperand(OperationType::JMP, endIf_op1, NULL, NULL);
			endIf->initByString("JMP", endIf_op1->_operandName, "NULL", "NULL");
			IRCodeFile.push_back(endIf);
			//LABEL _L0 NULL NULL
			shared_ptr<IRCode> label1 = make_shared<IRCode>();
			label1->initByOperand(OperationType::LABEL, test1_result, NULL, NULL);
			label1->initByString("LABEL", test1_result->_operandName, "NULL", "NULL");
			IRCodeFile.push_back(label1);
			//语句
			genCode((static_cast<ElseStmt*>(node->elseBody))->body);
			//LABEL _L1 NULL NULL
			shared_ptr<IRCode> label2 = make_shared<IRCode>();
			label2->initByOperand(OperationType::LABEL, endIf_op1, NULL, NULL);
			label2->initByString("LABEL", endIf_op1->_operandName, "NULL", "NULL");
			IRCodeFile.push_back(label2);
		}
		else
		{
			//LABEL _L0 NULL NULL 
			shared_ptr<IRCode> label1 = make_shared<IRCode>();
			label1->initByOperand(OperationType::LABEL, test1_result, NULL, NULL);
			label1->initByString("LABEL", test1_result->_operandName, "NULL", "NULL");
			IRCodeFile.push_back(label1);
		}
	}

	void IRCodeGen::genCode(ElseStmt *node)
	{
		//暂不用-------
	}
	void IRCodeGen::genCode(WhileStmt *node)
	{
		//Label _L1 NULL NULL
		shared_ptr<IRCode> whileLabel = make_shared<IRCode>();
		shared_ptr<OperandInfo> whileLabel_op1 = make_shared<OperandInfo>(OperandType::OpLabel,
																		  false,
																		  "_L" + (labelId++));
		whileLabel->initByOperand(OperationType::LABEL, whileLabel_op1, NULL, NULL);
		whileLabel->initByString("LABEL", whileLabel_op1->_operandName, "NULL", "NULL");
		IRCodeFile.push_back(whileLabel);
		//表达式翻译
		shared_ptr<OperandInfo> exp_res = genCode(node->condition);
		//JNT Exp结果 NULL _L0
		shared_ptr<IRCode> testCond = make_shared<IRCode>();
		shared_ptr<OperandInfo> testCond_res = make_shared<OperandInfo>(OperandType::OpLabel,
																		false,
																		"_L" + (labelId++));
		testCond->initByOperand(OperationType::JNT, exp_res, NULL, testCond_res);
		testCond->initByString("JNT", exp_res->_operandName, "NULL", testCond_res->_operandName);
		IRCodeFile.push_back(testCond);
		//语句
		genCode(node->body);
		//JMP _L1,NULL,NULL
		shared_ptr<IRCode> nextLoop = make_shared<IRCode>();
		nextLoop->initByOperand(OperationType::JMP, whileLabel_op1, NULL, NULL);
		nextLoop->initByString("JMP", whileLabel_op1->_operandName, "NULL", "NULL");
		IRCodeFile.push_back(nextLoop);
		//LABEL _L0,NULL,NULL
		shared_ptr<IRCode> endLoop = make_shared<IRCode>();
		endLoop->initByOperand(OperationType::LABEL, testCond_res, NULL, NULL);
		nextLoop->initByString("LABEL", testCond_res->_operandName, "NULL", "NULL");
		IRCodeFile.push_back(endLoop);
	}

	void IRCodeGen::genCode(RepeatStmt *node)
	{
		//Label _L0 NULL NULL
		shared_ptr<IRCode> repeatLabel = make_shared<IRCode>();
		shared_ptr<OperandInfo> repeatLabel_op1 = make_shared<OperandInfo>(OperandType::OpLabel,
																		   false,
																		   "_L" + (labelId++));
		repeatLabel->initByOperand(OperationType::LABEL, repeatLabel_op1, NULL, NULL);
		repeatLabel->initByString("LABEL", repeatLabel_op1->_operandName, "NULL", "NULL");
		IRCodeFile.push_back(repeatLabel);
		//语句
		genCode(node->body);
		//表达式翻译
		shared_ptr<OperandInfo> exp_res1 = genCode(node->untilCond);
		//JNT Exp结果 NULL _L0
		shared_ptr<IRCode> testCond = make_shared<IRCode>();
		testCond->initByOperand(OperationType::JNT, exp_res1, NULL, repeatLabel_op1);
		testCond->initByString("JNT", exp_res1->_operandName, "NULL", repeatLabel_op1->_operandName);
		IRCodeFile.push_back(testCond);
		//LABEL _L1,NULL,NULL
		shared_ptr<IRCode> endLoop1 = make_shared<IRCode>();
		shared_ptr<OperandInfo> endLoop_op1 = make_shared<OperandInfo>(OperandType::OpLabel,
																	   false,
																	   "_L" + (labelId++));
		endLoop1->initByOperand(OperationType::LABEL, endLoop_op1, NULL, NULL);
		endLoop1->initByString("LABEL", endLoop_op1->_operandName, "NULL", "NULL");
		IRCodeFile.push_back(endLoop1);

	}

	void IRCodeGen::genCode(ForStmt *node)
	{
		if (node->direction == Tag::TO)
		{
			//表达式1翻译
			shared_ptr<OperandInfo> exp1_res =
				genCode((static_cast<AssignStmt*>(node->startValue))->right);
			//ASSIGN_4 exp1结果 NULL 循环变量
			shared_ptr<IRCode> assignStmt = make_shared<IRCode>();
			shared_ptr<OperandInfo> assignStmt_loopVar = make_shared<OperandInfo>(OperandType::OpVar,
																				  (static_cast<AssignStmt*>(node->startValue))->left->value.value,
																				  true);
			assignStmt->initByOperand(OperationType::ASSIGN_4, exp1_res, NULL, assignStmt_loopVar);
			assignStmt->initByString("ASSIGN_4", exp1_res->_operandName, "NULL", assignStmt_loopVar->_operandName);
			IRCodeFile.push_back(assignStmt);
			//LABEL _L1 NULL NULL
			auto label1 = make_shared<IRCode>();
			auto label1_op1 = make_shared<OperandInfo>(OperandType::OpLabel,
													   "_L" + labelId++,
													   false);
			label1->initByOperand(OperationType::LABEL, label1_op1, NULL, NULL);
			label1->initByString("LABEL", label1_op1->_operandName, "NULL", "NULL");
			IRCodeFile.push_back(label1);
			//表达式2翻译
			shared_ptr<OperandInfo> exp2_res =
				genCode((static_cast<ComparisonExp*>(node->endValue))->right);
			//LE_4 循环变量 exp2结果 _T
			auto leStmt = make_shared<IRCode>();
			auto leStmt_res1 = make_shared<OperandInfo>(OperandType::OpVar,
														  "_T" + labelId,
														  false);
			leStmt->initByOperand(OperationType::LE_4, assignStmt_loopVar, exp2_res, leStmt_res1);
			leStmt->initByString("LE_4", assignStmt_loopVar->_operandName,
								 exp2_res->_operandName, leStmt_res1->_operandName);
			IRCodeFile.push_back(leStmt);
			//JNT _T NULL _L0
			auto test1 = make_shared<IRCode>();
			auto test1_op1 = make_shared<OperandInfo>(OperandType::OpLabel,
													  "_L" + labelId++,
													  false);
			test1->initByOperand(OperationType::JNT, leStmt_res1, NULL, test1_op1);
			test1->initByString("JNT", leStmt_res1->_operandName, "NULL", test1_op1->_operandName);
			IRCodeFile.push_back(test1);
			//语句
			genCode(node->body);
			//LABEL _L2 NULL NULL
			auto label2 = make_shared<IRCode>();
			auto label2_op1 = make_shared<OperandInfo>(OperandType::OpLabel,
													   "_L" + labelId++,
													   false);
			label2->initByOperand(OperationType::LABEL, label2_op1, NULL, NULL);
			label2->initByString("LABEL", label2_op1->_operandName, "NULL", "NULL");
			IRCodeFile.push_back(label2);
			//ADD_4 循环变量 1 循环变量
			auto addStmt = make_shared<IRCode>();
			auto addStmt_op2 = make_shared<OperandInfo>(OperandType::OpConst,
														"1",
														false);
			addStmt->initByOperand(OperationType::ADD_4, assignStmt_loopVar, addStmt_op2, assignStmt_loopVar);
			addStmt->initByString("ADD_4", assignStmt_loopVar->_operandName,
								  addStmt_op2->_operandName, assignStmt_loopVar->_operandName);
			IRCodeFile.push_back(addStmt);
			//JMP _L1 NULL NULL
			auto jmpStmt = make_shared<IRCode>();
			jmpStmt->initByOperand(OperationType::JMP, label1_op1, NULL, NULL);
			jmpStmt->initByString("JMP", label1_op1->_operandName, "NULL", "NULL");
			IRCodeFile.push_back(jmpStmt);
			//LABEL _L0 NULL NULL
			auto label0 = make_shared<IRCode>();
			label0->initByOperand(OperationType::LABEL, test1_op1, NULL, NULL);
			label0->initByString("LABEL", test1_op1->_operandName, "NULL", "NULL");
			IRCodeFile.push_back(label0);
		}
		else
		{

		}
	}

	void IRCodeGen::genCode(CaseStmt *node)
	{
		vector<shared_ptr<IRCode>> tests;
		//表达式
		shared_ptr<OperandInfo> exp1 = genCode(node->condition);
		//JE 表达式结果 常量 _Ln
#define GEN_POINT_NAME(Name) je##Name
#define GEN_JMP_NAME(Name) jmp##Name
#define GEN_CONST_NAME(Name) je_const##Name
#define GEN_LABEL_NAME(Name) je_label##Name
		int i = 0;
		for (auto item : node->body)
		{
			auto GEN_POINT_NAME(i) = make_shared<IRCode>();
			auto GEN_CONST_NAME(i) = make_shared<OperandInfo>(OperandType::OpConst,
															  item->list.at(0)->value,
															  true);
			auto GEN_LABEL_NAME(i) = make_shared<OperandInfo>(OperandType::OpLabel,
															  "_L" + labelId++,
															  false);
			GEN_POINT_NAME(i)->initByOperand(OperationType::JE_4, exp1, GEN_CONST_NAME(i), GEN_LABEL_NAME(i));
			GEN_POINT_NAME(i)->initByString("JE_4", exp1->_operandName, GEN_CONST_NAME(i)->_operandName,
											GEN_LABEL_NAME(i)->_operandName);
			IRCodeFile.push_back(GEN_POINT_NAME(i));
			tests.push_back(GEN_POINT_NAME(i));
			i++;
		}
		//JMP _L0 NULL NULL
		auto jmpStmt = make_shared<IRCode>();
		auto jmpStmt_label0 = make_shared<OperandInfo>(OperandType::OpLabel,
													   "_L" + labelId++,
													   false);
		jmpStmt->initByOperand(OperationType::JMP, jmpStmt_label0, NULL, NULL);
		jmpStmt->initByString("JMP", jmpStmt_label0->_operandName, "NULL", "NULL");
		//LOOP------------------------
		int j = 0;
		for (auto item : node->body)
		{
			//LABEL _L1 NULL NULL -> LABEL _Ln NULL NULL
			auto GEN_POINT_NAME(i) = make_shared<IRCode>();
			GEN_POINT_NAME(i)->initByOperand(OperationType::LABEL, tests[j]->_result, NULL, NULL);
			GEN_POINT_NAME(i)->initByString("LABEL", tests[j]->_result->_operandName, "NULL", "NULL");
			IRCodeFile.push_back(GEN_POINT_NAME(i));
			j++;
			i++;
			//语句1
			genCode(static_cast<Statement*>(item->list.at(0)));
			//JMP _L0 NULL NULL
			auto GEN_JMP_NAME(i) = make_shared<IRCode>();
			GEN_JMP_NAME(i)->initByOperand(OperationType::JMP, jmpStmt_label0, NULL, NULL);
			GEN_JMP_NAME(i)->initByString("JMP", jmpStmt_label0->_operandName, "NULL", "NULL");
			IRCodeFile.push_back(GEN_JMP_NAME(i));
		}
		//----------------------------
	}

	void IRCodeGen::genCode(FunctionStmt *node)
	{
	}

	void IRCodeGen::genCode(VariableStmt *node)
	{
	}

	void IRCodeGen::genCode(TypeStmt *node)
	{
	}

	void IRCodeGen::genCode(ConstantStmt *node)
	{
	}
}