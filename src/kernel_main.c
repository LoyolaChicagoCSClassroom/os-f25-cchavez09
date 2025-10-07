
#include "rprintf.h"
#include <stdint.h>

#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6

const unsigned int multiboot_header[]  __attribute__((section(".multiboot"))) = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};

uint8_t inb (uint16_t _port) {
    uint8_t rv;
   __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;

}

// outb function hw2
void outb (uint16_t _port, uint8_t val) {
__asm__ __volatile__ ("outb %0, %1" : : "a" (val), "dN" (_port) );
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

// Homework 2 
//
// Keyboard Map
unsigned char keyboard_map[128] =
{
   0,  27, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
 '9', '0', '-', '=', '\b',     /* Backspace */
 '\t',                 /* Tab */
 'q', 'w', 'e', 'r',   /* 19 */
 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
   0,                  /* 29   - Control */
 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     /* 39 */
'\'', '`',   0,                /* Left shift */
'\\', 'z', 'x', 'c', 'v', 'b', 'n',                    /* 49 */
 'm', ',', '.', '/',   0,                              /* Right shift */
 '*',
   0,  /* Alt */
 ' ',  /* Space bar */
   0,  /* Caps lock */
   0,  /* 59 - F1 key ... > */
   0,   0,   0,   0,   0,   0,   0,   0,
   0,  /* < ... F10 */
   0,  /* 69 - Num lock*/
   0,  /* Scroll Lock */
   0,  /* Home key */
   0,  /* Up Arrow */
   0,  /* Page Up */
 '-',
   0,  /* Left Arrow */
   0,
   0,  /* Right Arrow */
 '+',
   0,  /* 79 - End key*/
   0,  /* Down Arrow */
   0,  /* Page Down */
   0,  /* Insert Key */
   0,  /* Delete Key */
   0,   0,   0,
   0,  /* F11 Key */
   0,  /* F12 Key */
   0,  /* All other keys are undefined */
};

void keyboardScanner() {
    while(1) {
        // check if status register changed
        uint8_t status = inb(0x64);
        
        // if status register is 1, then there is scancode
        if (status & 1) {
            // get scancode from output buffer
            uint8_t scancode = inb(0x60);
            
            // make sure scancode is greater than 128
            if (scancode > 128) {
                continue;
            }

            // print out scancode and character
            esp_printf(putc, "0x%02x %c\n", scancode, keyboard_map[scancode]);
        }
    }
}

void main() {
  
    keyboardScanner();
    
}
