[TOC]

# Introduction to symbol visibility
原文链接：https://developer.ibm.com/articles/au-aix-symbol-visibility/

1. 不同的平台中，对于符号的导出有着不同的规则：
    > For example, when creating Executable and Linking Format (ELF) shared libraries on the IBM PowerLinux™ platform, by default, all the symbols are exported. While creating an XCOFF library on AIX that runs on the POWER platform, current XL C/C++ compiler may choose not to export any without the assistance of a tool.

    通过实践发现，`msvc` 默认不导出符号，`gcc clang` 默认是导出符号的。

2. 一般而言，不推荐导出模块中的所有符号，导出所需的符号即可。导出所有符号很容易在链接的时候出现符号冲突。而且假如链接的一个库中有和你的符号名称相同的符号，那么链接器可能会在解析的时候选择了库中的符号来覆盖你自己的符号。

3. 在一个 C 程序中，具有 extern 链接属性的符号会导出。
ps.
* 作用域（scope）：For each different entity that an identifier designates, the identifier is visible(can be used) only within a region of program text called its scope.There are four kinds of scopes:
    * function: A label name is the only kind of identifier that has function scope.It can be used(goto statement) anywhere in the function in which it appears,and is declared implicitly by its syntactic appearance.
    * file: If the declarator or type specifier that declares the identifier appears outside of any block or list of parameters, the identifier has file scope, which terminates at the end of the translation unit.
    * block: If the declarator or type specifier that declares the identifier appears inside a block or whthin the list of parameter declarations in a function definition, the identifier has block scope, which terminates at the end of the associated block.
    * function prototype: is a declaration of a function that declares the types of its parameters.
    作用域是相对于编译器而言，主要是看标识符的声明位置。（in a declarator or type specifier），一个声明由declarationspecifier 和 init-declarator-list 两部分组成，declarationspecifier 中有 typespecifier(int char ...)，declarator 则比如说 `* a`。
* 链接属性（linkage）：对于链接器而言，有三种：
    * external: 具有外部链接（external linkage）的特定标识符的每个声明，都指向同一个对象或函数；
    * internal: 在单个翻译单元内，具有内部链接（internal linkage）的标识符的每个声明，都指向同一个对象或函数；
    * none: 而无链接（no linkage）的标识符的每个声明，则指向一个独立的实体。

    具有文件作用域的对象标识符默认具有外部链接，函数标识符默认具有外部链接，当使用 static 关键字修饰以后，具有内部链接。以下标识符没有链接属性：
    * 除了声明为对象和函数的标识符。
    * 函数参数标识符。
    * 块作用域下的对象标识符，且没有使用 `extern` 关键字修饰。

    对于 `extern` 关键字，如果修饰的标识符在当前作用域中，之前已经通过 `static` 或者 `extern` 进行修饰，那么其链接属性不会发生改变；如果先前的声明不可见或者没有链接属性，则修饰的标识符将具有链接属性。