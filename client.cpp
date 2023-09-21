#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("usage:./tcpclient ip port\n"); 
    return -1;
  }

  int sockfd;
  struct sockaddr_in servaddr;
  char buf[1024];
 
  /**
   * @brief 使用socket()函数创建套接字  int socket(int af, int type, int protocol);
   * @param[in] af IP地址类型：AF_INET表示IPv4地址，AF_INET6表示IPv6地址
   * @param[in] type 数据传输方式/套接字类型 
   *                 SOCK_STREAM：流格式套接字/面向连接的套接字 
   *                 SOCK_DGRAM：数据报套接字/无连接的套接字
   * @param[in] protocol 传输协议：IPPROTO_TCP表示TCP传输协议，IPPTOTO_UDP表示UDP传输协议
   *                     将protocol的值设为0，系统会自动推演出应该使用什么协议
   * @return 返回值就是一个int类型的文件描述符，返回指向新创建的 socket(套接字) 的文件描述符
  */
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
    printf("socket() failed.\n"); 
    return -1; 
  }
  
  //输入IP地址(argv[1])和端口号(argv[2])时，其实输入的是字符串
  memset(&servaddr, 0, sizeof(servaddr));    //清零
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(atoi(argv[2]));      //端口  atoi()的作用就是把字符串转换成整型数
  servaddr.sin_addr.s_addr = inet_addr(argv[1]); //IP地址  inet_addr()为IP地址转换函数

  /**
   * @brief 客户端需要调用connect()连接服务器 
   *        int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
   * @param[in] sockfd socket文件描述符
   * @param[in] addr 传入参数，指定服务器端地址信息，含IP地址和端口号
   * @param[in] addrlen 传入参数，传入sizeof(addr)大小
   * @return 成功:0    失败:-1，设置errno
  */
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
  {
    printf("connect(%s:%s) failed.\n",argv[1],argv[2]); 
    close(sockfd);  
    return -1;
  }

  printf("connect ok.\n");

  for (int ii=0; ii<10000; ii++)
  {
    // 从命令行输入内容。
    memset(buf,0,sizeof(buf));
    printf("please input:"); 
    scanf("%s",buf);
    // sprintf(buf,"1111111111111111111111ii=%08d",ii);

    // write和read，如果没有执行，会一直阻塞
    if (write(sockfd, buf, strlen(buf)) <=0)
    { 
      printf("write() failed.\n");  
      close(sockfd);  
      return -1;
    }
		
    memset(buf,0,sizeof(buf));
    if (read(sockfd, buf, sizeof(buf)) <=0) 
    { 
      printf("read() failed.\n");  
      close(sockfd);  
      return -1;
    }

    printf("recv:%s\n", buf);

    // close(sockfd); break;
  }
  close(sockfd);
} 
