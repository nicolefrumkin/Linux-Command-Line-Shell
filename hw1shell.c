#include <stdlib.h> // Required for free, malloc, etc.
#include <stdio.h>  // For printf
#include <string.h> // For strcmp
#include <unistd.h> // For fork, execvp
#include <sys/types.h> // For pid_t
#include <sys/wait.h> // For waitpid
#include <errno.h>  // For errno
  
#define BUFFER_SIZE 1024
#define MAX_BG_CMD 4
#define MAX_TOKENS 64

void cd(const char* dir);
void jobs();
void exit_shell();
void execute_cmd(char* tokens[]);
void reap_background_processes();

typedef struct process {
    int pid;
    char command[256];
    struct process* next;
} ps;

ps* head = NULL;
int running_cmds = 0;
int is_background = 0;
 
int main() {
    char input[BUFFER_SIZE];
    char *tokens[MAX_TOKENS]; // define token array for parsing input from user
    int token_count = 0;
 
    while (1) {
        printf("hw1shell$ ");
        fgets(input, BUFFER_SIZE, stdin);
        // parse input to cmd and directory
        input[strcspn(input, "\n")] = '\0'; // removes new line symbol from input
        char *token = strtok(input, " "); // taken first token out of input
        while (token != NULL && token_count < MAX_TOKENS) { // saving all the tokens from the user
            tokens[token_count++] = token; // Store token pointer in the array
            token = strtok(NULL, " ");    // NULL tells strtok to continue parsing the same string
        }
        if (token_count == 0) { // if user put a empty line, continue loop
            continue;
        }

        if (!strcmp(tokens[0],"cd")){ // change directory
            printf("Changing directory...\n");
            cd(tokens[1]);
        }
        else if (!strcmp(tokens[0],"jobs")) { // display processes
            printf("Displaying processes...\n");
            jobs();
        }
        else if (!strcmp(tokens[0],"exit")){ // exit shell
            printf("Exiting shell...\n");
            exit_shell();
            break;
        }
        else{
            if (strcmp(tokens[token_count - 1], "&") == 0) { // check if it is a background command
                is_background = 1;
                }
            execute_cmd(tokens); // if the user didn't input any of the internal commands, execute the command
            }
        reap_background_processes();
        is_background = 0; // restart variable
        }
    return 0;
}

// executes external command
void execute_cmd(char* tokens[]){
    if (is_background && running_cmds >= MAX_BG_CMD) {
        fprintf(stderr, "hw1shell: too many background commands running\n");
        return;
    }

    pid_t pid = fork(); // duplicate current process and return its PID 
    if (pid < 0){ // fork faild
        printf("hw1shell: fork faild\n");
        fprintf(stderr, "hw1shell: %s failed, errno is %d\n", "fork", errno);
    }
    else if(pid == 0){ // the process is a child
        if (execvp(tokens[0], tokens) == -1) { // execute command using execvp
            printf("hw1shell: invalid command\n");
            fprintf(stderr, "hw1shell: %s failed, errno is %d\n", "execvp", errno);
        }
    }
    else{ // the process is a parent
        if (is_background) { // Background process: do not wait
        printf("hw1shell: pid %d started,\n", pid);
        running_cmds++; // Increment background process count
        } else {
        // Foreground process: wait for the child to finish
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            fprintf(stderr, "hw1shell: %s failed, errno is %d\n", "waitpid", errno);
        } else if (WIFEXITED(status)) {
            printf("Foreground process (PID: %d) exited with status: %d\n", pid, WEXITSTATUS(status));
            }
        }
    }
}

// causes the shell process to change its working directory
void cd(const char* dir){
    if (chdir(dir)==-1){
        perror("hw1shell: invalid command\n");
        fprintf(stderr, "hw1shell: %s failed, errno is %d\n", "chdir", errno);
    }
}

// display running processes
void jobs() {
    ps* current = head;

    if (!current) {
        printf("No background processes.\n");
        return;
    }

    printf("PID\tCommand\n");
    while (current) { // loop through linked list and display processes
        printf("%d\t%s\n", current->pid, current->command);
        current = current->next;
    }
}

// exit() reaps children and cleans up dynamic memory
void exit_shell(){
    // reap children
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) { // run a loop that wait for all children to be reaped, if the PID is a positive number it means we got the PID of the terminated child
    // -1 means it waits for any child process
    // it saves the status of the child in the variable status
    // WNOHANG Ensures the function does not block if there are no child processes to wait for
        if (WIFEXITED(status)) {
            printf("Reaped child process (PID: %d) with status: %d\n", pid, WEXITSTATUS(status));
            }
    }

    // clean up dynamic memory if we created it
    ps* current = head;
    ps* temp = NULL;
    while (current) { // loop through linked list and free memory
        temp = current->next;
        free(current);
        current = temp;
    }
    printf("freed memory\n");
}

void reap_background_processes() {
    pid_t pid;
    int status;

    // Check and reap all completed background processes
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("hw1shell: pid %d finished with status: %d\n", pid, WEXITSTATUS(status));
        }
        running_cmds--; // Decrement the background command counter
    }
}

