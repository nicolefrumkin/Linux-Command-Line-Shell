#include "hw1shell.h"

int running_cmds = 0;

int main() {
    char input[BUFFER_SIZE];
    char *tokens[MAX_TOKENS]; // define token array for parsing input from user
    char last_char;
    int token_count = 0;
    bool is_background;
    ps* head = NULL;

    while (1) {
        is_background = false;
        printf("hw1shell$ ");
        fgets(input, BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = '\0'; // removes new line symbol from input
        token_count = 0;
        memset(tokens, 0, sizeof(tokens)); // clean tokens memory
        // parse input to cmd and directory
        char *token = strtok(input, " "); // take the first token out of input
//      
        while (token != NULL && token_count < MAX_TOKENS) { // saving all the tokens from the user
            tokens[token_count++] = token; // Store token pointer in the array
            token = strtok(NULL, " "); // NULL tells strtok to continue parsing the same string
        }

        if (token_count == 0) { // if user put a empty line, continue loop
            reap_background_processes(&head); // if we got \n and we have finshed bg process
            continue;
        }

        if (!strcmp(tokens[0], "exit")) { // exit shell with exit shell function, break main loop
            exit_shell(head);
            break;
        } else if (!strcmp(tokens[0], "cd")) { // change directory
            if (token_count < 2) {
                fprintf(stderr, "hw1shell: cd: missing argument\n");
            } else {
                cd(tokens[1]);
            }
        } else if (!strcmp(tokens[0], "jobs")) { // display processes
            jobs(head);
        }
        else {
            last_char = tokens[token_count - 1][strlen(tokens[token_count - 1])-1];
            if(last_char == '&'){
                if(!strcmp(tokens[token_count - 1], "&")){ // means its only &
                     tokens[--token_count] = NULL; 
                }
                else{
                    tokens[token_count - 1][strlen(tokens[token_count - 1])-1] = '\0';
                }
                is_background = true;
            }
            execute_cmd(tokens, is_background, &head); // if the user didn't input any of the internal commands, execute the command
        }
        reap_background_processes(&head); // reap zombies
    }

    return 0;
}

// executes external command
void execute_cmd(char *tokens[], bool is_background, ps **head) {
    if (is_background && running_cmds >= MAX_BG_CMD) { // not sure it needs to be here!
        fprintf(stderr, "hw1shell: too many background commands running\n");
        return;
    }

    // save full command to print in jobs later
    char full_command[BUFFER_SIZE] = {0};
    for (int i = 0; tokens[i] != NULL; i++) {
        strncat(full_command, tokens[i], BUFFER_SIZE - strlen(full_command) - 1);
        if (tokens[i + 1] != NULL) { // Add space between tokens
            strncat(full_command, " ", BUFFER_SIZE - strlen(full_command) - 1);
        }
    }

    pid_t pid = fork(); // duplicate current process and return its PID 
    if (pid < 0) { // fork faild
        printf("hw1shell: %s failed, errno is %d\n", "fork", errno);
    } else if (pid == 0) { // the process is a child
        if (execvp(tokens[0], tokens) == -1) {
            printf("hw1shell: invalid command\n");
            printf("hw1shell: %s failed, errno is %d\n", "execvp", errno);
            exit(1);
        }
    } else { // the process is a parent
        if (is_background) { // Background process: do not wait
            printf("hw1shell: pid %d started\n", pid);
            add_ps(full_command, pid, head); // Pass the full command to add_ps
            running_cmds++; // Increment background process count
        } else {
        // Foreground process: wait for the child to finish
            int status;
            if (waitpid(pid, &status, 0) == -1) {
                printf("hw1shell: %s failed, errno is %d\n", "waitpid", errno);
            }
            else if (WIFEXITED(status)) {
                // do nothing
            }
        }
    }
}

// add process to linked list
void add_ps(const char *cmd, int pid, ps **head) {
    ps *new_node = (ps *)malloc(sizeof(ps));
    if (new_node == NULL) {
        printf("hw1shell: %s failed, errno is %d\n", "malloc", errno);
        return;
    }


    strncpy(new_node->command, cmd, CMD_SIZE - 1); 
    new_node->command[CMD_SIZE - 1] = '\0';        
    new_node->pid = pid;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
    } else {
        ps *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

// causes the shell process to change its working directory
void cd(const char *dir) {
    if (chdir(dir) == -1) {
        printf("hw1shell: %s failed, errno is %d\n", "chdir", errno);
    }
}

// display running back ground processes
void jobs(ps *head) {
    if (head == NULL) {
        printf("No background processes.\n");
        return;
    }

    printf("PID\tCommand\n");
    ps *current = head;
    while (current != NULL) {
        printf("%d\t%s\n", current->pid, current->command);
        current = current->next;
    }
}

void reap_background_processes(ps **head) {
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("hw1shell: pid %d finished\n", pid);
        }

        // Remove process from linked list
        ps **current = head;
        while (*current != NULL) {
            if ((*current)->pid == pid) {
                ps *temp = *current;
                *current = (*current)->next;
                free(temp);
                running_cmds--;
                break;
            }
            current = &((*current)->next);
        }
    }
}

// exit() reaps children and cleans up dynamic memory
void exit_shell(ps *head) {
    pid_t pid;
    int status;

    // reap children
    while ((pid = waitpid(-1, &status, 0)) > 0) { // run a loop that wait for all children to be reaped, if the PID is a positive number it means we got the PID of the terminated child
    // -1 means it waits for any child process
    // it saves the status of the child in the variable status
    // WNOHANG Ensures the function does not block if there are no child processes to wait for
        if (WIFEXITED(status)) {
            printf("hw1shell: pid %d finished\n", pid);
        }
    }

    // free memory
    ps *current = head;
    while (current != NULL) {
        ps *temp = current;
        current = current->next;
        free(temp);
    }
}
