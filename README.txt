NAME: Thai Bao Nguyen
RUID: 249008650

NAME:
RUID:

Design
------
spell.c
    Our code is designed to use binary search for search word in dictionary.
    We first load dictionary file in to a dynamic buffer (either from current working directory or from directory from user input) and return a variable to count number of words in dictionary.
    We use that number of words to decide the size of array to store all those words in that array.
    We use iter-persis.c from Professor's example to resize buffer for storing file and standard input.
    After that, we clean up the word, meaning: remove leading and trailing puntuation.
    
