NAME: Thai Bao Nguyen
RUID: 249008650

NAME: Zachary Adam
RUID: 

Design
------
Load dictionary
    Our code is designed to use binary search for search word in dictionary.
    We first load dictionary file in to a dynamic buffer (either from current working directory or from directory from user input) 
    and return a variable to count number of words in dictionary.
    We use that number of words to decide the size of array to store all those words in that array.
    Finally, we sort the dictionary for binary search.

Handling word in file (standard input is treated as a file)
    When checking words in file, we use iter-persis.c from Professor's example to resize buffer for storing file and standard input.
    Then, store all the valid words from file (or standard input) into dynamic array.
    After that, we clean up the word, meaning: remove leading and trailing puntuation.
    At final step, the function checking if word match any word in dictionary, if not print out an error message and return a flag to 
    indicate if there are any errors (1 if word is not matching in dictionary).

Handling directory:
    We use a function to handling suffix if user input -s.
    By using recursively for directory traversal, we open it until we find the file name from user's input, if not print out an error 
    message and return, also, a has_error flag.

----------------------------------------------------------------------------------------------------------------------------------------------------------
Test Plan:
----------

Test: verifying standard input works as well as the rows and columns

The dictionary file we tested here included some words such as "abandon" and "academic", so testing the standard input would include 
commands like:

./spell dict 
abandon academic 

for which the output should be empty because those are both correct spellings being input by the user.

then the user typing in the same run for example:

abandonn academic 

the output should be: 

2:1 abandonn 

as this is the second line of input and the mispelled word ("abandonn" in this case) is output by the program for starting at the 
second line first column while academic is not printed because it is still spelled correctly. This also checks that standard input is accepted in the
abscence of a second argument.

------------------------------------------------------------------------------------------------------------------------------------------------------------

Test: Verifying Capitalization rules using standard input

To verify the capitalization rules are met I tested the exact example used in the spec: if the dictionary contains "foo" and "Bar" then inputting the
words "Foo", "fOO", "BAr", "BAR", and "bar", then the only word it should not accept is "bar". 

./spell dict
Foo fOO BAr BAR bar

the output should be:

1:17 bar

Thus satisfying the capitalization requirement that the capitalization in the dictionary must be matched, but lower-case letters in the dictionary can be matched
with either upper or lowercase letters in the input.
---------------------------------------------------------------------------------------------------------------------------------------------------------------

Test 3