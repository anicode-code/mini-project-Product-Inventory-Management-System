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

int main()
{
    
    return 0;
}
