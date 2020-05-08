/*
 * Headstart for Assignment 2
 * CS 442/542
 * FQ 11
 * 
 * Student Name:
 * 
 * Student Name:
 * 
 * 
 * 
 * 
 */

#include "uthash.h"

/* the states */
#define COMMAND 1
#define ARG 2
#define INFILE 3
#define OUTFILE 4
#define UNDEF 5
#define VAR 6
#define EQUAL 7
#define VALUE 8

struct Assignment {
    char *varname;
    char *value;
    UT_hash_handle hh;
};

struct Command {
  char *command;
  char *args[11];
  int numargs;
};

struct CommandData {
  struct Command TheCommands[20];  /* the commands to be
          executed.  TheCommands[0] is the first command
          to be executed.  Its output is piped to
          TheCommands[1], etc. */
  int numcommands; /* the number of commands in the above array */
  char *infile;   /* the file for input redirection, NULL if none */
  char *outfile;  /* the file for output redirection, NULL if none */
  int  background;  /* 0 if process is to run in foreground, 1 if in background */
};


extern int IsAssignment(char *);
extern int ParseCommandLine(char *, struct CommandData *);
extern int ParseAssignment(char *, struct Assignment *);
