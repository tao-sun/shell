/*
 * mybash.h
 *
 *  Created on: Mar 20, 2016
 *      Author: taosun
 */

#define COMMAND_BUFSIZE 128
#define VAR_BUFSIZE 20



void init();
char* getUserCommand();
int assignVar(struct Assignment *);
int executeCommand(struct CommandData *);
int debug();
void printDebugInfo(struct CommandData *);
