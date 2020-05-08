#include "builtin.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parser.h"
#include "uthash.h"

int builtinCD(char ** args) {
    char * dir = NULL;
    if (args == NULL || args[0] == NULL) {
        if ((dir = getenv("HOME")) == NULL) {
            perror("mybash");
            exit(1);
        }
    } else {
        dir = args[0];
    }

    if (dir != NULL) {
        if (chdir(dir) != 0) {
            fprintf(stderr, "Error, changing dir\n");
            return 0;
        }
    } else {
        return 0;
    }

    return 1;
}

int builtinPWD(char **args) {
    char *homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        fprintf(stderr, "Error, get home dir\n");
        return 0;
    }
    printf("%s\n", homedir);
    return 1;
}

extern struct Assignment *assignments;
int builtinEXPORT(char **args) {
    if (args == NULL || args[0] == NULL) {
        fprintf(stderr, "Error, wrong args\n");
        return 0;
    }

    struct Assignment *assignment = (struct Assignment*)malloc(sizeof(struct Assignment));
    char *varname = args[0];
    HASH_FIND_STR(assignments, varname, assignment);

    if (assignment != NULL) {
        char *value = assignment->value;
        if ((setenv(varname, value, 1)) == -1) {
            fprintf(stderr, "Error, set env\n");
            return 0;
        }
    } else {
        printf("Error, no variable named %s\n", varname);
        return 0;
    }

    return 1;
}

extern char **environ;
int builtinSET(char **args) {
    int i=0;
    char *envstring;
    while((envstring = environ[i]) != NULL) {
        printf("%s\n", environ[i++]);
    }
    return 1;
}

int builtinEXIT(char **args) {
    exit(EXIT_SUCCESS);
}
