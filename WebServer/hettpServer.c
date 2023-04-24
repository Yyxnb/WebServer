/*
    1.实现客户端和服务器的连接
    2.处理客户端的请求
*/
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//实现socket 绑定 监听
int initServer()
{
    //创建套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1)
    {
        perror("创建socket失败");
        return -1;
    }
    printf("创建socket成功\n");
    //设置服务器地址簇
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1")
    //绑定
    int r = bind(fd, (struct sockaddr*)&addr, sizeof addr);
    if(r == -1)
    {
        perror("绑定失败");
        return -1;
    }
    printf("绑定成功\n");
    //监听
    r = listen(fd, 10);
    if(r == -1)
    {
        perror("监听失败");
        return -1;
    }
    printf("监听成功\n");
    return fd;
}
//处理客户端的请求
void handleCient(int fd)
{
    //接收客户端发来的信息
    char buf[1024*1024] = {0};
    int r = read(fd, buf, sizeof(buf));;
    if(r > 0)
    {
        printf("接收到客户端发来的请求:%s", buf);
    }

    //解析客户端请求
    char fileName[20] = {0};
    sscanf(buf, "GET /%s", fileName);
    printf("解析出的文件名为:%s\n", fileName);

    //显示文件
    //根据文件名 得到文件类型 告诉浏览器发送什么文件 
    char* mime = NULL;
    if(strstr(fileName, ".html"))
    {
        mime = "text/html"; //文本或者html类型
    }
    else if(strstr(fileName, ".jpg"))
    {
        mime = "image/jpg"; //图片类型
    }

    char response[4096*1024] = {0}; //响应头
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-type: %s\r\n\r\n", mime);
    int responseLen = strlen(response);
    int fileFd = open(fileName, O_RDONLY);
    int fileLen = read(fileFd, responseLen+response, sizeof(response)-responseLen);
    write(fd, response, responseLen+fileLen); 
    close(fileFd);
    sleep(1);
}

int main(int argc, char const *argv[])
{
    // 1.实现连接
    int serverFd = initServer();
    int clientFd;
    // 2.处理请求
    while(1)
    {
        // 2.1 等待客户端连接
        clientFd = accept(serverFd, NULL, NULL);
        if(clientFd == -1)
        {
            perror("服务器崩溃");
            break;
        }
        printf("有客户端连接服务器了！！！\n");
        // 2.2 处理客户端请求
        handleCient(clientFd);
        // 2.3 关闭
        close(clientFd);
    }
    return 0;
}
