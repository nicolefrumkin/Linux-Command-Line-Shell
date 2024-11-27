// tests:
// sleep 25 &

//to fix:
// printing forground... when prompt is pwd. it doent suppoust to.

#include "hw1shell.h"


int running_cmds = 0;
 
int main() {
    char input[BUFFER_SIZE];
    char *tokens[MAX_TOKENS]; // define token array for parsing input from user
    int token_count = 0;
    bool is_background;
    ps* head = NULL;
 
    token_count = 0;
    memset(tokens, 0, sizeof(tokens)); // making sure array is clean

    while (1) {
        is_background = false;
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
            jobs(head);
        }
        else if (!strcmp(tokens[0],"exit")){ // exit shell
            printf("Exiting shell...\n");
            exit_shell(head);
            break;
        }
        else{
            if (strcmp(tokens[token_count - 1], "&") == 0) { // check if it is a background command
                tokens[token_count - 1] = NULL; // we dont want this to be a problem later
                token_count -= 1;
                is_background = true;
                }
            execute_cmd(tokens, is_background, head); // if the user didn't input any of the internal commands, execute the command
            }
        //reap_background_processes(); //fic to handle zombies only
        //is_background = 0; // restart variable
        }
    return 0;
}

// executes external command
//TO FIX - parsing cmd with & , removing it 
void execute_cmd(char* tokens[], bool is_background, ps* head){
    if (is_background && running_cmds >= MAX_BG_CMD) { // not sure it needs to be here!
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
            // add exec kill pid, because of fork, with get pid. suecide
            fprintf(stderr, "hw1shell: %s failed, errno is %d\n", "execvp", errno);
        }
    }
    else{ // the process is a parent
        if (is_background) { // Background process: do not wait
        printf("hw1shell: pid %d started,\n", pid);
        add_ps(tokens[0],pid,head);
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

void add_ps(const char* cmd, int pid, ps* head){
    ps* new_node = (ps*)malloc(sizeof(ps));
    if (new_node == NULL) {
        perror("Failed to allocate memory for the new node");
        return;
    }

    // Initialize the new node
    strcpy(new_node->command,cmd);
    new_node->pid = pid;
    new_node->next = NULL;

    // If the list is empty, make the new node the head
    if (head == NULL) {
        head = new_node;
        return;
    }

    // Traverse the list to find the last node
    ps* current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    // Add the new node at the end of the list
    current->next = new_node;
}

// causes the shell process to change its working directory
void cd(const char* dir){
    if (chdir(dir)==-1){
        perror("hw1shell: invalid command\n");
        fprintf(stderr, "hw1shell: %s failed, errno is %d\n", "chdir", errno);
    }
}

// display running processes
void jobs(ps* head) {
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
void exit_shell(ps* head){
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

//why we need this and not only the exit? like when do we want to reap all bg proccess when we font exit?
void reap_background_processes() {
    pid_t pid;
    int status;

    // Check and reap all completed background processes
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("hw1shell: pid %d finished with status: %d\n", pid, WEXITSTATUS(status));
        }
        
        printf("pid is: %d\n",pid);
        running_cmds--; // Decrement the background command counter
    }
}

