#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>


#define BUFFER_SIZE 256

#define HEADER_SIZE 4
#define USER_ID_SIZE 2
#define MAX_USER_COUNT 5
#define LOG_IN_ACK_SIZE (HEADER_SIZE+2)

enum typeEnum
{
    LOG_IN = 1,
    USER_INFO,

};

struct struUser
{
    char name[256];
    int name_len;
    int id;
    bool login;
    struct sockaddr_in sock;
};

//创建一个套接字，并检测是否创建成功
int sockSer = -1;
struct sockaddr_in addrSer;  //创建一个记录地址信息的结构体 
struUser users[5];
struct sockaddr_in addrCli;
socklen_t addrlen;

short getMsgType(char* buffer)
{

    return *((short*)buffer);
}

void setMsgType(char* buffer, short type)
{
    *((short*)buffer) = type;
}

void setMsgUserId(char* buffer, short id)
{
    *((short*)(buffer + HEADER_SIZE)) = id;
}

int setMsgUserName(char* buffer, struUser *user)
{
    memcpy(buffer + HEADER_SIZE + USER_ID_SIZE, user->name, user->name_len);
    return HEADER_SIZE + USER_ID_SIZE + user->name_len;
}

void copyNameIntoUser(char* buffer, int len, struUser *user)
{
    memcpy(user->name, buffer + HEADER_SIZE, len - HEADER_SIZE);
    user->name_len = len - HEADER_SIZE;
}

int getValidUserId()
{

    for (int i = 0; i < MAX_USER_COUNT; i++)
    {
        if (users[i].login == false)
        {
            return i;
        }
    }
    return -1;
}

void handMsg(char* buffer, int len)
{
    char sendbuf[BUFFER_SIZE];    //申请一个发送数据缓存区

    int sendLen = 0;
    int id = -1;

    short type = getMsgType(buffer);
    if (LOG_IN == type)
    {
        id = getValidUserId();
        if (id != -1)
        {
            copyNameIntoUser(buffer, len, &users[id]);
            setMsgUserId(buffer, id);
            printf("send login response\n");
            sendto(sockSer, buffer, LOG_IN_ACK_SIZE, 0, (struct sockaddr*)&addrCli, addrlen);    //向客户端发送数据
            users[id].login = true;
            users[id].sock = addrCli;

            for (int i = 0; i < MAX_USER_COUNT; i++)
            {
                if (users[i].login == false) continue;
                if (id == i) continue;

                setMsgType(sendbuf, USER_INFO);
                setMsgUserId(sendbuf, i);
                sendLen = setMsgUserName(sendbuf, &users[i]);
                printf("send info of user[%d] to [%d]\n", i, id);
                sendto(sockSer, sendbuf, sendLen, 0, (struct sockaddr*)&addrCli, addrlen);    //向客户端发送数据

                setMsgType(sendbuf, USER_INFO);
                setMsgUserId(sendbuf, id);
                sendLen = setMsgUserName(sendbuf, &users[id]);
                printf("send info of user[%d] to [%d]\n", id, i);
                sendto(sockSer, sendbuf, sendLen, 0, (struct sockaddr*)&users[i].sock, addrlen);    //向客户端发送数据
            }

        }

    }
}

int main()
{
    sockSer = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockSer == -1)
        perror("socket");

    addrSer.sin_family = AF_INET;    //使用AF_INET协议族 
    addrSer.sin_port = htons(5050);     //设置地址结构体中的端口号
    addrSer.sin_addr.s_addr = htonl(INADDR_ANY);  //设置通信ip

    //将套接字地址与所创建的套接字号联系起来，并检测是否绑定成功
    addrlen = sizeof(struct sockaddr);
    int res = bind(sockSer, (struct sockaddr*)&addrSer, addrlen);
    if (res == -1)
        perror("bind");


    char recvbuf[256];    //申请一个接收数据缓存区
    memset(users, 0, sizeof(users));

    while (1)
    {
        int len = recvfrom(sockSer, recvbuf, 256, 0, (struct  sockaddr*)&addrCli, &addrlen);     //从指定地址接收客户端数据
        printf("Cli:>receive, len=%d\n", len);
        handMsg(recvbuf, len);
        //printf("Ser:>");
        //scanf("%s",sendbuf);
        //sendto(sockSer,sendbuf,strlen(sendbuf)+1,0,(struct sockaddr*)&addrCli, addrlen);    //向客户端发送数据
    }
    return 0;
}

