# make

## makefile 介绍

1. makefile的基本规则：
   ```makefile
   target : prerequisites ...
   	recipe
   ```

   `target` 可以是一个 目标文件（.o），也可以是一个可执行文件（.out），也可以是一个标签（label）

   `prerequisites` 是该 target 依赖的文件

   `recipe` 该 target 要执行的命令（任意的 shell 命令）

   **只要 `prerequisites`中有任意一个文件比 `target` 新，就会执行命令。** 

2.  make 的默认工作方式（输入make）:

   * 在当前文件夹中寻找 `Makefile`
   * 找到文件的第一个目标文件
   * 如果依赖文件不存在，就会去寻找目标是依赖文件的命令，然后去执行
   * make 会一层一层的寻找依赖文件，知道编译出第一个目标文件

   第一条规则中的第一个目标被确定为最终的目标，make就是要编译出该目标。

3. 伪目标（.PHONY）:
   用于显式的声明一个目标不是实际的文件名，而是一个伪目标。主要目的是为了**避免文件名冲突和目标冲突**。

   ```makefile
   .PHONY: clean
   
   clean:
       rm -rf *.o temp
   
   ```

   此时 clean 是一个伪目标。但是如果没有 `.PHONY`，那么当前文件夹中没有 clean 这个目标文件，就会去执行命令，但是这并不是我们想要的情况。

4. makefile只有单行注释，使用 `#` 字符进行注释。

5. 在 makefile 中的命令，必须以 tab 键开始。

## 书写规则

1. makefile 中可以使用以下三个通配符：

   * `*` : 匹配任意字符，如 *.o 表示所有后缀为 .o 的文件
   * `?` : 匹配一个字符
   * `~`: 表示用户目录（UNIX），在 windows 中所指的目录依靠环境变量 HOME 。

2. 在 makefile 中定义 `VPATH` 的值，可以让 make 在当前目录找不到目标文件或者依赖文件的时候去该路径下寻找。
   ```makefile
   # 定义了两个路径，不同路径之间使用 : 进行分隔
   VPATH = src : ../headers
   ```

   另一种方式是使用 `vpath` 关键字（全小写），使用方式有以下三种：

   * `vpath <pattern> <diretories> `：在 directories 目录下寻找模式为 pattern 的文件
   * `vpath <pattern>` ：清楚符合 pattern 模式的搜索目录
   * `vpath`：清楚所有已经设置的搜索目录

   `vpath` 使用的 \<pattern> 需要包含 `%` 字符：

   ```makefile
   # % 匹配零个或多个字符
   # 在../headers 目录下寻找以 .h 结尾的文件
   vpath %.h ../headers
   ```

   

