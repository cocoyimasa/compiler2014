#pragma once
#include "stdafx.h"
#include "lexer.h"
namespace swd
{

	enum class DeclaredType
	{
		Constant, Enumeration,
		Type, Variable, Record,
		Program, Procedure, Function,
		Undefined, 
		INT, FLOAT, STRING, USERTYPE /*可用可不用*/
	};

	class Declaration
	{
	public:
		DeclaredType type;//类型，constant，type，var
		string name;//标识符名称
	};

	class ConstantDecl :public Declaration
	{
	public:
		// const i=10
		Token value;
	};

	class VariableDecl :public Declaration
	{
	public:
		// var i:integer
		Token identity;

	};

	class TypeDecl :public Declaration //只支持record 不支持数组和枚举
	{
	public:
		vector<VariableDecl*> vars;
	};

	class FunctionDecl :public Declaration
	{
	public:
		std::vector<VariableDecl*> vars;
		std::string returnType;
	};
}