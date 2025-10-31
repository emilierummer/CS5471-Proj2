#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 

/*
* Executes the file specified by filename.
* If execution fails, prints an error and exits.
*/
int runFile(const char *filename) {
    execv(filename, NULL);
    // If it returns, execv failed
    perror("execv failed");
    exit(EXIT_FAILURE);
}

/*
 * Searches a file for the byte pattern 0xDEADBEEF.
 * Returns 1 if found, 0 otherwise.
 * Seeks to the position immediately after the pattern if found.
 */
long findDeadbeef(FILE *file) {
    printf("Searching for deadbeef pattern in file...\n");
    unsigned char buffer[4];
    int bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) == sizeof(buffer)) {
        if (buffer[0] == 0xde && buffer[1] == 0xad && buffer[2] == 0xbe && buffer[3] == 0xef) {
            printf("Found deadbeef pattern!\n");
            long currentPos = ftell(file);
            if (currentPos == -1) {
                perror("ftell failed");
                return -1;
            }
            return currentPos;
        }
    }
    printf("deadbeef pattern not found.\n");
    return -1;
}

int main() {
    FILE *file = fopen("./test", "rb");
    if (file == NULL) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }
    long deadbeefPos = findDeadbeef(file);
    printf("deadbeef position: %ld\n", deadbeefPos);
    fclose(file);
    return 0;
}