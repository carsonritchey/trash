#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

#define NAME "trash"
// gets concatinated onto the home dir 
#define TRASHCAN_DIRECTORY "/.trashcan"
#define DEFAULT_PERMS 777

// absolute path to trashcan
char* trashcan_dir;

// checks to see if trashcan exists and creates it if it doesn't
bool create_trashcan() {
	struct stat st;
	if(stat(trashcan_dir, &st) == -1) {
		printf("'%s' not found, creating directory ...\n", trashcan_dir);

		mkdir(trashcan_dir, DEFAULT_PERMS);
	}

	return true;
}

bool init() {
	trashcan_dir = getenv("HOME");

	if(trashcan_dir == NULL) {
		printf("%s: $HOME path variable not found\n", NAME);
		return false;
	}

	strcat(trashcan_dir, TRASHCAN_DIRECTORY);

	if(!create_trashcan()) {
		printf("%s: unable to create ~%s\n", NAME, TRASHCAN_DIRECTORY);
		return false;
	}

	return true;
}

// moves a file from one location to another
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
	if(!init()) {
		printf("%s: failed to initialize\nexiting ...\n", NAME);
	}

    if(argc != 3) {
        printf("%s: please provide sufficient arguments\n", NAME);
        return 0;
    }

	move(argv[1], argv[2]);

    return 0;
}

