#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>

#define TOTAL_WORDS 100000 // Maximum number of words in dictionary
#define BUFSIZE 256 // Buffer size for reading files
#define WORDLEN 128 // Maximum length of a word
#define PATH_LENGTH 4096 // Maximum path length

//Structure to handle line reading from file descriptor
typedef struct{
    char *buf;
    int fd;
    int pos;
    int bytes;
}LINES;

//Initialize the LINES structure
void lines_init(LINES *l, int fd){
    l->buf = malloc(BUFSIZE);
    l->pos = 0;
    l->bytes = 0;
    l->fd = fd;
}

//Destroy the LINES structure and free allocated memory
void lines_destroy(LINES *l){
    free(l->buf);
}

//Read the next line from the file descriptor
char *lines_next(LINES *l){
    char *line = NULL;
    int linelen = 0;

    if(l->bytes < 0) return NULL;

    do{
        int segstart = l->pos;
        while(l->pos < l->bytes){
            if(l->buf[l->pos] == '\n'){
                int seglen = l->pos - segstart;
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
            line = realloc(line, linelen + seglen + 1);
            memcpy(line + linelen, l->buf + segstart, seglen);
            linelen = linelen + seglen;
            line[linelen] = '\0';
        }

        l->pos = 0;
        l->bytes = read(l->fd, l->buf, BUFSIZE);
    }while (l->bytes > 0);

    l->bytes = -1;

    return line;
}

//Function to read the entire dictionary file into a buffer
char *dictionary(const char *path){
    int fd = open(path, O_RDONLY);

    if(fd < 0){
        perror("Error open");
        exit(EXIT_FAILURE);
    }

    char *buf = malloc(BUFSIZE);
    if(!buf){
        perror("Error malloc");
        close(fd);
        exit(EXIT_FAILURE);
    }

    int bytes;
    int total = 0; // Total bytes read
    int size = BUFSIZE; // Current buffer size

    while((bytes = read(fd, buf + total, size - total - 1)) > 0){
        total += bytes; // Update total bytes read

        // Resize buffer if string in line exceeds current size
        if(total >= size - 1){
            size *= 2;
            char *temp_buf = realloc(buf, size);

            // Check for realloc failure
            if(temp_buf == NULL){
                perror("Error realloc");
                free(buf);
                close(fd);
                exit(EXIT_FAILURE);
            }

            buf = temp_buf;
        }
    }

    if(bytes < 0){
        perror("Error read");
        free(buf);
        close(fd);
        exit(EXIT_FAILURE);
    }

    buf[total] = '\0';
    close(fd);

    return buf;
}

//Function to convert buffer into an array of words
int buff_to_array(char *buf, char *dict[]){
    int numb_words = 0;
    char *token = strtok(buf, "\n"); // Tokenize buffer by newline

    // Split buffer into words and store in dictionary array
    while(token && numb_words < TOTAL_WORDS){
        dict[numb_words++] = token;
        token = strtok(NULL, "\n"); // Get next token
    }

    return numb_words;
}

//Function to check if a word is valid (contains at least one alphabetic character)
int check_valid_word(const char *word){
    for(int i = 0; word[i] != '\0'; i++){
        if(isalpha(word[i])){
            return 1; // Valid word
        }
    }
    return 0; // Invalid word
}

//Function to clean up a word by removing leading and trailing punctuation
void cleanup_word(const char *src, char *dest){  
    int start = 0; // Index of first character
    int end = strlen(src) - 1; // Index of last character (exclude null terminator)

    // Remove leading punctuation
    while(start <= end && strchr("{[\"'", src[start]) != NULL){
        start++;
    }

    // Remove trailing punctuation
    while(end >= start && !isalnum(src[end])){
        end--;
    }

    // If no valid characters found, return empty string
    if(start > end){
        dest[0] = '\0';
        return;
    }

    // Copy cleaned word to new memory without changing original
    int len = end - start + 1;
    memcpy(dest, src + start, len);
    dest[len] = '\0';
}

//Function to check if a word matches any word in the dictionary (case-insensitive) (binary search)
int word_match_in_dict(const char *word, char *dict[], int numb_words){
    int low = 0;
    int high = numb_words - 1;

    while(low <= high){
        int mid = low + (high - low) / 2;
        int found = strcasecmp(word, dict[mid]);

        // Exact match found and handling case sensitivity
        if(found == 0){
            int match_word_upper = 1; // Match word uppercase flag

            // Check for uppercase letters in the dictionary word (other than the uppercase that has to match in dictionary, the other letters can be upper or lower from input)
            for(int i = 0; *(dict[mid] + i); i++){
                if(isupper(*(dict[mid] + i)) && word[i] != *(dict[mid] + i)){
                    match_word_upper = 0; // Word does not match case sensitivity
                    break;
                }
            }

            if(match_word_upper == 1){
                return 1; // Word match (including case sensitivity)
            }
        }

        if(found > 0)
            low = mid + 1;

        else high = mid - 1;
    }
    
    return 0;
}

//Function to check words in a file if matching word in the dictionary
int check_words_in_file(const char *path, char *dict[], int numb_words){
    int fd;

    if(strcmp(path, "/dev/stdin") == 0){
        fd = STDIN_FILENO;

        if(fd < 0){
            perror("Error open");
            return 0;
        }
    } 
    
    else{
        fd = open(path, O_RDONLY);
        if(fd < 0){
            perror("Error open file");
            return 0;
        }
    }

    LINES lines;
    lines_init(&lines, fd);
    char *line;
    int has_error = 0;
    int line_number = 1;

    // Read each line from the file
    while(line = lines_next(&lines)){
        int col = 1;
        int i = 0;
        char word[WORDLEN]; // Buffer to store the current word
        char cleaned_word[WORDLEN]; // Buffer to store cleaned word

        for(int pos = 0; ; pos++){
            char c = line[pos];

            if(isalpha(c) || c == '_' || c == '-' || isdigit(c) || c == '\''){
                if(i < WORDLEN - 1){
                    word[i++] = c; // Add character to current word then increment index
                }
            }

            else{
                if(i > 0){
                    word[i] = '\0';
                    cleanup_word(word, cleaned_word);

                    if(strlen(cleaned_word) > 0 && !word_match_in_dict(cleaned_word, dict, numb_words)){
                        printf("%s:%d:%d %s\n", path, line_number, col - i, cleaned_word);
                        has_error++;
                    }

                    i = 0; // Reset index for next word
                }
            }

            if(c == '\0'){
                break; // End of word, exit loop
            }

            col++; // Move to next column
        }

        free(line);
        line_number++;
    }

    lines_destroy(&lines);
    return has_error;
}

int check_suffix(const char *filename, const char *suffix){
    int len_filename = strlen(filename);
    int len_suffix = strlen(suffix);

    // If filename is shorter than suffix, it cannot match
    if(len_filename < len_suffix){
        return 0;
    }

    return strcmp(filename + len_filename - len_suffix, suffix) == 0; // Compare end of filename with suffix
}

// Function to traverse a directory and check files with a specific suffix
int directory_traverse(const char *dirpath, const char *suffix, char *dict[], int numb_words){
    DIR *dir = opendir(dirpath);

    if(dir == NULL){
        perror("Error open directory");
        return 1;
    }

    struct dirent *entry;
    struct stat file_stat;
    char file_path[PATH_LENGTH];
    int has_error = 0;

    // Read each entry in the directory
    while((entry = readdir(dir)) != NULL){
        // Skip files starting with a dot (hidden files) or special entries "." and ".."
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_name[0] == '.'){
            continue;
        }

        snprintf(file_path, PATH_LENGTH, "%s/%s", dirpath, entry->d_name);
        
        if(stat(file_path, &file_stat) < 0){
            perror("Error stat");
            continue; // Skip this entry on error
        }

        // If entry is a directory, recursively traverse it
        if(S_ISDIR(file_stat.st_mode)){
            has_error += directory_traverse(file_path, suffix, dict, numb_words);
        }

        // If entry is a regular file with matching suffix, check words in the file
        else if(S_ISREG(file_stat.st_mode) && check_suffix(entry->d_name, suffix)){
            has_error += check_words_in_file(file_path, dict, numb_words);
        }
    }

    closedir(dir);
    return has_error;
}

int main(int argc, char **argv){
    if(argc < 2){
        fprintf(stderr, "Usage: %s <dictionary_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *default_suffix = ".txt"; // Default file suffix

    // Check for optional suffix argument
    int arg_index = 1;
    if(argc > 2 && strcmp(argv[1], "-s") == 0){
        default_suffix = argv[2];
        arg_index = 3;

        // Ensure dictionary file is provided after suffix
        if(argc < 4){
            fprintf(stderr, "Usage: %s [-s suffix] <dictionary_file>\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    char *dict = dictionary(argv[arg_index]);
    char *dictionary_array[TOTAL_WORDS];
    int numb_words = buff_to_array(dict, dictionary_array);

    qsort(dictionary_array, numb_words, sizeof(char *), (int (*)(const void *, const void *))strcasecmp);

    int has_error = 0;

    // If no files or directories specified, traverse current directory
    if(argc <= arg_index + 1){
        has_error = check_words_in_file("/dev/stdin", dictionary_array, numb_words);
    }

    else{
        for(int i = arg_index + 1; i < argc; i++){
            struct stat path_stat;

            if(stat(argv[i], &path_stat) < 0){
                perror("Error stat");
                has_error = 1;
                continue;
            }

            // Check if path is a directory or regular file
            if(S_ISDIR(path_stat.st_mode)){
                has_error += directory_traverse(argv[i], default_suffix, dictionary_array, numb_words);
            }

            // If regular file, check words in the file
            else if(S_ISREG(path_stat.st_mode)){
                has_error += check_words_in_file(argv[i], dictionary_array, numb_words);
            }

            // Skip non-regular files
            else{
                fprintf(stderr, "Skipping non-regular file: %s\n", argv[i]);
            }
        }
    }

    free(dict);

    return has_error ? EXIT_FAILURE : EXIT_SUCCESS;
}