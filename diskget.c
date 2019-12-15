/* -------------------------- 
Author     :Jack Hanlon
Class      :CSC 360
Assignment :3
Filename   :diskget.c
Due        :November 29th 2019
----------------------------*/


#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>



int main(int argc,char* argv[]){
/*
    if args length is correct do operations else exit
*/
    if(argc == 4){

    char *fname;
    char *fplacement;
    fname = argv[2];
    fplacement = argv[3];

   
    
    int fd = open(argv[1],O_RDWR);
    if(fd < 0){
        printf("Error: Could not read disk img\n");
        exit(1);
    }

    struct stat buf;
    fstat(fd, &buf);
    char *address = mmap(NULL, buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);



    /****************************************************/
/*
	get contents of block size from superblock 
*/
    int blksize = 0;
    memcpy(&blksize,address + 8,2);
    blksize = htons(blksize);

/*
	get contents of FAT start location from superblock 
*/	
    int start = 0;
    memcpy(&start, address + 8, 4);
/*
	get contents of # of FAT blocks from superblock  
*/	
	int fatblks = 0;
	memcpy(&fatblks, address + 20, 4);
	fatblks = htons(fatblks);
/*
	get contents of directory start location from superblock 
*/
	int dirstart = 0;
	memcpy(&dirstart, address + 24, 4);
	dirstart = htons(dirstart);
/*
	get contents of # directory blocks from superblock 
*/	
	int dirblks = 0;
	memcpy(&dirblks, address + 29, 4);

    /***************************************************/

    int root_block = dirstart * blksize;

    while(1){
        
        int i;
        for (i = root_block; i < root_block + blksize; i += 64){
           int curr = 0;
           memcpy(&curr, address + i, 1);
            /*
                if it is a file entry
           */
           if (curr == 3){
               char name[31];
               memcpy(&name, address + i + 27, 31);
                /*
                    name comparator, if equivalent file is found
                */
               if (!strcmp(name,fname)){
                   FILE *fp;
                   fp = fopen(fplacement, "wb");

                   int current_block = 0, num_blocks = 0, fat_address = 0;
                    /*
                        Finds how many blocks are required
                   */
                   memcpy(&num_blocks, address + i + 5, 4);
                   num_blocks = htonl(num_blocks);

                   
                   memcpy(&current_block, address + i + 1, 4);
                   current_block = htonl(current_block);

                   int file_size;
                   memcpy(&file_size, address+i+9, 4);
                   file_size = htonl(file_size);

                   int j = 0;
                   while(j <num_blocks-1){
                       fwrite(address + (blksize * current_block), 1, blksize, fp);
                       fat_address = start * blksize + current_block * 4;

                       memcpy(&curr, address + fat_address, 4);
                       curr = htonl(curr);
                       current_block = curr;
                       j++;
                   }
                    /*
                        remaining # of bytes left to copy
                   */
                   int remain = file_size % blksize;
                    /*
                        If the modulus returns 0 then one blksize is left
                   */
                   if (remain == 0){
                        remain = blksize;
                   }
                    /*
                        Write the remaining info
                    */
                   fwrite(address + (blksize * current_block), 1, remain, fp);
                   fat_address = start * blksize + current_block * 4;
                   memcpy(&curr, address + fat_address, 4);

                   munmap(address, buf.st_size);
                   close(fd);
                   printf("Getting %s from %s and writing as %s.\n", fname, argv[1], fplacement);
                   return 0;
               }
           }
        }
        int fat_dir_location = (root_block/blksize)*4 + (start * blksize);
        memcpy(&root_block, address + fat_dir_location, 4);
        root_block = htonl(root_block);

        if(root_block != -1){
            root_block = root_block * blksize;
        }else{ 
            break;
        }
        
    }

    printf("File not found.\n");
    munmap(address, buf.st_size);
    close(fd);
    return 0;
    }
    printf("ERROR: Incorrect number of arguments\n");
    exit(1);



}