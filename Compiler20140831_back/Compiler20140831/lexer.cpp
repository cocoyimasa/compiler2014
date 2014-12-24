#include "lexer.h"

//保存关键字宏定义
#define RESERVE_KEYWORD(str,val)  keywords.insert(std::pair<std::string, Tag>(str, val));
namespace swd
{

	Token::Token(Tag t, std::string &v, int l)
	{
		tag = t;
		value = v;
		line = l;
	}
	Token::Token(const Token& t)
	{
		tag = t.tag;
		value = t.value;
		line = t.line;
	}
	bool Token::operator=(const Token& t)
	{
		tag = t.tag;
		value = t.value;
		line = t.line;
		return true;
	}
	Lexer::Lexer()
	{
		lineNum = 1;
		currIndex = 0;
		state = State::START;
		tokenStream.clear();
		std::string keys[] = { "program", "if", "else", "while", "case", "repeat",
			"until", "do", "break", "function", "procedure", "begin",
			"end", "int", "bool", "char", "float", "string", "or",
			"and", "then", "of", "var", "type", "for", "to", "downto", "record",
			"const", "forword"
		};
		Tag val[] = { PROGRAM, IF, ELSE, WHILE, CASE, REPEAT,
			UNTIL, DO, BREAK, FUNCTION, PROCEDURE, BEGIN,
			END, INT, BOOL, CHAR, FLOAT, STRING, OR,
			AND, THEN, OF, VAR, TYPE, FOR, TO, DOWNTO, RECORD,
			CONSTANT, FORWORD
		};
		for (int i = 0; i < sizeof(val) / sizeof(val[0]); i++)//计算数组长度
		{
			RESERVE_KEYWORD(keys[i], val[i])
		}
	}
	void Lexer::readSource(std::string path)
	{
		FILE *p = fopen(path.c_str(), "rb");
		_ASSERT(p != NULL);
		fpos_t pos;
		fseek(p, 0, SEEK_END);
		fgetpos(p, &pos);
		fseek(p, 0, SEEK_SET);
		char *multiBuffer = new char[(size_t)pos + 1];
		fread(multiBuffer, 1, (size_t)pos, p);
		multiBuffer[(size_t)pos] = '\0';
		fclose(p);

		code = multiBuffer;
		delete[] multiBuffer;
	}
	char Lexer::nextChar()
	{
		if (currIndex < code.length())
		{
			peek = code[currIndex++];
			return peek;
		}
		else
			return '\0';
	}
	bool Lexer::readch(char c)
	{
		nextChar();
		if (peek != c) return false;
		peek = ' ';
		return true;
	}
	void Lexer::addToken(Token tok)
	{
		tokenStream.push_back(tok);
	}
	void Lexer::scan()
	{
		Token* currToken = new Token;
		std::string lexeme = "";

		char cDelims[] = { ',', '(', ')', '[', ']', ';', ':', '=', '<', '>', '+', '-', '*', '/', '&' };
		auto isDelims = [&](char ach){
			for (auto i : cDelims)
			{
				if (ach == i)
					return true;
			}
			return false;
		};
		while (nextChar() != '\0')
		{
			bool isLexemeDone = false;
			switch (state)
			{
				case State::START:
					if (isEnterkey(peek))
					{
						lineNum++;
						continue;
					}
					else if (isWhitespace(peek))
					{
						continue;
					}
					else if (isNum(peek))
					{
						state = State::StInt;
					}
					else if (isLetter(peek))
					{
						state = State::StIdentify;
					}
					else if (peek == '"')
					{
						state = State::StString;
						continue;
					}
					else
					{
						switch (peek)
						{
							case ';':
								currToken->tag = SEMI;
								isLexemeDone = true;
								break;
							case '(':
								currToken->tag = OpenBracket;
								isLexemeDone = true;
								break;
							case ')':
								currToken->tag = CloseBracket;
								isLexemeDone = true;
								break;
							case ',':
								currToken->tag = Comma;
								isLexemeDone = true;
								break;
							case ':':
								switch (code.at(currIndex))
								{
									case '=':
										lexeme.append(&peek);
										currToken->tag = BIND;
										isLexemeDone = true;
										nextChar();
										break;
									default:
										currToken->tag = Colon;
										isLexemeDone = true;
										break;
								}
								break;
							case '&':
								currToken->tag = Concat;
								isLexemeDone = true;
								break;
							case '+':
								currToken->tag = ADD;
								isLexemeDone = true;
								break;
							case '-':
								currToken->tag = SUB;
								isLexemeDone = true;
								break;
							case '*':
								currToken->tag = MUL;
								isLexemeDone = true;
								break;
							case '/':
								currToken->tag = DIV;
								isLexemeDone = true;
								break;
								/*case '\\':
								break;
								case T('%'):
								AddToken(1, CodeTokenType::Mod);
								break;*/
							case '<':
								switch (code.at(currIndex))
								{
									case '=':
										lexeme.append(&peek);
										currToken->tag = LE;
										isLexemeDone = true;
										nextChar();
										break;
									case '>':
										lexeme.append(&peek);
										currToken->tag = UNEQ;
										isLexemeDone = true;
										nextChar();
										break;
									default:
										currToken->tag = LT;
										isLexemeDone = true;
										break;
								}
								break;
							case '>':
								switch (code.at(currIndex))
								{
									case '=':
										lexeme.append(&peek);
										currToken->tag = GE;
										isLexemeDone = true;
										nextChar();
										break;
									default:
										isLexemeDone = true;
										currToken->tag = GT;
										break;
								}
								break;
							case '=':
								currToken->tag = EQ;
								isLexemeDone = true;
								break;
						}
					}
					lexeme.append(&peek);
					break;
				case State::StInt:
					if (peek == '.')
					{
						lexeme.append(&peek);
						state = State::StFloat;
					}
					else if (isNum(peek))
					{
						//stay this state
						lexeme.append(&peek);
					}
					else if (isDelims(peek) || isWhitespace(peek))
					{
						isLexemeDone = true;
					}
					break;
				case State::StFloat:
					if (isNum(peek))
					{
						//stay this state
						lexeme.append(&peek);
					}
					else if (isDelims(peek) || isWhitespace(peek))
					{
						isLexemeDone = true;
					}
					break;
				case State::StIdentify:
					if (isLetterOrNum(peek))
					{
						//stay this state
						lexeme.append(&peek);
					}
					else if (isDelims(peek) || isWhitespace(peek))
					{
						isLexemeDone = true;
					}
					break;
				case State::StString:
					if (peek != '"')
					{
						//stay this state
						lexeme.append(&peek);
					}
					else if (peek == '"')//此时读到 "，引号并不需要，直接丢弃，读指针不再回退
					{
						isLexemeDone = true;
					}
					break;
			}
			if (isLexemeDone)
			{
				switch (state)
				{
					case State::START:
						break;
					case State::END:
						break;
					case State::StInt:
						currToken->tag = INT;
						currIndex--;
						break;
					case State::StFloat:
						currToken->tag = FLOAT;
						currIndex--;
						break;
					case State::StString:
						currToken->tag = STRING;//读指针不再回退
						break;
					case State::StIdentify:
						if (keywords.find(lexeme) != keywords.end())
						{
							currToken->tag = keywords[lexeme];
						}
						else
							currToken->tag = IDENT;
						currIndex--;
						break;
					default:
						break;
				}
				state = State::START;
				currToken->value = lexeme;
				currToken->line = lineNum;
				lexeme = "";
				addToken(*currToken);
			}
		}
		delete currToken;
		Token finish;
		finish.tag = FINISH; finish.value = ".";
		finish.line = lineNum + 1;
		addToken(finish);
	}
}
/*keywords.insert(pair<string, Tag>("program", PROGRAM));
keywords.insert(pair<string, Tag>("if", IF));
keywords.insert(pair<string, Tag>("else", ELSE));
keywords.insert(pair<string, Tag>("while", WHILE));
keywords.insert(pair<string, Tag>("case", CASE));
keywords.insert(pair<string, Tag>("repeat", REPEAT));
keywords.insert(pair<string, Tag>("until", UNTIL));
keywords.insert(pair<string, Tag>("do", DO));
keywords.insert(pair<string, Tag>("break", BREAK));
keywords.insert(pair<string, Tag>("function", FUNCTION));
keywords.insert(pair<string, Tag>("procedure", PROCEDURE));
keywords.insert(pair<string, Tag>("begin", BEGIN));
keywords.insert(pair<string, Tag>("end", END));
keywords.insert(pair<string, Tag>("int", INT));
keywords.insert(pair<string, Tag>("bool", BOOL));
keywords.insert(pair<string, Tag>("char", CAHR));
keywords.insert(pair<string, Tag>("float", FLOAT));
keywords.insert(pair<string, Tag>("string", STRING));
keywords.insert(pair<string, Tag>("or", OR));
keywords.insert(pair<string, Tag>("and", AND));*/
