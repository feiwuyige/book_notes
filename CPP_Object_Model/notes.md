[TOC]



# 深入探索C++对象模型

## 第一章 关于对象（Object Lessons）

### 1.1 C++ 对象模式

1. 在 C++ 中，有两种 class member：`static 和 nonstatic`，以及三种 class function：`static, nonstatic, virtual` ；所谓的 C++ 对象模式，就在于编译器如何来存放这些数据，目前 C++ 采用的方式如下：
   * static 数据成员存放在类对象之外， nonstatic 数据成员存放在类对象之内。
   * static 函数和 nonstatic 函数都存放在类对象之外。
   * virtual 函数通过以下方式支持：
     *  每个**类**产生出一堆指向虚函数的函数指针，存放在表格之中，这个表格被称为 virtual table（vtbl）。
     * 每一个**类对象**中添加一个指针，指向相关的 vtbl，这个指针被称为 vptr，vptr 的值由编译器来确定，每一个 class 所关联的 type_info object（用以支持 RTTI）也有 vtbl 来指出，一般位于表格的第一项。
2. C++ 的继承模型中，base class subobject 的 data members 直接放置于 derived class object 中。
3. 虚拟继承的情况下，base class 不管在继承的串链中被派生多少次，永远只会存在一个实体。

### 1.2 关键词所带来的差异

1.  C++ 中的 struct 关键字很大程度上是为了与 C 保持兼容，在 C 里面使用 struct 关键字是为了定义一系列数据的集合，在 C++ 中除了封装数据，也可以封装对于数据的操作，所以 struct 关键字和 class 关键字更多的是逻辑意义上的不同，在 C++ 中，有以下不同：
   * 使用 struct 关键字声明的类默认 access level 是 public，使用 class 关键字声明的类默认 access level 是 private。
   * 因为模板是 C++ 所特有的，所以在 typename 可以和 class 关键字来声明模板参数类型，不支持 struct。
   
2. 由于 `struct` 关键字的语意在 C 与 CPP 中有些许不同，这就导致一些 C 中的技巧在 CPP 中将不再有用，比如：
   ```c
   struct mumble{
       // members
       char pc[1];
   };
   //读取一个 string，然后为 struct 和字符串分配足够的内存
   struct mumble *pmumble = (struct mumble *)malloc(sizeof(struct mumble) + strlen(string) + 1);
   ```

   这里是**柔性数组**的概念，核心目的是在结构体中存放一个只有在运行的时候才能去确定长度的数组。但是在 CPP 中，这种技巧可能不再有用，因为 **CPP 中保证，处于同一个 access level 的数据，必定按照其声明顺序出现在内存布局当中，然而被放置在多个 access level 中的数据，顺序则不一定了。**除此以外，还有以下原因：

   * base object 和 derived object 的布局也没有谁先谁后的强制规定。
   * 虚函数的存在。

### 1.3 对象的差异

1.  CPP 支持三种 programming paradigms:
   * 程序模型，和 C 一样。
   * 抽象数据类型模型，class 定义数据成员和方法。
   * 面向对象模型，定义抽象接口，继承、多态。
2. 只有通过 pointer 和 reference，才能在面向对象模型中实现多态。在 OO 模型中，程序员需要处理一个未知实体，它的类型虽然有所界定，但是由于继承体系，它的实际类型理论上有无限种可能，即被指定的 object 类型在执行点被解析之前，是无法确切知道它的类型的，而对于 ADT 模型中，每个实体在编译时就已经确定好了。
3. CPP 使用下列方式支持多态：
   * 隐含的转换操作，即一个基类类型的指针和引用可以指向派生类类型。
   * 使用虚函数机制，利用虚表来实现多态。
   * 通过 `dynamic_cast` 和 `typeid` 运算符。
4. 不管什么类型的指针，占用的内存大小都是确定的，它的类型只是影响了编译器如何解释某个特定地址中的内存内容及大小。
5. 当我们使用指针进行函数调用的时候，会依据指针的静态类型（声明的类型）在编译期间确定如下两点：
   * 固定的可用接口。即通过某个指针调用函数的时候，这个函数一定要是其静态类型中的接口。
   * 访问级别。通过静态类型中的类的访问接口来判断是否有权进行调用。
6. 指针和引用之所以支持多态，是因为它们并不会引发内存中任何**“与类型有关的内存委托操作”**[^1]，受到影响的仅仅是它们所指向内存的大小和内容解释方式。
7. 假定我们调用p->mem（）（或者obj.mem（）），则依次执行以下4个步骤：
   * 首先确定p（或obj）的静态类型。因为我们调用的是一个成员，所以该类型必然是类类型。
   * 在p（或obj）的静态类型对应的类中查找mem。如果找不到，则依次在直接基类中不断查找直至到达继承链的顶端。如果找遍了该类及其基类仍然找不到，则编译器将报错。
   * 一旦找到了mem，就进行常规的类型检查以确认对于当前找到的mem，本次调用是否合法。
   * 假设调用合法，则编译器将根据调用的是否是虚函数而产生不同的代码：
     * 如果mem是虚函数且我们是通过引用或指针进行的调用，则编译器产生的代码将在运行时确定到底运行该虚函数的哪个版本，依据是对象的动态类型。
     * 反之，如果mem不是虚函数或者我们是通过对象（而非引用或指针）进行调用，则编译器将产生一个常规函数调用。



## 第二章 构造函数语意学

1. 编译器背着程序员做了很多事情，比如会将单参数的构造函数作为类型转换运算符来隐式转换（使用 `explicit` 关键字可以避免），CPP 标准库中 string 类型并没有提供转化为 const char* 的隐式类型转换运算符，而是提供了一个具名实体，即 `string.c_str()`。

### 2.1 Default Constructor 的建构操作

1. CPP 中，当编译器需要的时候，会合成一个 default constructor，这个构造函数只执行编译器所需要的行动。换句话说，如果一个 class 中没有声明任何构造函数，就会有一个 default constructor 被编译器合成，但是这个构造函数大概率会是一个 trivial 的（无能的）。
2. 以下四种情况编译器合成的 constructor 将会是一个 nontrival 的:
   * **”带有 Default Constructor “ 的 Member  Class Object**:
     有两个类 A 和 类 B，类 B 中含有类 A 的对象成员，且没有默认构造函数，类 A 含有默认构造函数，那么编译器就会为类 B 合成一个构造函数，合成的构造函数只会负责类 A 对象的初始化，其他成员（如 char*）的初始化则是程序员自己的事情。**这个合成操作只会在 constructor 真正需要被调用时才发生。**因为 cpp 编译单元的存在，所以在不同的编译单元中，如果定义类 B 对象，就会合成一个构造函数，为了避免链接错误，编译器会使用 `inline` 关键字，如果函数太长，无法 `inline` ，那么编译器就会合成一个 `explicit non-inline static` 的构造函数。**如果类 B 提供了默认构造函数，那么编译器将无法合成新的默认构造，它会将类 A 的默认构造函数内嵌到类 B 的默认构造函数中（若有多个类的类对象，则按照声明顺序插入）。**
   * **"带有 Default Constructor" 的 Base Class**：
     如果派生类中没有任何 constructor，但是基类中有 default constructor，那么编译器就会为派生类合成一个 default constructor，依据基类的声明次序，调用上一层的base class 的 default constructor，对于后继的派生类而言，这个合成的默认构造函数和被明确提供的默认构造函数没有什么差异。
     如果派生类中有其他构造函数但是没有默认构造函数，那么编译器会扩张现有的每一个 constructor，将所有必要的 default constructor 程序代码加进去。（**先基类的，然后member class object的**）
   * **"带有一个 virtual function" 的 class:**
     当一个类中声明或继承一个虚函数的时候，编译器会合成构造函数。这是因为 CPP 对于多态的支持，利用了虚函数表和虚表指针，这些对于程序员是透明的，编译器需要去生成虚表，存放虚函数地址，并且在每一个类对象中，包含虚表地址，这些工作需要编译器去做。
   * **"带有一个 virtual base class 的 class":**
     在实现虚拟继承的时候，不同的编译器之间差异很大，但是每一种实现的共同点在于**必须使得 virtual base class在每一个派生类对象中的位置，能够在执行期准备妥当**。比如我们定义了一个派生类对象指针，用这个指针指向基类中的成员变量，因为这个指针指向的真正类型可以改变，所以编译器必须修改对这个成员变量的访问方式，从而使得可以在运行期间再得到成员变量的实际偏移位置。一种方式是通过一个指针来实现，将这个指针指向虚基类，这就使得需要编译器去修改构造函数，将这个指针存放在类对象中。
3. 总结：编译器合成构造函数只是为了自己使用，它要么通过调用基类或类对象成员变量的构造函数，要么是为了支持虚函数和虚继承机制，除了这几种情况外，实际上并不会合成构造函数（即 implicit trivial default constructors）。且合成的构造函数只会初始化子对象或者基类部分，对于其他 nonstatic data member，那是程序员自己的事情！

### 2.2 Copy Constructor 的建构操作

1. 在 CPP 中，会有以下三种情况将一个 object 的内容作为另一个 class object 的初始值：
   * 明确的赋值操作或初始化操作。
   * object 类型是函数参数类型。
   * object 类型是函数返回值类型。
2. 如果 class 没有提供一个 explicit copy constructor，那么当 class object 以相同 class 的另一个 object 作为初始值时，其内部通过 default memberwise initialization 完成，即将每一个内建的或派生的成员变量，从某个 object 拷贝到另一个 object 身上，不过对于类成员变量，则不会拷贝，而是递归的调用memberwise initialization(这个操作概念上是通过 copy constructor 实现的)。
3. 与默认构造函数的合成一致，CPP 标准将 copy constructor 区分为 trivial 和 nontrivial 两种，只有 nontrivial 的实体才会被合成到程序中，决定一个拷贝构造函数是否为 trivial 的标准在于 class 是否展现出所谓的 bitwise copy semantics（位逐次拷贝）。
4. 以下四种情况一个 class 将不再展现出 bitwise copy semantics，即需要合成拷贝构造函数：
   * 一个类内含某一个类对象，而内含的类提供了拷贝构造。
   * 派生类继承的基类，基类中有拷贝构造（不管是显式声明还是合成的）。
   * 类中有虚函数。（当 base class object 以派生类的object内容做初始化时，类对象的虚表指针不能按位赋值，需编译器干预）
   * 继承串链中有 virtual base class。（当 base class object 以派生类的 object 内容初始化时，编译器必须明确的将 virtual base class pointer/offset 初始化）



### 2.3 程序转化语意学

1. 对于明确的初始化操作（X x1 = x0），编译器会进行程序转化：

   * 重写每一个定义，剔除其中的初始化操作。
   * class 的 copy constructor 调用操作会被安插进去。

2. 参数的初始化：把一个 class object 当作参数传递给一个函数（或是作为一个函数的返回值），相当于以下的初始化操作：
   ```cpp
   X xx = arg; //xx 代表形参或返回值，arg 代表真正的参数值
   ```

   对于一次函数调用，大多数编译器的实现都是生成一个临时对象，然后调用 copy constructor 将其初始化，然后将临时对象作为函数的参数进行函数调用（这就使得需要修改函数声明，将函数声明为引用类型，因为要修改临时对象），当函数完成以后，调用对应的析构函数释放临时对象。

3. 返回值的初始化：对于返回类对象的函数，需要以下转换：

   * 加上一个额外参数，类型是 class object 的一个 reference，该参数用来放置拷贝构造而得到的返回值。
   * 在 return 指令之前安插一个 copy constructor 调用操作，将额外参数的值设置为将要传回的值。

   所以真正的返回值就是这个额外参数，编译器也会改写函数，让函数返回 void。

4. Named Return Value(NRV)优化：在一些函数中，所有的 return 指令传回相同的 named value，那么编译器可以自己做优化，方法是以 result 参数（返回类对象时声明的额外参数）取代 named return  value，这样在函数操作中直接处理 result，省略后续的拷贝构造。

### 2.4 成员初始化列表（Member Initialization List）

1. 必须使用 member initialization list 的情况：
   * 初始化一个 reference member
   * 初始化一个 const member
   * 调用一个 base class 的 constructor，而它拥有一组参数时
   * 调用一个 member class 的 constructor，而它拥有一组参数时
2. 编译器会一一操作 initialization list，以适当次序（在类中的声明次序，与初始化列表中的书写次序无关）在 constructor 之内安插初始化操作，并且在任何 explicit user code 之前。
3. 可以在初始化列表中调用类的成员函数，因为当函数声明结束（即函数形参的括号）时，this 指针已经建构。



## 第三章 Data 语意学



[^1]: 这里指的是编译器在编译阶段为对象分配内存、确定成员偏移量、安排虚表指针位置等操作时，**必须依据一个具体的、已知的类型**来做出决定。编译器必须“承诺”这个对象在内存中严格按照某个特定类型的布局来组织。
