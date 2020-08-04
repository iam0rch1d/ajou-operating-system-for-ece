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
        fprintf(stderr,"Usage: ./a.out <virtual address>\n");
        
        return -1;
    }
    
    unsigned int reference = (unsigned int) atoi(argv[1]);
    int page_size = 0x1 << PAGE_SIZE_POWEROF2;
    int page_num = reference / page_size;
    int offset = = reference % page_size;
    
    printf("The address %d contains:\n", reference);
    printf("page number = %d\n", page_num);
    printf("offset = %d\n", offset);

    return 0;
}
