[TOC]

# C++并发编程

## 线程

1. 使用 `thread` 线程库，一个线程对象关联一个线程资源，使用一个可调用对象来初始化一个线程对象，当线程对象构造成功以后，就自动在新线程中开始执行对应的函数。
2. `t.join()` ：等待线程对象 `t` 关联的线程执行完毕，否则将一直阻塞。具体来说 ，`std::thread` 的析构函数通过 `joinable()` 来判断当前的线程对象目前是否有关联活跃线程，而 `t.join()` 就是用来修改线程对象的状态，如果调用析构函数的时候 `joinable` 为 `true` ，则表明该线程对象关联着某个活跃线程，所以会调用 `std::terminate`。
3. `std::terminate()`：调用当前安装的 `std::terminate_handler`，默认情况下就是 `std::abort`。
4. `std::hardware_concurrency` ：返回当前硬件支持的并发线程数量。
5. `std::detach()`：线程对象调用 `detach` ，放弃了对线程资源的所有权，不再管理此线程，允许该线程独立运行，在线程退出时释放所有分配的资源。
6. 使用一个**可调用对象初始化线程对象的时候，这个可调用对象的参数直接写在线程对象的构造函数中即可**，这些参数会**复制**到新线程的内存空间中，即使函数中的参数是引用，依然是**复制**。
7. `thread` 的构造的语义是启动线程，而并不是传入其构造函数的可调用对象进行执行，当线程启动以后，具体什么时候进行执行取决于操作系统进行调度。
8. `std::this_thread` 中包含了管理当前线程的函数：
   * `yield`：建议实现重新调度各执行线程。例如某个线程需要等待某个操作完成，那么代码中肯定是一个循环不断判断该操作是否完成，这就会使得这个线程占用大量`CPU` 时间，所以可以使用该函数让该线程交出 `CPU` 时间片让其他线程执行，过一会再来判断是否完成。
   * `get_id`：返回当前线程 `id`。
   * `sleep_for`：使当前线程停止执行指定时间。
   * `sleep_until`：使当前线程停止到指定的时间点。
9. `std::thread` 不可复制，一个 `std::thread` 对象就代表一个线程资源，`std::thread` 对于线程资源是独占所有权，**移动**操作可以将一个 `std::thread` 对象的线程资源所有权转移给另一个 `std::thread` 对象。



## std::thread的构造-源码解析

基于`gcc 14.1.0`，对`std::thread` 的构造函数源码进行解析。

首先是该类的数据成员：
```cpp
private:
    id				_M_id;
```

其中 `id` 也是一个自定义类，是一个线程的唯一标识符。

构造函数有以下几个：

* ```cpp
  public:
  	thread() noexcept = default;
  ```

  无参构造函数是默认的，由编译器生成。

* ```cpp
  thread(const thread&) = delete;
  ```

  拷贝构造函数进行删除，因为一个 `thread` 对象关联一个线程资源，所以应当禁止拷贝和复制。

* ```cpp
  template<typename _Tp>
        using __not_same = __not_<is_same<__remove_cvref_t<_Tp>, thread>>;
  
  public:
      template<typename _Callable, typename... _Args,
  	     typename = _Require<__not_same<_Callable>>>
        explicit
        thread(_Callable&& __f, _Args&&... __args)
        {
  	static_assert( __is_invocable<typename decay<_Callable>::type,
  				      typename decay<_Args>::type...>::value,
  	  "std::thread arguments must be invocable after conversion to rvalues"
  	  );
               using _Wrapper = _Call_wrapper<_Callable, _Args...>;
  	// Create a call wrapper with DECAY_COPY(__f) as its target object
  	// and DECAY_COPY(__args)... as its bound argument entities.
  	_M_start_thread(_State_ptr(new _State_impl<_Wrapper>(
  	      std::forward<_Callable>(__f), std::forward<_Args>(__args)...)),
  	    _M_thread_deps_never_run);
        }
  ```

  这是一个函数模板，有一个类型参数 `_Callable` 和 一个类型形参包 `_Args` ，最后是一个未命名的模板类型参数，它的默认值是`_Require<__not_same<_Callable>>>`，这里是通过 `SFINAE` 技术实现模板约束，只有当 `_Require` 返回有效类型时，该模板才会进行实例化。也就是说只有当我们传入的可调用对象的类型不是 `thread` 的时候，才会去实例化该构造函数。**如果没有该模板约束条件，当我们使用一个 `thread` 实例对象去初始化一个 `thread` 对象的时候，就会选择该构造函数进行执行，从而违背了 `thread` 对象不可拷贝这一基本原则**。然后在函数体中使用 `decay` 去去除类型的 cv 限定和引用，使用 `static_assert` 来判断是否满足调用要求，

* 
