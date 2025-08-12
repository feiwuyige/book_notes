# TCP/IP 网络编程
## chapter1 理解网络编程和套接字
## chapter2 套接字类型与协议设置
1. `socket` 函数: 
    ```c
    int socket(int domain, int type, int protocol);
    ```
    > socket() creates an endpoint for communication and returns a descriptor.这里的endpoint是通信的终端节点，是网络通信中数据收发的最小逻辑单位。 
    > * domain: 套接字协议族信息
    > * type：套接字数据传输类型信息
    > * protocol：协议

    套接字类型：
    * SOCK_STREAM：面向连接的套接字。具有特征：
        * 传输过程中数据不会丢失。
        * 按顺序传输数据。
        * 传输的数据不存在数据边界。也就是协议不会自动保留应用层数据的发送/接收次数和分块方式，而是将数据视为连续的字节流，即发送方可能多次发送，但是接收方存入缓冲区后一次读取。
        * 套接字连接必须一一对应。面向连接的套接字只能与另外一个同样特性的套接字连接。

    * SOCK_DGRAM：面向消息的套接字。具有特征：
        * 强调快速传输而非传输顺序。
        * 传输的数据可能丢失、损毁。
        * 传输的数据有数据边界。
        * 限制每次传输的大小。

2. 习题：

## chapter3 地址族与数据序列
1. IPv4 标准的 4 字节 IP 地址分为网络地址和主机地址（计算机地址），分为以下类型，可以通过 IP 地址的第一个字节来判断网络地址占用的字节数：
    * A 类：8位网络地址+24位主机地址，0 - 127，以 0 开头
    * B 类：16位网络地址+16位主机地址，128 - 191，以 10 开头
    * C 类：24位网络地址+8位主机地址，192 - 223， 以 110 开头
    * D 类：32位网络地址
    * E 类：32位网络地址

2. IP 用于区分计算机，操作系统利用接收的数据内的端口号把数据传输给相应端口的套接字（即对应的应用程序），端口号由 16 位构成，范围 0 - 65535，其中 0 - 1023 是系统预留端口，1024 - 65535 是用户端口。TCP 套接字和 UDP 套接字不会公用端口号，所以可以重复。

3. 表示 IPv4 地址的结构体：
    ```c
    struct sockaddr_in{
        sa_family_t sin_family; // 地址族, IPv4 or IPv6
        uint16_t sin_port; // 端口号，以网络字节序保存
        struct in_addr sin_addr; // IP 地址，以网络字节序保存
        char sin_zero[8]; // 用于填充，为了与 struct sockaddr 保持相同长度
    };
    struct in_addr{
        In_addr_t s_addr; // IP 地址
    };
    struct sockaddr{
        sa_family_t sa_family; // 地址族
        char sa_data[14]; // 地址，不是为 IPv4 设置，而是为所有地址族设置，IPv4：2 个字节端口号 + 4 字节 IP 地址
    };
    ```
    `bind` 函数的第二个参数类型位 `struct sockaddr*`，因为直接给 `sockaddr` 赋值不方便，所以通常使用 `struct sockaddr_in` 结构体。

4. CPU 字节序：
    * 大端序：高位在低地址，低位在高地址
    * 小端序：高位在高地址，低位在低地址
    
5. 网络字节序：大端序，先把数据数组转化成大端序格式再进行网络传输，计算机接收数据时应识别网络字节序，将其转化为本机字节序。

6. 字节序转换函数：
    ```c
    unsigned short ntohs(unsigned short netshort);
    unsigned short htons(unsigned short hostshort);
    unsigned long ntohl(unsigned long netlong);
    unsigned long htonl(unsigned long hostlong);
    ``` 
    * h 代表 host，n 代表 network
    * s 代表 short，l 代表 long

7. `struct sockaddr_in` 结构体中的字段为证书类型，所以我们通常需要将一个字符串类型的 IP 地址转化为 32 位整数类型，使用函数 `inet_addr` 可以返回 32 位大端序整数型值。
    ```c   
    in_addr_t inet_addr(const char *cp);
    int inet_aton(const char *cp, struct in_addr *inp); //直接填充 struct sockaddr_in.sin_addr
    char *inet_ntoa(struct in_addr in); //将网络字节序整数型 IP 地址转化为点分十进制字符串
    ```

8. `127.0.0.1` 是本地回环地址，指的时计算机自身 IP 地址。

9. `bind` 函数: 
    ```c
    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    ``` 
    > * sockfd: 要分配地址信息的套接字文件描述符
    > * addr: 要分配的地址信息
    > * addrlen: 地址信息长度

10. 习题：

## chapter4 基于 TCP 的服务器端/客户端（1）
1. TCP / IP 协议栈：
    * 链路层：物理链接领域标准化的结果，专门定义 LAN WAN MAN 等网络标准。
    * IP 层：在复杂的网络中考虑路径的选择。该层使用的协议就是 IP 协议，IP 本身是面向消息的、不可靠的协议。
    * TCP / UDP 层：根据 IP 层提供的路径信息来完成实际的数据传输，也叫传输层。
    * 应用层：上述三层中的数据传输是套接字通信过程中自动处理的，选择数据传输路径、数据确认过程都被隐藏在套接字内部。根据程序特点决定服务器端和客户端之间的数据传输规则，这便是应用层协议。

2. 进入等待连接请求状态：
    ```c
    int listen(int sockfd, int backlog);
    ```
    * sock：希望进入等待连接请求状态的套接字文件描述符，监听套接字。
    * backlog：最大允许的连接请求数量

3. 受理客户端连接请求：
    ```c
    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    ```
    * sockfd：服务器套接字的文件描述符
    * addr：保存发起连接请求的客户端地址信息的变量地址值，调用函数后向该变量指向的地址中填充客户端地址信息。
    * addrlen：保存客户端地址信息长度的变量地址值，调用函数后向该变量指向的地址中填充客户端地址信息长度。

    函数调用成功以后， `accept` 函数内部将产生用于与客户端进行数据交换的新套接字，新套接字的文件描述符通过函数返回值返回。

4. 客户端发起连接请求：
    ```c
    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    ```
    * sockfd：客户端套接字的文件描述符
    * addr：保存服务器地址信息的变量地址值
    * addrlen：保存服务器地址信息长度的变量地址值，以字节为单位。

    客户端调用 `connect` 函数以后，只有以下情况才会返回：
    * 服务器端接收连接请求。
    * 发生断网等异常情况而中断连接请求。

5. 客户端套接字不需要手动绑定 IP 地址和端口号，它是在调用 `connect` 函数的时候由操作系统在内核中进行绑定，绑定计算机的主机 IP 地址以及随机端口号。

6. 习题：

## chapter5 基于TCP的服务器端和客户端（2）
1. `TCP` 是面向字节流的，所以在 chapter4 中实现回声客户端的时候， `read` 函数并不能保证一次就读到所有的数据，所以最好是有一个循环调用 `read` 函数的过程，直到服务器发回的数据长度达到要求。服务器端不会存在问题，因为它收到客户端传来的数据就直接调用 `write`，换句话说，服务器不在意数据边界，它只需要转发，但是客户端需要进行显示，所以要注意数据边界。

2. 实际情况中，我们不能知道客户端应该收到多长的消息就可以结束，所以我们需要在应用层来设置一些协议，从而得到消息边界。

3. `read` `write` 函数调用的时候并不会立即收发数据，而是从输入缓冲中读取数据，或将数据移动至输出缓冲。`write` 系统调用在将数据移动至输出缓冲以后就会返回。

4. I/O 缓冲的特点：
    * 创建 `socket` 的时候自动创建。
    * 每个 `socket` 有自己独立的缓冲区。
    * 关闭 `socket` 不会影响输出缓冲区的内容，还会继续发送。
    * 关闭 `socket` 会丢失输入缓冲区的内容，无法进行读取。

5. 习题：

## chapter6 基于UDP的服务器端/客户端
1. UDP 的服务器端和客户端不用像 TCP 那样在连接状态下交换数据，无需经过连接过程（listen accept ），只有创建套接字和数据交换过程。

2. TCP 的时候，每有一个新的客户端建立连接，就需要给这个客户端在服务器中分配一个新的 socket，但是在 UDP 中，客户端和服务器都只需一个 socket 就可以了，所有的数据收发都通过这个sokcet 来进行。

3. 因为 TCP socket 保持与对方 socket 的连接，所以在传输数据的时候不需要再添加地址信息，而 UDP socket 则需要添加地址信息。
    ```c
    ssize_t sendto(int sock, void *buff, size_t nbytes, int flags, struct sockaddr *to, socklen_t addrlen);
    ```
    * sock：用于传输数据的 UDP socket 文件描述符。
    * buff：保存待发送数据的缓冲区的首地址。
    * nbytes：待发送数据的长度，以字节为单位。
    * flags：可选项参数，一般填 0。
    * to：保存目标地址信息 sockaddr 的结构体变量的地址。
    * addrlen：保存目标地址信息的变量长度。
    ```c
    ssize_t recvfrom(int sock, void *buff, size_t nbytes, int flags, struct sockaddr *from, socklen_t *addrlen);
    ```

4. UDP 程序中，调用 sendto 函数的时候自动分配 IP 和端口号。调用 sendto 函数大致有以下阶段：
* 向 UDP 套接字注册目标IP和端口号。
* 传输数据。
* 删除 UDP 套接字中注册的目标地址信息。

5. 当使用 UDP 套接字对同一个目标 IP 和 端口进行数据发送的时候，多次的注册和删除地址信息会导致性能下降，所以可以通过创建 connected UDP 套接字来避免这个问题，即对 UDP socket 调用 `connect` 函数。

6. 习题：