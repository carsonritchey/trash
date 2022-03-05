#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

#define NAME "trash"
// gets concatinated onto the home dir 
#define TRASHCAN_DIRECTORY "/.trashcan"
// permissions for trashcan directory 
#define DEFAULT_PERMS 0777

// absolute path to trashcan
char* trashcan_dir;

bool create_trashcan();
bool exists(char* file);
bool init();
bool move_to_trash(char* source_dir);
char* shorten_path(char* path);

int main(int argc, char* argv[]) {
	if(!init()) {
		printf("%s: failed to initialize\nexiting ...\n", NAME);
	}

    if(argc != 2) {
        printf("%s: please provide a file for disposal\n", NAME);
        return 0;
    }

	move_to_trash(argv[1]);

    return 0;
}

// checks to see if trashcan exists and creates it if it doesn't
bool create_trashcan() {
	struct stat st;
	if(stat(trashcan_dir, &st) == -1) {
		printf("'%s' not found, creating directory ...\n", trashcan_dir);

		mkdir(trashcan_dir, DEFAULT_PERMS);
	}

	return true;
}

// returns wether or not a file exists
bool exists(char* file) {
	struct stat st;
	return (stat(file, &st) == 0);
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

// moves file to trashcan dir
bool move_to_trash(char* source_dir) {
    FILE* source = fopen(source_dir, "r");
    if(!source) {
        printf("%s: '%s' was not found\n", NAME, source_dir);
        return false;
    }

    // trashcan_dir + / + source_dir + \0 
    char* destination_dir = (char*)malloc( (strlen(trashcan_dir) + strlen(source_dir) + 1 + 1) * sizeof(char));
    strcpy(destination_dir, trashcan_dir);
    strcat(destination_dir, "/");
    strcat(destination_dir, shorten_path(source_dir));

    FILE* destination = fopen(destination_dir, "w");
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
    free(destination_dir);

	return true;
}

// returns a pointer to the filename of a given path
// /home/name/file -> file

// may need to remove trailing / in the future.. idk 
char* shorten_path(char* path) {
	char* last_start = path;

	for(int i = 1; *(path+i) != '\0'; i++) {
		if(*(path+i) == '/' && *(path+i+1) != '\0') last_start = path+i+1;
	}

	return last_start; 
}
