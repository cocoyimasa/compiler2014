#include "SemanticAnalyzer.h"
#include "PascalAst.h"
#include "parser.h"
#include "SymbolTable.h"
#include "Declaration.h"
using namespace swd;
/*
*@time 2014-10-20
*@author wanghesai
*All rights reserved
*bugs fixed in 2014-12-24
*/
SemanticAnalyzer::SemanticAnalyzer()
{
	this->symTable = new SymbolTable("global");
	currentTable = symTable;
}

SemanticAnalyzer::~SemanticAnalyzer()
{
}
void SemanticAnalyzer::visit(Node *node)
{
	//记录program名字入符号表
	currentTable->add(node->value.value, node);
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
	
}
void SemanticAnalyzer::visit(ComparisonExp* node)
{
}
void SemanticAnalyzer::visit(AssignStmt* node)
{
}
void SemanticAnalyzer::visit(FuncCall* node)
{
	
}
void SemanticAnalyzer::visit(IfStmt* node)
{
	SymbolTable *symTable1 = new SymbolTable();
	currentTable->addInnerTable(symTable1);
	symTable1->outer = currentTable;
	currentTable = symTable1;
	node->thenBody->accept(this);
	if (node->haveElse)
	{
		node->elseBody->accept(this);
	}
	currentTable = symTable1->outer;
}
void SemanticAnalyzer::visit(ElseStmt* node)
{
}
void SemanticAnalyzer::visit(WhileStmt* node)
{
	SymbolTable *symTable1 = new SymbolTable();
	currentTable->addInnerTable(symTable1);
	symTable1->outer = currentTable;
	currentTable = symTable1;
	node->body->accept(this);
	currentTable = symTable1->outer;
}
void SemanticAnalyzer::visit(RepeatStmt* node)
{
	SymbolTable *symTable1 = new SymbolTable();
	currentTable->addInnerTable(symTable1);
	symTable1->outer = currentTable;
	currentTable = symTable1;
	currentTable = symTable1->outer;
}
void SemanticAnalyzer::visit(ForStmt* node)
{
	SymbolTable *symTable1 = new SymbolTable();
	currentTable->addInnerTable(symTable1);
	symTable1->outer = currentTable;
	currentTable = symTable1;
	node->body->accept(this);
	currentTable = symTable1->outer;
}
void SemanticAnalyzer::visit(CaseStmt* node)
{
	//shared_ptr<SymbolTable> symTable1 = make_shared<SymbolTable>();
	//symStack->push(symTable1);
}
void SemanticAnalyzer::visit(FunctionStmt* node)
{
	//Function declaration add in symbol table stack 
	currentTable->add(node->funcDecl->name, node);
	//Creat local symbol table and add in the table stack
	SymbolTable *symTable1 = new SymbolTable(node->funcDecl->name);
	currentTable->addInnerTable(symTable1);
	symTable1->outer = currentTable;
	currentTable = symTable1;
	//seems that the arguments is hard to be checked
	for (auto item : node->funcDecl->vars)
	{
		Expression *newnode=new Expression;
		newnode->value = item->identity;
		symTable1->add(item->name, newnode);
	}
	if (node->list.size() > 0)
	{
		node->list[0]->accept(this);
	}
	//this->visit(node->body.get());
	node->body->accept(this);
	currentTable = symTable1->outer;
}
void SemanticAnalyzer::visit(VariableStmt* node)
{
	if (node->varDeclare == NULL)
	{
		for (auto item : node->varRoot)
		{
			if (currentTable->lookup(item->varDeclare->name) != NULL)
			{
				Error err("variable redefined!", item->varDeclare->identity);
				errList.push_back(err);
			}
			else
			{
				currentTable->add(item->varDeclare->name, item.get());
			}
		}
	}
}
void SemanticAnalyzer::visit(TypeStmt* node)
{
	if (node->typeRoot.size() > 0)
	{
		for (auto item : node->typeRoot)
		{
			currentTable->add(item->typeDeclare->name, item.get());
			if (item->varStmts.size()>0)
			{
				for (auto j : item->varStmts)
				{
					//shared_ptr<VariableStmt> newnode(j);
					currentTable->add(item->typeDeclare->name + "_" + j->varDeclare->name, j.get());
				}
			}
		}
	}
}
void SemanticAnalyzer::visit(ConstantStmt* node)
{
	if (node->constDeclare == NULL && node->constRoot.size()>0)
	{
		for (auto item : node->constRoot)
		{
			if (currentTable->lookup(item->constDeclare->name) != NULL)
			{
				Error err("variable redefined!", node->value);
				errList.push_back(err);
			}
			else
			{
				//shared_ptr<ConstantStmt> newnode(item);
				currentTable->add(item->constDeclare->name, item.get());
			}
		}
	}
}