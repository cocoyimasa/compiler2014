#include "parser.h"
#include "SemanticAnalyzer.h"
#include "CodeGen.h"
#include "SymbolTable.h"
#include "VirtualMachine.h"
#include <time.h>
#include <fstream>

char* formatTime(){
	//系统时间
	time_t tim;
	struct tm *at;
	char* now = new char[80];
	time(&tim);
	at = localtime(&tim);
	strftime(now, 79, "%Y-%m-%d %H:%M:%S\0", at);
	return now;
}
void setConsoleColor(WORD color){
#ifdef __WINDOWS
	//更改控制台字体颜色
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut, color); // 前景色_加强
#endif
}
void outputIRCode(compiler::IRCodeGen* codeGen){
	//output IR code in a file
	ofstream f("out.pasc");
	for (auto item : codeGen->IRCodeFile)
	{
		//cout << item->toString() << endl;

		f << item->toString() << endl;
	}
}
void printSyntaxTree(swd::Parser* parser){
	parser->root->print();//print synatatic tree
}
void printTokens(swd::Lexer* lexer){
	for (auto i : lexer->tokenStream)
	{
		std::cout
			<< "{ " << i.value
			<< "  ||  "
			<< i.tag << "  ||   "
			<< i.line << " }"
			<< std::endl;
	}
}
#ifdef __WINDOWS
#define DETECT_MEM_LEAK _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#else
#define DETECT_MEM_LEAK

#endif

void getParserStatus(swd::Parser* parser){
	// if errors occur, print; else show text 'no error'.
	if (parser->errList.size() >= 1)
	{
		for (auto err : parser->errList)
		{
			err.errorPrint();
		}
	}
	else
	{
		std::cout << "building...no error!\n";
	}
}

int main(int argc, char* argv[])
{
	DETECT_MEM_LEAK
	{
		string src = "1.pas";
		if (argc > 1){
			src = argv[1];
		}
		swd::Lexer lexer;
		lexer.readSource(src);
		lexer.scan();
		//printTokens(&lexer);

		/*shared_ptr<swd::SymbolTable> symStack1 = NULL;
		shared_ptr<swd::SymbolTable> symStack = make_shared<swd::SymbolTable>();*/
		swd::SymbolTable *symStack = new swd::SymbolTable();
		swd::Parser parser(&lexer);

		parser.parseProgram();
		getParserStatus(&parser);

		//printSyntaxTree(&parser);
		swd::SemanticAnalyzer semanticAnalyzer;
		parser.root->accept(&semanticAnalyzer);
		symStack = semanticAnalyzer.symTable;//defined above the parser object--
		compiler::IRCodeGen codeGen;
		parser.root->genCode(&codeGen);

		outputIRCode(&codeGen);

		vm::VirtualMachine myVM(&codeGen, symStack);

		string now = formatTime();
		std::cout
			<< now
			<< "\n------------------------\n"
			<< "Compiled."
			<< "\n------------------------\n"
			<< endl;

		setConsoleColor(
			FOREGROUND_RED |      // 前景色_红色
			FOREGROUND_INTENSITY); // 前景色_加强

		///////////////////////////////
		myVM.run();
		///////////////////////////////
		//白色字体
		setConsoleColor(
			FOREGROUND_RED |   
			FOREGROUND_GREEN | 
			FOREGROUND_BLUE);  
		std::cout << "press Enter to exit." << endl;
	}
	getchar();
	return 0;
}