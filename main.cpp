#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <stddef.h>

#define BUFFER_SIZE 256

#define HEADER_SIZE 4
#define USER_ID_SIZE 2
#define MAX_USER_COUNT 5

enum typeEnum
{
    LOG_IN = 1,
    USER_INFO,

};

#pragma pack(1)
struct msgHeader
{
	short type;
	short res;
};

//登录消息体
struct msgBodyLogin
{
    char name;
};
struct msgLogin
{
    msgHeader header;
    msgBodyLogin body;
};

//登录ACK消息
struct msgBodyLoginAck
{
    short id;
};
struct msgLoginAck
{
    msgHeader header;
    msgBodyLoginAck body;
};

//用户信息消息体
struct msgBodyUserInfo
{
    short id;
    char name;
};
struct msgUserInfo
{
    msgHeader header;
    msgBodyUserInfo body;
};
#pragma pack()

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


int setMsgUserName(struct msgUserInfo *msg, struUser *user)
{
    int nameOffset = offsetof(struct msgUserInfo, body) + offsetof(struct msgBodyUserInfo, name);
    memcpy(&(msg->body.name), user->name, user->name_len);
    return nameOffset + user->name_len;
}

void copyNameIntoUser(struct msgLogin *msg, int len, struUser *user)
{
    int name_len = len - offsetof(struct msgLogin, body) - offsetof(struct msgBodyLogin, name);
    memcpy(user->name, &(msg->body.name), name_len);
    user->name_len = name_len;
}

int getValidUserId(msgLogin *msg)
{
    /*重复登录*/
    /*暂时用用户名区别，后续使用openid*/
    for (int i = 0; i < MAX_USER_COUNT; i++)
    {
        if (users[i].login == true)
        {
            if(0==memcmp(&msg->body.name, users[i].name, users[i].name_len))
                return i;
        }
        
    }

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


    msgHeader *pRecvHead = (msgHeader *)buffer;
    msgHeader *pSendHead = (msgHeader *)sendbuf;
    short type = pRecvHead->type;
    if (LOG_IN == type)
    {
        msgLogin *pRecvLogin = (msgLogin *)buffer;
        id = getValidUserId(pRecvLogin);
        if (id != -1)
        {

            msgLoginAck *pMsgLoginAck = (msgLoginAck *)sendbuf;

            copyNameIntoUser(pRecvLogin, len, &users[id]);
            pMsgLoginAck->body.id = id;
            pMsgLoginAck->header.type = LOG_IN;
            printf("send login response\n");
            //printf("Q%c Q%c Q%c Q%c Q%c Q%c Q%c Q%c Q%c \n", )
            sendto(sockSer, sendbuf, sizeof(msgLoginAck), 0, (struct sockaddr*)&addrCli, addrlen);    //向客户端发送数据
            users[id].login = true;
            users[id].sock = addrCli;

            msgUserInfo *pSendMsgUserInfo = (msgUserInfo *)sendbuf;
            for (int i = 0; i < MAX_USER_COUNT; i++)
            {
                if (users[i].login == false) continue;
                if (id == i) continue;

                pSendHead->type = USER_INFO;
                pSendMsgUserInfo->body.id = i;
                sendLen = setMsgUserName(pSendMsgUserInfo, &users[i]);
                printf("send info of user[%d] to [%d]\n", i, id);
                sendto(sockSer, sendbuf, sendLen, 0, (struct sockaddr*)&addrCli, addrlen);    //向客户端发送数据

                pSendHead->type = USER_INFO;
                pSendMsgUserInfo->body.id = id;
                sendLen = setMsgUserName(pSendMsgUserInfo, &users[id]);
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

