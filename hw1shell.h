#ifndef HW1SHELL_H
#define HW1SHELL_H

#include <stdlib.h>   // For memory management (malloc, free, etc.)
#include <stdio.h>    // For I/O operations (printf, fgets, etc.)
#include <string.h>   // For string manipulation (strcmp, strtok, etc.)
#include <unistd.h>   // For process control (fork, execvp, chdir, etc.)
#include <sys/types.h> // For pid_t and related types
#include <sys/wait.h> // For waitpid and process termination
#include <errno.h>    // For error handling (errno)
#include <stdbool.h>  // For boolean type (bool)

// Constants
#define BUFFER_SIZE 1024
#define MAX_BG_CMD 4
#define MAX_TOKENS 64
#define CMD_SIZE 256

// Struct to represent a process
typedef struct process {
    int pid;
    char command[CMD_SIZE];
    struct process* next;
} ps;

// Function Prototypes
void cd(const char* dir);
void jobs(ps* head);
void exit_shell(ps* head);
void execute_cmd(char* tokens[], bool is_background, ps** head);
void reap_background_processes(ps** head);
void add_ps(const char* cmd, int pid, ps** head);

#endif // HW1SHELL_H
