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
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/un.h>

typedef void (*sighandler_t)(int);

#define SIZE 256
using namespace std;
pthread_mutex_t mutex;

int client;
int flag_connect = 1, flag_sent = 1, flag_accept = 1;
pthread_t _thread_connect, _thread_accept, _thread_sent;
char sndbuf[SIZE];
char rcvbuf[SIZE];

//struct sockaddr_un clAddr;
struct sockaddr_un servAddr;

/**int out(string str)
{
    pthread_mutex_lock(&mutex);
    cout << str << "\n";
    pthread_mutex_unlock(&mutex);
    return 1;
}**/

void *client_sent(void *arg)
{
    int count = 0;
    while (flag_sent)
    {
        memset(sndbuf, '\0', SIZE);
        int size = sprintf(sndbuf, "attempt %d", count);
        int sentcount = send(client, sndbuf, size, 0);
        if (sentcount == -1)
        {
            perror("send");
        }
        else
        {
            pthread_mutex_lock(&mutex);
            cout << "The message was: \"";
            for (int i = 0; i < size; i++)
            {
                cout << sndbuf[i];
            }
            
            cout << "\"\n";
            pthread_mutex_unlock(&mutex);
        }
        sleep(1);
        count++;
    }
    return NULL;
}

void *client_accept(void *arg)
{
    while (flag_accept)
    {
        memset(rcvbuf, '\0', SIZE);
        int reccount = recv(client, rcvbuf, sizeof(rcvbuf), 0);
        if (reccount == -1)
        {
            perror("recv");
            sleep(1);
        }
        else if (reccount == 0)
        {
            sleep(1);
        }
        string s = rcvbuf;
        cout<< "Server answer was: \"" << s << "\"\n" << flush; //out
        //fflush(stdout);
        sleep(1);
    }
    return NULL;
}

void *client_connect(void *arg)
{
    servAddr = {0};
    servAddr.sun_family = AF_UNIX;
    strcpy(servAddr.sun_path, "clintsocket.soc"); 
    //clAddr.sun_family = AF_UNIX;
    //strcpy(clAddr.sun_path, "cltsocket.soc"); 

    while (flag_connect)
    {
        int res = connect(client, (struct sockaddr *)&servAddr, sizeof(servAddr));
        if (res < 0)
        {
            perror("connect");
            sleep(1);
            continue;
        }

        pthread_create(&_thread_accept, NULL, &client_accept, NULL);
        pthread_create(&_thread_sent, NULL, &client_sent, NULL);
        printf("Server has been connected!\n"); //out
        return NULL;
    }
    return NULL;
}

void sig_handler(int signo)
{
    cout << "\nForce exit...\n";
    shutdown(client, SHUT_RDWR);
    close(client);
    unlink("serversocket.soc");
    unlink("clintsocket.soc");
    pthread_mutex_destroy(&mutex);
    exit(0);
}

int main()
{
    signal(SIGPIPE, sig_handler);

    pthread_mutex_init(&mutex, NULL);
    client = socket(AF_UNIX, SOCK_STREAM, 0);

    int optval = 1;
    setsockopt(client, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    fcntl(client, F_SETFL, O_NONBLOCK);

    pthread_create(&_thread_connect, NULL, &client_connect, NULL);

    getchar();
    flag_connect = flag_sent = flag_accept = 0;

    pthread_join(_thread_connect, NULL);
    pthread_join(_thread_accept, NULL);
    pthread_join(_thread_sent, NULL);
    
    pthread_mutex_destroy(&mutex);
    shutdown(client, SHUT_RDWR);
    
    close(client);
    unlink("serversocket.soc");
    unlink("clintsocket.soc");
    
    return 0;
}