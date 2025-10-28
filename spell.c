#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define TOTAL_WORDS 1000000

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

int handling_capital(const char *word, const char *word_in_dict){
    if(isupper(word_in_dict[0])){
        if(isupper(word[0])){
            return (strcasecmp(word, word_in_dict) == 0);
        }

        return 0;
    }

    return (strcasecmp(word, word_in_dict) == 0);
}

int check_word(const char *word, char *dict[], int numb){
    for(int i = 0; i < numb; i++){
        if(handling_capital(word, dict[i])){
            return 1;
        }
    }
    
    return 0;
}

int check_word_file(const char *path){
    int fd = open(path, O_RDONLY);

    if(fd < 0){
        perror("Error open");
        return 0;
    }

    int bytes;
    char buf[256];
    
    while((bytes = read(fd, buf, 255)) > 0){
        if(buf[bytes - 1] == '\n' || buf[bytes - 1] == '\r' || buf[bytes - 1] == ' '){
            buf[bytes - 1] = '\0';
        }

        for(int i = 0; i < bytes; i++){
            if(!isalpha(buf[i])){
                buf[i++] = buf[i + 1];
                bytes--;
            }
        }
    }

    close(fd);
    return 1;
}

int main(int argc, char **argv){
    char *dict = dictionary(argv[1]);

    //printf("Dictionary as buffer:\n%s\n", dict);

    char *dictionary_array[TOTAL_WORDS];
    int numb_words = buff_to_array(dict, dictionary_array);

    /*printf("Dictionary as array:\n");
    for(int i = 0; i < numb_words; i++){
        printf("%s\n", dictionary_array[i]);
    }*/

    int fd = STDIN_FILENO;
    char *buf = malloc(256);
    int found;

    int bytes;
    while((bytes = read(fd, buf, 255)) > 0){
        buf[bytes - 1] = '\0';

        found = check_word(buf, dictionary_array, numb_words);

        if(found){
            printf("Word found in dictionary.\n");
        } 

        else{
            printf("Word not found in dictionary.\n");
        }
    }
    
    free(dict);
    free(buf);

    return EXIT_SUCCESS;
}
