#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstdio>
#include <pwd.h>
#include <string.h>
#include <ostream>



using namespace std;
sem_t *semaphore_write, *semaphore_read;


struct passwd *p;
uid_t  uid; 
dev_t*address;
int nm;

void *proc(void *flag)
{
    int *flag1 = (int *)flag;
    printf("Thread has started working\n");

    struct passwd *p;
    uid_t  uid;     //id-user'a
    char buf[256];
    while (*flag1 != 1)
    {   uid = (uid_t) 0;
        p = getpwuid(uid = getuid());
        string name = p->pw_name;
        for (int i = 0; i < name.size(); i++)
        {
          buf[i] = name[i];
          buf[i + 1] = '\0';
        }
        printf("  pw_name  : %s\n", p->pw_name); 
        memcpy(address, buf, name.size());
        sem_post(semaphore_write);
        sem_wait(semaphore_read);  
        sleep(1);
    }
    printf("Thread has finished working\n");
    pthread_exit((void *)44);
}

int main()
{
    setlocale(LC_ALL,"en");

    int flag1 = 0;
    int exitcode;
    pthread_t id;
    
    nm = shmget(ftok("lab6_1.cpp", 0), sizeof(uid_t), 0644 | IPC_CREAT);
    address = (dev_t*)shmat(nm, NULL, 0);
    semaphore_write = sem_open("/semaphore_write", O_CREAT, 0644, 0);
    semaphore_read = sem_open("/semaphore_read", O_CREAT, 0644, 0);

    pthread_create(&id, NULL, proc, &flag1);

    printf("Program is waiting for the key to be pressed\n\n");
    getchar();
    printf("Key has been pressed\n\n");

    flag1 = 1;
    pthread_join(id, (void **)&exitcode);
    printf("Thread exitcode = %d\n", exitcode);

    sem_close(semaphore_read);
    sem_unlink("/semaphore_read");
    sem_close(semaphore_write);
    sem_unlink("/semaphore_write");
    shmdt(address);
    shmctl(nm, IPC_RMID, NULL);

    return 0;
}
