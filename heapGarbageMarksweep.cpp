#include<iostream>
using namespace std;
struct node
{
    int data;
    bool mark;
    struct node* adjacent1;
    struct node* adjacent2;
    struct node* adjacent3;
    node(int num)
    {
        data = num;
        adjacent1=NULL;
        adjacent2=NULL;
        adjacent3=NULL;
        mark=false;
    }
}; //we know at most 3 connections are present
typedef struct root_tag
{
    node* pointer;
}root;

node* heap[8];
/*
root1->5 7 3
       |/|/|
root2->1 8 | 
       |\| |
       | |\|
       |\| |\
       2 9 10
*/
void initialize(root& root1,root& root2)
{                                                   
    node* temp = new node(5);
    heap[0]=temp;
    temp = new node(1);
    heap[1]=temp;
    temp = new node(2);
    heap[2]=temp;
    temp = new node(9);
    heap[3]=temp;
    temp = new node(10);
    heap[4]=temp;
    temp = new node(7);
    heap[5]=temp;
    temp = new node(8);
    heap[6]=temp;
    temp = new node(3);
    heap[7]=temp;
    temp = NULL;
    //now all nodes are in the 'heap',to create connections now
    root1.pointer=heap[0];//root1->5
    heap[0]->adjacent1=heap[1];//5->1
    root2.pointer=heap[1];//root2->1
    heap[1]->adjacent1=heap[2];//1->2
    heap[1]->adjacent2=heap[3];//1->9
    heap[1]->adjacent3=heap[4];//1->10
    heap[5]->adjacent1=heap[1];//7->1
    heap[5]->adjacent2=heap[6];//7->8
    heap[6]->adjacent1=heap[4];//8->10
    heap[7]->adjacent1=heap[6];//3->8
    heap[7]->adjacent2=heap[4];//3->10
    //connections done
}
void print_node(const node* node)
{
    if(node==NULL)
    {
        return;
    }
    cout<<" "<<node->data;
    if (node->adjacent1!=NULL || node->adjacent2!=NULL || node->adjacent3!=NULL)
    {
        cout<<"-[";
        print_node(node->adjacent1);
        print_node(node->adjacent2);
        print_node(node->adjacent3);
        cout<<" ]";
    }
}
void print_heap(node** arr)
{
    for(int i=0;i<8;i++)
    {
        if(arr[i]!=NULL)
        {
            print_node(arr[i]);
            cout<<endl;
        }
    }
}
void print_useful_heap(const root& value)
{
    cout<<"root->";
    print_node(value.pointer);
    cout<<endl;
}

void mark_node(node* ptr)
{
    node* head=ptr;
    node* tail=NULL;
    node* middle=NULL;
    int flag=1;
    while(head !=NULL)
    {
        if(!head->mark) //if node we are on is unmarked, mark it
        {
            head->mark=true;
        }
        if(head->adjacent1!=NULL && !head->adjacent1->mark) //if adjacent node to this is unmarked, move to it
        {
            tail=middle;
            middle=head;
            head=head->adjacent1;
        }
        else if(head->adjacent2!=NULL && !head->adjacent2->mark)//only move along one adjacent path every iteration
        {
            tail=middle;
            middle=head;
            head=head->adjacent2;   
        }
        else if(head->adjacent3!=NULL && !head->adjacent3->mark)
        {
            tail=middle;
            middle=head;
            head=head->adjacent3;   
        }
        else
        {
            head=middle;
            middle=tail;
            tail=NULL;
        }

    }
}
void mark(const root& value)
{
    mark_node(value.pointer);
}
void sweep(node** arr)
{
    for(int i=0;i<8;i++)
    {
        if(arr[i]!=NULL)
        {
            if(!arr[i]->mark)
            {
                //disconnect the node
                arr[i]->adjacent1=NULL;
                arr[i]->adjacent2=NULL;
                arr[i]->adjacent3=NULL;
                delete arr[i];
                arr[i]=NULL;
            }
        }
    }
}
void garbage_collector(const root& r1,const root& r2,node** hp)
{
    cout<<"Mark phase started"<<endl;
    mark(r1);
    mark(r2);
    cout<<"Marking done"<<endl;
    cout<<"Sweep phase started"<<endl;
    sweep(hp);
}
int main()
{
    root A,B;
    initialize(A,B);
    cout<<"This is the Simulation for mark sweep garbage collector\n";
    cout<<"the full heap is: "<<endl;

    print_heap(heap);
    cout << endl;
    cout<<"Heap connected to the roots is: "<<endl;

    print_useful_heap(A);
    print_useful_heap(B);
    cout << endl;

    garbage_collector(A,B,heap);
    cout<<"the garbage collector was called, let us see the full heap now\n";

    print_heap(heap);
    cout<<"let us see if it matches heap connected to roots printed below\n";

    print_useful_heap(A);
    print_useful_heap(B);
}