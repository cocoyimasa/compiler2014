#include "SymbolTable.h"

namespace swd
{
	void SymbolTable::add(std::string key, Declaration *val)
	{
		if (lookup(key) == NULL)
		{
			this->symTbl.insert(pair<string, Declaration*>(key, val));
		}
	}

	Declaration* SymbolTable::lookup(std::string key)
	{
		if (this->symTbl.find(key)!=symTbl.end())
		{
			return symTbl[key];
		}
		return NULL;
	}

	Declaration* SymbolTableStack::lookup(std::string val)
	{
		for (int i = currNestLevel; i >= 0; i--)
		{
			if (symTbls[i]->lookup(val) != NULL)
			{
				return symTbls[i]->lookup(val);
			}
		}
		return NULL;
	}

	void SymbolTableStack::push(SymbolTable *tableName)
	{
		symTbls.insert(pair<int, SymbolTable*>(currNestLevel, tableName));
		currNestLevel++;
	}

	SymbolTable* SymbolTableStack::getTable(int level)
	{
		return symTbls[level];
	}

	void SymbolTableStack::pop()
	{
		for (map<int, SymbolTable*>::iterator it = symTbls.begin(); 
			 it != symTbls.end();
			 ++it)
		{
			if (it->first == currNestLevel)
			{
				symTbls.erase(it);
				currNestLevel--;
			}
		}
	}
}