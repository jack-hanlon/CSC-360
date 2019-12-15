/**************************************************/
Files:
    diskinfo.c
    disklist.c
    diskget.c

    makefile
/**************************************************/

EXECUTION:

run "make" to execute the makefile to compile the files in this assignment.
to test PART 1 execute "./diskinfo.c test.img"
to test PART 2 execute "./disklist.c test.img"
to test PART 3 execute "./diskget.c /subdir/file.txt file.txt"

PART 1:

diskinfo.c displays the file information from the superblock of the .img.
Using superblock_t struct the starting address can be used to print out the 
contents by manually looking at the bytes that define each part of the superblock.
The FAT start location is in the superblock and is thus used to loop through the FAT entries
(which are 4 bytes each), to read their values. Their output will be only three options. 

0 => Free Block
1 => Reserved Block
>1 => Allocated Block

PART 2:

disklist.c displays the list of files in the root directory of the .img file. The file/directory designation in 
ubuntu is based on a status byte that holds the value of 3 or 5. If it is 3 the file is a file, and if it is a 
5 then it is a directory. This file loops through the 64 byte intervals (8 per block), and uses 2 helper functions 
to process this. Once this information is acquired the program will print the root directory information in the format
described by the assignment.

PART 3:

diskget.c gets a file from the .img file and writes it to the cwd. The program iterates through directory entries
until it finds a match with the requested argument. A new file will be created with a new placement name. Data, such as 
the # of blocks used by this file is then taken to be used for the copying process. A loop is used to copy the block size amount
of bytes into the new file. The FAT is checked to find the next block each time a block is copied, to get  the new information. 
This process continues until we reach the last block in which a modulus to check how many bytes are left is used, so that the last 
copy only copies that many bytes.  

PART 4 && PART 5:

Did not finish unfortunately due to the time constraint 