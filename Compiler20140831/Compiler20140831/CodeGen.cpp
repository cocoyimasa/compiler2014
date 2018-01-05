#include "CodeGen.h"
#include "Declaration.h"
/*
*@time 2014-09-10~09-31
*@author wanghesai
*All rights reserved
*/
#define RESERVE_OP(op,val)  opStringDict.insert(std::pair<OperationType, string>(op, val));
namespace compiler{
	///////data///////
	std::string values[] = {
		"mov", "add", "sub", "mul", "div",
		"cmp", "jmp", "jmpf", "je", "jmpt",
		"store", "load", "push", "pop",
		"eq","uneq","lt", "gt", "le", "ge", "and", "or",
		"label", "func", "param", "ret", "call",
		"iconst", "fconst"
	};
	OperationType ops[] = {
		MOV, ADD, SUB, MUL, DIV,
		CMP,JMP, JMPF, JE, JMPT,
		STORE, LOAD,PUSH, POP,
		EQ,UNEQ,LT, GT,	LE, GE,	AND, OR,
		LABEL, FUNC,PARAM,RET,CALL,
		IConst,FConst
	};
	Tag tags[] = {
		Tag::EQ, 
		Tag::UNEQ, 
		Tag::LT, 
		Tag::GT,
		Tag::LE, 
		Tag::GE, 
		Tag::AND, 
		Tag::OR,
		Tag::ADD, 
		Tag::SUB, 
		Tag::MUL, 
		Tag::DIV,
		Tag::EQ, 
		Tag::UNEQ, 
		Tag::LT, 
		Tag::GT,
		Tag::LE, 
		Tag::GE, 
		Tag::AND, 
		Tag::OR,
	};
	Tag logicTags[] = {
		Tag::EQ   ,
		Tag::UNEQ ,
		Tag::LT	  ,
		Tag::GT	  ,
		Tag::LE	  ,
		Tag::GE	  ,
		Tag::AND  ,
		Tag::OR	  ,
		Tag::ADD  ,
		Tag::SUB  ,
		Tag::MUL  ,
		Tag::DIV
	};
	OperationType logicOps[] = {
		OperationType::EQ    ,
		OperationType::UNEQ	 ,
		OperationType::LE	 ,
		OperationType::GT	 ,
		OperationType::LE	 ,
		OperationType::GE	 ,
		OperationType::AND	 ,
		OperationType::OR	 ,
		OperationType::ADD	 ,
		OperationType::SUB	 ,
		OperationType::MUL	 ,
		OperationType::DIV	 ,
		OperationType::ADD
	};
	///////data///////
	void IRCode::checkCount()
	{
		if (_op1 == ""&&_op2 == "")
		{
			Count = 0;
		}
		else if (_op2 == "")
		{
			Count = 1;
		}
		else
		{
			Count = 2;
		}
	}
	string IRCode::toString()
	{
		IRCodeGen tmp;
		checkCount();
		switch (Count)
		{
		case 0:
		return tmp.opStringDict[this->_opType];
		case 1:
		return tmp.opStringDict[this->_opType] + " " + _op1;
		case 2:
		return tmp.opStringDict[this->_opType] + " " + _op1 + " " + _op2;
		}
		return "";
	}
	
	IRCodeGen::IRCodeGen()
	{
		id = 0;
		
		for (int i = 0; i < sizeof(ops) / sizeof(ops[0]); i++)//计算数组长度
		{
			RESERVE_OP(ops[i], values[i])
		}
	}

	IRCodeGen::~IRCodeGen()
	{
	}

	void IRCodeGen::genCode(Node* node)
	{
		auto expRoot = make_shared<IRCode>(OperationType::LABEL,"start");
		IRCodeFile.push_back(expRoot);
		for (auto item : node->list)
		{
			item->genCode(this);
		}
	}

	void IRCodeGen::genCode(Statement* node)
	{
		if (node->value.value == "__Main__")
		{
			auto _Main_ = make_shared<IRCode>(OperationType::LABEL, "__Main__");
			IRCodeFile.push_back(_Main_);
		}
		for (auto item : node->list)
		{
			item->genCode(this);
		}
	}

	OperationType convertTag(Tag t)
	{
		auto findTag = [=](Tag t) {
			for (int i = 0;i < 12;i++) {
				if (t == logicTags[i]) {
					return i;
				}
			}
			return -1;
		};

		int id = findTag(t);
		if (id != -1) {
			return logicOps[id];
		}
		else {
			return OperationType::ADD;
		}
	}

	bool isOp(Tag t)
	{	
		for (auto i : tags)
		{
			if (t == i)
			{
				return true;
			}
		}
		return false;
	}

	void IRCodeGen::genCode(Expression* node)
	{
		//表达式树根节点为运算符或关系符 且子节点非空
		if (isOp(node->value.tag) && node->list.size() != 0)
		{
			auto expRoot = make_shared<IRCode>();
			genCode(static_pointer_cast<Expression>(node->list.at(0)).get());
			//必须做类型检查方可生成代码
			genCode(static_pointer_cast<Expression>(node->list.at(1)).get());
			expRoot->_opType = convertTag(node->value.tag);
			IRCodeFile.push_back(expRoot);
		}
		else if (node->list.size() == 0)
		{
			if (node->value.tag != Tag::IDENT)
			{
				auto child = node->value.value;
				auto pushVar = make_shared<IRCode>(OperationType::PUSH, child);
				IRCodeFile.push_back(pushVar);
			}
			else
			{
				//load a == push [a] 将a的内容入栈
				auto child = node->value.value;
				auto pushVar = make_shared<IRCode>(OperationType::LOAD, child);
				IRCodeFile.push_back(pushVar);
			}
		}
	}

	void IRCodeGen::genCode(AssignStmt* node)
	{
		//STORE A
		genCode(node->right.get());
		auto assStmt = make_shared<IRCode>(OperationType::STORE, node->left->value.value);
		IRCodeFile.push_back(assStmt);
	}
	//abandoned
	void IRCodeGen::genCode(ComparisonExp* node)
	{
		//表达式树根节点为运算符或关系符 且子节点非空
		if (isOp(node->value.tag) && node->list.size() != 0)
		{
			genCode(static_pointer_cast<Expression>(node->list.at(0)).get());
			//必须做类型检查方可生成代码
			genCode(static_pointer_cast<Expression>(node->list.at(1)).get());
			auto expRoot = make_shared<IRCode>(convertTag(node->op.tag));
			IRCodeFile.push_back(expRoot);
		}
		else if (node->list.size() == 0)
		{
			if (node->value.tag != Tag::IDENT)
			{
				auto child = node->value.value;
				auto pushVar = make_shared<IRCode>(OperationType::PUSH, child);
				IRCodeFile.push_back(pushVar);
			}
			else
			{
				//load a == push [a] 将a的内容入栈
				auto child = node->value.value;
				auto pushVar = make_shared<IRCode>(OperationType::LOAD, child);
				IRCodeFile.push_back(pushVar);
			}
		}
	}

	void IRCodeGen::genCode(FuncCall* node)
	{
		for (auto item : node->constants)
		{
			if (item.tag == Tag::IDENT)
			{
				//应该是查找符号表，此处暂时用LOAD A代替
				auto identifierVal = make_shared<IRCode>(OperationType::LOAD, item.value);
				IRCodeFile.push_back(identifierVal);
			}
			else
			{
				auto funcArgs = make_shared<IRCode>(OperationType::PUSH, item.value);
				IRCodeFile.push_back(funcArgs);
			}
		}
		auto funcCallStmt = make_shared<IRCode>(OperationType::CALL,node->funcName);
		IRCodeFile.push_back(funcCallStmt);
	}

	void IRCodeGen::genCode(IfStmt* node)
	{
		//表达式
		genCode(node->condition.get());
		//JNT _L0   ----->JMP Not true
		CodeGenInfo codeGenInfo = {"ifstart"+to_string(id++),"ifend"+to_string(id++)};
		shared_ptr<IRCode> test1 = make_shared<IRCode>(OperationType::JMPF, codeGenInfo.labelEnd);
		IRCodeFile.push_back(test1);
		//语句
		node->thenBody->genCode(this);//seems to be error
		//Else 存在与否
		if (node->haveElse)
		{
			//JMP _L1
			shared_ptr<IRCode> endIf = make_shared<IRCode>(OperationType::JMP, codeGenInfo.labelNext);
			IRCodeFile.push_back(endIf);
			//LABEL _L0
			shared_ptr<IRCode> label1 = make_shared<IRCode>(OperationType::LABEL, codeGenInfo.labelNext);
			IRCodeFile.push_back(label1);
			//语句
			(static_pointer_cast<ElseStmt>(node->elseBody))->body->genCode(this);
			//LABEL _L1
			shared_ptr<IRCode> label2 = make_shared<IRCode>(OperationType::LABEL, codeGenInfo.labelEnd);
			IRCodeFile.push_back(label2);
		}
		else
		{
			//LABEL _L0
			shared_ptr<IRCode> label1 = make_shared<IRCode>(OperationType::LABEL, codeGenInfo.labelEnd);
			IRCodeFile.push_back(label1);
		}
	}

	void IRCodeGen::genCode(ElseStmt* node)
	{
		//暂不用-------
	}
	void IRCodeGen::genCode(WhileStmt* node)
	{
		//Label _L1
		CodeGenInfo codeGenInfo = { "whileNext" + to_string(id++), "whileEnd" + to_string(id++) };

		shared_ptr<IRCode> whileLabel = make_shared<IRCode>(OperationType::LABEL, codeGenInfo.labelNext);
		IRCodeFile.push_back(whileLabel);
		//表达式翻译
		genCode(node->condition.get());
		//JNT _L0
		shared_ptr<IRCode> testCond = make_shared<IRCode>(OperationType::JMPF, codeGenInfo.labelEnd);
		IRCodeFile.push_back(testCond);
		//语句
		node->body->genCode(this);
		//JMP _L1
		shared_ptr<IRCode> nextLoop = make_shared<IRCode>(OperationType::JMP, codeGenInfo.labelNext);
		IRCodeFile.push_back(nextLoop);
		//LABEL _L0
		shared_ptr<IRCode> endLoop = make_shared<IRCode>(OperationType::LABEL, codeGenInfo.labelEnd);
		IRCodeFile.push_back(endLoop);
	}

	void IRCodeGen::genCode(RepeatStmt* node)
	{
		//Label _L0
		CodeGenInfo codeGenInfo = { "RepeatNext" + to_string(id++), "RepeatEnd" + to_string(id++) };
		shared_ptr<IRCode> repeatLabel = make_shared<IRCode>(OperationType::LABEL, codeGenInfo.labelNext);
		IRCodeFile.push_back(repeatLabel);
		//语句
		node->body->genCode(this);
		//表达式翻译
		genCode(node->untilCond.get());
		//JNT _L0
		shared_ptr<IRCode> testCond = make_shared<IRCode>(OperationType::JMPF, codeGenInfo.labelNext);
		IRCodeFile.push_back(testCond);
		//LABEL _L1
		shared_ptr<IRCode> endLoop1 = make_shared<IRCode>(OperationType::LABEL, codeGenInfo.labelEnd);
		IRCodeFile.push_back(endLoop1);

	}

	void IRCodeGen::genCode(ForStmt* node)
	{

		//表达式1翻译
		genCode((static_pointer_cast<AssignStmt>(node->startStmt))->right.get());
		//STORE 循环变量 ([stack])
		string assignStmt_loopVar = (static_pointer_cast<AssignStmt>(node->startStmt))->left->value.value;
		shared_ptr<IRCode> assignStmt = make_shared<IRCode>(OperationType::STORE, assignStmt_loopVar);
		IRCodeFile.push_back(assignStmt);

		CodeGenInfo codeGenInfo = { "ForNext" + to_string(id++), "ForEnd" + to_string(id++) };
		//LABEL _L1
		auto label1 = make_shared<IRCode>(OperationType::LABEL, codeGenInfo.labelNext);
		IRCodeFile.push_back(label1);
		//表达式2翻译
		genCode(static_pointer_cast<Expression>((node->endValue)).get());
		//CMP 循环变量 exp2结果 (var - res2)
		auto loadVar = make_shared<IRCode>(OperationType::LOAD, assignStmt_loopVar);
		auto leStmt = make_shared<IRCode>(OperationType::CMP);
		IRCodeFile.push_back(loadVar);
		IRCodeFile.push_back(leStmt);
		
		//JMPT _L0 标志位为true cmp结果为0时
		auto test1 = make_shared<IRCode>(OperationType::JMPT, codeGenInfo.labelEnd);
		IRCodeFile.push_back(test1);
		//语句
		node->body->genCode(this);
		//LABEL _L2
		auto label2 = make_shared<IRCode>(OperationType::LABEL,
										  "_L" + to_string(id++));
		IRCodeFile.push_back(label2);
		if (node->direction == Tag::TO){
			//ADD 循环变量 1
			auto addStmt = make_shared<IRCode>(OperationType::ADD, assignStmt_loopVar, "1");
			IRCodeFile.push_back(addStmt);
		}
		else
		{
			//SUB 循环变量 1
			auto addStmt = make_shared<IRCode>(OperationType::SUB, assignStmt_loopVar, "1");
			IRCodeFile.push_back(addStmt);
		}
		//JMP _L1
		auto jmpStmt = make_shared<IRCode>(OperationType::JMP, codeGenInfo.labelNext);
		IRCodeFile.push_back(jmpStmt);
		//LABEL _L0
		auto label0 = make_shared<IRCode>(OperationType::LABEL, codeGenInfo.labelEnd);
		IRCodeFile.push_back(label0);
	}

	void IRCodeGen::genCode(CaseStmt* node)
	{
		vector<shared_ptr<IRCode>> tests;
		//表达式
		genCode(node->condition.get());
		//JE _Ln

		for (auto item : node->body)
		{
			auto je_const = item->list.at(0)->value.value;
			auto pushConst = make_shared<IRCode>(OperationType::PUSH,je_const);
			auto je_label = "_L" + to_string(id++);
			auto cmp = make_shared<IRCode>(OperationType::CMP);
			auto je = make_shared<IRCode>(OperationType::JE, je_label);
			IRCodeFile.push_back(pushConst);
			IRCodeFile.push_back(cmp);
			IRCodeFile.push_back(je);
			tests.push_back(je);
		}
		//JMP _L0
		auto jmpStmt = make_shared<IRCode>(OperationType::JMP, "_L"+to_string(id++));
		IRCodeFile.push_back(jmpStmt);
		//LOOP------------------------
		int j = 0;
		for (auto item : node->body)
		{
			//LABEL _L1-> LABEL _Ln
			auto label1 = make_shared<IRCode>(OperationType::LABEL, tests[j]->_op1);
			IRCodeFile.push_back(label1);
			//语句1
			item->list.at(0)->genCode(this);
			//JMP _L0
			auto jmp = make_shared<IRCode>(OperationType::JMP, jmpStmt->_op1);
			IRCodeFile.push_back(jmp);
		}
		//LEBEL _L0----------------------------
		auto jmpStmt1 = make_shared<IRCode>(OperationType::LABEL, jmpStmt->_op1);
		IRCodeFile.push_back(jmpStmt1);
	}

	void IRCodeGen::genCode(FunctionStmt* node)
	{
		//decl->genCode
		genCode(node->funcDecl.get());
		//body->genCode
		node->body->genCode(this);
		auto funcReturn = make_shared<IRCode>(OperationType::RET);
		IRCodeFile.push_back(funcReturn);
	}
	void IRCodeGen::genCode(FunctionDecl* node)
	{
		//function Name
		//param
		auto funcName = make_shared<IRCode>(OperationType::FUNC, node->name);
		IRCodeFile.push_back(funcName);
		for (int i = node->vars.size()-1; i >= 0;i--)//果然需要倒序入栈，TMD运行时需要pop栈中的实参，实参此时正好是倒序
		{
			auto item = node->vars[i];
			auto funcParam = make_shared<IRCode>(OperationType::PARAM, item->name);
			IRCodeFile.push_back(funcParam);
		}
	}
	void IRCodeGen::genCode(VariableStmt* node)
	{
		if (node->varRoot.size() > 0)
		{
			for (auto item : node->varRoot)
			{
				if (item->varDeclare->identity.tag == Tag::INT)
				{
					auto varStmt_code = make_shared<IRCode>(OperationType::IConst, 
															item->varDeclare->name);
					IRCodeFile.push_back(varStmt_code);
				}
				else if (item->varDeclare->identity.tag == Tag::FLOAT)
				{
					auto varStmt_code = make_shared<IRCode>(OperationType::FConst,
															item->varDeclare->name);
					IRCodeFile.push_back(varStmt_code);
				}
			}
		}
	}

	void IRCodeGen::genCode(TypeStmt* node)
	{
		if (node->typeRoot.size() > 0)
		{
			for (auto item : node->typeRoot)
			{
				if (item->typeDeclare->type == DeclaredType::Record)
				{
					string prefix=item->typeDeclare->name;
					if (item->varStmts.size() > 0)
					{
						for (auto j : item->varStmts)
						{
							if (j->varDeclare->identity.tag == Tag::INT)
							{
								auto varStmt_code = make_shared<IRCode>(OperationType::IConst,
																		prefix+"_"+j->varDeclare->name);
								IRCodeFile.push_back(varStmt_code);
							}
							else if (j->varDeclare->identity.tag == Tag::FLOAT)
							{
								auto varStmt_code = make_shared<IRCode>(OperationType::FConst,
																		prefix + "_" + j->varDeclare->name);
								IRCodeFile.push_back(varStmt_code);
							}
						}
					}
					
				}
				
			}
		}
	}

	void IRCodeGen::genCode(ConstantStmt* node)
	{
		if (node->constRoot.size() > 0)
		{
			for (auto item : node->constRoot)
			{
				if (item->constDeclare->value.tag == Tag::INT)
				{
					auto varStmt_code = make_shared<IRCode>(OperationType::IConst,
															item->constDeclare->name,
															item->constDeclare->value.value);
					IRCodeFile.push_back(varStmt_code);
				}
				else if (item->constDeclare->value.tag == Tag::FLOAT)
				{
					auto varStmt_code = make_shared<IRCode>(OperationType::FConst,
															item->constDeclare->name,
															item->constDeclare->value.value);
					IRCodeFile.push_back(varStmt_code);
				}
			}
		}
	}
}
/*
#define GEN_POINT_NAME(Name) je##Name
#define GEN_JMP_NAME(Name) jmp##Name
#define GEN_CONST_NAME(Name) je_const##Name
#define GEN_LABEL_NAME(Name) je_label##Name

//待优化
auto getOperation = [&](Tag t)
{
OperationType ot;
switch (t)
{
case Tag::EQ:
ot = OperationType::EQU_4;
case Tag::UNEQ:
ot = OperationType::NEG_4;
case Tag::LT:
ot = OperationType::LE_4;
case Tag::GT:
ot = OperationType::GT_4;
case Tag::LE:
ot = OperationType::LE_4;
case Tag::GE:
ot = OperationType::GE_4;
case Tag::AND:
ot = OperationType::AND;
case Tag::OR:
ot = OperationType::OR;
case Tag::ADD:
ot = OperationType::ADD_4;
case Tag::SUB:
ot = OperationType::SUB_4;
case Tag::MUL:
ot = OperationType::MUL_4;
case Tag::DIV:
ot = OperationType::DIV_4;
}
return ot;
};
*/