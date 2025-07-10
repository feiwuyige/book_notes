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


