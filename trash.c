#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define NAME "trash"
#define TRASHCAN_DIRECTORY "~/.trashcan"
#define DEFAULT_PERMS 777

char* home_dir;

bool move(char* source_dir, char* destination_dir) {
    FILE* source, *destination;
    source = fopen(source_dir, "r");
    destination = fopen(destination_dir, "w");

    if(!source) {
        printf("%s: '%s' was not found\n", NAME, source_dir);
        return false;
    }
    if(!destination) {
        printf("%s: unable to move '%s' to '%s': no such file or directory\n", NAME, source_dir, destination_dir);
        return false;
    }

    int c;
    while((c = fgetc(source)) != EOF)
        fputc(c, destination);

    fclose(source);
    fclose(destination);

    remove(source_dir);

	return true;
}

int main(int argc, char* argv[]) {
	home_dir = getenv("HOME");
	if(home_dir == NULL) {
		printf("$HOME path variable not found\nexiting ...\n");
		return 0;
	}

	printf("%s\n", home_dir);

    if(argc != 3) {
        printf("%s: please provide sufficient arguments\n", NAME);
        return 0;
    }

	move(argv[1], argv[2]);

    return 0;
}

