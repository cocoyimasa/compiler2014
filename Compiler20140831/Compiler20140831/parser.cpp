#include "parser.h"
#include "Declaration.h"
/**
*@time 2014-08-31
*@author wanghesai
*All rights reserved
*many bugs fixed in 2014-12-23/12-24
*/
namespace swd
{
	void Node::print()
	{
		for (auto node : list)
		{
			std::cout <<"{ " <<"type:" <<node->value.tag<<" | "
				<<"node"<<" }" << endl;
			node->print();
		}
	}

	void Statement::print()
	{
		std::cout << "{ " << "type:" << this->value.tag << " | " 
			<< "Statements" << " }" << " ";
		for (auto item : list)
		{
			item->print();
		}
	}

	void Expression::print()
	{
		std::cout << "{ " << "type:" << this->value.tag 
			<< " | " << this->value.value << " }" << " ";
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
		cout << "ComparisonExp-->" << endl;
		left->print();
		cout << "{ " << "type:" << op.tag << " | " << op.value << " }" << " ";
		right->print();
		cout << endl;
	}

	void IfStmt::print()
	{
		cout << endl;
		cout << "IfStatement-->" << endl;
		condition->print();
		this->thenBody->print();
		if (this->haveElse)
		{
			this->elseBody->print();
		}
		cout << endl;
	}

	void WhileStmt::print()
	{
		cout << endl;
		cout << "WhileStatement-->" << endl;
		condition->print();
		this->body->print();
		cout << endl;
	}

	void RepeatStmt::print()
	{
		cout << endl;
		cout << "RepeatStmt-->" << endl;
		untilCond->print();
		this->body->print();
		cout << endl;
	}

	void ForStmt::print()
	{
		cout << endl;
		cout << "ForStmt-->" << endl;
		startStmt->print();
		endValue->print();
		startStmt->print();
		this->body->print();
		cout << endl;
	}

	void CaseStmt::print()
	{
		cout << endl;
		cout << "CaseStmt-->" << endl;
		condition->print();
		for (auto item : this->body)
		{
			item->print();
		}
		cout << endl;
	}

	void FuncCall::print()
	{
		cout<<this->funcName<<"( ";
		for (auto item : this->constants)
		{
			cout<<item.value<<" ";
		}
		cout <<" )"<< endl;
	}

	void FunctionStmt::print()
	{
		cout << this->funcDecl->name << " " << this->funcDecl->returnType << endl;
		this->body->print();
	}

	void ConstantStmt::print()
	{
		if (this->constRoot.size() == 0 || this->constDeclare != NULL)
		{
			cout << "Constant:" << this->constDeclare->name << endl;
		}
		if (this->constRoot.size() > 0)
		{
			for (auto item : this->constRoot)
			{
				item->print();
			}
		}
	}

	void TypeStmt::print()
	{
		if (this->typeRoot.size() == 0 || this->typeDeclare != NULL)
		{
			cout << "Constant:" << this->typeDeclare->name << endl;
		}
		if (this->typeRoot.size() > 0)
		{
			for (auto item : this->typeRoot)
			{
				item->print();
			}
		}
	}
	void VariableStmt::print()
	{
		if (this->varRoot.size() == 0 || this->varDeclare != NULL)
		{
			cout << "Constant:" << this->varDeclare->name << endl;
		}
		if (this->varRoot.size() > 0)
		{
			for (auto item : this->varRoot)
			{
				item->print();
			}
		}
	}
	//Node =
	bool Node::operator=(Node &node)
	{
		this->value = node.value;
		this->parent = node.parent;
		//copy(node.list.begin(), node.list.end(), this->list);//没有分配空间出错
		list.resize(node.list.size());//必不可少
		std::copy(node.list.begin(), node.list.end(), std::back_inserter(list));
		return 1;
	}
	//Error
	void Error::errorPrint()
	{
		std::cout << errToken.value << " " 
			<< errorStr << " in line " << errLine << std::endl;
	}
	///-----------------------------------------------------------------------
	///Parser begin-----------------------------------------------------------
	///-----------------------------------------------------------------------
	bool Parser::advance(Tag t)//超前查看可选的Follow，没有的话直接跳过
	{
		if ((it + 1)->tag == t)
		{
			return true;
		}
		else
			return false;
	}

	bool Parser::find(Tag t)//可选的Follow，没有的话直接跳过
	{
		if (it->tag == t)
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
			Error err("syntax error", *it);
			errList.push_back(err);
			return false;
		}
	}

	void Parser::parseProgram()
	{
		if (it->tag == PROGRAM)
		{
			it++;
			//根节点生成
			shared_ptr<Node> beginNode = make_shared<Node>();
			beginNode->value = *it;
			root = beginNode;
			currNode = root;
			it++;
			match(SEMI);
			parseBlock();
		}
		else
		{
			//unexpected symbol error
			Error err("unexpected symbol error", *it);
			errList.push_back(err);
		}
		if (it->tag != FINISH)
		{
			//missing the finish symbol
			Error err("missing the finish symbol", *it);
			errList.push_back(err);
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
				break;
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
					shared_ptr<Statement> block = 
						parseStmtList(currNode == root ? FINISH : END);
					//root的block与函数的block
					if (currNode != root)
					{
						(static_pointer_cast<FunctionStmt>(currNode))->body=block;
					}
					else
					{
						currNode->addNode(block);
					}
					bool tmp=it->tag == FINISH || it->tag == END;
					if (it->tag == END)
					{
						it++;
						match(SEMI);
					}
				}
				break;
		}
	}
	//函数和过程的通用解析方法
	shared_ptr<Statement> Parser::parseProcedureBase(bool isFunction)
	{
		shared_ptr<FunctionStmt> funcStmt = make_shared<FunctionStmt>();
		shared_ptr<FunctionDecl> funcDecl = make_shared<FunctionDecl>();
		funcStmt->funcDecl = funcDecl;
		//保存当前结点，将当前结点指向函数结点
		currNode->addNode(funcStmt);
		shared_ptr<Node> tmpNode = currNode;
		currNode = funcStmt;
		//获取函数名
		it++;
		if (it->tag == IDENT)
		{
			funcDecl->name = it->value;
		}
		//匹配左括号
		it++;
		match(OpenBracket);
		//解析参数
		it++;
		while (it->tag != CloseBracket)
		{
			vector<std::string> args;
			while (!args.empty())args.pop_back();
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
					shared_ptr<VariableDecl> varDecl = make_shared<VariableDecl>();
					varDecl->name = val;
					varDecl->type = DeclaredType::Variable;
					varDecl->identity = *it;
					//shared_ptr<VariableStmt> varStmt = make_shared<VariableStmt>();
					//varStmt->varDeclare = varDecl;
					funcDecl->vars.push_back(varDecl);
				}
			};
			//-----------------------------------------------
			if (it->tag == VAR)//有VAR为引用传递，否则为值传递
			{
				it++;
				parseArgs();
			}
			else
			{
				parseArgs();
			}
			it++;//match(";") or ")"
			if (find(SEMI))//如果是;,继续循环，否则应该是）,退出循环
			{
				it++;
			}
			else
			{
				break;
			}
		}
		it++;
		match(Colon);
		it++;//return value
		funcDecl->type = DeclaredType::Procedure;
		if (isFunction)//函数有返回值,过程没有
		{
			funcDecl->type = DeclaredType::Function;
			funcDecl->returnType = it->value;
			it++;
		}
		match(SEMI);
		if (!advance(FORWORD))//是否有forword关键字声明
		{
			//var or begin
			parseBlock();//无论下一个Token是VAR还是BEGIN，都可以用Block解析,且在内部加入到currentnode
			currNode = tmpNode;
			return funcStmt;
		}
		else
		{
			it++;
			match(FORWORD);
			it++;
			match(SEMI);
			return funcStmt;
		}
	}

	shared_ptr<Statement> Parser::parseProcedure()
	{
		return parseProcedureBase(false);
	}
	shared_ptr<Statement> Parser::parseFunction()
	{
		return parseProcedureBase(true);
	}

	shared_ptr<VariableStmt> Parser::parseVariables()
	{
		shared_ptr<VariableStmt> rootVal = make_shared<VariableStmt>();
		rootVal->varDeclare = NULL;
		while (advance(IDENT))
		{
			auto varDecl = parseVariable();
			auto varStmt = make_shared<VariableStmt>();
			varStmt->varDeclare = varDecl;
			rootVal->varRoot.push_back(varStmt);
		}
		currNode->addNode(rootVal);
		return rootVal;
	}

	shared_ptr<VariableDecl> Parser::parseVariable()
	{
		auto varDecl= make_shared<VariableDecl>();
		it++;
		varDecl->name = it->value;
		varDecl->type = DeclaredType::Variable;
		it++;
		match(Colon);
		it++;
		varDecl->identity = *it;
		it++;
		match(SEMI);
		return varDecl;
	}

	shared_ptr<TypeStmt> Parser::parseType()//暂不支持数组和枚举类型
	{
		it++;
		auto typeRootVal = make_shared<TypeStmt>();
		typeRootVal->typeDeclare = NULL;
		while (it->tag == IDENT)
		{
			auto typeDecl=make_shared<TypeDecl>();

			auto typeStmt = make_shared<TypeStmt>();
			typeStmt->typeDeclare = typeDecl;

			typeDecl->name = it->value;
			typeDecl->type = DeclaredType::Record;
			
			it++;
			match(EQ);
			it++;
			switch (it->tag)
			{
				case RECORD:
					while ((it + 1)->tag != END)
					{
						auto record1=parseVariable();
						auto varStmt = make_shared<VariableStmt>();
						varStmt->varDeclare = record1;
						typeDecl->vars.push_back(record1);
						typeStmt->varStmts.push_back(varStmt);
					}
					it++;//match(END);
					it++;
					match(SEMI);
					break;
				default:
					break;
			}
			typeRootVal->typeRoot.push_back(typeStmt);
		}
		currNode->addNode(typeRootVal);
		return typeRootVal;
	}

	shared_ptr<ConstantStmt> Parser::parseConstant()
	{
		shared_ptr<ConstantStmt> rootVal = make_shared<ConstantStmt>();
		rootVal->constDeclare = NULL;
		it++;
		while (it->tag == IDENT)
		{
			auto constDecl= make_shared<ConstantDecl>();
			auto constStmt = make_shared<ConstantStmt>();
			constStmt->constDeclare = constDecl;
			constDecl->name = it->value;
			constDecl->type = DeclaredType::Constant;
			it++;
			match(BIND);
			it++;
			constDecl->value = *it;
			it++;
			match(SEMI);
			++it;
			rootVal->constRoot.push_back(constStmt);
		}
		currNode->addNode(rootVal);
		it--;
		return rootVal;
	}

	shared_ptr<AssignStmt> Parser::parseAssign()
	{
		auto assign = make_shared<AssignStmt>();
		auto exp = make_shared<Expression>();
		exp->value = *it;
		assign->left = exp;
		it++;
		match(BIND);
		assign->value = *it;
		it++;
		assign->right = parseExpression();
		//it++;
		if (!(find(SEMI) || find(TO)))
		{
			Error err("syntax error", *it);
			errList.push_back(err);
		}
		//记录赋值表达式的id到符号表
		return assign;
	}

	shared_ptr<ComparisonExp> Parser::parseComparison()//there must be an error
	{
		auto compareStmt = shared_ptr<ComparisonExp>();
		auto exp = parseExpression();
		compareStmt->left = exp;
		it++;
		compareStmt->op = *it;
		it++;
		compareStmt->right = parseExpression();
		return compareStmt;
	}

	shared_ptr<Expression> Parser::parseExpression()
	{
		auto rootExp = parseExpression1();
		it++;
		
		while (it->tag == OR || it->tag == AND)
		{
			auto opExp = make_shared<Expression>();
			opExp->addNode(rootExp);
			opExp->value = *it;
			it++;
			opExp->addNode(parseExpression1());
			rootExp = opExp;
		}
		return rootExp;
	}

	shared_ptr<Expression> Parser::parseExpression1()
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
		auto rootExp = parseTerm();
		it++;
		while (it->tag == ADD || it->tag == SUB || isRealOp(it->tag))
		{
			auto opExp = make_shared<Expression>();
			opExp->addNode(rootExp);
			opExp->value = *it;
			it++;
			opExp->addNode(parseTerm());
			rootExp = opExp;
			it++;
		}
		if (!(it->tag == ADD || it->tag == SUB || isRealOp(it->tag)))
		{
			it--;
		}
		return rootExp;
	}

	shared_ptr<Expression> Parser::parseTerm()
	{
		auto rootExp = parseFactor();
		it++;
		
		while (it->tag == MUL || it->tag == DIV )
		{
			auto opExp = make_shared<Expression>();
			opExp->value = *it;
			opExp->addNode(rootExp);
			it++;
			opExp->addNode(parseTerm());
			rootExp = opExp;
			it++;
		}
		if (!(it->tag == MUL || it->tag == DIV))
		{
			it--;
		}
		return rootExp;
	}

	shared_ptr<Expression> Parser::parseFactor()
	{
		if (it->tag == IDENT || it->tag == INT || it->tag == FLOAT)
		{
			auto exp = make_shared<Expression>();
			exp->value = *it;
			return exp;
		}
		else if (it->tag == OpenBracket)
		{
			auto exp = parseExpression();
			match(CloseBracket);
			return exp;
		}
	}

	shared_ptr<Statement> Parser::parseStmtList(Tag teminator)
	{
		auto tmpNode = currNode;
		shared_ptr<Statement> stList = make_shared<Statement>();
		currNode = stList;//所有子节点将加入到block上

		stList->value = *it;
		it++;
		while (it->tag != teminator)
		{
			if (it->tag == END && teminator != END)//处理begin..end的匹配情况
			{
				shared_ptr<Statement> st1 = make_shared<Statement>();
				st1->value = *it;
				stList->addNode(st1);
				it++;
			}
			shared_ptr<Statement> st = parseStatement();
			stList->addNode(st);
			it++;
		}
		//恢复currNode
		currNode = tmpNode;
		return stList;
	}
	//Core Method
	shared_ptr<Statement> Parser::parseStatement()
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
					shared_ptr<AssignStmt> assign = parseAssign();
					//currNode->addNode(assign);
					return assign;
				}
				else if (advance(OpenBracket))
				{
					shared_ptr<FuncCall> funcCall = parseFunctionCall();
					//currNode->addNode(funcCall);
					return funcCall;
				}
			//暂不支持嵌套函数
			/*case FUNCTION:
				break;
			case PROCEDURE:
				break;*/
			case IF:
			{
				shared_ptr<Node> tmp = currNode;
				shared_ptr<IfStmt> ifstmt = static_pointer_cast<IfStmt>(parseIf());
				currNode = tmp;
				//currNode->addNode(ifstmt);
				return ifstmt; 
			}
			case WHILE:
			{
				shared_ptr<Node> tmp = currNode;
				shared_ptr<WhileStmt> whStmt = static_pointer_cast<WhileStmt>(parseWhile());
				currNode = tmp;
				//currNode->addNode(whStmt);
				return whStmt; 
			}
			case REPEAT:
			{
				shared_ptr<Node> tmp = currNode;
				shared_ptr<RepeatStmt> repeatStmt = static_pointer_cast<RepeatStmt>(parseRepeat());
				currNode = tmp;
				//currNode->addNode(repeatStmt);
				return repeatStmt; 
			}
			case FOR:
			{
				shared_ptr<Node> tmp = currNode;
				shared_ptr<ForStmt> forStmt = static_pointer_cast<ForStmt>(parseFor());
				currNode = tmp;
				//currNode->addNode(forStmt);
				return forStmt; 
			}
			case CASE:
			{
				shared_ptr<Node> tmp = currNode;
				shared_ptr<CaseStmt> caseStmt = static_pointer_cast<CaseStmt>(parseCase());
				currNode = tmp;
				//currNode->addNode(caseStmt);
				return caseStmt; 
			}
			default:
				return NULL;
		}
		return NULL;
	}

	shared_ptr<Statement> Parser::parseIf()
	{
		shared_ptr<IfStmt> ifstmt = make_shared<IfStmt>();
		ifstmt->value = *it;
		currNode = ifstmt;
		it++;
		auto cond = parseExpression();
		ifstmt->condition = cond;
		match(THEN);
		it++;
		if (match(BEGIN))
		{
			currNode = ifstmt;
			shared_ptr<Statement> body = parseStmtList(END);
			ifstmt->thenBody=body;
			match(END);
			it++;
			match(SEMI);
		}
		else
		{
			shared_ptr<Statement> body1 = parseStatement();
			ifstmt->thenBody = body1;
		}
		ifstmt->haveElse = false;
		while (advance(ELSE))
		{
			ifstmt->haveElse = true;
			it++;
			shared_ptr<ElseStmt> elseStmt = static_pointer_cast<ElseStmt>(parseElse());
			ifstmt->elseBody = elseStmt;
		}
		return ifstmt;
	}

	shared_ptr<Statement> Parser::parseElse()
	{
		shared_ptr<ElseStmt> elseStmt = make_shared<ElseStmt>();
		elseStmt->value = *it;
		it++;
		if (match(IF))//match if
		{
			shared_ptr<IfStmt> elseifnode = static_pointer_cast<IfStmt>(parseIf());
			elseStmt->addNode(elseifnode);
		}
		else if (match(BEGIN))//match begin Compound Statement 复合语句
		{
			shared_ptr<Statement> body = parseStmtList(END);
			elseStmt->body = body;
			match(END);
			it++;
			match(SEMI);
		}
		else//单行语句
		{
			shared_ptr<Statement> body1 = parseStatement();
			elseStmt->addNode(body1);
		}
		return elseStmt;
	}

	shared_ptr<Statement> Parser::parseWhile()
	{
		shared_ptr<WhileStmt> whStmt = make_shared<WhileStmt>();
		whStmt->value = *it;
		currNode = whStmt;
		it++;
		shared_ptr<Expression> cond = parseExpression();
		whStmt->condition = cond;
		//it++;
		match(DO);
		it++;
		if (match(BEGIN))
		{
			shared_ptr<Statement> body = parseStmtList(END);
			whStmt->body=body;
			match(END);
			it++;
			match(SEMI);
		}
		else
		{
			shared_ptr<Statement> body1 = parseStatement();
			whStmt->body = body1;
		}
		return whStmt;
	}

	shared_ptr<Statement> Parser::parseRepeat()
	{
		shared_ptr<RepeatStmt> repeatStmt = make_shared<RepeatStmt>();
		currNode = repeatStmt;
		shared_ptr<Statement> body=parseStmtList(UNTIL);
		repeatStmt->body = body;
		match(UNTIL);
		shared_ptr<Expression> cond = parseExpression();
		repeatStmt->untilCond = cond;
		return repeatStmt;
	}

	shared_ptr<Statement> Parser::parseCase()
	{
		shared_ptr<CaseStmt> caseStmt = make_shared<CaseStmt>();
		caseStmt->value = *it;
		currNode = caseStmt;

		shared_ptr<Expression> cond = parseExpression();
		caseStmt->condition = cond;
		match(OF);

		while (it->tag == INT || it->tag == CHAR)
		{
			shared_ptr<Statement> branch = parseCaseBranch();
			caseStmt->body.push_back( branch);
			it++;
		}
		match(END);
		it++;
		match(SEMI);
		return caseStmt;
	}

	shared_ptr<Statement> Parser::parseCaseBranch()
	{
		shared_ptr<Statement> caseBranch = make_shared<Statement>();
		shared_ptr<Expression> constNode = parseCaseConst();
		caseBranch->addNode(constNode);
		match(Colon);
		shared_ptr<Statement> st = parseStatement();
		caseBranch->addNode(st);
		return caseBranch;
	}

	shared_ptr<Expression> Parser::parseCaseConst()
	{
		shared_ptr<Expression> constNode = make_shared<Expression>();
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

	shared_ptr<FuncCall> Parser::parseFunctionCall()
	{
		shared_ptr<FuncCall> funcCall = make_shared<FuncCall>();
		funcCall->funcName = it->value;
		++it;
		match(OpenBracket);
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
			if (it->tag != CloseBracket)
			{
				match(Comma);
				it++;
			}
			else
				break;
		}
		match(CloseBracket);
		it++;
		match(SEMI);
		return funcCall;
	}

	shared_ptr<Statement> Parser::parseFor()
	{
		//for k=1 to 10 do 
		shared_ptr<ForStmt> forStmt = make_shared<ForStmt>();
		forStmt->value = *it;
		currNode = forStmt;
		it++;
		shared_ptr<AssignStmt> startVal = static_pointer_cast<AssignStmt>(parseStatement());
		forStmt->startStmt = startVal;

		Tag direction=it->tag;
		forStmt->direction = direction;
		bool tmpCheck=find(TO)||find(DOWNTO);
		if (!tmpCheck)
		{
			Error err("syntax error", *it);
			errList.push_back(err);
		}
		it++;
		auto endVal = parseExpression();
		forStmt->endValue = endVal;

		match(DO);
		it++;
		if (match(BEGIN))
		{
			shared_ptr<Statement> body = parseStmtList(END);
			forStmt->body = body ;
			match(END);
			it++;
			match(SEMI);
		}
		else
		{
			shared_ptr<Statement> body1 = parseStatement();
			forStmt->body = body1;
		}
		return forStmt;
	}
}
/*
if (isFunction)
{
if (symStack->lookup(funcStmt->funcName) != NULL)
{
shared_ptr<Declaration> decl = symStack->lookup(funcStmt->funcName);
symStack->pop();
shared_ptr<SymbolTable> symTable2 = symStack->getTable(symStack->currNestLevel);
symTable2->add(funcStmt->funcName, decl);
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
*/