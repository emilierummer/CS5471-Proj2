#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("Host running\n");
    if (argc > 1) {
        printf("Arguments passed to host:\n");
        for (int i = 0; i < argc; i++) {
            printf("  %s\n", argv[i]);
        }
    } else {
        printf("No arguments passed to host.\n");
    }
    return 0;
}