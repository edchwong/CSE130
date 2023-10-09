#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFSIZE 4096

/*
1.open
2.read
3.parse buffer
4.write buffer to stdout
5.repeat from 2 till EOF
*/

void fileHandling(char *fileName, char delim);

int error_count = 0;

int main(int argc, char **argv) {
    if (argc < 3) {
        //arg[0] = name of program
        //arg[1] = delimiter
        fprintf(stderr, "%s", "too few arguments");
        exit(EXIT_FAILURE);
    }

    if (strlen(argv[1]) > 1) {
        fprintf(stderr, "Cannot handle multi-character splits: %s\n", argv[1]);
        fprintf(stderr, "usage: %s: <split_char> [<file1> <file2> ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (int i = 2; i < argc; i++) {
        fileHandling(argv[i], argv[1][0]); //replace with fileHandling
    }

    if (error_count) {
        exit(EXIT_FAILURE);
    }

    return 0;
}

void fileHandling(char *fileName, char delim) {
    int fd = -1; // set file descriptor

    // open the file
    if ((strlen(fileName) == 1) && (fileName[0] == '-')) {
        fd = STDIN_FILENO;
    } else {
        fd = open(fileName, O_RDONLY);
    }
    if (fd == -1) {
        fprintf(stderr, "split: %s: No such file or directory\n", fileName);
        error_count++;
    }

    char *buff = malloc(BUFFSIZE * sizeof(char)); // create buffer
    if (buff == NULL) {
        fprintf(stderr, "%s", "malloc failed");
        exit(EXIT_FAILURE);
    }

    int br = read(fd, buff, BUFFSIZE); // br is bytes read, text added to buffer
    while (br > 0) { // while bytes read > 0
        // start parsing the buffer
        int i = 0;
        while (i < br) {
            if (buff[i] == delim) {
                buff[i] = '\n';
            }
            i++;
        }

        // write buffer to stdout
        write(STDOUT_FILENO, buff, br);

        br = read(fd, buff, BUFFSIZE);
    }

    free(buff);
    close(fd);
}
