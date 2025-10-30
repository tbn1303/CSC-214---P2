Authors: Thai Nguyen & Zachary Adam


10/27 Comments
- fix sized memory allocation for the dictionary + should also be renamed
- does linear search on the dictionary instead of binary search which could be slow for large dictionaries
- the logic for capitilization rules must be fixed
- column tracking and partial word parsing issues
- the line reader has potential to leak memory
- argument handling in main can lead to segfault errors. 


10/29 solved most of the prior issues
- Solved fix sized memory allocation
- changed linear search to binary search for more efficient lookup
- added valid checking for number of arguments
- added logic for cleaning the word of leading and trailing symbols
- case handling logic properly implemented

Test Plan: 

