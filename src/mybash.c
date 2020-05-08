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
#include "builtin.h"
#include "mybash.h"
#include "job.h"
#include "uthash.h"

struct Assignment *assignments = NULL;

char *envvars[] = {
    "PATH",
    "HOME",
    "DEBUG"
};

char *builtincmds[] = {
    "cd",
    "pwd",
    "set",
    "export",
    "exit"
};

int (*builtinfuncs[])(char **args) = {
    &builtinCD,
    &builtinPWD,
    &builtinSET,
    &builtinEXPORT,
    &builtinEXIT,
};

int main(int argc, char** argv)
{
    int status;
    while(1) {
        status = 1;
        init();

        char *cmd = getUserCommand();

        if (cmd != NULL && strlen(cmd) != 0) {
            if (IsAssignment(cmd)) {
                struct Assignment *assignment = (struct Assignment *)malloc(sizeof(struct Assignment));
                int validAssignment = ParseAssignment(cmd, assignment);
                if (validAssignment) {
                    status = assignVar(assignment);
                } else {
                    fprintf(stderr, "Assignment parse error: invalid assignment format\n");
                    status = 0;
                }
            } else {
                struct CommandData *cmdData = (struct CommandData *)malloc(sizeof(struct CommandData));
                int validCmd = ParseCommandLine(cmd, cmdData);
                if (validCmd) {
                   status = executeCommand(cmdData);
                } else {
                    fprintf(stderr, "Cmd parse error: invalid command format\n");
                    status = 0;
                }
            }
        }

        if (status == 0) {
            fprintf(stderr, "Error in the execution\n");
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

char* getUserCommand() {
    int position = 0;
    int bufsize = COMMAND_BUFSIZE;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
        fprintf(stderr, "cmd buffer: allocation error\n");
        return NULL;
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

        if (position >= bufsize) {
          bufsize += COMMAND_BUFSIZE;
          buffer = realloc(buffer, bufsize);
          if (!buffer) {
            fprintf(stderr, "cmd buffer: allocation error\n");
            return NULL;
          }
        }
    }

}

int assignVar(struct Assignment *assignment) {
    for(int i=0; i<sizeof(envvars)/sizeof(char *); i++) {
        if (strcmp(assignment->varname, envvars[i]) == 0) {
            if (setenv(assignment->varname, assignment->value, 1) == -1) {
                fprintf(stderr, "error: set env\n");
                return 0;
            }
            return 1;
        }
    }

    HASH_ADD_STR(assignments, varname, assignment);
    return 1;
}

int executeCommand(struct CommandData *cmdData) {
    int status;

    if (debug()) {
        printDebugInfo(cmdData);
    }

    if (cmdData->numcommands == 1) {
        struct Command *commands = cmdData->TheCommands;
        for(int i=0; i<sizeof(builtincmds)/sizeof(char *); i++) {
            if (strcmp(commands[0].command, builtincmds[i]) == 0) {
                status = (builtinfuncs[i])(commands[0].args);
                return status;
            }
        }
    }
    status = exeExtCmd(cmdData);
    return status;
}

int debug() {
    char * debug = getenv("DEBUG");

    if (debug != NULL) {
        if (strcmp(debug, "yes") == 0) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

void printDebugInfo(struct CommandData *cmdData) {
    printf("\n");
    printf("DEBUG OUTPUT: \n");
    printf("--------------------------\n");
    printf("DEBUG: Number of simple commands: %d\n", cmdData->numcommands);
    struct Command *commands = cmdData->TheCommands;

    for(int i=0; i<cmdData->numcommands; i++) {
        struct Command cmd = commands[i];

        printf("DEBUG: command%d : %s\n", (i+1), cmd.command);
        for(int j=0; j<cmd.numargs; j++) {
            printf("DEBUG: arg[%d] : %s\n", j, cmd.args[j]);
        }
    }

    printf("DEBUG: Input file : %s\n", cmdData->infile);
    printf("DEBUG: Output file : %s\n", cmdData->outfile);
    printf("DEBUG: Background option : %s\n", cmdData->background? "ON" : "OFF");
    printf("--------------------------\n");
    printf("\n");
}
