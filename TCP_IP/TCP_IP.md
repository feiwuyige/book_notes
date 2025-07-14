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

## chapter4 基于 TCP 的服务器端/客户端
