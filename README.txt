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

Thus satisfying the capitalization requirement that the capitalization in the dictionary must be matched, but lower-case letters in the 
dictionary can be matched with either upper or lowercase letters in the input.
---------------------------------------------------------------------------------------------------------------------------------------------------------------

Test: Verifying punctuation

to verify that the leading and trailing punctuation of a word is properly trimmed off for the word to be read, I tested using the same dict file 
with the word "academic" for example, and just included punctuation. The word should still be read correctly even given leading and trailing punctuation for example:

./spell dict 
[{!@.academic}]

should not return anything because the word aside from the trailing and leading punctuation is still correctly spelled. Whereas an incorrect 
spelling with punctuation would look like:

./spell dict
[{!@.academicc}]

should output:

1:6 academicc

as the word is spelled incorrectly to the dictionary, but the punctuations are still trimmed off of it, 
and still able to track the correct column number of the word.

--------------------------------------------------------------------------------------------------------------------------------------------------------------------

Test: Verifying the program can scan directories for text files as well as the optional flag for extensions

To make sure the program adequately scans directories we tested using an example directory called "quux" inside the P2 directory where the spell program is 
and inside of the quux directory is two files: "dirtest.txt" and "extensiontest.md" which purposefully both have different file extensions. 

if we wanted to scan the directory quux for .txt files by using the absence of the optional argument flag we would run

./spell dict quux

which should scan the quux directory looking for .txt extension files only and ignore the .md file. All incorrect word outputs should only come from "dirtest.txt"
and the output of our program correctly does this ignoring the "extensiontest.md" file, both testing our program for scanning directories and testing the default
optional flag absence of .txt files. In our specific case testing with the dict file should output:

quux/dirtest.txt:1:1 This
quux/dirtest.txt:1:6 is
quux/dirtest.txt:1:11 test
quux/dirtest.txt:1:16 of
quux/dirtest.txt:1:19 scanning
quux/dirtest.txt:1:28 directories

as every single word in the "dirtest.txt" file is incorrectly spelled, so every word should be printed but NO words from "extensiontest.md" should be printed.

If we wanted to scan the quux directory for .md files then we should run:

./spell -s .md dict quux

In this case the output should only contain incorrectly spelled words (that do not match our test file "dict") from the "extensiontest.md" file and NOT the
"dirtest.txt" file since "extensiontest.md" is the only .md file in the quux directory. and the output should be: 

quux/dirtest.txt:1:1 This
quux/dirtest.txt:1:6 is
quux/dirtest.txt:1:11 test
quux/dirtest.txt:1:16 of
quux/dirtest.txt:1:19 scanning
quux/dirtest.txt:1:28 directories

as every word in the "extensiontest.md" file is spelled incorrectly (compared to the test dict file we have) and NO words from "dirtest.txt" should be printed.

This test verifies the ability of our program to scan a directory and read files as well as scan a directory specifically for a certain file extension. 

