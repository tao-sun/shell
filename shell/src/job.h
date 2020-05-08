#define PIPERD 0
#define PIPEWR 1

extern int exeExtCmd(struct CommandData *);
extern void launchProcess(struct Command *, int, int, int);
