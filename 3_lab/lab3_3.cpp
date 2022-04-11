#define _POSIX_SOURCE
#include <cstdio>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>


int fd[2];

void *proc1(void *flag)
{
    int *flag1 = (int *)flag;
    printf("Thread 1 has started working\n");

    struct passwd *p;
    uid_t  uid;     //id-user'a
    
    while (*flag1 != 1)
    {        
        int rv = write(fd[1], &uid, sizeof(uid_t));
        if (rv == -1)
        {
            perror("write");
        }
        sleep(1);
    }
    printf("Thread 1 has finished working\n");
    pthread_exit((void *)33);
}

void *proc2(void *flag)
{
    int *flag2 = (int *)flag;
    printf("Thread 2 has started working\n");

    struct passwd *p;
    uid_t  uid;

    while (*flag2 != 1)
    {   
        
        uid = (uid_t) 0;
        int rv = read(fd[0], &uid, sizeof(uid_t));
        if (rv == -1)
        {
            perror("read");
            sleep(1);
        }
        else
        {   p = getpwuid(uid = getuid());
            printf("  pw_name  : %s\n", p->pw_name);
        }
    }
    printf("Thread 2 has finished working\n");
    pthread_exit((void *)44);
}

int main()
{
    pthread_t id1, id2;
    int flag1 = 0, flag2 = 0;
    int exitcode1, exitcode2;

    pipe(fd);
    fcntl(fd[0], F_SETFL, O_NONBLOCK);
    fcntl(fd[1], F_SETFL, O_NONBLOCK);
    pthread_create(&id1, NULL, proc1, &flag1);
    pthread_create(&id2, NULL, proc2, &flag2);
    
    printf("Program is waiting for the key to be pressed\n\n");
    getchar();
    printf("Key has been pressed\n\n");

    flag1 = flag2 = 1;
    pthread_join(id1, (void**) &exitcode1);
    pthread_join(id2, (void**) &exitcode2);
    printf("\nThread 1 exitcode = %d\n", exitcode1);
    printf("Thread 2 exitcode = %d\n", exitcode2);

    close(fd[0]);
    close(fd[1]);
    return 0;
}
