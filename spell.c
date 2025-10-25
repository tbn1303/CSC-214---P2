#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define TOTAL_WORDS 100000

char *dictionary(const char *path){
    int fd = open(path, O_RDONLY);

    if(fd < 0){
        perror("Error open");
        exit(EXIT_FAILURE);
    }

    char *buf = malloc(TOTAL_WORDS + 1);
    if(!buf){
        perror("Error malloc");
        close(fd);
        exit(EXIT_FAILURE);
    }

    int bytes = read(fd, buf, TOTAL_WORDS);

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

int buff_to_array(char *buf, char *dict[]){
    int numb_words = 0;
    char *token = strtok(buf, "\n");

    while(token && numb_words < TOTAL_WORDS){
        dict[numb_words++] = token;
        token = strtok(NULL, "\n");
    }

    return numb_words; 
}

int main(int argc, char **argv){
    char *dict = dictionary(argv[1]);

    printf("Dictionary as buffer:\n%s\n", dict);

    char *dictionary_array[TOTAL_WORDS];
    int a = buff_to_array(dict, dictionary_array);

    printf("Dictionary as array:\n");
    for(int i = 0; i < a; i++){
        printf("%s\n", dictionary_array[i]);
    }
    
    free(dict);

    return EXIT_SUCCESS;
}
