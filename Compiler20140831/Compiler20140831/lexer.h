#pragma once
#include "stdafx.h"
namespace swd
{
	enum Tag
	{
		FINISH, BLOCK,/*virtual node*/
		IF, THEN,ELSE, 
		WHILE, 
		FUNCTION, PROCEDURE, 
		BREAK, 
		BEGIN, END, 
		PROGRAM, 
		CASE, OF, 
		REPEAT, UNTIL, 
		DO,
		VAR,
		TYPE,
		RECORD,
		CONSTANT,
		FOR, TO, DOWNTO, FORWORD,/*keyword*/
		INT, CHAR, FLOAT, BOOL, STRING, IDENT,/*Type*/
		GT, LT, EQ, UNEQ, GE, LE, AND, NOT, OR,/*OP*/
		OpenBracket, CloseBracket, 
		Comma,/*,*/
		Colon,/*:*/
		Concat,/*&*/ 
		SEMI,/*;*/
		ADD, SUB, MUL, DIV, BIND
	};

	enum class State
	{
		START      , 
		END        ,
		StInt      , 
		StFloat    , 
		StString   , 
		StIdentify
	};

	class Token
	{
	public:
		Tag tag;
		std::string value;
		int line;
		// ctors
		Token(){}
		Token(Tag t, std::string &v, int l);
		Token(const Token& t);

		bool operator=(const Token& t);

		~Token(){}
	};

	class Lexer
	{
	public:
		int lineNum;
		std::string code;
		int currIndex;
		State state;
		char peek;
		std::map<std::string, Tag> keywords;
		std::vector<Token> tokenStream;
		
		Lexer();
		void readSource(std::string path);
		void scan();
		void addToken(Token tok);
	protected:
		char nextChar();
		bool readch(char c);
	};
}