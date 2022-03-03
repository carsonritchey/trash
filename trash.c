#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>

#define TRASHCAN_DIRECTORY "~/.trashcan"
#define DEFAULT_PERMS 777

int main(int argc, char* argv[]) {
    if(argc != 3) {
        printf("trash: please provide sufficient arguments\n");
        return 0;
    }

    FILE* file1, *file2;
    file1 = fopen(argv[1], "r");
    file2 = fopen(argv[2], "w");

    if(!file1) {
        printf("trash: '%s' was not found\n", argv[1]);
        return 0;
    }
    if(!file2) {
        printf("trash: unable to move '%s' to '%s': no such file or directory\n", argv[1], argv[2]);
        return 0;
    }

    int c;
    while((c = fgetc(file1)) != EOF)
        fputc(c, file2);

    fclose(file1);
    fclose(file2);

    remove(argv[1]);

    return 0;
}
