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
double calcExpression(const StackItem& item1, const StackItem& item2,char op){
	double result = 0;
#define __Atof(item) atof(item.value.c_str())
#define __ApplyMath(op) result = __Atof(item1) op __Atof(item2);
	switch (op)
	{
	case '+':
		__ApplyMath(+)
		break;
	case '-':
		__ApplyMath(-)
		break;
	case '*':
		__ApplyMath(*)
		break;
	case '/':
		__ApplyMath(/)
		break;
	default:
		__ApplyMath(+)
	}
	return result;
}
void VirtualMachine::computeDirective(char op)
{
	double result = 0;
	Tag resultType;
	StackItem item1 = vStack.back();
	vStack.pop_back();
	StackItem item2 = vStack.back();
	vStack.pop_back();
#define calcString(item1,item2)               \
	string res = "";                          \
	resultType = Tag::STRING;                 \
	if (op != '+'){							  \
		res = item1.value;					  \
	}										  \
	else {									  \
		res = item1.value + item2.value;	  \
	}										  \
	StackItem sitem = {						  \
		res + "_result",                      \
		res,								  \
		Tag::STRING							  \
	};vStack.push_back(sitem);

	//此处见得带后缀的指令的好处
	if (item1.type == Tag::INT && item2.type == Tag::INT)
	{
		resultType = Tag::INT;
		result = calcExpression(item1, item2, op); // calc expression
	}
	else if (item1.type == Tag::FLOAT || item2.type == Tag::FLOAT)
	{
		resultType = Tag::FLOAT;
		result = calcExpression(item1, item2, op); // calc expression
	}
	else if (item1.type == Tag::STRING)
	{
		calcString(item1,item2);
		return;
	}
	else if (item2.type == Tag::STRING){
		calcString(item2, item1);
		return;
	}
	else {
		result = calcExpression(item1, item2, op); // calc expression
	}

	if (resultType == Tag::INT)
	{
		StackItem sitem = {
			item1.value + item2.value + "_result",
			to_string((int)result), Tag::INT
		};
		vStack.push_back(sitem);
	}
	else
	{
		StackItem sitem = {
			item1.value + item2.value + "_result",
			to_string(result), Tag::FLOAT
		};
		vStack.push_back(sitem);
	}
}
void VirtualMachine::computeDirectiveWithAgrs(char op)
{
	auto _op1 = (*it)->_op1;
	auto _op2 = (*it)->_op2;
	if (varStack.find(_op1) != varStack.end())
	{
		string val = varStack[_op1];
#define __ApplyMathString(op) varStack[_op1] = \
to_string(atoi(val.c_str()) op atoi(_op2.c_str()));
		switch (op)
		{
		case '+':
			__ApplyMathString(+)
			break;
		case '-':
			__ApplyMathString(-)
			break;
		case '*':
			__ApplyMathString(*)
			break;
		case '/':
			__ApplyMathString(/ )
			break;
		default:
			__ApplyMathString(+)
		}
	}
}
void VirtualMachine::compute(char op)
{
	if ((*it)->Count == 0)
	{
		computeDirective(op); // compute directive with no args
	}
	else if ((*it)->Count == 2)//一般来说，此类表达式出现于for循环 add a 1
	{
		computeDirectiveWithAgrs(op); // compute directive with 2 args
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
	if (funcName == "read" && args==1)
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
	if (item2.value _CMP_OP_ item1.value)     \
	{                                         \
	reg_flag = true;                          \
	}                                         \
	else                                      \
	{reg_flag=false;} 

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
	case OperationType::PUSHF:
	{
		StackItem sitem = { "", (*it)->_op1, Tag::FLOAT };
		vStack.push_back(sitem);
		break;
	}
	case OperationType::POPF:
	{
		vStack.pop_back();
		break;
	}
	case OperationType::PUSHS:
	{
		StackItem sitem = { "", (*it)->_op1, Tag::STRING };
		vStack.push_back(sitem);
		break;
	}
	case OperationType::POPS:
	{
		vStack.pop_back();
		break;
	}
	case OperationType::STORE:
	{
		StackItem aitem = vStack.back(); //从全局栈弹出，等待存入局部栈
		vStack.pop_back();
		if (varStack.find((*it)->_op1) != varStack.end() && 
			varStack[(*it)->_op1] != aitem.value)
		{
			//如果局部栈已经存在变量_op1，则更新其值
			varStack[(*it)->_op1] = aitem.value;
		}
		else
			varStack.insert(std::pair<string, string>((*it)->_op1, aitem.value));
		break;
	}
	case OperationType::LOAD:
	{
		//局部栈弹出变量，存入全局栈
		string varValue = "";
		Node *varNode = currentTable->lookup((*it)->_op1);
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
		if (labelPos.find((*it)->_op1) != labelPos.end())
		{
			isJmpOrCall = true;
			it = generator->IRCodeFile.begin() + labelPos[(*it)->_op1];
		}
		break;
	}
	case OperationType::JMPT:
	{
		if (reg_flag && labelPos.find((*it)->_op1) != labelPos.end())
		{
			isJmpOrCall = true;
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
		if (abs(atof(item1.value.c_str()) - atof(item2.value.c_str())) <= 0.000001) 
		// to be fixed
		//相等时，标志位为真
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
		if (!reg_flag && labelPos.find((*it)->_op1) != labelPos.end())
		{
			isJmpOrCall = true;
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
		static bool isTableFound=false;
		//果然需要倒序入栈，TMD运行时需要pop栈中的实参，实参此时正好是倒序
		StackItem item = vStack.back();
		vStack.pop_back();
		if (!isTableFound)
		{
			currentTable = currentTable->findInnerTable(rtInfo.currentScope);
			isTableFound = true;
		}
		//Tag t = currentTable->lookupInScope((*it)->_op1)->value.tag;
		rtInfo.paramNum++;//有几个param。初始化时实参已经加入到了stack上，但是ret时需要将实参也删除，此处必须确定实参个数
		varStack.insert(std::pair<string, string>((*it)->_op1, item.value));
		//distance 对iterator做减法
		rtInfo.varStackItems.push_back(
			distance(varStack.find((*it)->_op1),
			varStack.begin()));
		break;
	}
	case OperationType::RET:
	{
		isJmpOrCall = true;//ret也是跳转指令
		if (labelPos.find(rtInfo.currentScope + "_call") != labelPos.end())
		{
			it = generator->IRCodeFile.begin() + labelPos[rtInfo.currentScope + "_call"];
		}
		string tmp = rtInfo.lastScope;
		rtInfo.lastScope = rtInfo.currentScope;
		rtInfo.currentScope = tmp;
		int vStackClear= vStack.size() - rtInfo.vStackItems-rtInfo.paramNum;
		
		int constStackClear = constStack.size() - rtInfo.constStackItems;
		while (vStackClear>0)//clear stack
		{
			vStack.pop_back();
			vStackClear--;
		}
		while (constStackClear > 0)
		{
			constStack.pop_back();
			constStackClear--;
		}
		while (rtInfo.varStackItems.size()>0)//删除新加的局部变量
		{
			int item = rtInfo.varStackItems.back();
			//map是非线性的容器，iterator不能直接加减
			map<string,string>::iterator iter= varStack.begin();
			advance(iter, item);//前移iterator
			varStack.erase(iter);
			rtInfo.varStackItems.pop_back();
		}
		rtInfo.constStackItems = constStack.size();
		currentTable = currentTable->outer;//恢复符号表
		
		break;
	}
	case OperationType::CALL:
	{
		if (labelPos.find((*it)->_op1) != labelPos.end())
		{
			rtInfo.lastScope = rtInfo.currentScope;
			rtInfo.currentScope = (*it)->_op1;
			rtInfo.vStackItems = vStack.size();
			rtInfo.constStackItems = constStack.size();
			isJmpOrCall = true;
			labelPos.insert(std::pair<string, int>((*it)->_op1 + "_call", programCounter + 1));
			it = generator->IRCodeFile.begin() + labelPos[(*it)->_op1];
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
		isJmpOrCall = false;
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