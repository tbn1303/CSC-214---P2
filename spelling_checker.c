#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define BUFSIZE 100000

char *dictionary(const char *path){
    int fd = open(path, O_RDONLY);

    if(fd < 0){
        perror("Error open");
        exit(EXIT_FAILURE);
    }

    char *buf = malloc(BUFSIZE + 1);
    if(!buf){
        perror("Error malloc");
        close(fd);
        exit(EXIT_FAILURE);
    }

    int bytes = read(fd, buf, BUFSIZE);

    if(bytes < 0){
        perror("Error read");
        free(buf);
        close(fd);
        exit(EXIT_FAILURE);
    }

    buf[bytes] = '\0';

    close(fd);

    return buf;
}

int main(int argc, char **argv){
   
    char *dict = dictionary(argv[1]);

    printf("Dictionary:\n%s\n", dict);

    

    return EXIT_SUCCESS;
}