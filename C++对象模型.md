[TOC]

# C++对象模型

## 第一章 关于对象

1. `C++` 使用封装以后，其布局成本并没有增加，每个 `data member` 直接内含在一个 `class object` 之中，`member function` 虽然含在 `class` 的声明之内，但是不会出现在 `object` 之中，每一个 `non-inline member function` 只会产生一个函数实例。至于每一个“拥有零个或一个定义”的 `inline function`则会在其每一个使用者（模块）身上产生一个函数实例。 也就是说使用 `#include` 的时候，每个翻译单元都会有该 `inline function` ，`inline`函数可以在多个编译单元中定义，但所有定义必须**严格一致**（语法和语义相同）。即使每个编译单元生成了一个函数实例，链接器也会将它们视为同一个实体，最终只保留一个实例或合并它们，不会报重复定义错误。`inline` 主要有两个作用：
   * 优化提示，建议编译器将函数内联展开
   * 允许函数在多个翻译单元（即不同的.cpp文件）中拥有相同的定义，而不会引发链接错误

### 1.1 C++对象模式

1. **简单对象模型**：一个 `object` 就是一系列的 `slots`，每一个 `slot`  指向一个 `member` ，`member` 按照其声明顺序，各被指定一个 `slot` 。这种模型降低了 `C++` 编译器的设计复杂度，但是空间和执行期效率较低。也就是说，每一个 `object` 并不存放 `member` 本身，存放的指示指向 `member` 的指针。

2. **表格驱动对象模型**：对所有 `class` 的所有 `object` 都有一致的表达方式，将 `member` 信息抽取出来，放在一个 `data member table` 和 一个 `member function table` 之中，而每一个 `object` 中存放两个指针，分别指向该两个 `table` 。

3. **C++对象模型：**`Nonstatic data members` 存放在每一个 `object` 中，`static data member 、 static nonstatic function member` 被存放在 `object` 之外。对于 `virtual function` ，通过以下两个机制：

   * 每一个 `class` 产生一堆指向 `virtual function` 的指针，放在表格之中，即 `virtual table(vtbl)`。
   * 每一个`object` 被安插一个指针，指向相关的 `virtual table` ，通常该指针被称为 `vptr`。该指针的相关操作是由每个类的构造函数、析构函数、拷贝赋值运算符自动完成。**每一个类所关联的 `type_info object` 也由 `vtbl` 指出，通常放在表格的第一个 `slot` **。
     ps. `type_info` 是 C++ 标准库中定义的一个类（位于 `<typeinfo>` 头文件），用于表示一个类型的运行时信息。

   对于**继承**模型，C++最初的时候不采用任何间接性， `base class subobject` 的 `data member` 被直接放置在 `derived class object` 中，这保证了对于基类数据成员最紧凑和最有效率的存储，但是一旦基类发生改变，所有派生类都需要重新编译。加入虚继承以后，则是在 `class object` 中为每一个有关联的 `virtual base class` 加上一个指针。

### 1.2 关键词所带来的差异

1. `C++` 中凡处于同一个 `access section` 的数据，必定保证以其声明顺序出现在内存布局当中，然后被放置在多个 `access section` 的数据，排列顺序不做保证。

### 1.3 对象的差异

1. `C++` 程序设计模型直接支持三种 `programming paradigms` :
   * 程序模型：即 `C` 语言这种编程范式。
   * 抽象数据类型模型：所谓抽象是和一组表达式（public接口）一起提供。处理的是具体实例，在编译使其就已经完全定义好了。
   * 面向对象模型：有一些彼此相关的类型， 通过一个抽象的 `base class` (提供接口)被封装起来。原则上被指定的 `object` 的真实类型在每一个特定执行点之前是无法解析的，通过 `pointer` 和 `reference` 间接处理，实现多态。
2. 一个 `class object` 的内存大小一般包含以下几个方面：
   * `nonstatic data members` 的总和大小
   * 对齐要求填补的空间
   * 为了支持 `virtual` 产生的开销

## 第三章 Data语意学

### 3.1 Data Member 的绑定

1. 一个 `inline` 函数实体，在整个 `class` 声明未被完全看见之前，是不会被评估求值的，也就是说，对于 `member function` 本体的分析，会直到整个 `class` 的声明都出现了才开始。然而，这对于 `member function` 的 `argument list` 并不为真，参数列表中的名称韩式会在它们第一次遭遇时被适当地决议完成。

### 3.2 Data Member 的布局

1. 对于一个类中的 `Nonstatic data members`，在 `class object` 中的排列顺序将和其被声明的顺序一样，任何中间的 `static data members` 都不会被放入对象布局之间，而是存放在程序的 `data segment` 中，和个别的 `class object` 无关。**C++ 标准规定，在同一个 `access section` 中的数据成员，它的排列只需要符合”较晚出现的成员在 `class object` 中有较高的地址“，并不一定要连续排列。同时 `access section` 的排列顺序不做规定。**

### 3.3 Data Member 的存取

#### Static Data Members

1.  每一个 `static data member`  只有一个实例，存放在程序的 `data segment` 之中，每次程序访问 `static member` 时，会被内部转化为对该唯一 `extern` 实例的直接访问操作。
   ```cpp
   class Point3d{
       public:
       	// ...
       private:
       	float x;
       	static const int chunkSize = 250;
       	float y;
       	float z;
   };
   Point3d origin, *pt = &origin;
   
   //origin.chunkSize == 250;
   Point3d::chunkSize == 250;
   //pt->chunkSize == 250;
   Point3d::chunkSize == 250;
   ```

   从指令执行的观点来看，这是 **C++语言中通过一个指针和通过一个对象来存取 `members` 时，结论完全相同的唯一一种情况**。通过 `.` 和 `->` 运算符对一个 `static data member` 进行存取操作只是为了文法上的方便。

2. 取一个 `static data member` 的地址，会得到一个指向其数据类型的指针，而不是一个指向其 `class member` 的指针。

3. 如果有多个 `class` 声明了一个 `static member` ，那么当它们都被放在程序的 `data segment` 时，就会导致名称冲突，编译器会使用 `name-mangling`，暗中对每一个 `static data member` 编码。

#### Nonstatic Data Members

1. `Nonstatic data members` 直接存放在每一个 `class object` 之中，除非通过显式的或隐式的（成员函数中的 `this` 指针）`class object` ，否则没有办法直接存取它们。

2. 想对一个 `nonstatic data member` 进行存取操作，编译器需要把 `class object` 的起始地址加上 `data member` 的偏移位置，比如：
   ```cpp
   origin._y = 0;
   //&origin._y 将等于
   &origin + (&Point3d::y - 1);
   ```

   此处的 `-1` 操作是因为指向 `data member`  的指针，其 `offset` 值总是被 `+1` ，这样可以使得编译系统区分出”一个指向 `data member` 的指针，用以指出 `class` 的第一个 `member` “ 和 ”一个指向 `data member` 的指针，没有指出任何 `member` “ 两种情况。**这是因为C++中一个类如果没有任何数据成员，则会插入一字节的数据从而保证两个不同的类对象有不同的地址**。

3. ```cpp
   point3d origin, *pt = &origin;
   origin.x = 0;
   pt->x = 0;
   ```

   从 `origin` 和 `pt` 中存取的重大差异在于，当 `point3d` 是一个 `derived class`，并且其继承结构中有一个 `virtual base class` ，并且被存取的 `member` 是从 `virtual base class` 继承而来的 `member` 时，我们无法在编译器知道 `pt` 必然指向哪个类型，也就不能在编译时期来确定这个 `member` 的 `offset` ，所以这个存取操作延迟到执行期执行。

### 3.4 ”继承“与Data Member

1. 在 C++ 继承模型中，一个 `derived class object` 所表现出的东西，是自己的 `members` 加上其 `base class members` 的总和。但是基类和派生类数据成员的排列顺序，并没有在 C++ 标准中强制指定。

#### 只要继承不要多态

1. C++ 语言保证：**出现在 `derived class` 中的 `base class subobject` 有其完整原样性**。
   ```cpp
   class Concrete{
       public:
       	//...
       private:
       	int val;
       	char c1;
       	char c2;
       	char c3;
   };//四字节对齐，所以占八个字节
   
   //分裂成三层结构
   class Concrete1{
   	private:
       	int val;
       	char c1;
   }; //8个字节
   class Concrete2 : public Concrete1{
       private:
       	char c2;
   }; //保持Concrete1的完整，8个字节，自己补齐，4个字节，共12个字节
   class Concrete3 : public Concrete2{
       private:
       	char c3;
   }; //保持Concrete2的完整，12个字节，自己补齐，4个字节，共16个字节
   ```

   这样子设计的目的是因为当派生类对象被赋值或传递给基类对象时，会发生**对象切片**（基类部分被复制，派生类部分被丢弃）。如果派生类覆盖了基类的填充空间，基类子对象的内存布局会被破坏，导致未定义行为。或者当用一个基类对象去赋值一个派生类对象时，应该只拷贝基类对象有的那些 `member` ，如果不保留这些空闲空间，可能就会导致派生类对象中的成员有非法值。

#### 加上多态

1. 使用虚函数在空间和时间上的额外开销：
   * 导入一个 `virtual table`，用来存放它所声明的每一个 `virtual function` 的地址，这个 `table` 的元素个数一般而言是声明的虚函数的个数，再加上一两个 `slots` 用来支持 `runtime type identification`（RTII）（`typeid` `dynamic_cast` ）。
   * 在每一个 `class object` 中导入一个 `vptr` ，使得每一个 `object` 可以找到对应的 `virtual table`。
   * 加强 `constructor` ，使得它可以为 `vptr` 设置正确的初值。
   * 加强 `deconstructor` ，去删除 `vptr` 。
2.  现在的编译器一般将 `vptr` 放置在 `class object` 的前端。





