# BOOST::ASIO

## Chapter 1. The Basics

### 1. Creating an endpoint

1.  **endpoint**：A pari of values consisting of an IP address and a protocol port number that uniquely identifies a particular application running on a particular host in a computer network is called an endpoint.
   通过 IP 地址和端口号来在互联网中唯一标识一个在主机上运行的应用程序。

2. To sum up, the endpoints serve two goals:

   * The client application uses an endpoint to designate a particular server application it wants to communicate with.
   * The server application usea an endpoint to specify a local IP address and a port number on which it wants to receive incoming messages.If there is more than one IP address on the host, the server application will want to create a special endpoint representing all IP addresses at once.

   对于客户端，它要使用终端节点来指明它想与服务器上面的哪一个应用程序交互，所以应该由服务器的IP地址以及服务器上分配给该应用程序的端口号来指定；对于服务器，它使用终端节点来监听（告诉操作系统去监听这个终端节点收到的数据包），来判断是否有客户端给其发送消息，从而正确接受到客户端发送过来的消息，由于可能主机上有多个地址，所以可以来指定监听多个 IP 地址 。

3. example code：
   ```cpp
   #include <boost/asio.hpp>
   #include <iostream>
   //服务器
   using namespace boost;
   
   int main()
   {
     // Step 1. Here we assume that the server application has
     //already obtained the protocol port number.
     unsigned short port_num = 3333;
   
     // Step 2. Create special object of asio::ip::address class
     // that specifies all IP-addresses available on the host. Note
     // that here we assume that server works over IPv6 protocol.
     asio::ip::address ip_address = asio::ip::address_v6::any();
   
     // Step 3.
     asio::ip::tcp::endpoint ep(ip_address, port_num);
   
     // Step 4. The endpoint is created and can be used to 
     // specify the IP addresses and a port number on which 
     // the server application wants to listen for incoming 
     // connections.
   
     return 0;
   }
   ```

### 2. Creating an active socket

1. **socket** : An abstract object representing a communication session context.

2. There are two types of sockets:

   *  A socket intended to be used to send and receive data to and from a remote application or to initiate a connection establishment process with it is called an **active socket**.
   * A **passive socket** is the one used to passively wait for incoming connection requests from remote applictions.

   收发数据以及发起连接的 socket 是主动的，等待连接的 socket 是被动的（服务器上的）。

3. `asio::io_service` ：This class is a central componet in the Boost.Asio I/O infrastructure.It provides access to the network I/O services of the underlying operating system.Boost.Asio sockets get access to those services through the object of this class.Therefore, all socket class constructors require an object of asio::io_service as an argument.

4. example code：
   ```cpp
   #include <boost/asio.hpp>
   #include <iostream>
   
   using namespace boost;
   
   int main()
   {
     // Step 1. An instance of 'io_service' class is required by
     // socket constructor. 
     asio::io_service ios;
   
     // Step 2. Creating an object of 'tcp' class representing
     // a TCP protocol with IPv4 as underlying protocol.
     asio::ip::tcp protocol = asio::ip::tcp::v4();
   
     // Step 3. Instantiating an active TCP socket object.
     asio::ip::tcp::socket sock(ios);
   
     // Used to store information about error that happens
     // while opening the socket.
     boost::system::error_code ec;
   
     // Step 4. Opening the socket.
     sock.open(protocol, ec);
   
     if (ec.value() != 0) {
       // Failed to open the socket.
       std::cout
         << "Failed to open the socket! Error code = "
         << ec.value() << ". Message: " << ec.message();
         return ec.value();
     }
   
     return 0;
   }
   ```

   The real operating system's socket is allocated in step 4 when we call the open() method and pass an object specifying protocol to it as an argument.

### 3. Creating an passive socket

1. **passive socket**：A passive socket or acceptor socket is a type of socket that is used to wait for connection establishment requests from remote applications that communicate over the TCP protocol.

   * Passive sockets are used only in server applications or hybrid applications that may play both roles of the client and server
   * Passive sockets are defined only for the TCP protocol. As the UDP protocol doesn't imply connection establishment, there is no need for a passive socket when communication is performed over UDP.

2. example code:
   ```cpp
   #include <boost/asio.hpp>
   #include <iostream>
   
   using namespace boost;
   
   int main()
   {
     // Step 1. An instance of 'io_service' class is required by
     // socket constructor. 
     asio::io_service ios;
   
     // Step 2. Creating an object of 'tcp' class representing
     // a TCP protocol with IPv6 as underlying protocol.
     asio::ip::tcp protocol = asio::ip::tcp::v6();
   
     // Step 3. Instantiating an acceptor socket object.
     asio::ip::tcp::acceptor acceptor(ios);
   
     // Used to store information about error that happens
     // while opening the acceptor socket.
     boost::system::error_code ec;
   
     // Step 4. Opening the acceptor socket.
     acceptor.open(protocol, ec);
   
     if (ec.value() != 0) {
       // Failed to open the socket.
       std::cout
         << "Failed to open the acceptor socket!"
         << "Error code = "
         << ec.value() << ". Message: " << ec.message();
       return ec.value();
     }
   
     return 0;
   }
   ```

   主要区别在于第三步，使用的是 `asio::ip::tcp::acceptor` 类，所有接收器本质上也是一个 socket.

### 4. Resolving a DNS name

1. **DNS**：DNS is a distributed naming system that allows associating human-friendly names with devices in a computer network. A DNS name or a domain name is a string that represents a name of a device in the computer network.It doesn't name a particular physical device but an IP address that can be assigned to a device.

2. example code:
   ```cpp
   #include <boost/asio.hpp>
   #include <iostream>
   
   using namespace boost;
   
   int main()
   {
     // Step 1. Assume that the client application has already
     // obtained the DNS name and protocol port number and 
     // represented them as strings.
     std::string host = "www.baidu.com";
     std::string port_num = "80";
   
     // Step 2.
     asio::io_service ios;
   
     // Step 3. Creating a query.
     asio::ip::udp::resolver::query resolver_query(host,
       port_num, asio::ip::udp::resolver::query::numeric_service);
   
     // Step 4. Creating a resolver.
     asio::ip::udp::resolver resolver(ios);
   
     // Used to store information about error that happens
     // during the resolution process.
     boost::system::error_code ec;
   
     // Step 5.
     asio::ip::udp::resolver::iterator it =
       resolver.resolve(resolver_query, ec);
   
     // Handling errors if any.
     if (ec) {
       // Failed to resolve the DNS name. Breaking execution.
       std::cout << "Failed to resolve a DNS name."
         << "Error code = " << ec.value() 
         << ". Message = " << ec.message();
   
       return ec.value();
     }
   
     asio::ip::udp::resolver::iterator it_end;
   
     for (; it != it_end; ++it) {
       // Here we can access the endpoint like this.
       asio::ip::udp::endpoint ep = it->endpoint();
       std::cout << ep << std::endl;
     }
     return 0;
   }
   ```

### 5. Binding a socket to an endpoint

1. **binding**: The process of associating a socket with a particular endpoint is called binding.When a socket is bound to an endpoint, all network packets coming into the host from the network with that endpoint as their target address will be redirected to that particular socket by the operating system.

2. * Usually, the client application doesn't need to explicitly bind an active socket to a specific endpoint just because it doesn't need that specific endpoint to communicate with the server; it only needs anyendpoint for that purpose. Therefore, it usually delegates the right to choose the IP address and the port number to which the socket should be bound to the operating system. 
   * The server application usually needs to bind its acceptor socket to a particular endpoint explicitly. This is explained by the fact that the server's endpoint must be known to all the clients that want to communicate with it and should stay the same after the server application is restarted.

   一般而言，对于客户端程序不需要自己将 socket 绑定到一个 endpoint 上，这是因为客户端可以使用任意一个 endpoint 来与服务器进行通信，所以这一绑定的过程移交至操作系统来完成，需要注意的是，即使只有一个网络接口或者主机上只有一个 IP 地址的时候，操作系统也不保证每次分配给客户端固定的端口号。

   但是对于服务器而言，由于它需要客户端与其进行连接后来进行通信，所以它的 endpoint 每次都不能发生变化，所以每次必须进行显示绑定，将 socket 绑定到一个固定的 endpoint 上，否则客户端就找不到服务器，将无法进行通信。

3. example code :
   ```cpp
   #include <boost/asio.hpp>
   #include <iostream>
   
   using namespace boost;
   
   int main()
   {
     // Step 1. Here we assume that the server application has
     // already obtained the protocol port number.
     unsigned short port_num = 3333;
   
     // Step 2. Creating an endpoint.
     asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(),
       port_num);
   
     // Used by 'acceptor' class constructor.
     asio::io_service ios;
   
     // Step 3. Creating and opening an acceptor socket.
     // socket 有两种构造，一种是先构造，后面调用open打开
     // 还有一种就是这样，构造的同时进行打开，需要传入使用的协议作为参数
     asio::ip::tcp::acceptor acceptor(ios, ep.protocol());
   
     boost::system::error_code ec;
   
     // Step 4. Binding the acceptor socket.
     acceptor.bind(ep, ec);
   
     // Handling errors if any.
     if (ec != 0) {
       // Failed to bind the acceptor socket. Breaking
       // execution.
       std::cout << "Failed to bind the acceptor socket."
         << "Error code = " << ec.value() << ". Message: "
         << ec.message();
   
       return ec.value();
     }
   
     return 0;
   }
   ```

### 6. Connecting  a socket

1. The connection is performed synchronously, which means that the method blocks the caller thread until either the connection operation is established or an error occurs.
2. Before performing the connection establishment procedure, the socket's connect() method will bind the socket to the endpoint consisting of an IP address and a protocol port number chosen by the operating system.即客户端程序不需要手动去绑定终端节点，只需要发起建立连接的请求即可（调用 `connect` 函数）

### 7. Accepting connections

1. Note that the acceptor socket is only used to establish connections with client applications and is not used in the further communication process.

2. example code:
   ```cpp
   #include <boost/asio.hpp>
   #include <iostream>
   
   using namespace boost;
   
   int main()
   {
     // The size of the queue containing the pending connection
     // requests.
     const int BACKLOG_SIZE = 30;
   
     // Step 1. Here we assume that the server application has
     // already obtained the protocol port number.
     unsigned short port_num = 3333;
   
     // Step 2. Creating a server endpoint.
     asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(),
       port_num);
   
     asio::io_service ios;
   
     try {
       // Step 3. Instantiating and opening an acceptor socket.
       asio::ip::tcp::acceptor acceptor(ios, ep.protocol());
   
       // Step 4. Binding the acceptor socket to the 
       // server endpint.
       acceptor.bind(ep);
   
       // Step 5. Starting to listen for incoming connection
       // requests.
       acceptor.listen(BACKLOG_SIZE);
   
       // Step 6. Creating an active socket.
       asio::ip::tcp::socket sock(ios);
   
       // Step 7. Processing the next connection request and 
       // connecting the active socket to the client.
       acceptor.accept(sock);
   
       // At this point 'sock' socket is connected to 
       //the client application and can be used to send data to
       // or receive data from it.
     }
     catch (system::system_error &e) {
       std::cout << "Error occured! Error code = " << e.code()
         << ". Message: " << e.what();
   
       return e.code().value();
     }
   
     return 0;
   }
   ```

   `listen` 的时候是将服务器切换到监听模式，只有在此模式下，操作系统才会将客户端发送的连接请求放置一个队列中处理，否则直接拒绝，但是 `listen` 操作不会阻塞，阻塞的是 `accept` 操作，它回去检查待处理请求队列，如果有请求，那么就会使用新创建的 socket 来与客户端进行通信；如果队列为空，则阻塞等待。

## Chapter 2. I / O Operations

### 1. Using fixed length I/O buffers

1. In Boost.Asio, a fixed length buffer is represented by one of the two classes:`asio::mutable_buffer` or `asio::const_buffer`.However, neither the `asio::mutable_buffer` nor `asio::const_buffer` classes are used in Boost.Asio I/O functions and methods directly. Instead, the `MutableBufferSequence` and `ConstBufferSequence` concepts are introduced.

2. The `MutableBufferSequence` concept specifies an object that represents a collection of the `asio::mutable_buffer` objects. Correspondingly, the `ConstBufferSequence` concept specifies an object that represents a collection of the `asio::const_buffer` objects.

3. The `asio::buffer()` free function has 28 overloads that accept a variety of representations of a buffer and return an object of either the `asio::mutable_buffers_1` or `asio::const_buffers_1` classes. If the buffer argument passed to the `asio::buffer()` function is a read-only type, the function returns an object of the `asio::const_buffers_1` class; otherwise, an object of the `asio::mutable_buffers_1` class is returned.

4. example code:
   ```cpp
   #include <boost/asio.hpp>
   #include <iostream>
   
   using namespace boost;
   
   int main()
   {
     std::string buf; // 'buf' is the raw buffer. 
     buf = "Hello";   // Step 1 and 2 in single line.
   
     // Step 3. Creating buffer representation that satisfies 
     // ConstBufferSequence concept requirements.
     asio::const_buffers_1 output_buf = asio::buffer(buf);
   
     // Step 4. 'output_buf' is the representation of the
     // buffer 'buf' that can be used in Boost.Asio output
     // operations.
   
     return 0;
   }
   ```

5. `asio::buffer()` 只是一个适配器，提供一个接口来使得满足 `asio` 对于缓冲区的要求，而 `raw buf` 本身并不从属于 `asio::buffer`，即 `raw buf` 本身的生命周期需要自己管理。

### 2. Using extensible stream-oriented I/O buffers

1. ```cpp
   typedef basic_streambuf<> streambuf;
   ```

   此处是 `basic_streambuf` 有默认类型参数，所以是将该类模板的默认类型实例化后的类类型定义为别名 `streambuf`。

### 3. Writing to a TCP socket synchronously

