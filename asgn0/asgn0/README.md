Split.c is a program that takes a single character delimiter, and then reads all filespassed in, as well as standard input, and replaces the delimiter with a newline (\n) each time it is seen, and outputs everything to standard out. To use the program, entera command following the following format: 
./split <delimiter> <file1> ...
where the delimiter is a single character file1 is the name of any file. You can also replace a file name with a '-' if you wish to take input from standard input.The ... just mean that you can input as many file names as you want. 

Although this program only has two functions, main and fileHandling, I broke this process down into multiple steps. 
1.open the file
2.read the file into a buffer
3.parse buffer
4.write buffer to stdout
5.repeat from 2 till EOF
6.repeat from 1 until all files have been split
The main function handles passing each file name and the delimiter to fileHandling, while fileHandling deals with everything else. The buffer was done by repeatedly allocating and freeing memory. The file name is checked to see if it will be taking in either input from stdin or from a file, and then reads and stores it into a decently sized buffer that is allocated in memory. The buffer is then traversed to check each character against the delimiter, and replaced with a '\n' if found. Once the end of the buffer has been reached, or the end of the file character has been found in the buffer, it is then printed to stdout, and the buffer is then freed and reallocated. The buffer may not alawys be large enough to store the file/input all at once, so the buffer is constantly freed and reallocated to give the program a quick and reliable place to store its data. 

There are many possible errors in this program. Most errors will cause an exit of the program with message describing what went wrong. Going through the list of errors that I encountered, starting from the top: Too Few Arguments, multicharacter delimiter,  malloc failed to allocate memory for the buffer, invalid file name. Invalid file name was the only error that did not cause an exit from the program. It instead caused the program to skip that argument and moved on to the next file.

My design is reasonably efficient due to a decently sized buffer. The size of 4096 Bytes is small enough to not cost alot of time traversing, and is large enough to not need to constantly access the disk. Since I am always creating a buffer when it's needed, and freeing immediately after it is done being used, memory leaks, or possibly overwriting allocated data. 
