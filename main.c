#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define MAX_PATH_LENGTH 1024

void clean() {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(".");
    if (dir == NULL) {
        perror("Error reading directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Skip directories and dotfiles
        if (entry->d_type == DT_DIR || entry->d_name[0] == '.') {
            continue;
        }

        // Get file extension
        char *ext = strrchr(entry->d_name, '.');
        if (ext && strcmp(ext, ".9") != 0) { // Check if extension is not empty and not ".9"
            ext++; // Skip the dot
            char destDir[MAX_PATH_LENGTH];
            if (snprintf(destDir, sizeof(destDir), "Documents/%s", ext) >= (int)sizeof(destDir)) {
                fprintf(stderr, "Destination directory path too long: %s\n", ext);
                continue;
            }

            // Create directory if it doesn't exist
            if (mkdir(destDir, 0750) == -1 && errno != EEXIST) {
                perror("Error creating directory");
                continue;
            }

            char srcPath[MAX_PATH_LENGTH];
            char destPath[MAX_PATH_LENGTH];
            if (snprintf(srcPath, sizeof(srcPath), "./%s", entry->d_name) >= (int)sizeof(srcPath)) {
                fprintf(stderr, "Source path too long: %s\n", entry->d_name);
                continue;
            }
            if (snprintf(destPath, sizeof(destPath), "%s/%s", destDir, entry->d_name) >= (int)sizeof(destPath)) {
                fprintf(stderr, "Destination path too long: %s/%s\n", destDir, entry->d_name);
                continue;
            }

            // Check if the file already exists in the destination directory
            struct stat buffer;
            if (stat(destPath, &buffer) == 0) {
                printf("File %s already exists in %s\n", srcPath, destDir);
                continue;
            }

            // Move the file
            if (rename(srcPath, destPath) == -1) {
                perror("Error moving file");
            } else {
                printf("Moved %s to %s\n", srcPath, destDir);
            }
        }
    }

    closedir(dir);
}

int main() {
    clean();
    return 0;
}
