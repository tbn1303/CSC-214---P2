#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define TOTAL_WORDS 1000000
#define BUFSIZE 256

typedef struct {
    char *buf;
    int fd;
    int pos;
    int bytes;
} LINES;

void lines_init (LINES *l, int fd){
    l->buf = malloc(BUFSIZE);
    l->pos = 0;
    l->bytes = 0;
    l->fd = fd;
}

void
lines_destroy (LINES *l){
    free(l->buf);
}

char *lines_next (LINES *l){
    char *line = NULL;
    int linelen = 0;

    if (l->bytes < 0) return NULL;

    do {
        int segstart = l->pos;
        while (l->pos < l->bytes) {
            if (l->buf[l->pos] == '\n') {
                int seglen = l->pos - segstart;
                //if (DEBUG) printf("[%d/%d/%d found newline %d+%d]\n", segstart, l->pos, l->bytes, linelen, seglen);
                line = realloc(line, linelen + seglen + 1);
                memcpy(line + linelen, l->buf + segstart, seglen);
                line[linelen + seglen] = '\0';
                l->pos++;
                
                return line;
            }

            l->pos++;
        }

        if (segstart < l->pos) {
            int seglen = l->pos - segstart;
            //if (DEBUG) printf("[%d/%d/%d extending line %d+%d]\n", segstart, l->pos, l->bytes, linelen, seglen);
            line = realloc(line, linelen + seglen + 1);
            memcpy(line + linelen, l->buf + segstart, seglen);
            linelen = linelen + seglen;
            line[linelen] = '\0';
        }

        l->pos = 0;
        l->bytes = read(l->fd, l->buf, BUFSIZE);
        //if (DEBUG) printf("[got %d bytes]\n", l->bytes);
    } while (l->bytes > 0);

    l->bytes = -1;

    return line;
}

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

int check_word(const char *word){
    for(int i = 0; word[i] != '\0'; i++){
        if(isalpha(word[i])){
            return 1;
        }
    }

    return 0;
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

int word_match_in_dict(const char *word, char *dict[], int numb_words){
    for(int i = 0; i < numb_words; i++){
        if(handling_capital(word, dict[i])){
            return 1;
        }
    }
    
    return 0;
}

int check_word_in_file(const char *path, char *dict[], int numb_words){
    int fd = open(path, O_RDONLY);

    if(fd < 0){
        perror("Error open");
        return 0;
    }

    LINES lines;
    lines_init(&lines, fd);
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

        found = word_match_in_dict(buf, dictionary_array, numb_words);

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
