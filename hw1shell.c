#include <stdio.h>

#define BUFFER_SIZE 1024

int main() {
    char input[BUFFER_SIZE];
    while (1) {
        printf("hw1shell$ ");
        fgets(input, BUFFER_SIZE, stdin);
    }
    return 0;
}
