#include <stdio.h>
#include <stdlib.h>
// #include <stdbool.h>
#include <pthread.h>

#include "os-sim.h"

struct node_struct
{
    pcb_t* pcb;
    struct node_struct *next;
};

typedef struct node_struct node;

struct list_struct
{
	node* head;
	node* tail;
	int size;
};

typedef struct list_struct list;

struct node_struct* create_list(pcb_t *pcb);


struct node_struct* add_to_list(pcb_t *pcb, int add_to_end);


struct node_struct* search_in_list(pcb_t *pcb, node **prev);


int delete_from_list(pcb_t *pcb);


struct node_struct* get_Head();

pthread_cond_t empty_ready;
list* list_ptr;