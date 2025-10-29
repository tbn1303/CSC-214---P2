Authors: Thai Nguyen & Zachary Adam

Overall review of current code: 

- fix sized memory allocation for the dictionary + should also be renamed
- does linear search on the dictionary instead of binary search which could be slow for large dictionaries
- the logic for capitilization rules must be fixed
- column tracking and partial word parsing issues
- standard input is not handled by the streamer and therefore can only go word by word
- the line reader has potential to leak memory
- argument handling in main can lead to segfault errors. 

overall this is combining the approach of the dictionary and read into one single buffer, rather than consistently 
using stream for all input including standard input and file.