#include "SemanticAnalyzer.h"
#include "PascalAst.h"
#include "parser.h"
using namespace swd;
/*
*@time 2014-10-20
*@author wanghesai
*All rights reserved
*bugs fixed in 2014-12-24
*/
SemanticAnalyzer::SemanticAnalyzer()
{
	this->symStack = make_shared<SymbolTableStack>();
	symStack->currNestLevel = 0;
}


SemanticAnalyzer::~SemanticAnalyzer()
{
}
void SemanticAnalyzer::visit(Node *node)
{
	//记录program名字入符号表 符号表项为名字 值 域
	//---to be implemented
	shared_ptr<SymbolTable> symTable = make_shared<SymbolTable>();
	shared_ptr<Declaration> progDecl = make_shared<Declaration>();
	progDecl->name = node->value.value;
	progDecl->type = DeclaredType::Program;

	symTable->tableName = node->value.value;
	symTable->add(node->value.value, progDecl);
	symStack->push(symTable);
	for (auto item : node->list)
	{
		item->accept(this);
	}
}
void SemanticAnalyzer::visit(Statement* node)
{
	for (auto item : node->list)
	{
		item->accept(this);
	}
}
void SemanticAnalyzer::visit(Expression* node)
{
	if (node->list.size() > 2)
	{
		for (auto item : node->list)
		{
			item->accept(this);
		}
	}
	else if(node->list.size() == 2)
	{
		if (node->list[0]->value.tag != node->list[1]->value.tag)
		{
			Error err("type error.No operation is allowed in different type", node->value);
			errList.push_back(err);
		}
	}
	else
	{
		Error err("expression declared in a wrong way", node->value);
		errList.push_back(err);
	}
	
}
void SemanticAnalyzer::visit(ComparisonExp* node)
{
	//--to be implemented
}
void SemanticAnalyzer::visit(AssignStmt* node)
{
	if (symStack->lookup(node->left->value.value))
	{
		//type ?
		node->right->accept(this);
	}
	else
	{
		Error err("variable is not defined", node->value);
		errList.push_back(err);
	}
}
void SemanticAnalyzer::visit(FuncCall* node)
{
	shared_ptr<SymbolTable> symTable = symStack->getTable(symStack->currNestLevel);
	
}
void SemanticAnalyzer::visit(IfStmt* node)
{}
void SemanticAnalyzer::visit(ElseStmt* node)
{}
void SemanticAnalyzer::visit(WhileStmt* node)
{}
void SemanticAnalyzer::visit(RepeatStmt* node)
{}
void SemanticAnalyzer::visit(ForStmt* node)
{}
void SemanticAnalyzer::visit(CaseStmt* node)
{}
void SemanticAnalyzer::visit(FunctionStmt* node)
{
	//Function declaration add in symbol table stack 
	auto funcDeclTemp = node->funcDecl;
	shared_ptr<SymbolTable> symTable = symStack->getTable(symStack->currNestLevel);
	symTable->add(funcDeclTemp->name, funcDeclTemp);
	//Creat local symbol table and add in the table stack
	shared_ptr<SymbolTable> symTable1 = make_shared<SymbolTable>();
	symStack->push(symTable1);
	//seems that the arguments is hard to be checked
	for (auto item : funcDeclTemp->vars)
	{
		symTable1->add(item->name,item);
	}
	if (node->list.size() > 0)
	{
		node->list[0]->accept(this);
	}
	//this->visit(node->body.get());
	node->body->accept(this);
}
void SemanticAnalyzer::visit(VariableStmt* node)
{
	if (node->varDeclare != NULL)
	{
		//find if this variable name already exists 
		if (symStack->lookup(node->varDeclare->name, true) != NULL)
		{
			Error err("variable redefined!", node->value);
			errList.push_back(err);
		}
		else
		{
			symStack->symTbls[symStack->currNestLevel]->add(node->varDeclare->name, 
																node->varDeclare);
		}
	}
	//if variable is root var,visit the child
	else if (node->varDeclare == NULL)
	{
		for (auto item : node->varRoot)
		{
			item->accept(this);
		}
	}
}
void SemanticAnalyzer::visit(TypeStmt* node)
{
	shared_ptr<SymbolTable> symTable = symStack->getTable(symStack->currNestLevel);
	for (auto item : node->typeRoot)
	{
		symTable->add(item->typeDeclare->name, item->typeDeclare);
		for (auto j : item->varStmts)
		{
			symTable->add(item->typeDeclare->name +"_"+ j->varDeclare->name, item->typeDeclare);
		}
	}
	
}
void SemanticAnalyzer::visit(ConstantStmt* node)
{
	shared_ptr<SymbolTable> symTable = symStack->getTable(symStack->currNestLevel);
	for (auto item : node->constRoot)
	{
		//const is used only for integer as default
		symTable->add(item->constDeclare->name, item->constDeclare);
	}
}