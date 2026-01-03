#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Product
{
    struct Product *prev, *next;
    int productId;
    char name[100];
    char category[100];
    float price;
    int stock;
} Product;

Product *head = NULL;

void display() // for debug purpose
{
    Product *temp = head;
    if (head == NULL)
    {
        printf("\nNo products available.\n");
        return;
    }
    printf("\nProducts available:");
    printf("\n```````````````````\n");
    while (temp != NULL)
    {
        printf("Product ID : %d\n", temp->productId);
        printf("Name       : %s\n", temp->name);
        printf("Category   : %s\n", temp->category);
        printf("Price      : %g\n", temp->price);
        printf("Stock      : %d\n\n", temp->stock);
        temp = temp->next;
    }
}

Product *create()
{
    Product *newnode = (Product *)malloc(sizeof(Product));
    newnode->prev = NULL;
    newnode->next = NULL;
    return newnode;
}

Product *insert()
{
    Product *temp = head;
    // Product* newnode=(Product*)malloc(sizeof(Product));
    // newnode->prev=NULL;
    // newnode->next=NULL;
    Product *newnode = create();

    printf("\n\nenter ID:");
    scanf("%d", &newnode->productId);
    printf("enter name:");
    scanf("%s", newnode->name);
    printf("enter category:");
    scanf("%s", newnode->category);
    printf("enter price:");
    scanf("%f", &newnode->price);
    printf("enter stock:\n");
    scanf("%d", &newnode->stock);

    if (!head)
    {
        head = newnode;
    }
    else
    {
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = newnode;
        newnode->prev = temp;
    }
    // printf("successfully inserted\n\n");
    // print();
    return head;
}
Product *revdll()
{
    Product *temp, *curr;
    temp = NULL;
    curr = head;
    while (curr != NULL)
    {
        temp = curr->prev;
        curr->prev = curr->next;
        curr->next = temp;
        curr = curr->prev;
    }
    if (temp != NULL)
    {
        head = temp->prev;
    }
    // printf("\nsuccessfully reversed \n\n");
    print();
    return head;
}

int main()
{

    return 0;
}
