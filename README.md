Compiler2014
=============

![type](https://img.shields.io/badge/type-library-pink.svg)
![platform](https://img.shields.io/badge/platform-windows-brightgreen.svg)
![build](https://img.shields.io/wercker/ci/wercker/docs.svg)
![license](https://img.shields.io/aur/license/yaourt.svg)

### my compiler for *pascal* (C++ Edition)

<b>I have used many C++11 features in my code.<b/>

So VC++2012 or 2013 is needed.(I use *VS2013 community edition*)

###Timeline

> 2014/08/31---09/30   compiler v0.1 生成语法树，生成中间代码

> 2014/12/23---12/27   compiler v1.0 更改中间代码表示形式，确定指令的执行方式，
                                   > 添加解释指令的虚拟机

> 2015/01/14---01/15   compiler v1.1 添加IO函数，修改bug

--------------------------------------------------------------------------

语法不过是借了Pascal个壳子，其实我并不想做Pascal。只是C++或者Java编译器做起来比较烦，噪音或者说

语法冗余非常多，核心反而不突出，容易陷入细节不可自拔。所以选择语法定义比较精确的Pascal。至于Pascal

各种语法是否精确，见仁见智。至少我觉得歧义比较少，parser比较简单。

### 支持以下Pascal语言特性：

> 1.基本数学运算和赋值

> 2.float int string类型变量，常量，record声明

> 3.write(NumberOrString)库函数

> 4.if-else while-do for语句

> 5.自定义函数

### pascal源文件为*.pas 中间代码格式为 *.pasc

--------------------------------------------------------------

欲编译新的文件，请更改main函数的lexer.readSource()的参数.

&copy; Apache License 2.0 2014 *BUPT*  *WangHesai* 作者保留一切解释权
