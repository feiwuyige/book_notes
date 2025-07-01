# CMAKE
从项目出发，阅读 `CMakeLists.txt` 文件，学习一些常用的指令，以及 `CMake` 的运行逻辑。
1. cmake 文档中的 `<>` 表示需要用户替换的内容。
2. cmake 中的变量名是大小写敏感的。
## include<file | module>
**加载并运行** `file/module` 给出的 cmake 代码。变量的读写作用域与调用者保持一致。如果是 `module` 而不是通过 `file` 进行 include，那么会首先在变量 `CMAKE_MODULE_PATH` 的路径中寻找 `modulename.cmake` ，然后是 cmake module directory。但是如果调用 include 的 cmake file 位于 cmake 内置 module directory，则会先搜索cmake module directory，然后是 `CMAKE_MODULE_PATH`。
**ps.加载并运行指的是直接加载并执行目标文件中的所有可执行语句（可以理解为将文件中的内容直接插入到当前作用域执行）**
## project
设置项目的名称，并且将这个值存放在变量 `PROHECT_NAME` 中，如果是在最顶层的 `CMakelists.txt` 中进行调用，也会将这个值存放在变量 `CMAKE_PROJECT_NAME` 中。同时还会设置以下变量:
* PROJECT_SOURCE_DIR,<PROJECT-NAME>_SOURCE_DIR: 源码的绝对路径。
* PROJECT_BINARY_DIR,<PROJECT-NAME>_BINARY_DIR：构建生成的产物的绝对路径。

