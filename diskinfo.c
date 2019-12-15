/* -------------------------- 
Author     :Jack Hanlon
Class      :CSC 360
Assignment :3
Filename   :diskinfo.c
Due        :November 29th 2019
----------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>

typedef struct Block_info {
    int start_block;
    int end_block;
    char *sub_name;
    char *address;
} block;

struct __attribute__((__packed__)) superblock_t {
   uint8_t   fs_id [8];
   uint16_t block_size;
   uint32_t file_system_block_count;
   uint32_t fat_start_block;
   uint32_t fat_block_count;
   uint32_t root_dir_start_block;
   uint32_t root_dir_block_count;
};




int main(int argc, char* argv[]){

/*
	checks correct number of files are in args
*/
    if(argc < 2){
        printf("Error: Enter ./diskinfo test.img");
        exit(1);
    }
/*
	opens file directory in read/write or throws error
*/
    int fd = open(argv[1],O_RDWR);
    if(fd < 0){
        printf("Error: Could not read disk img");
        exit(1);
    }

/*
	create buffer to feed file info into char* address var.
*/
    struct stat buf;
    fstat(fd, &buf);
    char* address = mmap(NULL,buf.st_size,PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    

    //Print Superblock Information
    /*********************************************************************************/
    printf("Super block information:\n"); 
/*
	get contents of block size from superblock and print
*/
    int blksize = 0;
    memcpy(&blksize,address + 8,2);
    blksize = htons(blksize);
    printf("Block size: %d\n",blksize);
/*
	get contents of block count from superblock and print
*/
    int blkcount = 0;
	memcpy(&blkcount, address + 12, 4);
	blkcount = htons(blkcount);
	printf("Block count: %d\n", blkcount);
/*
	get contents of FAT start location from superblock and print
*/	
    int start = 0;
    memcpy(&start, address + 8, 4);
	printf("FAT starts: %d\n", start);
/*
	get contents of # of FAT blocks from superblock and print
*/	
	int fatblks = 0;
	memcpy(&fatblks, address + 20, 4);
	fatblks = htons(fatblks);
	printf("FAT blocks: %d\n", fatblks);
/*
	get contents of directory start location from superblock and print
*/
	int dirstart = 0;
	memcpy(&dirstart, address + 24, 4);
	dirstart = htons(dirstart);
	printf("Root directory start: %d\n", dirstart);
/*
	get contents of # directory blocks from superblock and print
*/	
	int dirblks = 0;
	memcpy(&dirblks, address + 29, 4);
	printf("Root directory blocks: %d\n\n", dirblks);
    //Print FAT Information
	/*********************************************************************************/

/*
	Create struct super to hold superblock info
*/
	struct superblock_t* super;
    super = (struct superblock_t*) address;
/*
	re-order the bytes for use
*/
	int fat_start_block = htons(super->block_size) * ntohl(super->fat_start_block);
    int fat_end_block = htons(super->block_size) * ntohl(super->fat_block_count);
/*
	Initialize block variables
*/
	int free_blocks = 0;
    int reserved_blocks = 0;
    int allocated_blocks = 0;
    int i = fat_start_block;
/*
	iterate through fat blocks and check if they're equal to 0 1 or else allocated.
*/
	while(i < fat_start_block + fat_end_block){
		int curr = 0;
		memcpy(&curr, address + i, 4);
		curr = htonl(curr);
        if (curr == 0) free_blocks++;
        else if (curr == 1) reserved_blocks++;
        else allocated_blocks++;
		i += 4;
	}

/*
	Print the FAT info
*/
    printf("FAT information:\n");
    printf("Free Blocks: %d\n",free_blocks);
    printf("Reserved Blocks: %d\n",reserved_blocks);
    printf("Allocated Blocks: %d\n",allocated_blocks);
/*
	Close fd and return
*/
    munmap(address, buf.st_size);
    close(fd);



    return 0;
}
