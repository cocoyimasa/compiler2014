#include "parser.h"
namespace swd
{
	void Node::print()
	{
		for (auto node : list)
		{
			std::cout <<"<" <<"#type:" <<node->value.tag<<"#"<<node->value.value<<">" << endl;
			node->print();
		}
	}

	void Statement::print()
	{
		std::cout << "<" << "#type:" << this->value.tag << "#" << this->value.value << ">" << " ";
		for (auto item : list)
		{
			item->print();
		}
	}

	void Expression::print()
	{
		std::cout << "<" << "#type:" << this->value.tag << "#" << this->value.value << ">" << " ";
		for (auto item : list)
		{
			item->print();
		}
	}

	void AssignStmt::print()
	{
		cout << endl;
		left->print();
		right->print();
	}

	void ComparisonExp::print()
	{
		cout << endl;
		cout << "-->" << endl;
		left->print();
		cout << "<" << "#type:" << op.tag << "#" << op.value << ">" << " ";
		right->print();
		cout << endl;
	}

	void IfStmt::print()
	{
		cout << endl;
		cout << "-->" << endl;
		condition->print();
		for (auto item : this->list)
		{
			item->print();
		}
		cout << endl;
	}

	void WhileStmt::print()
	{
		cout << endl;
		cout << "-->" << endl;
		condition->print();
		for (auto item : this->list)
		{
			item->print();
		}
		cout << endl;
	}

	void RepeatStmt::print()
	{
		cout << endl;
		cout << "-->" << endl;
		untilCond->print();
		for (auto item : this->list)
		{
			item->print();
		}
		cout << endl;
	}

	void ForStmt::print()
	{
		cout << endl;
		cout << "-->" << endl;
		startValue->print();
		endValue->print();
		nextValue->print();
		for (auto item : this->list)
		{
			item->print();
		}
		cout << endl;
	}

	void CaseStmt::print()
	{
		cout << endl;
		cout << "-->" << endl;
		condition->print();
		for (auto item : this->list)
		{
			item->print();
		}
		cout << endl;
	}

	bool Node::operator=(Node &node)
	{
		this->value = node.value;
		this->parent = node.parent;
		//copy(node.list.begin(), node.list.end(), this->list);//没有分配空间出错
		list.resize(node.list.size());//必不可少
		std::copy(node.list.begin(), node.list.end(), std::back_inserter(list));
		return 1;
	}

	void Error::errorPrint()
	{
		std::cout << errToken.value << " " << errorStr << " in line " << errLine << std::endl;
	}
	bool Parser::advance(Tag t)//可选的Follow，没有的话直接跳过
	{
		if ((it + 1)->tag == t)
		{
			return true;
		}
		else
			return false;
	}

	bool Parser::match(Tag t)//必须有的部分，没有就报错
	{
		if (it->tag == t)
			return true;
		else
		{
			Error err("syntax err", *it);
			errList.push_back(err);
			return false;
		}
	}

	void Parser::parseProgram()
	{
		if (it->tag == PROGRAM)
		{
			it++;
			//记录program名字入符号表 符号表项为名字 值 域
			//---to be implemented
			SymbolTable *symTable = new SymbolTable;
			Declaration *progDecl=new Declaration;
			progDecl->name = it->value;
			progDecl->type = DeclaredType::Program;

			symTable->tableName = it->value;
			symTable->add(it->value, progDecl);
			symStack->push(symTable);

			Node *beginNode = new Node;
			beginNode->value = *it;
			root = beginNode;
			currNode = root;
			it++;
			_ASSERT(match(SEMI));
			parseBlock();
		}
		else
		{
			//unexpected symbol error
		}
		if (it->tag != FINISH)
		{
			//missing the finish symbol
		}
	}
	//lead into the program
	void Parser::parseBlock()
	{
		++it;
		switch (it->tag)
		{
			case CONSTANT:
				parseConstant();
				parseBlock();
			case VAR:
				parseVariables();
				parseBlock();
				break;
			case TYPE:
				parseType();
				parseBlock();
				break;
			case FUNCTION:
				parseFunction();
				parseBlock();
				break;
			case PROCEDURE:
				parseProcedure();
				parseBlock();
				break;
			case BEGIN:
				{
					Statement *block = parseStmtList(currNode == root ? FINISH : END);
					currNode->addNode(block);
					currNode = block;
					_ASSERT(it->tag == FINISH || it->tag == END);
					if (it->tag == END)
					{
						it++;
						_ASSERT(match(SEMI));
					}
				}
				break;
		}
	}
	//函数和过程的通用解析方法
	Statement* Parser::parseProcedureBase(bool isFunction)
	{
		FunctionStmt *funcStmt = new FunctionStmt;
		//将函数声明加入符号表
		FunctionDecl *funcDecl = new FunctionDecl;
		SymbolTable *symTable = symStack->getTable(symStack->currNestLevel);
		symTable->add(funcStmt->funcName, funcDecl);
		//创建局部符号表加入符号栈
		SymbolTable *symTable = new SymbolTable;
		symStack->push(symTable);
		//保存当前结点，将当前结点指向函数结点
		currNode->addNode(funcStmt);
		Node* tmpNode = currNode;
		currNode = funcStmt;
		//获取函数名
		it++;
		if (it->tag == IDENT)
		{
			funcStmt->funcName = it->value;
			funcDecl->name = it->value;
		}
		//匹配左括号
		it++;
		_ASSERT(match(OpenBracket));
		//解析参数
		it++;
		while (it->tag != CloseBracket)
		{
			vector<std::string> args;
			//解析参数列表Lambda表达式-------------------------------
			auto parseArgs = [&](){
				while (it->tag != Colon)
				{
					args.push_back(it->value);
					it++;
					if (it->tag == Comma)
					{
						it++;
					}
				}
				it++;
				for (auto val : args)
				{
					VariableDecl *varDecl = new VariableDecl;
					varDecl->name = val;
					varDecl->type = DeclaredType::Variable;
					varDecl->identity = *it;
					VariableStmt *varStmt = new VariableStmt;
					varStmt->varDeclare = varDecl;

					funcStmt->arguments.push_back(varStmt);
					funcDecl->vars.push_back(varDecl);

					symTable->add(varDecl->name, varDecl);
				}
			};
			//-----------------------------------------------
			it++;
			if (it->tag == VAR)//有VAR为引用传递，否则为值传递
			{
				it++;
				parseArgs();
			}
			else
			{
				parseArgs();
			}

			if (!advance(SEMI))//如果是;,继续循环，否则应该是）,退出循环
			{
				it++;
			}
		}
		it++;
		funcDecl->type = DeclaredType::Procedure;
		if (isFunction)//函数有返回值,过程没有
		{
			funcStmt->retType = *it;
			funcDecl->type = DeclaredType::Function;
			funcDecl->returnType = it->value;
			it++;
		}
		_ASSERT(match(SEMI));
		if (!advance(FORWORD))//是否有forword关键字声明
		{
			parseBlock();//无论下一个Token是VAR还是BEGIN，都可以用Block解析
			currNode = tmpNode;
			currNode->addNode(funcStmt);
			if (isFunction)
			{
				if (symStack->lookup(funcStmt->funcName) != NULL)
				{
					Declaration *decl = symStack->lookup(funcStmt->funcName);
					symStack->pop();
					SymbolTable *symTable = symStack->getTable(symStack->currNestLevel);
					symTable->add(funcStmt->funcName, decl);
				}
				else
				{
					Error err("Function无返回值,请书写返回值", *it);
					errList.push_back(err);
				}
			}
			else{
				symStack->pop();
			}
			return funcStmt;
		}
		else
		{
			it++;
			_ASSERT(match(FORWORD));
			it++;
			_ASSERT(match(SEMI));
			symStack->pop();
		}
	}

	Statement* Parser::parseProcedure()
	{
		parseProcedureBase(false);
	}
	Statement* Parser::parseFunction()
	{
		parseProcedureBase(true);
	}

	void Parser::parseVariables()
	{
		SymbolTable *symTable = symStack->getTable(symStack->currNestLevel);
		while (advance(IDENT))
		{
			VariableDecl *varDecl = parseVariable();
			symTable->add(varDecl->name, varDecl);
			VariableStmt *varStmt = new VariableStmt;
			varStmt->varDeclare = varDecl;
			currNode->addNode(varStmt);
		}
	}

	VariableDecl* Parser::parseVariable()
	{
		VariableDecl *varDecl=new VariableDecl;
		it++;
		varDecl->name = it->value;
		varDecl->type = DeclaredType::Variable;
		it++;
		_ASSERT(match(Colon));
		it++;
		varDecl->identity = *it;
		it++;
		_ASSERT(match(SEMI));
		return varDecl;
	}

	void Parser::parseType()//暂不支持数组和枚举类型
	{
		SymbolTable *symTable = symStack->getTable(symStack->currNestLevel);
		it++;
		while (it->tag == IDENT)
		{
			TypeDecl *typeDecl=new TypeDecl;

			TypeStmt *typeStmt = new TypeStmt;
			typeStmt->typeDeclare = typeDecl;

			typeDecl->name = it->value;
			typeDecl->type = DeclaredType::Record;
			
			it++;
			_ASSERT( match(EQ));
			it++;
			switch (it->tag)
			{
				case RECORD:
					while ((it + 1)->tag != END)
					{
						VariableDecl *record1=parseVariable();
						VariableStmt *varStmt = new VariableStmt;
						varStmt->varDeclare = record1;
						typeDecl->vars.push_back(record1);
						typeStmt->varStmts.push_back(varStmt);
					}
					it++;
					//match(END);
					it++;
					_ASSERT(match(SEMI));
					break;
				default:
					break;
			}
			symTable->add(typeDecl->name, typeDecl);
			currNode->addNode(typeStmt);
		}
	}

	void Parser::parseConstant()
	{
		SymbolTable *symTable = new SymbolTable;
		it++;
		while (it->tag == IDENT)
		{
			ConstantDecl *constDecl=new ConstantDecl;
			ConstantStmt *constStmt = new ConstantStmt;
			constStmt->constDeclare = constDecl;
			constDecl->name = it->value;
			constDecl->type = DeclaredType::Constant;
			it++;
			_ASSERT(match(Colon));
			it++;
			constDecl->value = *it;
			it++;
			_ASSERT(match(SEMI));
			++it;
			symTable->add(constDecl->name, constDecl);
			currNode->addNode(constStmt);
		}
		symStack->push(symTable);
	}

	AssignStmt* Parser::parseAssign()
	{
		AssignStmt *assign = new AssignStmt;
		Expression *exp = new Expression;
		exp->value = *it;
		assign->left = exp;
		it++;
		_ASSERT(match(BIND));
		assign->value = *it;
		it++;
		assign->right = parseExpression();
		//it++;
		_ASSERT(match(SEMI)||match(TO));
		//记录赋值表达式的id到符号表
		return assign;
	}

	ComparisonExp* Parser::parseComparison()//there must be an error
	{
		ComparisonExp *compareStmt = new ComparisonExp;
		Expression *exp = parseExpression();
		compareStmt->left = exp;
		it++;
		compareStmt->op = *it;
		it++;
		compareStmt->right = parseExpression();
		return compareStmt;
	}

	Expression* Parser::parseExpression()
	{
		Expression *rootExp = parseExpression1();
		it++;
		Tag tags[] = { EQ, UNEQ, LT, GT, LE, GE };

		auto isRealOp = [&](Tag t){
			for (auto i : tags)
			{
				if (t == i)
				{
					return true;
				}
			}
			return false;
		};
		if (isRealOp(it->tag))
		{
			while (isRealOp(it->tag))
			{
				Expression *opExp = new Expression;
				opExp->addNode(rootExp);
				opExp->value = *it;
				it++;
				opExp->addNode(parseExpression1());
				rootExp = opExp;
			}
		}
		return rootExp;
	}

	Expression* Parser::parseExpression1()
	{
		Expression *rootExp = parseTerm();
		it++;
		while (it->tag == ADD || it->tag == SUB || it->tag == OR )
		{
			Expression *opExp = new Expression;
			opExp->addNode(rootExp);
			opExp->value = *it;
			it++;
			opExp->addNode(parseTerm());
			rootExp = opExp;
			it++;
		}
		if (!(it->tag == ADD || it->tag == SUB || it->tag == OR))
		{
			it--;
		}
		return rootExp;
	}

	Expression* Parser::parseTerm()
	{
		Expression *rootExp = parseFactor();
		it++;
		
		while (it->tag == MUL || it->tag == DIV || it->tag == AND)
		{
			Expression *opExp = new Expression;
			opExp->value = *it;
			opExp->addNode(rootExp);
			it++;
			opExp->addNode(parseTerm());
			rootExp = opExp;
			it++;
		}
		if (!(it->tag == MUL || it->tag == DIV || it->tag == AND))
		{
			it--;
		}
		return rootExp;
	}

	Expression* Parser::parseFactor()
	{
		if (it->tag == IDENT || it->tag == INT || it->tag == FLOAT)
		{
			Expression *exp = new Expression;
			exp->value = *it;
			return exp;
		}
		else if (it->tag == OpenBracket)
		{
			Expression *exp = parseExpression();
			match(CloseBracket);
			return exp;
		}
	}

	Statement* Parser::parseStmtList(Tag teminator)
	{
		Statement *stList = new Statement;
		stList->value = *it;
		it++;
		while (it->tag != teminator)
		{
			if (it->tag == END && teminator != END)//处理begin..end的匹配情况
			{
				Statement *st1 = new Statement;
				st1->value = *it;
				stList->addNode(st1);
				it++;
			}
			Statement *st = parseStatement();
			stList->addNode(st);
			it++;
		}
		return stList;
	}
	//Core Method
	Statement* Parser::parseStatement()
	{
		Tag tags[] = { EQ, UNEQ, LT, GT, LE, GE };

		auto isRealOp = [&](Tag t){
			for (auto i : tags)
			{
				if (t == i)
				{
					return true;
				}
			}
			return false;
		};
		switch (it->tag)
		{
			//VAR,TYPE,Function call,If,Else,While,For,有的可改变currNode,有的不可
			/*case BEGIN:
				break;*/
			case IDENT://此处默认为赋值表达式，实际上大错特错，如果是 函数调用 何如?
				if (advance(BIND))
				{
					AssignStmt *assign = parseAssign();
					currNode->addNode(assign);
					return assign;
				}
				else if (advance(BIND))
				{
					FuncCall *funcCall = static_cast<FuncCall*>(parseFunctionCall());
					currNode->addNode(funcCall);
					return funcCall;
				}
			//暂不支持嵌套函数
			/*case FUNCTION:
				break;
			case PROCEDURE:
				break;*/
			case IF:
			{
				Node* tmp = currNode;
				IfStmt *ifstmt = static_cast<IfStmt*>(parseIf());
				currNode = tmp;
				currNode->addNode(ifstmt);
				return ifstmt; 
			}
			case WHILE:
			{
				Node* tmp = currNode;
				WhileStmt *whStmt = static_cast<WhileStmt*>(parseWhile());
				currNode = tmp;
				currNode->addNode(whStmt);
				return whStmt; 
			}
			case REPEAT:
			{
				Node* tmp = currNode;
				RepeatStmt *repeatStmt = static_cast<RepeatStmt*>(parseRepeat());
				currNode = tmp;
				currNode->addNode(repeatStmt);
				return repeatStmt; 
			}
			case FOR:
			{
				Node* tmp = currNode;
				ForStmt *forStmt = static_cast<ForStmt*>(parseFor());
				currNode = tmp;
				currNode->addNode(forStmt);
				return forStmt; 
			}
			case CASE:
			{
				Node* tmp = currNode;
				CaseStmt *caseStmt = static_cast<CaseStmt*>(parseCase());
				currNode = tmp;
				currNode->addNode(caseStmt);
				return caseStmt; 
			}
			default:
				return NULL;
		}
		return NULL;
	}

	Statement* Parser::parseIf()
	{
		IfStmt *ifstmt = new IfStmt;
		ifstmt->value = *it;
		currNode = ifstmt;
		it++;
		Expression* cond = parseExpression();
		ifstmt->condition = cond;
		it++;
		_ASSERT(match(THEN));
		it++;
		if (match(BEGIN))
		{
			currNode = ifstmt;
			Statement *body = parseStmtList(END);
			ifstmt->thenBody=body;
			_ASSERT(match(END));
			it++;
			_ASSERT(match(SEMI));
		}
		else
		{
			Statement *body1 = parseStatement();
			ifstmt->thenBody = body1;
		}
		ifstmt->haveElse = false;
		while (advance(ELSE))
		{
			ifstmt->haveElse = true;
			it++;
			ElseStmt *elseStmt = static_cast<ElseStmt*>(parseElse());
			ifstmt->elseBody = elseStmt;
		}
		return ifstmt;
	}

	Statement* Parser::parseElse()
	{
		ElseStmt *elseStmt = new ElseStmt;
		elseStmt->value = *it;
		it++;
		if (match(IF))//match if
		{
			IfStmt *elseifnode = static_cast<IfStmt*>(parseIf());
			elseStmt->addNode(elseifnode);
		}
		else if (match(BEGIN))//match begin Compound Statement 复合语句
		{
			Statement *body = parseStmtList(END);
			elseStmt->body = body;
			_ASSERT(match(END));
			it++;
			_ASSERT(match(SEMI));
		}
		else//单行语句
		{
			Statement *body1 = parseStatement();
			elseStmt->addNode(body1);
		}
		return elseStmt;
	}

	Statement* Parser::parseWhile()
	{
		WhileStmt *whStmt = new WhileStmt;
		whStmt->value = *it;
		currNode = whStmt;
		it++;
		Expression* cond = parseExpression();
		whStmt->condition = cond;
		it++;
		_ASSERT(match(DO));
		it++;
		if (match(BEGIN))
		{
			Statement *body = parseStmtList(END);
			whStmt->body=body;
			_ASSERT(match(END));
			it++;
			_ASSERT(match(SEMI));
		}
		else
		{
			Statement *body1 = parseStatement();
			whStmt->body = body1;
		}
		return whStmt;
	}

	Statement* Parser::parseRepeat()
	{
		RepeatStmt *repeatStmt = new RepeatStmt;
		currNode = repeatStmt;
		Statement *body=parseStmtList(UNTIL);
		repeatStmt->body = body;
		_ASSERT(match(UNTIL));
		Expression *cond = parseExpression();
		repeatStmt->untilCond = cond;
		return repeatStmt;
	}

	Statement* Parser::parseCase()
	{
		CaseStmt *caseStmt = new CaseStmt;
		caseStmt->value = *it;
		currNode = caseStmt;

		Expression *cond = parseExpression();
		caseStmt->condition = cond;
		_ASSERT(match(OF));

		while (it->tag == INT || it->tag == CHAR)
		{
			Statement *branch = parseCaseBranch();
			caseStmt->body.push_back( branch);
			it++;
		}
		_ASSERT(match(END));
		it++;
		_ASSERT(match(SEMI));
		return caseStmt;
	}

	Statement* Parser::parseCaseBranch()
	{
		Statement *caseBranch = new Statement;
		Expression *constNode = parseCaseConst();
		caseBranch->addNode(constNode);
		_ASSERT(match(Colon));
		Statement *st = parseStatement();
		caseBranch->addNode(st);
		return caseBranch;
	}

	Expression* Parser::parseCaseConst()
	{
		Expression *constNode = new Expression;
		while (it->tag == INT || it->tag == CHAR)
		{
			constNode->addNode(parseExpression());
			++it;
			if (it->tag == Comma)
			{
				++it;
			}
		}
		return constNode;
	}

	Statement* Parser::parseFunctionCall()
	{
		FuncCall *funcCall = new FuncCall;
		funcCall->funcName = it->value;
		++it;
		_ASSERT(match(OpenBracket));
		++it;
		Tag arg[] = { Tag::INT, Tag::CHAR, Tag::FLOAT, Tag::BOOL, Tag::STRING, Tag::IDENT };
		auto isArg = [&](Tag tag){
			for (auto i : arg)
			{
				if (i == tag)
				{
					return true;
				}
			}
			return false;
		};
		while(isArg(it->tag))
		{
			funcCall->constants.push_back(*it);
			it++;
		}
		_ASSERT(match(CloseBracket));
		it++;
		_ASSERT(match(SEMI));
		return funcCall;
	}

	Statement* Parser::parseFor()
	{
		ForStmt *forStmt = new ForStmt;
		forStmt->value = *it;
		currNode = forStmt;
		it++;
		AssignStmt *startVal = static_cast<AssignStmt*>(parseStatement());
		forStmt->startValue = startVal;

		Tag direction=it->tag;
		forStmt->direction = direction;
		_ASSERT(match(TO)||match(DOWNTO));

		Expression *opExp = new Expression;
		Token lt;
		Statement *assignVal = new Statement;
		Expression *arithNode = new Expression;

		//k<n;k:=k+1 生成 小于 加1 两个表达式
		lt.line = it->line;
		lt.tag = LT;
		lt.value = direction==TO?"<":">";
		opExp->value = lt;
		opExp->addNode(startVal->left);//获取循环的变量值
		it++;
		Expression *endVal = parseExpression();
		opExp->addNode(endVal);
			
		lt.line = it->line;
		lt.tag = BIND;
		lt.value = ":=";
		assignVal->value = lt;
		assignVal->addNode(startVal->left);

		lt.line = it->line;
		lt.tag = ADD;
		lt.value = direction == TO ? "+":"-";
		arithNode->value = lt;
		arithNode->addNode(startVal->left);

		lt.line = it->line;
		lt.tag = INT;
		lt.value = "1";
		Expression *expOne = new Expression;
		expOne->value = lt;
		arithNode->addNode(expOne);

		assignVal->addNode(arithNode);
		forStmt->endValue = opExp;
		forStmt->nextValue = assignVal;
		_ASSERT(match(DO));
		it++;
		if (match(BEGIN))
		{
			Statement *body = parseStmtList(END);
			forStmt->body = body ;
			_ASSERT(match(END));
			it++;
			_ASSERT(match(SEMI));
		}
		else
		{
			Statement *body1 = parseStatement();
			forStmt->body = body1;
		}
		return forStmt;
	}
}