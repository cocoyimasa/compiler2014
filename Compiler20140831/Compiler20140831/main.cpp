#include "parser.h"
#include "SemanticAnalyzer.h"
#include "CodeGen.h"
#include "SymbolTable.h"
#include "VirtualMachine.h"
#include <time.h>
#include <fstream>
int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	{
		swd::Lexer lexer;
		lexer.readSource("1.pas");
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
			std::cout << "building...no error!\n";
		}
		//parser.root->print();//print synatatic tree
		swd::SemanticAnalyzer semanticAnalyzer;
		parser.root->accept(&semanticAnalyzer);
		symStack = semanticAnalyzer.symTable;//defined above the parser object--
		compiler::IRCodeGen codeGen;
		parser.root->genCode(&codeGen);

		//output IR code in a file
		ofstream f("out.pasc");
		for (auto item : codeGen.IRCodeFile)
		{
			//cout << item->toString() << endl;

			f << item->toString() << endl;
		}

		vm::VirtualMachine myVM(&codeGen, symStack);
		//系统时间
		time_t tim;
		struct tm *at;
		char now[80];
		time(&tim);
		at = localtime(&tim);
		strftime(now, 79, "%Y-%m-%d %H:%M:%S", at);
		std::cout << now << "---------" << "Compiled.-------------------------" << endl;
		//更改控制台字体颜色
		HANDLE hOut= GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hOut,
			FOREGROUND_RED |      // 前景色_红色
			FOREGROUND_INTENSITY); // 前景色_加强

		///////////////////////////////
		myVM.run();
		///////////////////////////////
		//白色字体
		SetConsoleTextAttribute(hOut,
			FOREGROUND_RED |   
			FOREGROUND_GREEN | 
			FOREGROUND_BLUE);  
		std::cout << "press Enter to exit." << endl;
	}
	getchar();
	return 0;
}