#pragma once
#include "stdafx.h"

namespace swd
{
	/*
	符号表中所放置的为符号信息，比如变量a的相关属性（名字，类型）函数（名字，参数，返回值）之类
	而不是变量取值
	*/
	class Node;

	class SymbolTable
	{
	public:
		string name;
		int tableIndex;
		SymbolTable* outer;
		vector<SymbolTable*> inner;
		SymbolTable();
		SymbolTable(string name);
		std::map<std::string, swd::Node* > dict;
		bool add(std::string key, Node* val);
		bool addInnerTable(SymbolTable *innerTable);
		SymbolTable* findInnerTable(string name);
		Node* lookup(std::string key);
		Node* lookupInScope(std::string key);
	};
}