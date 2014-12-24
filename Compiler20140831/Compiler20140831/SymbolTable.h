#pragma once
#include "Declaration.h"

namespace swd
{
	class SymbolTable
	{
	public:
		string tableName;
		std::map<std::string, shared_ptr<Declaration> > symTbl;
		SymbolTable()
		{
			symTbl.clear();
		}
		void add(std::string key, shared_ptr<Declaration> val);
		shared_ptr<Declaration> lookup(std::string key);
	};

	class SymbolTableStack
	{
	public:
		int currNestLevel;
		map<int, shared_ptr<SymbolTable> > symTbls;//k-v:level-table
		SymbolTableStack()
		{
			symTbls.clear();
		}
		shared_ptr<Declaration> lookup(std::string val);
		shared_ptr<Declaration> lookup(std::string val,bool isDecl);
		void push(shared_ptr<SymbolTable> tableName);
		void pop();
		shared_ptr<SymbolTable> getTable(int level);
	};
}