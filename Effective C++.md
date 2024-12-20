# Effective C++

## 1.View C++ as a federation of languages

1. C++的组成：

   * C
   * Object-Oriented C++
   * Template C++
   * STL

2. C++高效编程守则视状况而变化，取决于你使用 C++ 的哪一部分。

   

## 2. Prefer consts, enums, and inlines to #defines

1. 记号 `pi` 也许从未被编译器看见，也许在编译器处理源码之前它就已经被预处理器移走了，于是记号名称 `pi` 有可能没有进入记号表内。最好的方法是使用常量替换宏。


   ```cpp
   #define pi 3.14
   const double pi = 3.14;
   ```

2. 当我们用常量替换宏的时候，有两个特殊情况：

   * 定义常量指针：注意 `const` 要修饰指针，而不是指针所指之物

   * class专属常量：为了将一个常量的作用域限定在一个class内部，必须要让其成为class的一个成员，为了确保此常量至多只有一份实体，则必须让它成为一个 `static` 成员。
     ```cpp
     //GamePlayer.h
     class GamePlayer{
     private:
         static const int NumTurns = 5; //声明而非定义
         int scores[NumTurns];
     };
     
     //GamePlayer.cpp
     //class专属常量且是static且为整数类型（ints,chars,bools），需特殊处理
     //如果你取某个class专属常量的地址，或你的编译器（不正确）坚持要看到一个定义式，则必须提供定义式
     const int GamePlayer::NumTurns; //定义,class常量在声明时已经获得初值，所以定义的时候不可以再设置初值
     ```

     有些编译器不匀速 `static` 成员在其声明式中获得初值，此外 “in-class 初值设定“ 也只允许对整数常量进行，但是可以将初值放在定义式中。

     但是有一种情况，就是 class 在编译期间，需要一个 class 常量值，如上面例子中的 NumTurns，如果你的编译器不允许进行  “in-class 初值设定“ ，则可以使用所谓的 **the enum hack**，其理论基础是：“一个属于枚举类型（enumerated type）的数值可权充ints被使用”:
     ```cpp
     //GamePlayer.h
     class GamePlayer{
     private:
         enum{ NumTurns = 5}; 
         int scores[NumTurns];
     };
     ```

     enum 更像 #define，例如取一个const的地址是合法的，但取一个enum的地址就不合法，而取一个＃define的地址通常也不合法。

3. 宏相对于函数而言，不会有函数调用产生的额外开销，但是宏的实现往往会出现很多错误，所以最好使用 `template inline` 来替代宏，既可以获得宏带来的效率，也有一般函数所有可预料行为以及类型安全性。

## 3.Use const whenever possible

1. `const` 出现在 `*` 左边，表示指针指向的对象是常量，出现在右边表示指针自身是常量，两边都有则表示被指物和指针两者都是常量。

2. STL 的迭代器是以指针为根据的，一个迭代器相当于一个`T*` 指针，所以如果想表示一个迭代器不可以指向其他的东西，则声明这个迭代器为 `const` 类型，相当于使用 `T* const`；如果想表示一个迭代器指向的东西不可以更改（即模拟 `const T*` ），则需要使用 `const_iterator`。
   ```cpp
   std::vector<int> vec;
   const std::vector<int>::iterator iter = vec.begin(); //相当于 T* const
   std::vector<int>::const_iterator cIter = vec.begin(); //相当于 const T*
   ```

3. 在 `cpp` 中，如果函数的返回类型是一个内置类型，那么改动函数返回值不合法。这是因为本质上 cpp 以 `by value` 的方式返回一个对象，所以其返回的是一个副本，所以返回的是一个右值，是一个临时对象。**返回一个值的方式和初始化一个变量或形参的方式完全一样：返回的值用于初始化调用点的一个临时量，该临时量就是函数调用的结果。**调用一个返回引用类型的函数得到左值，其他返回类型得到右值。

4. 成员函数是const:

   * bitwise(physical) constness: 不更改对象的任何成员变量，即不更改对象内的任何一个bit。但是存在某种情况下，对象内部的成员变量是指针，我们不改变指针的值，但通过指针改变指针指向的值，这也能通过bitwise constness 测试。**这是c++对常量性的定义。**
   * logical constness: 一个 `const ` 成员函数可以修改它处理的对象内的某些 `bits` ，但是只有在客户端侦测不出的情况下才可以。（相当于一个黑盒）

   **编译器强制实施 `bitwise constness`，但是编写程序时应当使用 概念上的常量。**

5. mutable(可变的)：该关键字可以释放掉 `non-static` 成员变量的 `bitwise constness` 约束。

6. 当 `const` 和 `non-const` 成员函数有着实质等价的实现时，令 `non-const` 版本调用 `const` 版本可以避免代码重复。**注意反着是错误的**，因为 `const` 承诺不会对对象进行改变，所以我们不能强制转换，可是 `non-const` 可以对对象进行改变。
   ```cpp
   char& const[](std::size_t position){
       return
           const_cast<char&>(static_cast<const TextBlock&>(*this)[position]);
   }
   ```



## 4.Make sure that objects are initialized before they're used

1. 通常情况下如果使用的是 `C part of C++` ，而且初始化可能会导致运行期成本时，那么就不保证会发生初始化；在其他部分，规则发生变化。所以对于array(c part of c++)不保证初始化，而 vector(stl) 则有此保证。

2. C++规定，对象的成员变量初始化动作发生在进入构造函数本体之前（初始化列表）。同时如果成员变量是  `const`  或者  `reference` ，也只能使用初始化列表，因为他们不能被赋值。

3. C++的初始化次序：

   * `base class` 更早于其 `derived class`  被初始化，而 `class` 成员变量总是以其声明次序（构造函数参数的顺序）被初始化。
   * 定义于不同编译单元内的 `non-local static` 对象的初始化次序并无明确定义。

   ps. `static` 对象，其寿命从被构造出来直到程序结束为止，包括 `global` 对象，定义于 `namespace` 作用域内的对象，在 `class` 内、函数内以及在 `file` 作用域内被声明为`static` 的对象，函数内的 `static` 对象称为 `local static` 对象，其它为 `non local static` 对象。**程序结束时 `static` 对象会自动销毁，也就是它们的析构函数会在 main 函数结束时调用**。

4. 作用域：

   * 代码块作用域：位于一对花括号之前的所有语句称为一个代码块，任何在代码块开始的位置声明的标识符都有**代码块作用域**。
   * 文件作用域：任何在所有代码块之外声明的标识符都具有**文件作用域**，它表示这些标识符从它们的声明之处直到它所在的源文件结尾处都是可以访问的。
   * 原型作用域：只适用于在函数原型中 **声明** 的参数名。
   * 函数作用域：只适用于语句标签，语句标签用于 goto 语句，即一个函数中的所有语句标签必须唯一。

5. 链接属性：用于链接器在链接多个目标文件的时候，相同的标识符出现在不同的源文件中，链接属性用来决定这些标识符表示的时同一个实体还是不同的实体，有以下三种：

   * external(外部)：标识符不论声明多少次，位于几个源文件都表示同一个实体。
   * internal(内部)：在同一个源文件内的所有声明都指向同一个实体，但位于不同源文件的多个声明则分属不同的实体。
   * none(无)：当作单独的个体，也就是说该标识符的多个声明被当作不同的独立实体。

   ps.缺省情况下，函数名的链接属性，文件作用域下的变量是 external，其余为none。

6. extern 和 static 关键字用于在声明中修改标识符的链接属性，如果某个声明在正常情况下具有 external 链接属性，使用 static 挂机案子可以使得它的链接属性变为 internal。当extern关键字用于源文件中一个标识符的第1次声明时，它指定该标识符具有external链接属性。但是，如果它用于该标识符的第2次或以后的声明时，它并不会更改由第1次声明所指定的链接属性。（个人理解：此时他只是告诉编译器这个变量在其他文件定义）

7. 存储类型：指的是存储变量值的内存类型，变量的内存类型决定变量何时创建、何时销毁以及它的值将保持多久：

   * 普通内存
   * 运行时堆栈
   * 硬件寄存器

   变量的缺省存储类型取决于它的声明位置，凡是在任何代码块之外声明的变量（文件作用域）总是存储在静态内存中，也就是不属于堆栈的内存，这类变量称为静态（static）变量。当extern关键字用于源文件中一个标识符的第1次声明时，它指定该标识符具有external链接属性。但是，如果它用于该标识符的第2次或以后的声明时，它并不会更改由第1次声明所指定的链接属性。在代码块内部声明的变量的缺省存储类型是自动的(automatic)，也就是说它存储于堆栈中，称为自动(auto)变量。在程序执行到声明自动变量的代码块时，自动变量才被创建，当程序的执行流离开该代码块时，这些自动变量便自行销毁。对于在代码块内部声明的变量，如果给它加上关键字static，可以使它的存储类型从自动变为静态。关键字register可以用于自动变量的声明，提示它们应该存储于机器的硬件寄存器而不是内存中，这类变量称为寄存器变量。

8. 当static用于函数定义时，或用于代码块之外的变量声明时，static关键字用于修改标识符的链接属性（从external改为internal），但标识符的存储类型和作用域不受影响。用这种方式声明的函数或变量只能在声明它们的源文件中访问。当它用于代码块内部的变量声明时，static关键字用于修改变量的存储类型，从自动变量修改为静态变量，但变量的链接属性和作用域不受影响。用这种方式声明的变量在程序执行之前创建，并在程序的整个执行期间一直存在，而不是每次在代码块开始执行时创建，在代码块执行完毕后销毁。



## 5. Know what functions C++silently writes and calls

1. 如果没有声明任何构造函数，那么编译器会声明一个默认构造函数。当声明一个空类的时候，编译器会为它声明一个拷贝构造函数，一个拷贝复制操作符和一个析构函数。惟有当这些函数被需要（被调用），它们才会被编译器创建出来。**所有这些函数都是 public 且 inline 的。**
   ```cpp
   class Empty{};
   //上面的声明相当于
   class Empty{
   public:
       Empty() {...}
       Empty(const Empty& rhs) {...}
       ~Empty() {...} 
       
       Empty& operator=(const Empty& rhs) {...} //copy assignment 操作符
   };
   ```

2. 编译器产生的析构函数是个 `non-virtual` 的，除非这个类的基类自身声明有 `virtual` 析构函数；至于拷贝构造函数和拷贝赋值运算符，编译器创建的版本只是单纯的将来源对象的每一个 `non-static` 成员变量拷贝到目标对象。
