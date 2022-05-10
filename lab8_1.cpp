#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cstdlib>

#include <stdlib.h>
#include <sys/select.h>
#include <termios.h>

#include <csignal>
#include <sys/mman.h>
#include <fcntl.h>

#include <errno.h>
#include <string.h>
#include <sys/un.h>

#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <signal.h>

#include <fstream>
#include <mqueue.h>
#include <pwd.h>

typedef void (*sighandler_t)(int);

#define SIZE 256
using namespace std;

int server; 
                     
pthread_mutex_t queue_mutex, out_mutex;   
vector<string> msglist; 

char sndbuf[SIZE];
char rcvbuf[SIZE];
int flag_receive = 0; 
int flag_request = 0; 
int flag_connect = 0; 

pthread_t _thread_receive, _thread_request, _thread_connect;
int s;

struct sockaddr_un servAddr;
struct sockaddr_un clAddr;

/**int out(string str)
{
    pthread_mutex_lock(&out_mutex);
    cout << str << "\n";
    pthread_mutex_unlock(&out_mutex);
    return 1;
}**/

void *get_receive(void *arg)
{
    
    printf("Receive start.\n"); //out
    while (!flag_receive)
    {
        memset(rcvbuf, '\0', SIZE);
        int reccount = recv(s, rcvbuf, sizeof(rcvbuf), 0);//принять запрос из сокета
        if (reccount == -1)
        {
            perror("recv");
            sleep(1);
        }
        else if (reccount == 0)
        {
            printf("\nEmpty queue..."); //out
            shutdown(server, SHUT_RDWR);//разъединение;
            sleep(1);
        }
        else
        {
            pthread_mutex_lock(&queue_mutex);
            string str = rcvbuf;
            msglist.push_back(str);
            pthread_mutex_unlock(&queue_mutex);
        }
        sleep(1);
    }
    return NULL;
}

void *get_request(void *arg)
{
    printf("Request start"); // out
    int count = 0;

    int size = 0;

    struct passwd *p;
    uid_t  uid; 

    while (!flag_request)
    {
        pthread_mutex_lock(&queue_mutex);
        if (!msglist.empty())
        {
            if (msglist.size() == 0)
                continue;
            string S = msglist.back();
            printf("Client's message: \"" , S , "\""); //out
            msglist.pop_back();
            pthread_mutex_unlock(&queue_mutex);


            memset(sndbuf, '\0', SIZE);
            uid = (uid_t) 0;
            p = getpwuid(uid = getuid());
            string name = p->pw_name; //-
            size = sprintf(sndbuf, "pw_name  : %s, count = %d\n", p->pw_name, count);
            count++;

            pthread_mutex_lock(&out_mutex);
            cout << "Message to client will be: \"";
            for (int i = 0; i < size; i++)
            {
                cout << sndbuf[i];
            }
            cout << "\"\n\n";
            pthread_mutex_unlock(&out_mutex);

            int sentcount = send(s, sndbuf, size, 0);
            if (sentcount == -1)
            {
                perror("send");
            }
        }
        else
        {
            pthread_mutex_unlock(&queue_mutex);
            printf("empty queue"); //out
        }
        sleep(1);
    }
    return NULL;
}

void *get_connect(void *arg)
{
    socklen_t len = sizeof(clAddr);
    while (!flag_connect)
    {
        s = accept(server, (struct sockaddr *)&clAddr, &len);
        if (s < 0)
        {
            printf("Server is waiting..."); //out
            sleep(1);
            continue;
        }
        else
        {
            printf("Client has been connected!"); //
            
            pthread_create(&_thread_receive, NULL, &get_receive, NULL);
            pthread_create(&_thread_request, NULL, &get_request, NULL);
            break;
        }
    }
    return NULL;
}

void sig_handler(int signo)
{
    cout << "\nForce exit...\n";
    shutdown(server, SHUT_RDWR);
    close(server);
    unlink("serversocket.soc");
    unlink("clintsocket.soc");
    pthread_mutex_destroy(&queue_mutex);
    pthread_mutex_destroy(&out_mutex);
    exit(0);
}


int main()
{
    signal(SIGPIPE, sig_handler);

    pthread_mutex_init(&queue_mutex, NULL);
    pthread_mutex_init(&out_mutex, NULL);

    server = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    servAddr = {0};
    servAddr.sun_family = AF_UNIX;
    strcpy(servAddr.sun_path, "serversocket.soc"); 

    clAddr = {0};
    clAddr.sun_family = AF_UNIX;
    strcpy(clAddr.sun_path, "clintsocket.soc"); 
    

    int optval = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    fcntl(server, F_SETFL, O_NONBLOCK);

    
    int res = bind(server, (struct sockaddr *)&clAddr, sizeof(clAddr));
    if (res == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    res = listen(server, 4);
    if (res == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    res = pthread_create(&_thread_connect, NULL, &get_connect, NULL);
    if (res == -1)
    {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    res = getchar();
    if (res == -1)
    {
        perror("getchar");
    }
    flag_connect = 1;
    flag_receive = 1;
    flag_request = 1;

    pthread_join(_thread_connect, NULL);
    pthread_join(_thread_request, NULL);
    pthread_join(_thread_receive, NULL);
    shutdown(server, SHUT_RDWR);
    close(server);
    unlink("serversocket.soc");
    unlink("clintsocket.soc");
    pthread_mutex_destroy(&queue_mutex);
    pthread_mutex_destroy(&out_mutex);
    
    return 0;
}