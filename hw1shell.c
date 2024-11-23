#include <stdio.h>

#define BUFFER_SIZE 1024

int main() {
    char input[BUFFER_SIZE];
    while (1) {
        printf("hw1shell$ ");
        scanf("%(BUFFER_SIZE-1)s",input);
    }
    return 0;
}
