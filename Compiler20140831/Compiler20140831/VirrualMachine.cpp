#include "VirtualMachine.h"
using namespace vm;
/*
*@time 2014-12-24
*@author wanghesai
*All rights reserved
*/
VirtualMachine::VirtualMachine(compiler::IRCodeGen *generator)
{
	this->generator = generator;
	it = generator->IRCodeFile.begin();
}
/*
MOV,ADD,SUB,MUL,DIV,
CMP,
JMP,JMPF,JE,JMPT,
STORE,LOAD,
PUSH,POP,
EQ, UNEQ,
LT,GT,
LE,GE,
AND,OR,
LABEL,FUNC,PARAM,RET,
IConst,FConst
*/
void VirtualMachine::run()
{
#define GENE_CONSTITEM(TAG_TYPE)                                  \
    if ((*it)->Count == 3)                                        \
	{                                                             \
		StackItem sitem = { (*it)->_op1, (*it)->_op2, TAG_TYPE }; \
		constStack.push_back(sitem);                              \
	}else if ((*it)->Count == 2)                                  \
	{                                                             \
		StackItem sitem = { (*it)->_op1, "", TAG_TYPE };          \
		constStack.push_back(sitem);                              \
	}                                                             

	switch ((*it)->_opType)
	{
	case OperationType::IConst:
	{
		GENE_CONSTITEM(Tag::INT);
		break;
	}
	case OperationType::FConst:
	{
		GENE_CONSTITEM(Tag::FLOAT);
		break;
	}
	case OperationType::PUSH:
	{
		StackItem sitem = { (*it)->_op1, "", Tag::INT };
		vStack.push_back(sitem);
		break;
	}
	case OperationType::POP:
	{
		vStack.pop_back();
		break;
	}
	case OperationType::STORE:
	{
		StackItem aitem = vStack.back();
		vStack.pop_back();
		varStack.insert(std::pair<string, string>((*it)->_op1, aitem.value));
		break;
	}
	case OperationType::LOAD:
	{
		string varValue = varStack[(*it)->_op1];
		StackItem sitem = { (*it)->_op1, varValue, Tag::INT };
		vStack.push_back(sitem);
		break;
	}
	case OperationType::ADD:
	{
		double result = 0;
		if ((*it)->Count == 1)
		{
			//如果不是立即数，而是变量该如何处理.此处得见符号表之重要
			StackItem item1 = vStack.back();
			vStack.pop_back();
			StackItem item2 = vStack.back();
			vStack.pop_back();
			//此处见得带后缀的指令的好处
			if (item1.type == Tag::FLOAT)
			{
				if (item2.type == Tag::INT)
				{
					result = atof(item1.value.c_str()) + atoi(item2.value.c_str());
				}
				else if (item2.type == Tag::FLOAT)
				{
					result = atof(item1.value.c_str()) + atof(item2.value.c_str());
				}
			}
			else if (item1.type == Tag::INT)
			{
				if (item2.type == Tag::INT)
				{
					result = atoi(item1.value.c_str()) + atoi(item2.value.c_str());
				}
				else if (item2.type == Tag::FLOAT)
				{
					result = atoi(item1.value.c_str()) + atof(item2.value.c_str());
				}
			}
		}
		else if ((*it)->Count == 3)
		{

		}
		
	}
	}
}