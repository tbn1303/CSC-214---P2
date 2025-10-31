NAME: Thai Bao Nguyen
RUID: 249008650

NAME: Zachary Adam
RUID: 

Design
------
Load dictionary
    Our code is designed to use binary search for search word in dictionary.
    We first load dictionary file in to a dynamic buffer (either from current working directory or from directory from user input) and return a variable to count number of words in dictionary.
    We use that number of words to decide the size of array to store all those words in that array.
    Finally, we sort the dictionary for binary search.

Handling word in file (standard input is treated as a file)
    When checking words in file, we use iter-persis.c from Professor's example to resize buffer for storing file and standard input.
    Then, store all the valid words from file (or standard input) into dynamic array.
    After that, we clean up the word, meaning: remove leading and trailing puntuation.
    At final step, the function checking if word match any word in dictionary, if not print out an error message and return a flag to indicate if there are any errors (1 if word is not matching in dictionary).

Handling directory:
    We use a function to handling suffix if user input -s.
    By using recursively for directory traversal, we open it until we find the file name from user's input, if not print out an error message and return, also, a has_error flag.


Test Plan:

