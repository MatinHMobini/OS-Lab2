/*--------------------------------------------------------------------------
File: mon2.c

Description: This program creates a process to run the program identified
             on the commande line.  It will then start procmon in another
	     process to monitor the change in the state of the first process.
	     After 20 seconds, signals are sent to the processes to terminate
	     them.

	     Also a third process is created to run the program filter.  
	     A pipe is created between the procmon process and the filter
	     process so that the standard output from procmon is sent to
	     the standard input of the filter process.
--------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: mon2 fileName\n where fileName is an executable file.\n");
        exit(-1);
    }

    char *program = argv[1];
    pid_t pid_prog, pid_procmon, pid_filter;
    int pipefd[2];
    char pid_str[20];

    // Step 1: Fork the process for the program to monitor
    if ((pid_prog = fork()) == 0) {
        execl(program, program, NULL);
        perror("execl failed for program");
        exit(1);
    }

    // Step 2: Create the pipe
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(1);
    }

    // Step 3: Fork the filter process
    if ((pid_filter = fork()) == 0) {
        // Connect read end of pipe to stdin
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        execl("./filter", "filter", NULL);
        perror("execl failed for filter");
        exit(1);
    }

    // Step 4: Fork the procmon process
    if ((pid_procmon = fork()) == 0) {
        // Connect write end of pipe to stdout
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        sprintf(pid_str, "%d", pid_prog);
        execl("./procmon", "procmon", pid_str, NULL);
        perror("execl failed for procmon");
        exit(1);
    }

    // Parent: close pipe ends
    close(pipefd[0]);
    close(pipefd[1]);

    // Step 5: Let the processes run for 20 seconds
    sleep(20);

    // Step 6: Kill the monitored program
    kill(pid_prog, SIGTERM);

    // Step 7: Wait and kill the others
    sleep(2);
    kill(pid_procmon, SIGTERM);
    kill(pid_filter, SIGTERM);

    waitpid(pid_prog, NULL, 0);
    waitpid(pid_procmon, NULL, 0);
    waitpid(pid_filter, NULL, 0);

    return 0;
}


