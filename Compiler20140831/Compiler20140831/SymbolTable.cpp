#include "SymbolTable.h"

namespace swd
{
	void SymbolTable::add(std::string key, shared_ptr<Declaration> val)
	{
		if (lookup(key) == NULL)
		{
			symTbl.insert(pair<string, shared_ptr<Declaration>>(key, val));
		}
	}

	shared_ptr<Declaration> SymbolTable::lookup(std::string key)
	{
		if (symTbl.empty())
		{
			return NULL;
		}
		if (!symTbl.empty() && symTbl.find(key) != symTbl.end())
		{
			return symTbl[key];
		}
		return NULL;
	}

	shared_ptr<Declaration> SymbolTableStack::lookup(std::string val)
	{
		if (symTbls.empty())
		{
			return NULL;
		}
		for (int i = currNestLevel; i >= 0; i--)
		{
			shared_ptr<Declaration> decl = symTbls[currNestLevel]->lookup(val);
			if (decl!=NULL)
				return decl;
		}
		return NULL;
	}

	shared_ptr<Declaration> SymbolTableStack::lookup(std::string val,bool isDecl)
	{
		if (!isDecl)return NULL;
		if (symTbls.empty())
		{
			return NULL;
		}
		shared_ptr<Declaration> decl = symTbls[currNestLevel]->lookup(val);
		return decl;
	}

	void SymbolTableStack::push(shared_ptr<SymbolTable> tableName)
	{
		currNestLevel++;
		symTbls.insert(pair<int, shared_ptr<SymbolTable> >(currNestLevel, tableName));
	}

	shared_ptr<SymbolTable> SymbolTableStack::getTable(int level)
	{
		if (!symTbls.empty())
		{
			return symTbls[level];
		}
		return NULL;
	}

	void SymbolTableStack::pop()
	{
		if (symTbls.empty())
		{
			return;
		}
		for (map<int, shared_ptr<SymbolTable> >::iterator it = symTbls.begin();
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