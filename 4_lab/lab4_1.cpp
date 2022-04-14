
#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


using namespace std;


int main(int argc, char* argv[], char *env[])
{
    pid_t pid;
    cout << "Program 1 Start" << endl;
	cout << "Program 1 Current ID: " << getpid() << endl; 
	cout << "Program 1 Parent ID: " << getppid() << endl;

       for (int i = 0; env[i] != NULL; i++){
           cout << env[i] << endl;
        }
        sleep(1);
     
    cout << "Program 1 finished!\n";
    return 1;
}
