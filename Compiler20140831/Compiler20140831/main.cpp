#include "parser.h"
#include "SemanticAnalyzer.h"
#include "CodeGen.h"
#include "SymbolTable.h"
int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	{
		swd::Lexer lexer;
		lexer.readSource("2.txt");
		lexer.scan();
		/*for (auto i : lexer.tokenStream)
		{
		std::cout <<"{ "<< i.value << "  ||  " << i.tag <<"  ||   "<<i.line<<" }"<< std::endl;
		}*/
		/*shared_ptr<swd::SymbolTable> symStack1 = NULL;
		shared_ptr<swd::SymbolTable> symStack = make_shared<swd::SymbolTable>();*/
		swd::SymbolTable *symStack = new swd::SymbolTable();
		swd::Parser parser(&lexer);

		parser.parseProgram();
		if (parser.errList.size() >= 1)
		{
			for (auto err : parser.errList)
			{
				err.errorPrint();
			}
		}
		else
		{
			std::cout << "Parse成功，没有错误!^_^\n";
		}
		parser.root->print();
		swd::SemanticAnalyzer semanticAnalyzer;
		parser.root->accept(&semanticAnalyzer);
		symStack = semanticAnalyzer.symTable;//defined above the parser object--
		compiler::IRCodeGen codeGen;
		parser.root->genCode(&codeGen);
		for (auto item : codeGen.IRCodeFile)
		{
			cout << item->toString() << endl;
		}
	}
	getchar();
	return 0;
}