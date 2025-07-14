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

   * `base class` 更早于其 `derived class`  被初始化，而 `class` 成员变量总是以其声明次序（在类中的定义顺序）被初始化。
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

## 6. Explicity disallow the use of compiler-generated functions you do not want

1. 有些类我们需要禁止拷贝和拷贝赋值，但是由于如果我们没有声明相应的函数的时候，编译器会为我们声明这样的函数，而且这些函数是 `public` 的。所以为了阻止编译器声明，我们可以主动声明这些函数，但是为了防止对象进行调用，我们可以将这样的函数声明为 `private` 的，这样就可以禁止对象进行调用。
   但是这个作法并不安全，因为类的 **成员函数和友元函数** 还是可以访问私有成员，所以对于拷贝构造函数和拷贝赋值运算符函数我们可以只声明，而不定义，这样如果有人调用了这了两个函数，就会出现链接错误。
2. 还有一种方式可以将链接错误提前到编译器错误，我们可以声明一个基类 `uncopy` ，将这个基类的拷贝构造函数和拷贝赋值运算符函数声明为 `private` 且不去定义，其他类去继承这个类就可以了。

## 7.Declare destructors virtual in polymorphic base classes

1. 有许多种做法可以记录时间，所以我们可以声明一个基类，然后在这个基类的基础上再声明很多其他的派生类，然后有一个工厂函数，返回一个基类类型的指针，但是实际上是指向某一个派生类对象，这样我们就可以通过该工厂函数来获得不同的表现。
   但是必须要注意的是，我们必须将基类的析构函数定义为虚函数。**因为C++明白指出，当derived class对象经由一个base class指针被删除，而该base class带着一个non-virtual析构函数，其结果未有定义——实际执行时通常发生的是对象的derived 成分没被销毁。**这样子就会导致局部资源未释放，造成内存泄漏。

2. 纯虚函数，值得注意的是，我们也可以为纯虚函数提供定义，不过函数体必须定义在类的外部。也就是说，我们不能在类的内部为一个=0的函数提供函数体。
   ```cpp
   
   class abstrct{
   	double price() = 0;
   };
   double abstract::price(){
   	std::cout << "hello" << std::endl;
   }
   ```

3. 理解c++中的函数调用过程：
   ```cpp
   p->mem();
   obj.mem();
   ```

   * 首先确定 `p` （或 `obj` ，下省略）的静态类型。因为我们调用的是一个成员，所以该类型必然是 **类类型**。
   * 在 `p` 的静态类型对应的类中查找成员 `mem` ，如果找不到，则在它的直接基类中查找直至继承链的顶端。如果到继承链的顶端都找不到，则编译器报错。
   * 一旦找到了 `mem` ，就进行常规的 **类型检查** 来判断此次调用是否合法。
   * 如果合法，则编译器根据 `mem` 是否是虚函数来生成不同的代码：
     * 如果 `mem` 是虚函数，且我们是通过指针或者引用来进行调用，则编译器产生的代码（读虚表）将依据对象的动态类型在运行时确定虚函数的版本。
     * 如果 `mem` 不是虚函数，或者我们不是通过指针、引用来进行调用，则编译器产生一个常规的函数调用。

## 8. Prevent exceptions from leaving destructors

1. 析构函数不要抛出异常，比如有一个 `animal` 类，我们定义了一个 `std::vector<animal> ` ，那么当我们去析构这个 `vector` 的时候，肯定会去处理其中的每一个元素，也就是对每一个 `animal` 对象调用析构函数，如果析构函数抛出异常，在处理多个元素的时候都抛出了异常，就会出现错误，因为对于C++而言，**在两个异常同时存在的情况下，程序若不是结束执行就会导致不明确的行为。**
2. 有两种方式可以避免这一问题：
   * 在析构函数中捕获异常，然后利用 `std::abort` 终止程序。
   * 在析构函数中捕获异常并进行处理。

## 9. Never call virtual functions during construction or destruction

1. 由 C++ 的构造规则可以知道，derived class对象内部的 base class成分会在derived class自身成分被构造之前先构造妥当。所以如果基类的构造函数中使用了虚函数，我们初始化一个派生类对象，那么在构造派生类对象的过程中，调用基类的构造函数的时候，虚函数的行为只能是基类的行为，毕竟派生类的成分还没有构造，即**在base class 构造期间，virtual 函数不是 virtual  函数。**
2. 析构函数的调用是反着来的，所以当调用到基类的析构函数的时候，派生类的成分已经析构完毕了，那么相当于此时还是只有一个基类成分。**其实还有比上述理由更根本的原因：在derived class对象的base class构造期间，对象的类型是 base class 而不是 derived class。不只 virtual 函数会被编译器解析至（resolve to）base class，若使用运行期类型信息（runtime type information，例如dynamic_cast（见条款27）和typeid），也会把对象视为base class类型。**
3. 当我们实现一个虚函数的时候，大概率都会用到派生类中的成员变量（不然没有必要声明为虚函数），但是不管是在析构函数还是在构造函数中调用虚函数的时候，此时的派生类成员已经被销毁或者是还没有构造，**使用未初始化的变量是十分危险的**，所以c++禁止此种用法。而且即使编译器没有报错，那么调用的虚函数版本大概率也不是你想要的版本。

## 10. Have assignment operators return a reference to *this

1. 为了实现连锁赋值，赋值操作符必须返回一个reference指向操作符左侧实参。
   ```cpp
   a = b = c = 15; //a = (b = (c = 15));
   class Wedget{
   public:
       Wedget& operator=(const Wedget& rhs){
   		//...
           return *this;
       }
   };
   //若不定义为引用
   (a = b) = c;
   //a != c，因为a = b 返回的是一个临时量，并不是 a 的引用
   ```


## 11. Handle assignment to self in opetator=

1. 让 `opetator=` 具备 “异常安全性” 往往自动获得 “自我赋值安全” 的回报。
   ```cpp
   Widget& Widget::operator=(const Widget& rhs){
       Bitmap* pOrig = pb;
       pb = new Bitmap(*rhs.pb);
       delete pOrig;
       return *this;
   }
   ```

   此时如果 `new bitmap` 抛出异常，那么 `pb` 会保持原状，而且这段代码还能处理自我赋值。

2. 除了在 `operator=` 中手工排列语句，还可以使用 `copy and swap` 技术：
   ```cpp
   class Widget{
       //...
       void swap(Widget &rhs);
       //...
   };
   Widget& Widget::operator=(const Widget& rhs){
   	Widget temp(rhs);
       swap(temp);
       return *this;
   }
   ```

   还有一种技术使用了以下事实：

   * 某 class 的 `copy assignment` 操作符可能被声明为 “以 `by value` 方式接受实参”
   * 以 `by value` 方式传递东西会造成一份副本

   ```cpp
   Widget& Widget::operator=( Widget rhs){ //pass by value
       swap(rhs);
       return *this;
   }
   ```

   **这样做将 `copying` 动作从函数本体内移至 “函数参数构造阶段”。**

## 12. Copy all parts of an object

1. 任何时候只要需要 **为 derived class 撰写 copying 函数**，就必须很小心的赋值其 `base class` 部分，那些成分往往是 `private` ，所以无法直接访问，应该让 `derived class` 的 `copying` 函数去调用相应的 `base class` 函数。
   ```cpp
   PriorityCustomer::PriorityCustomer(const PriorityCustomer& rhs)
   	: Customer(rhs), priority(rhs.priority) //继承自customer类
   {
   }
   ```

2. 该条款所说的 `all parts`：

   * 复制所有 local 成员变量
   * 调用所有 base classes 内的适当的 copying 函数。

3. 不要尝试以某个copying函数实现另一个copying函数。应该将共同机能放进第三个函数中，并由两个coping函数共同调用。即使用copy构造函数调用copy构造函数，使用 copy assignment 函数调用 copy assignment 函数。

## 13. Use objects to manage resources

1. 动态内存分配获得的指针，用户往往会忘记 `delete` 进行手动释放，即使进行了 `delete` ，但可能由于某些情况，代码并不会执行到`delete` 处，此时就造成了内存泄漏，为了避免这种情况，一种好的处理方式是使用编译器自动调用析构函数这一特点，来使用对象进行资源管理，具体而言，就是当返回资源的时候，用该资源构造某一个对象，那么当这个对象生命周期结束的时候，编译器会自动调用析构函数，从而达到释放资源的目的。即：
   * 获得资源立刻放进管理对象
   * 管理对象运用析构函数确保资源被释放
2. RAII(Resource Acquisition Is Initialization)：资源取得时机便是初始化时机
3. RCSP(reference-count smart pointer)：引用计数型智能指针

## 14. Think carefully about copying behavior in resource-manage class

1. 我们使用智能指针管理动态内存的资源已经足够，这是因为资源是分配在堆上的，但是还有些资源并不如此，那么我们就需要自己去定义资源管理类，此时要考虑该类中的复制操作。
2. 常见的复制操作有以下几种处理方式：
   * 禁止复制：有的时候，有些资源复制并不合理，所以我们要禁用拷贝构造函数和拷贝赋值运算符，通过声明为 `private` 或者继承一个 `Uncopable` 类来实现。（条款6）
   * 对底层资源使用引用计数法：正如 `shared_ptr` 一样，同时，`shared_ptr` 可以自定义删除器，当引用计数为 0 时调用该函数。
   * 复制底层资源：此时应该要注意进行深度拷贝。
   * 转移底层资源的拥有权：正如 `auto_ptr` 一样，当我们调用 `auto_ptr p2(p1)` 的时候，那么 `p2` 将指向 `p1` 之前指向的资源，且 `p1` 将指向 `null` 。

## 15. Provide access to raw resources in resource-manage class

1. APIs往往要求访问原始资源（raw resources），所以每一个RAII class应该提供一个“取得其所管理之资源”的办法。

2. 

   * 提供隐式转换，可以将一个资源管理类对象隐式转化为一个底层资源，如：				

     ```cpp
   FontHandle getFont(); //C API
   	void releaseFont(FontHandle fh); //C API
   class Font{
   public:
     explicit Font(FontHandle fh) : f(fh){}
     ~Font(){releaseFont(f);}
       operator FontHandle() const{   // 类型转换操作符重载
           return f;
       }
     private:
     FontHandle f;
   };
   	```
   
   * 提供显式转换，例如  `get()` 方法。

## 16. Use the same form in corresponding uses of new and delete

1.  `new` 运算符在调用时出现以下两个行为：

   * 通过 `operator new` 的函数分配内存
   * 针对此内存有一个或多个构造函数被调用

   `delete` 运算符在调用时出现以下两个行为：

   * 针对此内存有一个或多个析构函数被调用
   * 通过 `operator delete` 的函数释放内存

2. 一定要成对出现，否则程序的行为是 **UB**：
   ```cpp
   int *p = new int(); //值初始化
   delete p;
   int *parray = new int[10]{1, 2, 3}; //列表初始化
   delete[] parray;
   ```


## 17. Store newed objects in smart pointers in standalone statements

1. 因为 `shared_ptr` 的构造函数时 `explicit` 的，也就是禁止进行隐式转换：
   ```cpp
   shared_ptr<int> sp = new int; //错误，不能隐式转换
   shared_ptr<int> sp(new int); //正确，调用对应的构造函数
   ```

2. ```cpp
   processWidget(std::shared_ptr<Widget>(new Widget), priority());
   ```

   该语句只能保证 `new` 的运行早于 `shared_ptr` 构造函数的运行，所以可能存在以下情况：

   * new
   * priority()
   * 构造函数

   所以即使使用了RAII来管理资源，也可能会出现问题。

## 18. Make interface easy to use correctly and hard to use incorrectly

1. 组织接口被误用的方式可以通过设计新的类型来完成。比如：
   ```cpp
   class Date{
       Date(int month, int day, int year);
   };
   //上述的接口设计就不是很好
   class month{
   	mouth(int m);
   };
   class day{
   	day(int d);
   };
   class year{
   	year(int y);
   };
   class Date{
   	Date(month m, day d, year y);
   };
   ```

2. shared_ptr支持定制型删除器（custom deleter）。这可防范DLL问题，这个问题发生于“对象在动态连接程序库（DLL）中被 new创建，却在另一个 DLL 内被delete销毁”。在许多平台上，这一类“跨 DLL 之 new/delete成对运用”会导致运行期错误。因为会在引用计数为0的时候自动去调用构造智能指针时指定的删除器。

## 19. Treat class design as type design

1. 定义一个新的 class 其实就是设计一个新的类型，想设计一个好的类型一定要去思考以下问题：
   * 新 type 的对象应该如何被创建和销毁（构造函数、析构函数、内存分配函数、释放函数（new delete new[] delete[]）)
   * 对象的初始化和对象的赋值该有什么样的差别（构造函数 和 赋值操作运算符）
   * 新 type 的对象如果被 passed by value ，意味着什么。（拷贝构造函数用来定义一个 type 的pass by value 该如何实现）
   * 什么是新 type 的合法值。
   * 你的新 type 需要配合某个继承图系吗
   * 你的新 type 需要什么样的转换
   * 什么样的操作符和函数对此新 type 而言是合理的
   * 什么样的标准函数应该驳回（声明为private）
   * 谁该取用新 type 的成员
   * 什么是新 type 的 ”未声明接口“
   * 你的新 type 有多么的一般化
   * 你真的需要一个新 type 吗

## 20. Prefer pass-by-reference-to-const to pass-by-value

1. 缺省情况下C++以by value方式（一个继承自C的方式）传递对象至（或来自）函数。除非你另外指定，否则函数参数都是以实际实参的复件（副本）为初值，而调用端所获得的亦是函数返回值的一个复件。这些复件（副本）系由对象的copy构造函数产出，这可能使得pass-by-value成为昂贵的（费时的）操作。
2. 以引用方式传递参数有以下好处：
   * 减少构造函数和析构函数的调用，提高效率
   * 避免对象切割问题，当一个派生类对象以 by value 方式传递并被视为一个基类对象时，基类的拷贝构造函数会被调用，所以派生类对象所有的那些特质就会完全消失，造成对象切割问题。
3. 以上规则并不适用于内置类型，以及 STL 的迭代器和函数对象。对它们而言，pass-by-value往往比较适当。

## 21. Don't try to return a reference when you must return an object

1. 所谓引用只是一个名称，任何时候看到一个引用的声明式，都要问自己，该引用代表的对象是哪一个。

2. 如果函数要返回一个引用，那么这个引用一定指向了某一个对象，而要求这个对象在函数调用之前存在并不合理，往往是在函数内部再构造对象，所以有以下几种情况：

   * 在 stack 空间上分配对象：此时函数调用结束以后，空间被释放，返回的引用将指向一段垃圾内容，出错。

   * 在 heap 空间上分配对象：此时为了避免造成内存泄漏问题，那么就必须要有人去负责调用 delete 操作去回收内存，但是比如是重载了乘法运算符：
     ```cpp
     d = a * b * c;
     d = operator*(operator*(a, b), c);
     ```

     此时括号内部的返回的指针我们并无法合理拿到，更不用谈删除。

   * 声明为一个 static 对象，考虑以下代码：
     ```cpp
     if((a * b) == (c * d)){
     
     }
     ```

     此时 if 的条件永远为 true， 因为此时会调用两次乘法运算符，修改完 static 对象，所以两者的结果最终都是最后调用的那一次乘法运算得到的结果。

## 22. Declare data members private

1. 将成员变量声明为private。这可赋予客户访问数据的一致性、可细微划分访问控制、允诺约束条件获得保证，并提供class作者以充分的实现弹性。
2. 封装性可以通过取消一个变量以后有多少代码需要进行改变来进行衡量，对于一个 public 成员变量如果取消，那么所有使用到该成员变量的代码都要进行改变；而对于 protected 成员变量，所有的派生类中也要进行改变，所以 protected 并不比 public 更具有封装性。

## 23. Prefer non-member non-friend functions to member functions

1. 如果某些东西被封装，它就不再可见。愈多东西被封装，愈少人可以看到它。而愈少人看到它，我们就有愈大的弹性去变化它，因为我们的改变仅仅直接影响看到改变的那些人事物。因此，愈多东西被封装，我们改变那些东西的能力也就愈大。这就是我们首先推崇封装的原因：它使我们能够改变事物而只影响有限客户。
2. 如果要你在一个member函数（它不只可以访问class内的private数据，也可以取用private函数、enums、typedefs等等）和一个non-member，non-friend函数（它无法访问上述任何东西）之间做抉择，而且两者提供相同机能，那么，导致较大封装性的是non-member non-friend函数，因为它并不增加“能够访问class内之private成分”的函数数量。
3. namespace 和 classes 不同，前者可跨越多个源码文件而后者不能。这很重要，因为像clearBrowser这样的函数是个“提供便利的函数”，如果它既不是 members 也不是 friends，就没有对 WebBrowser的特殊访问权力，也就不能提供“WebBrowser客户无法以其他方式取得”的机能。

## 24.Declare non-member functions when type conversions should apply to all parameters

1. 假设我们定义了一个有理数类，其中有一个非 explicit 的构造函数，参数是 int，所以就允许编译器隐式类型转换将一个 int 转化为该类类型，然后我们重载了 * 运算符：
   ```cpp
   result = r * 2; //正确，可以将 2 隐式类型转为有理数类
   result = 2 * r; //错误，相当于 2.operator*(r)，而 2 的这个 int 类里面并没有重载这个函数，然后编译器
   //去寻找类似于 operator*(2, r); 这样的函数，但是也没法找到一个参数为int 和 有理数类的 函数
   ```

2. 如果你需要为某个函数的所有参数（包括被 this指针所指的那个隐喻参数）进行类型转换，那么这个函数必须是个non-member。

## 25. Consider support for a non-throwing swap

1. 看的不是很懂！！！

