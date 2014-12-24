#include "parser.h"
#include "SemanticAnalyzer.h"
#include "CodeGen.h"

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	{
		swd::Lexer lexer;
		lexer.readSource("2.txt");
		lexer.scan();
		/*for (auto i : lexer.tokenStream)
		{
		std::cout <<"<# "<< i.value << "  ||  " << i.tag <<"  ||   "<<i.line<<" #>"<< std::endl;
		}*/
		shared_ptr<swd::SymbolTableStack> symStack = make_shared<swd::SymbolTableStack>();
		symStack->currNestLevel = 0;
		swd::Parser parser(&lexer);
		//parser.symStack = symStack;
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
		symStack = semanticAnalyzer.symStack;//defined above the parser object--
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