#include "SymbolTable.h"
#include "PascalAst.h"
namespace swd
{
	SymbolTable::SymbolTable()
	{
		tableIndex = 0;
		this->name = "";
		outer = NULL;
	}
	SymbolTable::SymbolTable(string name)
	{
		tableIndex = 0;
		this->name = name;
		outer = NULL;
	}
	bool SymbolTable::addInnerTable(SymbolTable *innerTable)
	{
		if (innerTable->name == "")
		{
			innerTable->name = to_string(tableIndex);
		}
		tableIndex++;
		inner.push_back(innerTable);
		return true;
	}
	SymbolTable* SymbolTable::findInnerTable(string name)
	{
		if (inner.size() > 0)
		{
			for (auto item : inner)
			{
				if (item->name == name)
				{
					return item;
				}
			}
		}
	}
	bool SymbolTable::add(std::string key, Node* val)
	{
		if (dict.find(key)==dict.end())
		{
			dict.insert(std::pair<string, Node*>(key, val));
			return true;
		}
		return false;
	}
	Node* SymbolTable::lookup(std::string key)
	{
		SymbolTable *env = this;
		while (env != NULL)
		{
			if (env->dict.find(key)!=env->dict.end())
			{
				return env->dict[key];
			}
			if (env->outer != NULL)
			{
				env = env->outer;
			}
			else
				break;
		}
		return NULL;
	}
	Node* SymbolTable::lookupInScope(std::string key)
	{
		if (dict.find(key) != dict.end())
		{
			return dict[key];
		}
		return NULL;
	}
}