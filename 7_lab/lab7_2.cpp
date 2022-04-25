#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cstring>
#include <fstream>
#include <semaphore.h>
#include <cstdlib>

#include <stdlib.h>
#include <sys/select.h>
#include <termios.h>

#include <csignal>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <mqueue.h>
#include <pwd.h>

#define msg_size 256
#define my_mq_maxmsg 47 

using namespace std;

int flag = 1;
mqd_t mq;

void *thread2(void *arg)
{
    printf("Thread 2 has started working\n");

    while (flag != 0)
    {
        char buffer[msg_size];
        int result = mq_receive(mq, buffer, msg_size, 0);
        if (result == -1)
        {
            perror("mq_receive");
            sleep(1);
            continue;
        }
        for (auto i: buffer)
        {
            cout << i;
        }
        cout << "Thread 2 has written the message.\n\n";
        sleep(1);
    }
    cout << "Thread 2 has finished\n";
    return NULL;
}

int main()
{ 
    
    struct mq_attr attr = {0, my_mq_maxmsg, msg_size, 0};
    flag = 1;
    mq = mq_open("/mq", O_CREAT | O_RDONLY | O_NONBLOCK, 0644, &attr); //создаю очередь сообщений
    int t = mq_getattr(mq, &attr);

    pthread_t id;
    cout << "Program 2 is ready\n";
    pthread_create(&id, NULL, &thread2, NULL);

    printf("Program is waiting for the key to be pressed\n\n");
    getchar();
    printf("Key has been pressed\n\n");

    flag = 0;
    pthread_join(id, NULL);
    mq_close(mq);
    mq_unlink("/mq");
    return 0;
}