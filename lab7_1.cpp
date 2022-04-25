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


void *thread1(void *arg)
{
    int count = 0;
    printf("Thread 1 has started working\n");

    int size = 0;

    struct passwd *p;
    uid_t  uid;     //id-user'a
    char buf[256];
    while (flag != 0)
    {
        char buffer[msg_size];
        uid = (uid_t) 0;
        p = getpwuid(uid = getuid());
        
        size = sprintf(buffer, "pw_name  : %s, count = %d\n", p->pw_name, count);

        for (int i = 0; i < size; i++)
        {
            cout << buffer[i];
        }

        int result = mq_send(mq, buffer, size, 0);//помещаю сообщение в очередь
        if (result == -1)
        {
            perror("mq_send");
            sleep(1);
            continue;
        }
        cout << "Thread 1 has wrote the message.\n\n";
        sleep(1);
        count++;
    }

    cout << "Thread 1 has finished\n";
    return NULL;
}

int main()
{
    flag = 1;
    struct mq_attr attr = {0, my_mq_maxmsg, msg_size, 0};
    mq = mq_open("/mq", O_CREAT | O_WRONLY | O_NONBLOCK, 0644, &attr); //создаю очередь сообщений
    int t = mq_getattr(mq, &attr);

    pthread_t id;
    cout << "Program 1 is ready\n";
    pthread_create(&id, NULL, &thread1, NULL);

    printf("Program is waiting for the key to be pressed\n\n");
    getchar();
    printf("Key has been pressed\n\n");

    flag = 0;
    pthread_join(id, NULL);
    mq_close(mq);
    mq_unlink("/mq");
    return 0;
}
