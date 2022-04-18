#include <cstdio>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <iostream>


using namespace std;
sem_t *semaphore;
FILE* file;

void *proc(void *flag)
{
    printf("proc1 is started\n");
    printf("\n");

    while (*((bool *)flag) != 0) 
    {        
        sem_wait(semaphore);
        for(int i = 0; i < 5; ++i) {
            fputc('2', file);
            fflush(file);
            putchar('2');
            fflush(stdout);
            sleep(1);
        }
        sem_post(semaphore);
        usleep(1000);
    }
    printf("proc1 finished working\n");
    pthread_exit((void *)44);
}


int main()
{
    semaphore = sem_open("/semaphore_5", O_CREAT, 0644, 1);
    file = fopen("lab5.txt", "a+"); //  "a+" Открывает файл для чтения и записи; помещает указатель в конец
                                    //файла. Если файл не существует - пытается его создать.
    int *flag = new int;
    *flag = 1;

    int exitcode;
    pthread_t id;    

    pthread_create(&id, NULL, proc, ((void *)flag));

    
    cout << "Press enter to kill all threads: " << endl;
    cout << "Program is waiting" << endl;
    getchar();
    cout << "Enter is pressed" << endl;

    *flag = 0; 

    pthread_join(id, (void**) &exitcode);
    printf("\nThread exitcode = %d\n", exitcode);

    fclose(file);
    sem_close(semaphore);
    sem_unlink("/semaphore_5");
    sem_destroy(semaphore);

    delete flag;
    return 0;
}