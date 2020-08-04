/**
 * Program that masks page number and offset from an
 * unsigned 32-bit address.
 */

#include <stdio.h>
#include <unistd.h>

#define PAGE_SIZE_POWEROF2 12 // Page size in power of 2
/**
 * The size of a page is 4 KB (12 bits)
 * A memory reference appears as:
 * |------------|-----|
 *  31        12 11  0
 *             ^ 
 *     PAGE_SIZE_POWEROF2
 */ 

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr,"Usage: ./addr <virtual address>\n");
        
        return -1;
    }
    
    unsigned int referenceAddress = (unsigned int) atoi(argv[1]);
    int pageSize = 0x1 << PAGE_SIZE_POWEROF2;
    int pageNo = referenceAddress / pageSize;
    int offset = referenceAddress % pageSize;
    
    printf("The address %d contains:\n", referenceAddress);
    printf("page number = %d\n", pageNo);
    printf("offset = %d\n", offset);

    return 0;
}
