#include "parser.h"

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	swd::Lexer lexer;
	lexer.readSource("2.txt");
	lexer.scan();
	/*for (auto i : lexer.tokenStream)
	{
		std::cout <<"<# "<< i.value << "  ||  " << i.tag <<"  ||   "<<i.line<<" #>"<< std::endl;
	}*/
	swd::SymbolTableStack *symStack = new swd::SymbolTableStack;
	symStack->currNestLevel = 0;
	swd::Parser parser(&lexer);
	parser.symStack = symStack;
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
		std::cout << "Parse成功，没有错误!^_^";
	}
	parser.root->print();
	getchar();
	return 0;
}