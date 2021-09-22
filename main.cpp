#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <stddef.h>
#include <vector>
#include <ctime>
#include <random>
#include <iostream>
#include <algorithm>
#include <set>
using namespace std;

#define BUFFER_SIZE 256

#define HEADER_SIZE 4
#define USER_ID_SIZE 2
#define MAX_USER_COUNT 6
#define MIN_USER_COUNT 4 //最少X人开局

#define MAX_CARD_NUM 13 //A=1, 2=2 ... K=13
#define CARD_REPEAT 4 //重复4个
#define MAX_CARD_COUNT (13*CARD_REPEAT+2)//每幅牌的个数
#define MAX_PACK_NUM 3 //最多副牌

enum typeEnum
{
    LOG_IN = 1,
    USER_INFO,
    READY_INF0,
    CARDS_INFO,
    SHOW_CARDS_INFO
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
    short readyStatus;
    char name;
};
struct msgUserInfo
{
    msgHeader header;
    msgBodyUserInfo body;
};

//准备消息
struct msgBodyReadyInfo
{
    short id;
};
struct msgReadyInfo
{
    msgHeader header;
    msgBodyReadyInfo body;
};

//牌信息
struct msgBodyCardsInfo
{
    short id;
    short size; /*纸牌的张数*/
    short cards;
};
struct msgCardsInfo
{
    msgHeader header;
    msgBodyCardsInfo body;
};
#pragma pack()

struct struUser
{
    char name[256];
    int name_len;
    int id;
    bool login;
    bool readyStatus;
    bool GameStatus;
    struct sockaddr_in sock;
    //short cards[MAX_CARD_COUNT*MAX_PACK_NUM/MIN_USER_COUNT];
    short cards[MAX_CARD_COUNT*MAX_PACK_NUM];
    short cardSize = 0;
};


//创建一个套接字，并检测是否创建成功
int sockSer = -1;
struct sockaddr_in addrSer;  //创建一个记录地址信息的结构体 
struUser users[MAX_USER_COUNT];
struct sockaddr_in addrCli;
socklen_t addrlen;


struct element{     //用来排序的数据结构 
		int data;  // 数据 
		int index;  // 序号 
};


int cmp(const void *a,const void *b){   // 升序排序
	return((struct element*)a)->data - ((struct element*)b)->data;
}
void rand_of_n(short a[],int n){
	int i;
	struct element ele[MAX_CARD_COUNT*MAX_PACK_NUM];
	srand((int)time(0));  // 初始化随机数种子 
	for(i=0;i<n;i++){
		ele[i].data=rand();  // 随机生成一个数 
        ele[i].index=(i+1)%MAX_CARD_COUNT+1;
	}
	qsort(ele,n,sizeof(ele[0]),cmp);  //排序 
	for(i=0;i<n;i++){
		a[i]=ele[i].index;
	}
}

void giveOutCard(int UserCount, int pack=2)
{
    
    int cardCount = MAX_CARD_COUNT*pack;
    short a[MAX_CARD_COUNT*MAX_PACK_NUM] = {0};

    rand_of_n(a, cardCount);

    // for(int i = 0;i<cardCount;i++)
    // {
    //     cout<<a[i]<<" ";     
    // }
    // cout<<endl;

    for(int i = 0;i<UserCount;i++)
    {
        users[i].cardSize = 0;     
    }

    for(int i = 0;i<cardCount;i++)
    {
        users[i%UserCount].cards[i/UserCount] = a[i];     
        users[i%UserCount].cardSize++;  
    }

    for(int i = 0;i<UserCount;i++)
    {
        sort(users[i].cards, users[i].cards+users[i].cardSize);
        for(int j = 0;j<users[i].cardSize;j++)
        {
            cout<<users[i].cards[j]<<" ";
        }     
        cout<<endl;
    }    
}

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

void sendUserInfotoClient(char *sendbuf, short usrId, short client)
{
    msgUserInfo *pSendMsgUserInfo = (msgUserInfo *)sendbuf;
    
    pSendMsgUserInfo->header.type = USER_INFO;
    pSendMsgUserInfo->body.id = usrId;
    pSendMsgUserInfo->body.readyStatus = users[usrId].readyStatus;
    int sendLen = setMsgUserName(pSendMsgUserInfo, &users[usrId]);
    printf("send LOG_IN info of user[%d] to [%d]\n", usrId, client);
    sendto(sockSer, sendbuf, sendLen, 0, (struct sockaddr*)&users[client].sock, addrlen);    //向客户端发送数据
}

void sendCardsInfo(char *sendbuf, short usrId, short client)
{
    msgCardsInfo *pMsg = (msgCardsInfo *)sendbuf;

    pMsg->header.type = CARDS_INFO;
    pMsg->body.id = usrId;
    pMsg->body.size = users[usrId].cardSize;
    int cardsInfoLength = users[usrId].cardSize*sizeof(users[usrId].cards[0]);
    memcpy(&pMsg->body.cards, users[usrId].cards, cardsInfoLength);
    //int Offset = offsetof(struct msgCardsInfo, body) + offsetof(struct msgBodyCardsInfo, cards);
    int sendLen = cardsInfoLength + (int)((char*)(&pMsg->body.cards) - (char*)pMsg);
    printf("send CARDS_INFO info of user[%d] to user[%d], sendLen = %d\n", usrId, client, sendLen);
    sendto(sockSer, sendbuf, sendLen, 0, (struct sockaddr*)&users[client].sock, addrlen);    //向客户端发送数据
}

void differenceCards(int UserId, short * arr, int size)
{
	multiset<int> iset1(users[UserId].cards, users[UserId].cards + users[UserId].cardSize);
	multiset<int> iset2(arr, arr + size);
	int ivec[MAX_CARD_COUNT*MAX_PACK_NUM];
	auto iter = set_difference(iset1.begin(), iset1.end(), iset2.begin(), iset2.end(), ivec);	//ivec为：2,3,5,6,7
	users[UserId].cardSize = (int)(iter - ivec);

    printf("update user[%d] cards: ", UserId);
	for (int i=0;i<users[UserId].cardSize;i++)
	{
		cout << ivec[i] << " ";
	}
}

void printBuff(char* buffer, int len)
{
    short *p =  (short *)(buffer);
    
    printf("printBuff\n");
    for(int i = 0 ;i<len/2;i++)
    {
        short val = *(p+i);
        printf("%d ", val);
    }
    printf("printBuff end\n");
}

void handMsg(char* buffer, int len)
{
    char sendbuf[BUFFER_SIZE];    //申请一个发送数据缓存区
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

            
            for (int i = 0; i < MAX_USER_COUNT; i++)
            {
                if (users[i].login == false) continue;
                if (id == i) continue;

                sendUserInfotoClient(sendbuf, i, id);
                sendUserInfotoClient(sendbuf, id, i);
            }

        }

    }
    else if(READY_INF0 == type)
    {
        msgReadyInfo *pMsgReadyInfo = (msgReadyInfo *)buffer;
        id = pMsgReadyInfo->body.id;
        printf("receive READY_INF0 id = %d\n", pMsgReadyInfo->body.id);
        printBuff(buffer, len);
        users[id].readyStatus = true;
        //转发给所有人
        for (int i = 0; i < MAX_USER_COUNT; i++)
        {
            if (users[i].login == false) continue;
            printf("send READY_INF of user[%d] to [%d]\n", pMsgReadyInfo->body.id, i);
            sendto(sockSer, buffer, len, 0, (struct sockaddr*)&users[i].sock, addrlen);    //向客户端发送数据
        }

        //
        giveOutCard(5, 2);
        
        sendCardsInfo(sendbuf, id, id);
    }
    else if(SHOW_CARDS_INFO == type)
    {
        msgCardsInfo *pReceiveMsg = (msgCardsInfo *)buffer;
        id = pReceiveMsg->body.id;
        printf("receive SHOW_CARDS_INFO id = %d\n", pReceiveMsg->body.id);
        // 更新该用户的cars
        // users[id].cards =......;DifferenceUserCards(i, , )
        differenceCards(id, &pReceiveMsg->body.cards, pReceiveMsg->body.size);
        //转发给所有人
        for (int i = 0; i < MAX_USER_COUNT; i++)
        {
            if (users[i].login == false) continue;
            //if (users[i].GameStatus == false) continue;
            printf("send SHOW_CARDS_INFO of user[%d] to [%d]\n", id, i);
            sendto(sockSer, buffer, len, 0, (struct sockaddr*)&users[i].sock, addrlen);    //向客户端发送数据
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

