#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define NAME "trash"
// gets concatinated onto the home dir 
#define TRASHCAN_DIRECTORY "/.trashcan"
// permissions for trashcan directory 
#define DEFAULT_PERMS 0777

// absolute path to trashcan
char* trashcan_dir;

bool create_trashcan();
void clear_trashcan();
bool exists(char*);
bool init();
bool move_to_trash(char*);
char* shorten_path(char*);

// flags
bool clear_f = false; const char clear_c = 'c';

int main(int argc, char* argv[]) {
	if(!init()) {
		printf("%s: failed to initialize\nexiting ...\n", NAME);
	}

    int flag_offset = 0;
    for(int i = 1; i < argc; i++) {
        if(argv[i][0] == '-') {
            for(int j = 1; j < strlen(argv[i]); j++) {
                if(argv[i][j] == clear_c) clear_trashcan();
            }

            flag_offset++; 
        } else break;
    }

    if(argc != 2 + flag_offset) {
        if(!clear_f) printf("%s: please provide a file for disposal\n", NAME);
        return 0;
    }

	move_to_trash(argv[1 + flag_offset]);

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

// removes all files in trashcan_dir
void clear_trashcan() {
    int t = 0;
    clear_f = true;
    DIR* d;
    struct dirent* dir;

    d = opendir(trashcan_dir);
    if(d) {
        while((dir = readdir(d)) != NULL) {
            if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) continue;
            if(dir->d_type == DT_DIR) continue;

            // trashcan_dir + / + dir->d_name + \0
            char* absolute = (char*)malloc((strlen(trashcan_dir) + 1 + strlen(dir->d_name) + 1) * sizeof(char));
            strcpy(absolute, trashcan_dir);
            strcat(absolute, "/");
            strcat(absolute, dir->d_name);

            t++;
            remove(absolute);
            free(absolute);
        }

        closedir(d);
    }

    if(t == 0) {
        printf("trashcan was already empty\n");
        return;
    }
    printf("cleared %s: (%d files removed)\n", trashcan_dir, t);
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
char* shorten_path(char* path) {
	char* last_start = path;

	for(int i = 1; *(path+i) != '\0'; i++) {
		if(*(path+i) == '/' && *(path+i+1) != '\0') last_start = path+i+1;
	}

	return last_start; 
}
