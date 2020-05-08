#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "parser.h"
#include "job.h"

int exeExtCmd(struct CommandData *cmdData) {
    struct Command *commands = cmdData->TheCommands;
    int background = cmdData->background;

    int cmdinfilefd = STDIN_FILENO;
    int cmdoutfilefd = STDOUT_FILENO;
    if (cmdData->infile != NULL) {
        cmdinfilefd = open(cmdData->infile, O_RDONLY, 0);
    }
    if (cmdData->outfile != NULL) {
        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
        cmdoutfilefd = open(cmdData->outfile, O_WRONLY | O_CREAT | O_TRUNC, mode);
    }

    int pipefd[2], infilefd, outfilefd;
    infilefd = cmdinfilefd;
    for(int i=0; i<cmdData->numcommands; i++) {

        if (i < cmdData->numcommands-1) {
            pipe(pipefd);
            outfilefd = pipefd[PIPEWR];
        } else {
            outfilefd = cmdoutfilefd;
        }

        launchProcess(&commands[i], infilefd, outfilefd, background);

        if (infilefd != cmdinfilefd) {
            close(infilefd);
        }
        if (outfilefd != cmdoutfilefd) {
            close(outfilefd);
        }
        infilefd = pipefd[PIPERD];
    }

    return 1;
}

void launchProcess(struct Command *cmd, int infilefd, int outfilefd, int background) {
    int pid = fork();
    if (pid == 0) {
        if (infilefd != STDIN_FILENO){
            dup2 (infilefd, STDIN_FILENO);
            close(infilefd);
        }
        if (outfilefd != STDOUT_FILENO){
            dup2 (outfilefd, STDOUT_FILENO);
            close(outfilefd);
        }

        char **args = cmd->args;
        char *cmdargv[13];
        cmdargv[0] = cmd->command;
        for(int i=1; i<sizeof(cmdargv)/sizeof(char *); i++) {
            cmdargv[i] = args[i-1];
        }
        cmdargv[12] = NULL;

        execvp(cmd->command, cmdargv);

        perror("execvp");
        exit(1);
    } else {
        int status;
        if (!background) {
            waitpid(pid, &status, WUNTRACED);
        }
    }
}
