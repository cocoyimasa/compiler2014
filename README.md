#compiler2014

============

### my compiler for pascal (C++ Edition)

I have used many C++11 features in my code.

So VC++2012 or 2013 is needed.(I use VS2013 community edition)

###Timeline

> 2014/08/31---09/30   compiler v0.1 生成语法树，生成中间代码

> 2014/12/23---12/27   compiler v1.0 更改中间代码表示形式，确定指令的执行方式，
                                   > 添加解释指令的虚拟机

> 2015/01/14---01/15   compiler v1.1 添加IO函数，修改bug

--------------------------------------------------------------------------

### 支持以下Pascal语言特性：

> 1.基本数学运算和赋值

> 2.float int string类型变量，常量，record声明

> 3.write(NumberOrString)库函数

> 4.if-else while-do for语句

> 5.自定义函数

pascal源文件为*.pas 中间代码格式为 *.pasc

--------------------------------------------------------------

欲编译新的文件，请更改main函数的lexer.readSource()的参数.

&copy; 2014 ###BUPT  WangHesai
