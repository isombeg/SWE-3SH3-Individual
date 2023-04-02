/*
*
* Author: Prof. Neerja Mhaskar
* Course: Operating Systems Comp Sci/SFWRWENG 3SH3

*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//Define the needed macros
#define BUFFER_SIZE 10
#define PAGES 8 //Number of pages needed
#define OFFSET_BITS 12
#define PAGE_SIZE 4096
#define OFFSET_MASK PAGE_SIZE-1

#define getPageNumber(virtualAddr) virtualAddr >> OFFSET_BITS
#define getOffset(virtualAddr) virtualAddr & OFFSET_MASK
#define getPhysicalAddr(frameNum, offset) (frameNum << OFFSET_BITS) | offset
#define getFrameNumber(pageTable, pageNumber) pageTable[pageNumber]

int main(int argc, char *argv[]) {
    //declare variables and declare and initialize the page table.
    char buff[BUFFER_SIZE];
    int virtualAddr, pageNumber, offset, frameNumber, physicalAddr;
    int pageTable[PAGES] = {6,4,3,7,0,1,2,5};

    /*Declare a file pointer fptr. fopen initializes an object of type file.
     This is stored in fptr. The mode 'r' indicates that the file is for
    read only purpose.*/
    FILE *fptr = fopen("labaddr.txt", "r");

    /*Read characters from the file at a time.
    These characters are stored in buff to read the logical addresses*/
    while(fgets(buff, BUFFER_SIZE, fptr) != NULL){

        virtualAddr = atoi(buff);

        /*While you read the logical addresses using bitwise operators in C to compute the physical address for each logical address read and print it*/
        pageNumber = getPageNumber(virtualAddr);
        offset = getOffset((virtualAddr));

        frameNumber = getFrameNumber(pageTable, pageNumber);
        physicalAddr = getPhysicalAddr(frameNumber, offset);
        printf("Virtual addr is %d: Page number = %d & Offset= %d.\tPhysical addr = %d\n", virtualAddr, pageNumber, offset, physicalAddr);
    }

    /*Close the file.*/
    fclose(fptr);

    return 0;
}
