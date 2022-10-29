/* Linked Lists

What are linked lists: They are are data structure that let you store data
The give a way to keep a list of data where we can insert and delete where ever we want
We allocate memory for each item on the lists and add a pointer to it that points to the next
item in the list. In memeory a normal array is all stuck together in memory, with a linked list,
the items of the list can sit anywhere in memory. Each element is just linked up with next pointers.

I have two examples below. One statically allocates three nodes and links them up and the
second on is more advanced with dynamically allocates the nodes on the stack and then has
function to add to the list in both directions

Terms:
Nodes: each piece of data/element
Next Pointer/Edges: pointer to the next node
Head: where to list starts, first item in the linked list
Tail: End of the list

Great Video on Linked Lists: https://www.youtube.com/watch?v=VOpjAHCee7c&t=544s

Some other languages like Java have linked lists baked in

This file just shows how to code the data structure.

IMPORTANT!!: THIS CODE WILL NOT RUN BECAUSE I DO NOT HAVE VSCODE SET UP
TO COMPILE AND RUN C PROPERLY. FOLLOW THE FOLLOWING STEPS TO SET IT UP. 

https://code.visualstudio.com/docs/cpp/config-mingw
https://www.youtube.com/watch?v=522VxAk4GoI
https://www.youtube.com/watch?v=VOpjAHCee7c&t=544s

*/

#include <stdlib.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////
//  Basic Example
////////////////////////////////////////////////////////////////////

//Node for the Linked List
struct node
{
    int value; //Value storage
    struct node* next; //Next pointer storage
};
typedef struct node node_t;

//Prints out a node when passes the head pointer
void printlist(node_t *head)
{
    node_t *temporary = head;

    //Cycle through all nodes and print them out one by one until we reach the end of the list which we had set to NULL
    while(temporary != NULL)
    {
        printf("%d - ", temporary->value);
        temporary = temporary->next; //Move to the next node
    }
    printf("\n"); //New line
}


int main(void)
{
    //Statically allocated nodes
    node_t n1, n2, n3; //Creating three nodes
    node_t *head; //Creating a pointer which will be used for the head

    //Set all values of the nodes
    n1.value = 45;
    n2.value = 8;
    n3.value = 32;

    //Link all nodes up (Just choosing to link them in the order n3>n2>n1 but it doesnt matter)
    //You can try to re arrange the list and see how it prints out
    head = &n3; //Set head pointer to address of n3
    n3.next = &n2; //Set n3 next pointer to the address of n2 (our next node)
    n2.next = &n1;
    n1.next = NULL; // Setting NULL so we know when to stop traversing in a search

    printlist(head);

    //Adding a new node
    node_t n4;
    n4.value = 13;
    //Adding new node to the midde of the list
    n4.next = &n3;
    n2.next = &n4;

    printlist(head);

    //Remove the fist node by moving the head pointer down a link
    head = head->next;

    printlist(head);

    return 0;
}





////////////////////////////////////////////////////////////////////
//  Advanced Example: 
//  Linked lists are normally used when we dont know how long our
//  list is going to end up.
////////////////////////////////////////////////////////////////////

//Node for the Linked List
struct node
{
    int value; //Value storage
    struct node* next; //Next pointer storage
};
typedef struct node node_t;

//Prints out a node when passes the head pointer
void printlist(node_t *head)
{
    node_t *temporary = head;

    //Cycle through all nodes and print them out one by one until we reach the end of the list which we had set to NULL
    while(temporary != NULL)
    {
        printf("%d - ", temporary->value);
        temporary = temporary->next; //Move to the next node
    }
    printf("\n"); //New line
}

//Creates new nodes for our linked list
//Returns a pointer to the new node
node_t *create_new_node(int value)
{
    node_t *result = malloc(sizeof(node_t)); //allocate a new node
    result->value = value; //set value
    result->next = NULL; //set next pointer to NULL to be safe

    return result;
}

//Inserts a new node at the head of the linked list
//We can pass in a pointer to had pointer so that way we can change it inside the function
node_t *insert_at_head(node_t **head, node_t *node_to_insert)
{
    node_to_insert->next = head;
    *head = node_to_insert;
    return node_to_insert;
}

//Finds node in the list
node_t *find_node(node_t *head, int value)
{
    node_t *temp = head;
    while(temp != NULL)
    {
        if(temp->value ==value) return temp;
        temp = temp->next;
    }
    return NULL;
}

//Insert nodes one after another
void insert_after_node(node_t *node_to_insert_after, node_t *newnode)
{
    newnode->next = node_to_insert_after->next;
    node_to_insert_after->next = newnode;
}

int main(void)
{
    node_t *head; //Creating a pointer which will be used for the head
    node_t *temp;

    //Create new nodes and add them to the front of the list
    temp = create_new_node(32); //create a new node with value set to 32
    head = temp; //set head to the start of the list

    temp = create_new_node(8);
    temp->next = head; //set next pointer to the second node
    head = temp;

    temp = create_new_node(34);
    temp->next = head;
    head = temp;

    printlist(head);

    //What if we want to add a whole bunch of nodes to the list
    //Adding 25 nodes with values 0 to 25 set
    node_t * head = NULL; //We need to declare head set to NULL because this list will print backwards
    for(int i=0; i < 25; i++)
    {
        temp = create_new_node(i);
        insert_at_head(&head, temp);
    }

    printlist(head);

    //Find a node in the list
    temp = find_node(head, 13);
    printf("Found node with value %d\n", temp->value);

    //Inserting a new node in the list the follows temp which was the node with value 13
    insert_after_node(temp, create_new_node(75));

    return 0;
}