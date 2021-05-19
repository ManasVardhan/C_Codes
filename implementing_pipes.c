#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define READ_END 0
#define WRITE_END 1

int son, grandson, greatgrandson;
int pipe1[2];
int pipe2[2];
int pid;

void son_process()
{
    // is executing ls -l

    //this is happening in son process

    if (son == 0)
    {
        //Cloning pipe to get info from stdout

        dup2(pipe1[WRITE_END], 1);

        //Closing unnecessary ends of pipe

        close(pipe1[WRITE_END]);
        close(pipe1[READ_END]);

        execlp("/bin/ls", "ls", "-l", NULL); //Is giving output to stdout so we'll need to clone pipe to stdout
    }
}

void grandson_process()
{

    //Happening in grandson proces

    //executes grep .c$

    //Taking input from pipe1
    dup2(pipe1[READ_END], 0);
    dup2(pipe2[WRITE_END], 1);

    //Closing all pipe openings
    close(pipe1[READ_END]);
    close(pipe1[WRITE_END]);
    close(pipe2[READ_END]);
    close(pipe2[WRITE_END]);

    execlp("/bin/grep", "grep", ".c$", NULL);
}

void greatgrandson_process()
{

    //Happening in great grandson

    //execcutes wc -l

    dup2(pipe2[READ_END], 0); //Cloning stdin to pipe2 so that pipe2 data can be used as input

    close(pipe2[READ_END]);
    close(pipe2[WRITE_END]);

    execlp("/bin/wc", "wc", "-l", NULL);
}

void main()
{

    // create pipe1
    if (pipe(pipe1) == -1)
    {
        perror("bad pipe1");
        exit(1);
    }

    // fork (ps aux)
    if ((pid = fork()) == -1)
    {
        perror("bad fork1");
        exit(1);
    }
    else if (pid == 0)
    {
        // stdin --> ps --> pipe1
        son_process();
    }
    // parent

    // create pipe2
    if (pipe(pipe2) == -1)
    {
        perror("bad pipe2");
        exit(1);
    }

    // fork (grep root)
    if ((pid = fork()) == -1)
    {
        perror("bad fork2");
        exit(1);
    }
    else if (pid == 0)
    {
        // pipe1 --> grep --> pipe2
        grandson_process();
    }
    // parent

    // close unused fds
    close(pipe1[0]);
    close(pipe1[1]);

    // fork (grep sbin)
    if ((pid = fork()) == -1)
    {
        perror("bad fork3");
        exit(1);
    }
    else if (pid == 0)
    {
        // pipe2 --> grep --> stdout
        greatgrandson_process();
    }
    // parent
}
