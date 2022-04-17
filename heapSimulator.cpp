#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum
{
    FAILURE,
    SUCCESS
} status_code;
typedef struct free_Node_tag
{
    int address;
    int size;
    struct free_Node_tag *next;
    struct free_Node_tag *prev;
} free_Node;

typedef struct var_Node_tag
{
    char name[10];
    int address;
    int size;
    struct var_Node_tag *next;
    struct var_Node_tag *prev;
} var_Node;

typedef struct Heap_tag
{
    int size;
    free_Node *freelist;
    var_Node *varlist;
} Heap;

// function for creating free node
free_Node *free_node(int address, int size)
{
    free_Node *nptr = (free_Node *)malloc(sizeof(free_Node));

    nptr->size = size;
    nptr->address = address;
    nptr->next = NULL;
    nptr->prev = NULL;

    return nptr;
}

// function creates var_node and returns pointer to var_node
var_Node *var_node(char *name, int address, int size)
{
    var_Node *nptr = (var_Node *)malloc(sizeof(var_Node));

    strcpy(nptr->name, name);
    nptr->size = size;
    nptr->address = address;
    nptr->next = NULL;
    nptr->prev = NULL;

    return nptr;
}

// intialize heap with number of bytes that user give
void intialize(Heap *hptr, int size)
{
    hptr->size = size;
    hptr->freelist = free_node(0, hptr->size);
    hptr->varlist = NULL;
}

void free_sortedInsert(free_Node **headptr, free_Node *nptr)
{
    free_Node *ptr, *prev_node, *head;
    head = *headptr;
    nptr->next = nptr->prev = NULL;

    if (head == NULL) // Empty list case
    {
        head = nptr;
    }
    else
    {
        ptr = head;
        while (ptr->next != NULL && ptr->address < nptr->address) // Skip all nodes having address lesser than nptr->address
        {
            ptr = ptr->next;
        }

        if (ptr->next == NULL && ptr->address < nptr->address) // If inserted node address is more than address of all nodes in the list
        {
            ptr->next = nptr;
            nptr->prev = ptr;
        }
        else
        {
            if (ptr == head) // If inserted node address is less than address of all nodes in the list
            {
                head = nptr;
            }
            prev_node = ptr->prev;

            // Joining all links
            if (prev_node != NULL)
            {
                prev_node->next = nptr;
            }
            nptr->prev = prev_node;
            nptr->next = ptr;
            ptr->prev = nptr;
        }
    }
    *headptr = head;
}

void var_sortedInsert(var_Node **headptr, var_Node *nptr)
{
    var_Node *ptr, *prev_node, *head;
    head = *headptr;
    nptr->next = nptr->prev = NULL;

    if (head == NULL) // Empty list case
    {
        head = nptr;
    }
    else
    {
        ptr = head;
        while (ptr->next != NULL && ptr->address < nptr->address) // Skip all nodes having address lesser than nptr->address
        {
            ptr = ptr->next;
        }

        if (ptr->next == NULL && ptr->address < nptr->address) // If inserted node address is more than address of all nodes in the list
        {
            ptr->next = nptr;
            nptr->prev = ptr;
        }
        else
        {
            if (ptr == head) // If inserted node address is less than address of all nodes in the list
            {
                head = nptr;
            }
            prev_node = ptr->prev;

            // Joining all links
            if (prev_node != NULL)
            {
                prev_node->next = nptr;
            }
            nptr->prev = prev_node;
            nptr->next = ptr;
            ptr->prev = nptr;
        }
    }
    *headptr = head;
}

status_code free_deleteNode(free_Node **lpptr, free_Node *p)
{
    status_code ret_val = SUCCESS;
    free_Node *prev_node, *next_node;

    if (p == NULL)
    {
        ret_val = FAILURE;
    }
    else
    {
        prev_node = p->prev;
        next_node = p->next;

        if (prev_node != NULL) // If we are not deleting the first node
        {
            prev_node->next = next_node;
        }
        else // If we are deleting the first node
        {
            *lpptr = next_node;
        }

        if (next_node != NULL) // If we are not deleting the last node
        {
            next_node->prev = prev_node;
        }

        free(p);
    }

    return ret_val;
}

status_code var_deleteNode(var_Node **lpptr, var_Node *p)
{
    status_code ret_val = SUCCESS;
    var_Node *prev_node, *next_node;

    if (p == NULL)
    {
        ret_val = FAILURE;
    }
    else
    {
        prev_node = p->prev;
        next_node = p->next;

        if (prev_node != NULL) // If we are not deleting the first node
        {
            prev_node->next = next_node;
        }
        else // If we are deleting the first node
        {
            *lpptr = next_node;
        }

        if (next_node != NULL) // If we are not deleting the last node
        {
            next_node->prev = prev_node;
        }

        free(p);
    }

    return ret_val;
}

// function allocating memory according to first fit
int MyMalloc(Heap *hptr, char *varName, int varSize)
{
    int i = 1;
    free_Node *free_lptr, *fptr;
    var_Node *var_lptr, *vptr;

    free_lptr = hptr->freelist;
    fptr = free_lptr;
    var_lptr = hptr->varlist;
    vptr = var_lptr;

    int found = 0;

    while (fptr != NULL && !found) // Search for the first fit block in the free list.
    {
        if (fptr->size >= varSize)
        {
            found = 1;
        }
        else
        {
            fptr = fptr->next;
        }
    }

    if (!found || varSize == 0) // If not found, failed to allocate memory
    {
        i = 0;
    }
    else // If found
    {
        var_Node *var_nptr = var_node(varName, fptr->address, varSize); // make a new var_Node of same size and address, and given name
        var_sortedInsert(&var_lptr, var_nptr);                          // and sorted insert into the varlist

        fptr->address = fptr->address + varSize; // Updating the free block after partition
        fptr->size = fptr->size - varSize;

        if (fptr->size == 0) // If size becomes 0, delete from free list
        {
            free_deleteNode(&free_lptr, fptr);
        }
    }

    hptr->freelist = free_lptr;
    hptr->varlist = var_lptr;
    return i;
}

int Myfree(Heap *hptr, char *varName)
{
    int sc = 1;
    free_Node *free_lptr, *fptr, *free_prev_node;
    var_Node *var_lptr, *vptr;
    int found = 0;

    free_lptr = hptr->freelist;
    fptr = free_lptr;
    var_lptr = hptr->varlist;
    vptr = var_lptr;

    while (vptr != NULL && !found) // Search the varlist for the var_Node of given name
    {
        if (strcmp(varName, vptr->name) == 0)
        {
            found = 1;
        }
        else
        {
            vptr = vptr->next;
        }
    }

    if (!found) // If not found, failed to free memory
    {
        sc = 0;
    }
    else // If found
    {
        free_Node *free_nptr = free_node(vptr->address, vptr->size); // make a new free_Node of same size and address.
        free_sortedInsert(&free_lptr, free_nptr);                    // and sorted insert into free list
        var_deleteNode(&var_lptr, vptr);                             // delete the var_Node from varlist

        // Merging free nodes
        free_Node *free_next_node, *free_prev_node;
        free_next_node = free_nptr->next;
        free_prev_node = free_nptr->prev;

        if (free_next_node != NULL)
        {
            if (free_nptr->address + free_nptr->size == free_next_node->address) // If next free_Node is adjacent to the newly inserted free_Node
            {
                free_nptr->size = free_nptr->size + free_next_node->size;
                free_deleteNode(&free_lptr, free_next_node);
            }
        }
        if (free_prev_node != NULL)
        {
            if (free_prev_node->address + free_prev_node->size == free_nptr->address) // If previous free_Node is adjacent to the newly inserted free_Node
            {
                free_prev_node->size = free_prev_node->size + free_nptr->size;
                free_deleteNode(&free_lptr, free_nptr);
            }
        }
    }

    hptr->freelist = free_lptr;
    hptr->varlist = var_lptr;
    return sc;
}

void free_list_print(free_Node *free_ptr)
{
    free_Node *ptr = free_ptr;
    printf("Displaying free list:\n");
    if (free_ptr == NULL)
    {
        printf("No free space available\n");
    }
    else
    {
        printf("Start_Address\tEnd_Address\tSize\n");
        while (ptr != NULL)
        {
            printf("%d\t\t%d\t\t%d\n", ptr->address, ptr->address + ptr->size - 1, ptr->size);
            ptr = ptr->next;
        }
    }
    printf("\n");
}

void variable_list_print(var_Node *var_lptr)
{
    var_Node *ptr = var_lptr;
    printf("Displaying variable list:\n");
    if (var_lptr == NULL)
    {
        printf("No allocated space available\n");
    }
    else
    {
        printf("Start_Address\tEnd_Address\tSize\tVariable_name\n");
        while (ptr != NULL)
        {
            printf("%d\t\t%d\t\t%d\t%s\n", ptr->address, ptr->address + ptr->size - 1, ptr->size, ptr->name);
            ptr = ptr->next;
        }
    }
    printf("\n");
}

void printHeap(Heap *hptr)
{
    var_Node *vptr = hptr->varlist;
    free_Node *fptr = hptr->freelist;
    printf("Total heap space = %d bytes\n", hptr->size);
    printf("Heap memory display:\n");
    printf("Start_Address\tEnd_Address\tSize\tAllocation_Status\n");
    while (vptr != NULL && fptr != NULL)
    {
        if (vptr->address < fptr->address)
        {
            printf("%d\t\t%d\t\t%d\tAllocated\n", vptr->address, vptr->address + vptr->size - 1, vptr->size);
            vptr = vptr->next;
        }
        else
        {
            printf("%d\t\t%d\t\t%d\tFree\n", fptr->address, fptr->address + fptr->size - 1, fptr->size);
            fptr = fptr->next;
        }
    }

    if (vptr != NULL)
    {
        while (vptr != NULL)
        {
            printf("%d\t\t%d\t\t%d\tAllocated\n", vptr->address, vptr->address + vptr->size - 1, vptr->size);
            vptr = vptr->next;
        }
    }
    else
    {
        while (fptr != NULL)
        {
            printf("%d\t\t%d\t\t%d\tFree\n", fptr->address, fptr->address + fptr->size - 1, fptr->size);
            fptr = fptr->next;
        }
    }
    printf("\n");
}

void freeHeap(Heap *hptr)
{
    free_Node *free_lptr = hptr->freelist;
    free_Node *fptr = free_lptr;
    var_Node *var_lptr = hptr->varlist;
    var_Node *vptr = var_lptr;

    while (free_lptr != NULL)
    {
        fptr = free_lptr;
        free_lptr = free_lptr->next;
        free(fptr);
    }

    while (var_lptr != NULL)
    {
        vptr = var_lptr;
        var_lptr = var_lptr->next;
        free(vptr);
    }

    hptr->freelist = NULL;
    hptr->varlist = NULL;
}

int main()
{
    char control_line;
    Heap MyHeap;
    int isize;
    printf("Of how much memory heap do you need?\n");
    scanf("%d", &isize);
    intialize(&MyHeap, isize);

    char varName[10];
    int size;
    int sc;

    control_line = '2';
    while (control_line != '1')
    {

        printf("1.To exit the terminal\n");
        printf("2.To print heap list\n");
        printf("3.To allocate Memory in heap\n");
        printf("4.To free Memory from heap\n");
        printf("Enter the proper code to excute: ");
        scanf(" %c", &control_line);
        printf("\n");
        switch (control_line)
        {
        case '1': // Exit terminal
        {
            printf("Terminal exited\n");
            break;
        }
        case '2': // Print Heap, free list and variable list
        {
            printHeap(&MyHeap);
            free_list_print(MyHeap.freelist);
            variable_list_print(MyHeap.varlist);
            break;
        }
        case '3': // Memory allocation with first fit strategy
        {
            printf("Enter variable/tag name\n");
            scanf("%s", varName);
            printf("Enter size that you want to allocate\n");
            scanf("%d", &size);
            sc = MyMalloc(&MyHeap, varName, size);
            if (sc == 1)
                printf("Memory Allocation is Successful\n");
            else
                printf("Memory Allocation is unsuccessful\n");
            break;
        }
        case '4': // Memory free
        {
            printf("Enter Variable/tag name\n");
            scanf("%s", varName);
            sc = Myfree(&MyHeap, varName);
            if (sc == 1)
                printf("Memory free Successful\n");
            else
                printf("Memory free unsuccessful\n");
            break;
        }
        default:
        {
            printf("incorrect code\n");
            break;
        }
        }
    }
    freeHeap(&MyHeap);
    return 0;
}