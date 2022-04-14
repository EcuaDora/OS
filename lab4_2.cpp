#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <time.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

using namespace std;



int main(int argc, char *argv[])
{
    cout << "Program 2 Start" << endl;
	cout << "Program 2 Curent ID: " << getpid() << endl;
	cout << "Program 2 Parent ID: " << getppid() << endl;
    cout << endl;

    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        return -1;
    }

    if (pid == 0)
    {
        cout << "Program 2 Child ID: " << getpid() << endl;
        char *new_env[] = {"lab4_1.cpp",NULL};
        execle("lab4_1",argv[0],NULL,new_env);
    }
    if (pid > 0)
    {
        cout << "Program 2 Parent ID: " << getpid() << "\n";

        int status;
        while (waitpid(pid, &status, WNOHANG) == 0)
        {
            cout << "Program 2 Parent is waiting\n";
            sleep(1);
        }
        if (waitpid(pid, &status, WNOHANG) == -1){
            perror("waitpid");
        }
        else{
        cout << "Program 2 exit with status " << WEXITSTATUS(status) << "\n";
        }
    }

    return 0;
}