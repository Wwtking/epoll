#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAXEVENTS 100

// 把socket设置为非阻塞的方式。
int setnonblocking(int sockfd);

// 初始化服务端的监听端口。
int initserver(int port);

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("usage:./tcpepoll port\n"); 
    return -1;
  }

  // 初始化服务端用于监听的socket
  int listensock = initserver(atoi(argv[1]));
  printf("listensock=%d\n",listensock);

  if (listensock < 0)
  {
    printf("initserver() failed.\n");
    return -1;
  }

  setnonblocking(listensock);

  int epollfd;

  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));

  // 创建一个描述符
  epollfd = epoll_create(1);

  // 添加监听描述符事件
  struct epoll_event ev;
  ev.data.fd = listensock;
  ev.events = EPOLLIN | EPOLLET;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, listensock, &ev);

  while (1)
  {
    struct epoll_event events[MAXEVENTS]; // 存放有事件发生的结构数组。

    // 等待监视的socket有事件发生。
    int infds = epoll_wait(epollfd, events, MAXEVENTS, -1);
    // printf("epoll_wait infds=%d\n",infds);

    sleep(5);

    // 返回失败。
    if (infds < 0)
    {
      printf("epoll_wait() failed.\n"); 
      perror("epoll_wait()"); 
      break;
    }

    // 超时。
    if (infds == 0)
    {
      printf("epoll_wait() timeout.\n");
      continue;
    }

    // 遍历有事件发生的结构数组
    for (int ii=0; ii<infds; ii++)
    {
      if ((events[ii].data.fd == listensock) && (events[ii].events & EPOLLIN))
      {
        // 如果发生事件的是listensock，表示有新的客户端连上来。
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        //如果服务器调用accept()时还没有客户端的连接请求，就阻塞等待直到有客户端连接上来
        /**
         * @brief accept()函数接受连接请求等待队列中待处理的客户端连接请求
         *        int accept(int sockfd, struct spckaddr *addr, skcklen_t *sddlen);
         * @param[in] sockfd 服务器套接字的文件描述符
         * @param[in] addr 传出参数，返回的连接成功的信息，所以我们不需要对这个套接字进行初始化。返回链接客户端地址信息，含IP地址和端口号
         * @param[in] addrlen 传入传出参数，一开始传入一个参数防止溢出，调用完成之后长度会发生改变。返回真正接收到地址结构体的大小
         * @return 成功:返回一个新的socket文件描述符，用于和客户端通信。    失败:-1，设置errno
        */
        int clientsock = accept(listensock, (struct sockaddr*)&client, &len);
        if (clientsock < 0)
        {
          printf("accept() failed.\n"); 
          continue;
        }

        // 把新的客户端添加到epoll中
        memset(&ev, 0, sizeof(struct epoll_event));
        ev.data.fd = clientsock;
        ev.events = EPOLLIN | EPOLLET;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, clientsock, &ev);

        printf ("client(socket=%d) connected ok.\n",clientsock);
        //std::cout << infds << std::endl;
        continue;
      }
      else if (events[ii].events & EPOLLIN)
      {
        // 客户端有数据过来或客户端的socket连接被断开。
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));

        // 读取客户端的数据。
        /**
         * @brief 读取文件中的数据
         *        ssize_t read(int fd, void *buf, size_t nbyres);
         * @param[in] fd 显示数据接收对象的文件描述符
         * @param[in] buf 保存接收数据的缓冲地址值
         * @param[in] nbytes 要接收数据的最大字节数
         * @return 成功:返回接收的字节数(但遇到文件结尾则返回0)    失败:返回-1
        */
        ssize_t isize = read(events[ii].data.fd, buffer, sizeof(buffer));

        // 发生了错误或socket被对方关闭。
        if (isize <=0)
        {
          printf("client(eventfd=%d) disconnected.\n",events[ii].data.fd);

          // 把已断开的客户端从epoll中删除
          memset(&ev,0,sizeof(struct epoll_event));
          ev.events = EPOLLIN;
          ev.data.fd = events[ii].data.fd;
          epoll_ctl(epollfd, EPOLL_CTL_DEL, events[ii].data.fd, &ev);
          close(events[ii].data.fd);
          continue;
        }

        printf("recv(eventfd=%d,size=%d):%s\n", events[ii].data.fd, (int)isize, buffer);
        std::cout << infds << std::endl;

        sleep(2);
        // 把收到的报文原样发回给客户端。
        /**
         * @brief 将数据写入文件
         *        ssize_t write(int fd, const void *buf, size_t nbytes);
         * @param[in] fd 显示数据传输对象的文件描述符
         * @param[in] buf 保存要传输数据的缓冲地址值
         * @param[in] nbytes 要传数据的字节数
        */
        write(events[ii].data.fd, buffer, strlen(buffer));
      }
    }
  }

  //int close(int fd);
  //fd: 需要关闭的文件或套接字的文件描述符
  close(listensock);
  close(epollfd);

  return 0;
}

// 初始化服务端的监听端口。
int initserver(int port)
{
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    printf("socket() failed.\n"); return -1;
  }

  // Linux如下
  int opt = 1; 
  unsigned int len = sizeof(opt);
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, len);   //设置套接字描述符的属性
  setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &opt, len);

  //主机字节序(小端字节序)转换成网络字节序(大端字节序)
  //uint32_t htonl(uint32_t hostlong);
  //uint16_t htons(uint16_t hostshort);
  struct sockaddr_in servaddr;
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  //INADDR_ANY表示0.0.0.0，泛指本机
  servaddr.sin_port = htons(port);

  //服务器程序所监听的网络地址和端口号通常是固定不变的，
  //客户端程序得知服务器程序的地址和端口号后就可以向服务器发起连接，
  //因此服务器需要调用bind绑定一个固定的网络地址和端口号
  /**
   * @brief 对套接字进行地址和端口的绑定，将参数sockfd和addr(端口号和地址)绑定在一起 
   *        int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
   * @param[in] sockfd socket文件描述符
   * @param[in] addr 传入参数，指定服务器端地址信息，含IP地址和端口号
   * @param[in] addrlen 传入参数，传入sizeof(addr)大小
   * @return 成功:0    失败:-1，设置errno
  */
  if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
  {
    printf("bind() failed.\n"); 
    close(sock); 
    return -1;
  }

  //socket三次握手是在listen中完成，accept只从完成连接的队列中拿出一个连接
  /**
   * @brief 设立服务器端套接字sock来监听客户端传来的连接请求
   *        int listen(int sock, int backlog);
   * @param[in] sock 希望进入等待连接请求状态的套接字文件描述符,
   *                 传递的描述符套接字参数成为服务器端套接字(监听套接字!!!!)
   * @param[in] backlog 连接请求等待队列的长度,若为5,则队列长度为5,表示最多使5个连接请求进入队列,
   *                    其中也包含排队建立3次握手队列和刚刚建立3次握手队列的连接数之和
   * @return 成功:0    失败:-1
  */
  if (listen(sock, 5) != 0)
  {
    printf("listen() failed.\n"); 
    close(sock); 
    return -1;
  }

  return sock;
}

// 把socket设置为非阻塞的方式。
int setnonblocking(int sockfd)
{  
  if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1)  return -1;

  return 0;  
}  
