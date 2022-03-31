
#include <cstdio>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std; 

bool end_1 = false;
bool end_2 = false;

void* proc1(void* flag)
{
  printf("proc1 is started\n");
  int i = 0;
  while(!end_1)
  { 
    printf("1\n");
    sleep(1);
    i++;
  }

  cout << "proc1 is finished with: " << i << endl;
  pthread_exit(NULL);
}

void* proc2(void* flag)
{
  printf("proc2 is started\n");
  int i = 0;
  while(!end_2)
  {
    printf("2\n");
    sleep(1);
    i++;
  }

  cout << "proc2 is finished with: " << i << endl;
  pthread_exit(NULL);
}

int main()
{ 
  cout << "Program is started" << endl;
	pthread_t threads[2];
    printf("thread_1 is created\n");

	if (pthread_create(&threads[0], NULL, proc1, NULL) == -1) 
    {
	    printf("thread_1 can't be created\n");
		return -1;
	}

	printf("thread_2 is created\n");
	if (pthread_create(&threads[1], NULL, proc2, NULL) == -1) {
		printf("thread_2 can't be created\n");
		return -1;
	}

	cout << "Press enter to kill all threads: " << endl;
  cout << "Program is waiting" << endl;
	getchar();
  cout << "Enter is pressed" << endl;

	end_1 = true;
	end_2 = true;

	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	return 0;
}