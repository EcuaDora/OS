
#include <cstdio>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

void *proc1(void *flag)
{
    printf("proc1 is started\n");
    int i = 0;
    while (*((int *)flag) != 0)
    {
        printf("1\n");
        cout << "1" << flush; 
        sleep(1);
        i++;
    }

    cout << "proc1 is finished with: " << i << endl;
    pthread_exit(NULL);
}

void *proc2(void *flag)
{
    printf("proc2 is started\n");
    int* i = new int; 
    *i = 0;
    while (*((int *)flag) != 0)
    {
        printf("2\n");
        cout << "2" << flush; 
        sleep(1);
        (*i)++;
    }
    sleep(1);
    cout << "proc2 is finished with: " << *i << endl;
    pthread_exit((void *)i);
}

int main()
{

    cout << "Program is started" << endl;
    pthread_t threads[2];
    int *flag = new int;
    *flag = 1;
    printf("thread_1 is created\n");

    if (pthread_create(&threads[0], NULL, proc1, ((void *)flag)) == -1)
    {
        printf("thread_1 can't be created\n");
        return -1;
    }

    printf("thread_2 is created\n");
    if (pthread_create(&threads[1], NULL, proc2, ((void *)flag)) == -1)
    {
        printf("thread_2 can't be created\n");
        return -1;
    }

    cout << "Press enter to kill all threads: " << endl;
    cout << "Program is waiting" << endl;
    getchar();
    cout << "Enter is pressed" << endl;
    *flag = 0; 
    int *join = new int; 
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], (void **)&join);
    cout << "Main got: " << *join << endl;
    delete join;
    delete flag;
    return 0;
}