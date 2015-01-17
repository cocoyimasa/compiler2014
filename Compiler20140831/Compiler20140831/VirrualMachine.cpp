#include "VirtualMachine.h"
#include "parser.h"
#include "SymbolTable.h"
#include "PascalAst.h"
#include "Declaration.h"
using namespace vm;
/*
*@time 2014-12-24
*@author wanghesai
*All rights reserved
*
*2015-1-14,1-15 增加IO库函数支持 更新LOAD指令的支持
*1-18 更新read库函数的支持
*/
VirtualMachine::VirtualMachine()
{
	programCounter = 0;
	reg_flag = false;
	//it = generator->IRCodeFile.begin();
	//this->labelScan();
}
VirtualMachine::VirtualMachine(compiler::IRCodeGen *generator)
{
	programCounter = 0;
	reg_flag = false;
	this->generator = generator;
	it = generator->IRCodeFile.begin();
	this->labelScan();
	this->initRuntimeInfo();
	this->symTable->initFunctions();
}
VirtualMachine::VirtualMachine(compiler::IRCodeGen *generator, 
							   swd::SymbolTable *symTable)
{
	programCounter = 0;
	reg_flag = false;
	this->generator = generator;
	this->symTable = symTable;
	currentTable = symTable;
	it = generator->IRCodeFile.begin();
	this->labelScan();
	this->initRuntimeInfo();
	this->symTable->initFunctions();
}

void VirtualMachine::move()
{
	it++;
	programCounter=it - generator->IRCodeFile.begin();//永远保持与it同步
}

void VirtualMachine::initRuntimeInfo()
{
	rtInfo.lastScope = "";
	rtInfo.currentScope = "global";
	rtInfo.vStackItems = vStack.size();
	//rtInfo.varStackItems
	rtInfo.constStackItems=constStack.size();
	rtInfo.paramNum=0;
}

void VirtualMachine::labelScan()
{
	vector<shared_ptr<IRCode>>::iterator iter; 
	int pos=0;
	for (iter = this->generator->IRCodeFile.begin(); 
		 iter != this->generator->IRCodeFile.end(); ++iter)
	{
		pos++;
		if ((*iter)->_opType == OperationType::LABEL || (*iter)->_opType == OperationType::FUNC)
		{
			labelPos.insert(std::pair<string, int>((*iter)->_op1, pos));
		}
	}
}

void VirtualMachine::compute(char op)
{
	double result = 0;
	Tag resultType;
	if ((*it)->Count == 0)
	{
		StackItem item1 = vStack.back();
		vStack.pop_back();
		StackItem item2 = vStack.back();
		vStack.pop_back();

		//此处见得带后缀的指令的好处
		if (item1.type == Tag::INT && item2.type == Tag::INT)
		{
			resultType = Tag::INT;
		}
		else
		{
			resultType = Tag::FLOAT;
		}

		switch (op)
		{
		case '+':
		result = atof(item1.value.c_str()) + atof(item2.value.c_str());
		break;
		case '-':
		result = atof(item1.value.c_str()) - atof(item2.value.c_str());
		break;
		case '*':
		result = atof(item1.value.c_str()) * atof(item2.value.c_str());
		break;
		case '/':
		result = atof(item1.value.c_str()) / atof(item2.value.c_str());
		break;
		default:
		result = atof(item1.value.c_str()) + atof(item2.value.c_str());

		}

		if (resultType == Tag::INT)
		{
			StackItem sitem = { item1.value + item2.value + "_result", to_string((int)result), Tag::INT };
			vStack.push_back(sitem);
		}
		else
		{
			StackItem sitem = { item1.value + item2.value + "_result", to_string(result), Tag::FLOAT };
			vStack.push_back(sitem);
		}
	}
	else if ((*it)->Count == 2)//一般来说，此类表达式出现于for循环 add a 1
	{
		if (varStack.find((*it)->_op1) != varStack.end())
		{
			string val = varStack[(*it)->_op1];
			switch (op)
			{
			case '+':
			varStack[(*it)->_op1] = to_string(atoi(val.c_str()) + atoi((*it)->_op2.c_str()));
			break;
			case '-':
			varStack[(*it)->_op1] = to_string(atoi(val.c_str()) - atoi((*it)->_op2.c_str()));
			break;
			case '*':
			varStack[(*it)->_op1] = to_string(atoi(val.c_str()) * atoi((*it)->_op2.c_str()));
			break;
			case '/':
			varStack[(*it)->_op1] = to_string(atoi(val.c_str()) / atoi((*it)->_op2.c_str()));
			break;
			default:
			varStack[(*it)->_op1] = to_string(atoi(val.c_str()) + atoi((*it)->_op2.c_str()));

			}
			
		}
	}
}
template<typename T>
void VirtualMachine::write(T stringOrNum)
{
	cout << stringOrNum << endl;
}
template<typename T>
void VirtualMachine::read(T* varName)
{
	cin >> *varName;
	cin.get();//consume enter key
}

void VirtualMachine::functionExec(string funcName, StackItem *params, int args)
{
	if (funcName == "read"&& args==1)
	{
		string s;
		read(&s);
		params[0].value = s;
		//更新varStack的k,读之前已经加载到了变量栈
		varStack[params[0].name]=params[0].value;
	}
	else if (funcName == "write" && args == 1)
	{
		write(params[0].value);
	}
	else
	{
		//
	}
}

/*
MOV,ADD,SUB,MUL,DIV,*^
CMP,*
JMP,JMPF,JE,JMPT,*
STORE,LOAD,*
PUSH,POP,*
EQ, UNEQ,*
LT,GT,^
LE,GE,^
AND,OR,^
LABEL,FUNC,PARAM,RET,CALL,*
IConst,FConst*
*/
#define GENE_CONSTITEM(TAG_TYPE)                                  \
    if ((*it)->Count == 2)                                        \
	{                                                             \
	StackItem sitem = { (*it)->_op1, (*it)->_op2, TAG_TYPE };     \
	constStack.push_back(sitem);                                  \
	}else if ((*it)->Count == 1)                                  \
	{                                                             \
		StackItem sitem = { (*it)->_op1, "", TAG_TYPE };          \
		constStack.push_back(sitem);                              \
	}                                                             

#define GENE_CMP_OP(_CMP_OP_)                 \
	StackItem item1 = vStack.back();          \
	vStack.pop_back();                        \
	StackItem item2 = vStack.back();          \
	vStack.pop_back();                        \
	if (item1.value _CMP_OP_ item2.value)     \
	{                                         \
	reg_flag = true;                          \
	} 

void VirtualMachine::scan()
{
	static bool isJmpOrCall=false;
	
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
		StackItem sitem = { "", (*it)->_op1, Tag::INT };
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
		string varValue="";
		Node *varNode=currentTable->lookup((*it)->_op1);
		Tag t = Tag::INT;
		if (varNode != NULL)
		{
			t = varNode->value.tag;//Node为ConstantStmt,TypeStmt,VariableStmt
			switch (t)
			{
			case Tag::CONSTANT:
				t = ((ConstantStmt*)varNode)->constDeclare->value.tag;
				varValue = ((ConstantStmt*)varNode)->constDeclare->value.value;
				break;
			case Tag::VAR:
				varValue = varStack[(*it)->_op1];
				t = ((VariableStmt*)varNode)->varDeclare->identity.tag;
				break;
			case Tag::TYPE:
				t = Tag::TYPE;
				varValue = "";
				break;
			}
		}
		StackItem sitem = { (*it)->_op1, varValue, t };
		vStack.push_back(sitem);
		break;
	}
	case OperationType::ADD:
	{
		compute('+');
		break;
	}
	case OperationType::SUB:
	{
		compute('-');
		break;
	}
	case OperationType::MUL:
	{
		compute('*');
		break;
	}
	case OperationType::DIV:
	{
		compute('/');
		break;
	}
	case OperationType::JMP:
	{
		isJmpOrCall = true;
		if (labelPos.find((*it)->_op1) != labelPos.end())
		{
			it = generator->IRCodeFile.begin() + labelPos[(*it)->_op1];
		}
		break;
	}
	case OperationType::JMPT:
	{
		isJmpOrCall = true;
		if (reg_flag && labelPos.find((*it)->_op1) != labelPos.end())
		{
			it = generator->IRCodeFile.begin() + labelPos[(*it)->_op1];
		}
		break;
	}
	case OperationType::JE:
	{
		isJmpOrCall = true;
		StackItem item1 = vStack.back();
		vStack.pop_back();
		StackItem item2 = vStack.back();
		vStack.pop_back();
		if (atof(item1.value.c_str()) - atof(item2.value.c_str()) == 0)//相等时，标志位为真
		{
			reg_flag = true;
		}
		if (reg_flag && labelPos.find((*it)->_op1) != labelPos.end())
		{
			it = generator->IRCodeFile.begin() + labelPos[(*it)->_op1];
		}
		break;
	}
	case OperationType::JMPF:
	{
		isJmpOrCall = true;
		if (!reg_flag && labelPos.find((*it)->_op1) != labelPos.end())
		{
			it = generator->IRCodeFile.begin() + labelPos[(*it)->_op1];
		}
		break;
	}
	case OperationType::EQ:
	{
		GENE_CMP_OP( == );
		break;
	}
	case OperationType::UNEQ:
	{
		GENE_CMP_OP( != );
		break;
	}
	case OperationType::LT:
	{
		GENE_CMP_OP(< );
		break;
	}
	case OperationType::GT:
	{
		GENE_CMP_OP(> );
		break;
	}
	case OperationType::LE:
	{
		GENE_CMP_OP(<= );
		break;
	}
	case OperationType::GE:
	{
		GENE_CMP_OP(>= );
		break;
	}
	case OperationType::CMP:
	{
		StackItem item1 = vStack.back();
		vStack.pop_back();
		StackItem item2 = vStack.back();
		vStack.pop_back();
		if (atof(item1.value.c_str()) - atof(item2.value.c_str()) == 0)//相等时，标志位为真
		{
			reg_flag = true;
		}
		break;
	}
	case OperationType::LABEL:
	{
		break;//nothing to do with label instruction
	}
	case OperationType::FUNC:
	{
		if (rtInfo.currentScope == "global")
		{
			it = generator->IRCodeFile.begin() + labelPos["__Main__"];
			isJmpOrCall = true;//第一次到这里的时候要跳到__Main__
		}
		break;//nothing to do with FUNC instruction
	}
	case OperationType::PARAM:
	{
		//果然需要倒序入栈，TMD运行时需要pop栈中的实参，实参此时正好是倒序
		StackItem item = vStack.back();
		vStack.pop_back();
		currentTable = currentTable->findInnerTable(rtInfo.currentScope);
		//Tag t = currentTable->lookupInScope((*it)->_op1)->value.tag;
		rtInfo.paramNum++;//有几个param。初始化时实参已经加入到了stack上，但是ret时需要将实参也删除，此处必须确定实参个数
		varStack.insert(std::pair<string, string>((*it)->_op1, item.value));
		//distance 对iterator做减法
		rtInfo.varStackItems.push_back(distance(varStack.find((*it)->_op1),varStack.begin()));
		break;
	}
	case OperationType::RET:
	{
		isJmpOrCall = true;//ret也是跳转指令
		if (labelPos.find(rtInfo.currentScope+"_call") != labelPos.end())
		{
			it = generator->IRCodeFile.begin() + labelPos[(*it)->_op1];
		}
		rtInfo.lastScope = rtInfo.currentScope;
		rtInfo.currentScope = rtInfo.lastScope;
		int vStackClear= vStack.size() - rtInfo.vStackItems-rtInfo.paramNum;
		
		int constStackClear = constStack.size() - rtInfo.constStackItems;
		while (vStackClear>0 ||constStackClear>0)//clear stack
		{
			vStack.pop_back();
			//varStack.erase
			constStack.pop_back();
			vStackClear--;
			constStackClear--;
		}
		while (rtInfo.varStackItems.size() > 0)//删除新加的局部变量
		{
			int item = rtInfo.varStackItems.back();
			//map是非线性的容器，iterator不能直接加减
			map<string,string>::iterator iter= varStack.begin();
			advance(iter, item);//前移iterator
			varStack.erase(iter);
			rtInfo.varStackItems.pop_back();
		}
		rtInfo.vStackItems=vStack.size();//恢复栈帧
		currentTable = currentTable->outer;//恢复符号表
		break;
	}
	case OperationType::CALL:
	{
		if (labelPos.find((*it)->_op1) != labelPos.end())
		{
			isJmpOrCall = true;
			it = generator->IRCodeFile.begin() + labelPos[(*it)->_op1];
			labelPos.insert(std::pair<string, int>((*it)->_op1 + "_call", programCounter+1));
			rtInfo.lastScope = rtInfo.currentScope;
			rtInfo.currentScope = (*it)->_op1;
			rtInfo.vStackItems = vStack.size();
		}
		else if (symTable->lookupFunction((*it)->_op1))//built-in functions
		{
			isJmpOrCall = false;
			StackItem item = vStack.back();
			vStack.pop_back();
			//built-in function call
			functionExec((*it)->_op1, &item, 1);
		}
		break;
	}
	}
	if (!isJmpOrCall)//非跳转指令指针移动
	{
		move();
	}
	else//跳转指令无需移动
	{
		programCounter = it - generator->IRCodeFile.begin();
	}
}
void VirtualMachine::run()
{
	while (it != generator->IRCodeFile.end())
	{
		scan();
	}
}