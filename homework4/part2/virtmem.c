/**
 * virtmem.c
 * Written by Junyeong Yoon 201723300
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

// Macros
#define TLB_SIZE 16
#define PAGES 256
#define PAGE_BITS 8
#define PAGE_MASK 255
#define PAGE_SIZE 256
#define OFFSET_BITS 8
#define OFFSET_MASK 255
#define MEMORY_SIZE PAGES * PAGE_SIZE

// Max number of characters per line of input file to read
#define BUFFER_SIZE 10

// Type definitions
struct TlbEntry {
    unsigned char pageNo;
    unsigned char frameNo;
};

// Global variables

// TLB is kept track of as a circular array, with the oldest element being overwritten once the TLB is full
struct TlbEntry tlb[TLB_SIZE];

// Number of inserts into TLB that have been completed.
// Use as tlbIndex % TLB_SIZE for the index of the next TLB line to use
int tlbIndex = 0;

// pagetable[logical_page] is the physical page number for logical page.
// Value is -1 if that logical page isn't yet in the table
int pagetable[PAGES];

signed char *backingStore; // Pointer to memory mapped backing file

// Function prototypes
int checkPageInTlb(int pageNo);
void addToTlb(int pageNo, int frameNo);
int checkPagetable(int pageNo);
void addToPagetable(int pageNo, int frameNo);

int main(int argc, const char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: ./virtmem <backing store> <virtual address vector>\n");
        exit(1);
    }

    // Load backing store data(*.bin)
    const char *backingStoreFileName = argv[1];
    int backingStoreFileDescriptor = open(backingStoreFileName, O_RDONLY);

    // Access backing store as memory through [backingStore] pointer
    backingStore = mmap(0, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, backing_fd, 0);

    // Load virtual address vector data(*.txt)
    const char *virtualAddressVectorFileName = argv[2];
    FILE *file = fopen(virtualAddressVectorFileName, "r");

    // Fill all elements of the TLB entries with -1 for initially empty table
    int i;

    for (i = 0; i < TLB_SIZE; i++) {
        tlb[i].pageNo = -1;
        tlb[i].frameNo = -1;
    }

    // Fill page table entries with -1 for initially empty table
    for (i = 0; i < PAGES; i++) {
        pagetable[i] = -1;
    }

    // Character buffer for reading lines of input file
    char buffer[BUFFER_SIZE];

    // Data that is needed to keep track of to compute stats at end
    int numAddress = 0;
    int numFrame = 0;
    int numTlb = 0;
    int countTlbHit = 0;
    int countPageFault = 0;

    // Number of the next unallocated physical page in main memory
    unsigned char free_page = 0;

    // Address translation data
    int logicalAddress;
    int physicalAddress;

    // Reference page number and frame number
    int pageNo;
    int frameNo;
    int offset;
    signed char value;

    printf("----------------------------------------------\n");
    printf("| Logical address | Physical address | Value |\n");
    printf("|-----------------+------------------+-------|\n");

    while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
        numAddress++;
        logicalAddress = atoi(buffer);
        pageNo = (logicalAddress >> PAGE_BITS) & PAGE_MASK;
        offset = logicalAddress & OFFSET_MASK;
        frameNo = checkPageInTlb(pageNo);

        // If the page was not found from the TLB
        if (frameNo == -1) {
            frameNo = checkPagetable(pageNo);

            // If the page was not brought into memory(page fault)
            if (frameNo == -1) {
                numFrame++;
                frameNo = numFrame % PAGES;

                addToPagetable(pageNo, frameNo);

                countPageFault++;
            }

            addToTlb(pageNo, frameNo);
        } else {
            countTlbHit++;
        }

        // Translate the frame into the physical address
        physicalAddress = (frameNo << OFFSET_BITS) + offset;

        // Get the value
        value = backingStore[frameNo];

        printf("| %15d | %16d | %5d |\n", logicalAddress, physicalAddress, value);
    }

    printf("----------------------------------------------\n\n");
    printf("Number of translated addresses = %d\n", numAddress);
    printf("Page faults = %d\n", countPageFault);
    printf("Page fault rate = %.3f\n", countPageFault / (1. * numAddress));
    printf("TLB hits = %d\n", countTlbHit);
    printf("TLB hit rate = %.3f\n", countTlbHit / (1. * numAddress));

    return 0;
}

// Get the frame number of the page in the TLB, return -1 if the page is not in the tlb
int checkPageInTlb(int pageNo) {
    int i;
    for (i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].pageNo == pageNo) {
            return tlb[i].frameNo;
        }
    }

    return -1;
}

// Add "tlb" node to the TLB
void addToTlb(int pageNo, int frameNo) {
    tlb[tlbIndex].pageNo = pageNo;
    tlb[tlbIndex].frameNo = frameNo;
    tlbIndex = (tlbIndex + 1) % TLB_SIZE;
}

// Get the frame at given page number, return -1 if the page is empty
int checkPagetable(int pageNo) {
    return pagetable[pageNo];
}

// Add "pagetable" node the page table
void addToPagetable(int pageNo, int frameNo) {
    pagetable[pageNo] = frameNo;
}
