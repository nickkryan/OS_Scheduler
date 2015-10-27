#include "list.h"
#include "os-sim.h"

extern pthread_cond_t empty_ready;


// node *head = NULL;
// node *tail = NULL;
extern list *list_ptr;

struct node_struct* create_list(pcb_t *pcb)
{
    list_ptr = (struct list_struct*)malloc(sizeof(struct list_struct));
    struct node_struct *ptr = (struct node_struct*)malloc(sizeof(struct node_struct));
    if(NULL == ptr)
    {
        return NULL;
    }
    (*ptr).pcb = pcb;
    (*ptr).next = NULL;

    (*list_ptr).head = ptr;
    (*list_ptr).tail = ptr;
    (*list_ptr).size = 1;
    return ptr;
}

struct node_struct* add_to_list(pcb_t *pcb, int add_to_end)
{
    // fprintf(stderr, "Add_to_node begins\n");

    if(NULL == list_ptr)
    {
        return (create_list(pcb));
    }
    struct node_struct *ptr = (struct node_struct*)malloc(sizeof(struct node_struct));
    if(NULL == ptr)
    {
        return NULL;
    }
    (*ptr).pcb = pcb;
    (*ptr).next = NULL;

    if(add_to_end)
    {
        list_ptr->tail->next = ptr;
        list_ptr->tail = ptr;
    }
    else
    {
        ptr->next = list_ptr->head;
        list_ptr->head = ptr;
    }
    list_ptr->size += 1;
    return ptr;
}

struct node_struct* search_in_list(pcb_t *pcb, struct node_struct **prev)
{
    struct node_struct *ptr = list_ptr->head;
    struct node_struct *tmp = NULL;
    int found = 0;

    while(ptr != NULL)
    {
        if((*ptr).pcb == pcb)
        {
            found = 1;
            break;
        }
        else
        {
            tmp = ptr;
            ptr = (*ptr).next;
        }
    }

    if(1 == found)
    {
        // if(prev)
        *prev = tmp;
        return ptr;
    }
    else
    {
        return NULL;
    }
}

int delete_from_list(pcb_t *pcb)
{
    struct node_struct *prev = NULL;
    struct node_struct *del = NULL;

    del = search_in_list(pcb,&prev);
    if(del == NULL)
    {
        return -1;
    }
    else
    {
        if(prev != NULL)
            (*prev).next = (*del).next;

        if (del == list_ptr->head && del == list_ptr->tail)
        {
            list_ptr = NULL;
        }
        else if(del == list_ptr->head)
        {
            list_ptr->head = (*del).next;
            //
        }
        else if(del == list_ptr->tail)
        {
            list_ptr->tail = prev;
        }
        if (list_ptr != NULL) list_ptr->size -= 1;
    }

    // free(del);
    // del = NULL;

    return 0;
}

struct node_struct* get_Head()
{
    if(NULL == list_ptr)
    {
        return NULL;
    }

    return list_ptr->head;
}

// struct list_struct* get_List()
// {
//     if (NULL == list_ptr)
//     {
//         return NULL;
//     }
//     return list_ptr;
// }