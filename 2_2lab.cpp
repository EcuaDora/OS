#include <cstdio>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

using namespace std;
sem_t semaphore;
int exit_2 = 2;

void *proc1(void *flag) 
{
    printf("proc1 is started\n");
	while (*((bool *)flag) != 0) 
    {
		while (sem_trywait(&semaphore) != 0) {
			cout << "semaphore 1 failed" << endl;
			sleep(3);
		}

		cout << "semaphore 1 is locked" << endl;
		for (int i = 0; i < 3; i++) {
			cout << 1 << endl;
			sleep(1);
		}
		cout << "semaphore 1 is unlocked " << endl;
		sem_post(&semaphore);
		sleep(1);
	}
	pthread_exit(NULL);
}

void *proc2(void *flag) {

    printf("proc2 is started\n");
    while (*((bool *)flag) != 0) {
		while (sem_trywait(&semaphore) != 0) {
			cout << "semaphore 2 failed" << endl;
			sleep(3);
		} 
		cout << "semaphore 2 is locked" << endl;
		for (int i = 0; i < 3; i++) {
			cout << 2 << endl;
			sleep(1);
		}
		cout << "semaphore 2 is unlocked" << endl;
		sem_post(&semaphore);
		sleep(1);
	}
	pthread_exit(&exit_2);
}


int main() {

	cout << "Program is started" << endl;
	sem_init(&semaphore, 0, 1);

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

    sem_destroy(&semaphore);

    delete join;
    delete flag;
    return 0;

}