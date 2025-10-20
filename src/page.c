#include "page.h"
#include <stddef.h>

#define NUM_PAGES 128

struct ppage physical_page_array[NUM_PAGES];

struct ppage *head; //temp pointer to eventual head of the list

void init_pfa_list(void) {
    for (int i = 0; i < NUM_PAGES; i++) {
        physical_page_array[i].physical_addr = (void *)((unsigned long)i * (2 * 1024 * 1024));
        if (i < NUM_PAGES - 1) {
            physical_page_array[i].next = &physical_page_array[i + 1];
        } else {
            physical_page_array[i].next = NULL;
        }

        if (i > 0) {
            physical_page_array[i].prev = &physical_page_array[i - 1];
        } else {
            physical_page_array[i].prev = NULL;
        }
        
    }
    head = &physical_page_array[0];
}

struct ppage *allocate_physical_pages(unsigned int npages) {
    if (npages == 0 || head == NULL) {
        return NULL;
    }

    struct ppage *first = head;
    struct ppage *last = head;

    for (int i = 1; i < NUM_PAGES; i++) {
        if (last->next == NULL) {
            return NULL;
        }
        last = last->next;
    }

    head = last->next;
    if (head != NULL) {
        head->prev = NULL;
    }
    last->next = NULL;
    first->prev = NULL;

    return first;
}

void free_physical_pages(struct ppage *ppage_list) {
    if (ppage_list == NULL) {
        return;
    }

    struct ppage *last = ppage_list;
    while (last->next != NULL) {
        last = last->next;
    }

    last->next = head;
    if (head != NULL) {
        head->prev = last;
    }

    ppage_list->prev = NULL;
    head = ppage_list;
}


