#pragma once
#include "Declaration.h"

namespace swd
{
	class SymbolTable
	{
	public:
		string tableName;
		std::map<std::string, Declaration*> symTbl;
		void add(std::string key, Declaration *val);
		Declaration* lookup(std::string key);
	};

	class SymbolTableStack
	{
	public:
		int currNestLevel;
		map<int, SymbolTable*> symTbls;//k-v:level-table
		Declaration* lookup(std::string val);
		void push(SymbolTable *tableName);
		void pop();
		SymbolTable* getTable(int level);
	};
}