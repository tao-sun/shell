/*
 * Headstart for Assignment 2
 * CS 4420/5420
 *
 *
 * Student Name:
 *
 *
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "parser.h"

void init();
char* getUserCommand();
//void execBuiltInCommand(struct CommandData);
int builtinCD(char **);
void executeCommand(struct CommandData);
void launchProcess(struct Command, int, int, int);


int builtinCD(char ** args) {
    if (args == NULL || sizeof(args) ==0) {
        // go to HOME
    } else {
        char *dir = args[0];
        if (chdir(dir) != 0) {
            perror("mybash");
            exit(1);
        }
    }

    return 1;
}

char *builtincmds[] = {
    "cd"/*,
    "export",
    "set",
    "pwd",
    "export"*/
};

int (*builtinfuncs[])(char **args) = {
    &builtinCD,

};

int main(int argc, char** argv)
{
    while(1) {
        init();
        char* cmd = getUserCommand();

        if(cmd != NULL && strlen(cmd) != 0) {
            struct CommandData cmdData;
            int validCmd = ParseCommandLine(cmd, &cmdData);
            if(validCmd) {
                if(0)//BuiltInTest(cmdData) == 1 )
                    //execBuiltInCommand(cmdData);
                    printf("builtin test\n");
                else{
                    executeCommand(cmdData);
                }
            } else {
                fprintf(stderr, "Cmd parse error: invalid command format\n");
                exit(EXIT_FAILURE);
            }

        }

    }
}

void init() {
    long size;
    char *buf;
    char *cwd;

    size = pathconf(".", _PC_PATH_MAX);

    if ((buf = (char *)malloc((size_t)size)) != NULL) {
        cwd = getcwd(buf, (size_t)size);
        printf("%s> ", cwd);
    }
}

#define COMMAND_BUFSIZE 128
char* getUserCommand() {
    int position = 0;
    char *buffer = malloc(sizeof(char) * COMMAND_BUFSIZE);
    int c;

    if (!buffer) {
        fprintf(stderr, "cmd buffer: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Read a character
        c = getchar();

        // If we hit EOF, replace it with a null character and return.
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // Exceeds maximum length.
        if (position >= COMMAND_BUFSIZE) {
            fprintf(stderr, "Exceed cmd maximum length: 128 chars\n");
            exit(EXIT_FAILURE);
        }
    }

}

#define PIPERD 0
#define PIPEWR 1
void executeCommand(struct CommandData cmdData) {
    /*The steps for executing the command are:
    (1) Redirect Input/Output for the command, if necessary
    (2) Create a new process which is a copy of the calling process (-> fork()
            system call)
    (3) Child process will run a new program (-> one of the exec() system calls)
    (4) if( BackgroundOption(CommandStructure) == 0 )
        the parent shell will wait(), otherwise it will continue the while loop*/
    struct Command *commands = cmdData.TheCommands;
    int background = cmdData.background;
    printf("background: %d\n", background);

    if (cmdData.numcommands > 1) {
        if (cmdData.numcommands == 2) {
            int pipefd[2];
            pipe(pipefd);

            launchProcess(commands[0], STDIN_FILENO, pipefd[PIPEWR], background);

            int outfilefd = STDOUT_FILENO;
            if (cmdData.outfile != NULL) {
                outfilefd = open(cmdData.outfile, O_WRONLY, 0);
            }
            launchProcess(commands[1], pipefd[PIPERD], outfilefd, background);
        } else {
            // more than 2 commands are not supported
        }
    } else {
        for(int i=0; i<sizeof(builtincmds)/sizeof(char *); i++) {
            if (strcmp(commands[0].command, builtincmds[i]) == 0) {
                (builtinfuncs[i])(commands[0].args);
                return;
            }
        }

        int infilefd = STDIN_FILENO, outfilefd = STDOUT_FILENO;
        if (cmdData.infile != NULL) {
            printf("input file: %s\n", cmdData.infile);
            infilefd = open(cmdData.infile, O_RDONLY, 0);
            printf("input file fd: %d\n", infilefd);
        }
        if (cmdData.outfile != NULL) {
            printf("output file: %s\n", cmdData.outfile);
            mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
            outfilefd = open(cmdData.outfile, O_WRONLY | O_CREAT | O_TRUNC, mode);
            printf("output file fd: %d\n", outfilefd);
        }

        launchProcess(commands[0], infilefd, outfilefd, background);
    }
}

void launchProcess(struct Command cmd, int infilefd, int outfilefd, int background) {
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

        char *cmdargv[12];
        cmdargv[0] = cmd.command;
        for(int i=1; i<sizeof(cmdargv)/sizeof(char *); i++) {
            cmdargv[i] = cmd.args[i-1];
        }
        execvp(cmd.command, cmdargv);

        perror("execvp");
        exit(1);
    } else {
        if (infilefd != STDIN_FILENO) {
            close(infilefd);
        }
        if (outfilefd != STDOUT_FILENO) {
            close(outfilefd);
        }

        int status;
        if (!background) {
            printf("waiting...\n");
            waitpid (pid, &status, WUNTRACED);
        }
    }
}
