/* -------------------------- 
Author     :Jack Hanlon
Class      :CSC 360
Assignment :3
Filename   :disklist.c
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


typedef struct Block_info{
    int start_block;
    int end_block;
    char *sub_name;
    char *address;
} block;

struct __attribute__((__packed__)) superblock_t{
   uint8_t   fs_id [8];
   uint16_t block_size;
   uint32_t file_system_block_count;
   uint32_t fat_start_block;
   uint32_t fat_block_count;
   uint32_t root_dir_start_block;
   uint32_t root_dir_block_count;
};
struct __attribute__((__packed__)) dir_entry_timedate_t{
   uint16_t year;
   uint8_t month;
   uint8_t day;
   uint8_t hour;
   uint8_t minute;
   uint8_t second;
};
struct __attribute__((__packed__)) dir_entry_t{
   uint8_t status;
   uint32_t starting_block;
   uint32_t block_count;
   uint32_t size;
   struct dir_entry_timedate_t create_time;
   struct dir_entry_timedate_t modify_time;
   uint8_t filename[31];
   uint8_t unused[6];
};

void print_dir_ent(char *address, int i){
   int size = 0;
   int yr = 0; 
   int mon = 0; 
   int day = 0;
   int hrs = 0; 
   int mins = 0; 
   int secs = 0;

   struct dir_entry_t* dir_ent;
   dir_ent = (struct dir_entry_t*) (address+i);
   int status = dir_ent->status;
   if(status == 3 || status == 5){
       if(status == 3){
           status = 'F';
       }else{
           status = 'D';
       }
   }

   size = ntohl(dir_ent->size);
   unsigned char* name = dir_ent->filename;
   yr = htons(dir_ent->modify_time.year);
   mon = dir_ent->modify_time.month;
   day = dir_ent->modify_time.day;
   hrs = dir_ent->modify_time.hour;
   mins = dir_ent->modify_time.minute;
   secs = dir_ent->modify_time.second;

    printf("%c %10d %30s %d/%02d/%02d %02d:%02d:%02d\n",status, size, name, yr, mon, day, hrs, mins, secs);
}

void print_root_dir(int fat_start, int fat_blocks, int block_size,int root_start_block, int root_end_block, char *address){
   int root_block = root_start_block;

   while(1){
     int i = root_block;
     /*
        This loop iterates through directory block and prints dir entries
     */
     while(i < root_block+block_size){
        int curr = 0;
        memcpy(&curr, address + i, 1);
        // continue if not a file
        if(curr != 3 && curr != 5){ 
            i +=64;
            continue; 
        }
        print_dir_ent(address, i);
        i +=64;
     }
     /*
        Iterate through dir entries whilst following FAT with regard to directory not being sequential
     */ 
       int fat_dir_location =  (fat_start * block_size)+(root_block/block_size)*4;
       memcpy(&root_block, address + fat_dir_location, 4);
       root_block = htonl(root_block);
    /*
        break if at the end of the FAT
    */
       if(root_block == -1){ break; }
       root_block = root_block * block_size;
    }
}

int main(int argc,char* argv[]){

/*  
    For args < 2 output error
*/
if(argc < 2){
        printf("Error: Enter ./diskinfo test.img");
        exit(1);
    }
/*
    - For args = 2 assume no sub directory and just list directory
    - For args = 3 assume there is a sub directory and list sub dir.
*/

int i = 0;
char* tok[256];
/*
    Tokenize argv[] based on "/"
*/
if(argc == 3){
    char* args = strtok(argv[2],"/");
    while(args){
        tok[i] = args;
        i++;
        args = strtok(NULL,"/");
    }
    tok[i] = NULL;
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
/*
    create struct to fill superblock info with
*/
    struct superblock_t* super;
    super = (struct superblock_t*) address;
    int fat_start = ntohl(super->fat_start_block);
    int fat_blocks = ntohl(super->fat_block_count);

/***************************************************************************************************************/
/*
    Initialize required superblock data
*/
int block_size = 0;
int dir_root_start = 0;
int dir_root_blocks = 0;

/*
	get contents of block size from superblock 
*/
    memcpy(&block_size, address + 8, 2);
    block_size = htons(block_size);
/*
	get contents of directory root from superblock 
*/
    memcpy(&dir_root_start, address + 24, 2);
    dir_root_start = htons(dir_root_start);
/*
	get contents of # directory root blocks from superblock 
*/
    memcpy(&dir_root_blocks, address + 28, 2);
    dir_root_blocks = htons(dir_root_blocks);
/*
    Initialize  start block and end block vars
*/
    int root_start_block = dir_root_start * block_size;
    int root_end_block = dir_root_blocks * block_size;
/*
    Print out only the root directory if there are only 2 arguments
*/
    if(argc == 2){
        print_root_dir(fat_start,fat_blocks,block_size,root_start_block, root_end_block, address);
    }

    // Did not finish this section (time constraint)
    else{
 
        /* CODE TO PRINT SUBDIRECTORY HERE */
     
     }
    munmap(address, buf.st_size);
    close(fd);
    return 0;
/***************************************************************************************************************/



}