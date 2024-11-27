#include <stdlib.h> // Required for free, malloc, etc.
#include <stdio.h>  // For printf
#include <string.h> // For strcmp
#include <unistd.h> // For fork, execvp
#include <sys/types.h> // For pid_t
#include <sys/wait.h> // For waitpid
#include <errno.h>  // For errno
#include <stdbool.h> // for bool vars

  
#define BUFFER_SIZE 1024
#define MAX_BG_CMD 4
#define MAX_TOKENS 64
#define CMD_SIZE 256

typedef struct process {
    int pid;
    char command[CMD_SIZE];
    struct process* next;
} ps;

void cd(const char* dir);
void jobs(ps* head);
void exit_shell(ps* head);
void execute_cmd(char* tokens[],bool is_background, ps* head);
void reap_background_processes();
void add_ps(const char* cmd, int pid, ps* head);

