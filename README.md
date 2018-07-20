# Sequential-Search-OpenMP
Description:
Searches for any or all instances of a pattern in text using OpenMP using 4 cores.

Input:
Jobscript.sh is for running the program. The program tries to read text files within a folder called inputs. Number the text files "text1", "text2" etc. and number the pattern files "pattern1", "pattern2" etc...





Output:
First 2 columns indicate the text file number and pattern file number.
Final column indicates the following:
Searching for any: Result -2 if pattern found, -1 if not found
Searching for all: Index number if found, -1 if not found
