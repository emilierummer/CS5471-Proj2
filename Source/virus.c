#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 

int runFile(const char *filename) {
    execv(filename, NULL);
    // If it returns, execv failed
    perror("execv failed");
    exit(EXIT_FAILURE);
}

int main() {
    runFile("./host");
    return 0;
}