
#include "rprintf.h"
#include <stdint.h>

#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6

const unsigned int multiboot_header[]  __attribute__((section(".multiboot"))) = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};

uint8_t inb (uint16_t _port) {
    uint8_t rv;
   __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;

}

// QEMU definitions
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define MEMORY_ADDRESS 0xB8000

// Created static variables
static char* v = (char*)MEMORY_ADDRESS;
static int row = 0;
static int column = 0;

// created a scroll function, with first loop allowing the rows to be copied up 1 row
// and the second loop to be cleared
void scroll() {
    for (int row = 1; row < SCREEN_HEIGHT; row++) {
        for (int column = 0; column < SCREEN_WIDTH; column++) {
            v[((row - 1) * SCREEN_WIDTH + column) * 2] = v[(row * SCREEN_WIDTH + column) * 2];
        }
    }
    for (int column = 0; column < SCREEN_WIDTH; column++) {
        v[((SCREEN_HEIGHT - 1) * SCREEN_WIDTH + column) * 2] = ' ';
        v[((SCREEN_HEIGHT - 1) * SCREEN_WIDTH + column) * 2 + 1] = 0x07;
    }
}

// Look for \n cases and reset column to 0 while incrementing row
// if not adding the character to the position while updating column
int putc(int data) {
    if (data == '\n') {
        row++;
        column = 0;
    } else {
        v[(row * SCREEN_WIDTH + column) * 2] = (char)data;
        v[(row * SCREEN_WIDTH + column) * 2 + 1] = 0x07;

        column++;
    }
    
    // check if column is at end of screen width
    if (column >= SCREEN_WIDTH) {
        // if so, reset column to start of screen while moving down 
        // 1 row
        column = 0;
        row++;
    }
    // check if row is at the end of screen height
    if (row >= SCREEN_HEIGHT) {
        // if so, call scroll function to move all messages up 1
        // while clearing bottom row, and decrement row to max height - 1
        scroll();
        row = SCREEN_HEIGHT - 1;
    }
     
    return 0; 
}


void main() {
    

    // for loop to repeat phrase iterating which line the 
    // loop is on
    for (int i = 0; i < 30; i++) {
        esp_printf(putc, "I am currently on line %d\n", i); 
    }


    while(1);

}
