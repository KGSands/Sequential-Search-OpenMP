# Sequential-Search-OpenMP
<b>Description</b>:
Searches for any or all instances of a pattern in text in OpenMP using 4 cores with the pattern searched left to right with all characters tested separately.

<b>Input</b>:
Jobscript.sh is for running the program. The program tries to read text files within a folder called inputs. Number the text files "text1", "text2" etc. and number the pattern files "pattern1", "pattern2" etc...

<b>Output</b>:
First 2 columns indicate the text file number and pattern file number.
Final column indicates the following:
Searching for any: Result -2 if pattern found, -1 if not found
Searching for all: Index number if found, -1 if not found
