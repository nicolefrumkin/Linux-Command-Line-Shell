#include <stdio.h> // for printf()
#include <string.h> // for strcmp()
#include <unistd.h> // for fork()
#include <sys/types.h> // for wait()
#include <sys/wait.h> // for wait()

  
#define BUFFER_SIZE 1024
#define MAX_BG_CMD 4
#define MAX_TOKENS 64

struct process {
    int pid;
    char* bg_cmd;
    struct process* next;
} ps;

void cd(const char* dir);
void jobs();
void exit();

int running_cmds = 0;
 
  int main() {
    char input[BUFFER_SIZE];
    char *tokens[MAX_TOKENS];
    int token_count = 0;
 
    while (1) {
        printf("hw1shell$ ");
        fgets(input, BUFFER_SIZE, stdin);
        // parse input to cmd and directory
        input[strcspn(input, "\n")] = '\0';
        char *token = strtok(input, " ");
        while (token != NULL && token_count < MAX_TOKENS) {
            tokens[token_count++] = token; // Store token pointer in the array
            token = strtok(NULL, " ");    // NULL tells strtok to continue parsing the same string
        }

        if (running_cmds>MAX_BG_CMD){
            perror("hw1shell: too many background commands running");
        }
        else if (!strcmp(cmd,"cd")){
            cd(dir);
        }
        else if (!strcmp(cmd,"jobs")) {
            jobs();
        }
        else if (!strcmp(cmd,"exit")){
            //exit();
            break;
        }
    }
      return 0;
  }

// causes the shell process to change its working directory
void cd(const char* dir){
    if (chdir(dir)==-1){
        perror("hw1shell: invalid command");
    }
}

// display running processes
void jobs() {
    
}

// reap children and clean up dynamic memory
void exit(){

}

