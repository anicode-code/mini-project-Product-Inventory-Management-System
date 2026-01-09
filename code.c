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
struct Product* searchProduct(struct Product* head, int id) {
    struct Product* temp = head;

    while (temp != NULL) {
        if (temp->productId == id)
            return temp;
        temp = temp->next;
    }

    return NULL;
}

void updateProduct(struct Product* head, int id) {
    struct Product* p = searchProduct(head, id);

    if (p == NULL) {
        printf("Product not found!\n");
    } else {
        printf("Product Found: %s\n", p->name);

        printf("Enter new price: ");
        scanf("%f", &p->price);

        printf("Enter new stock: ");
        scanf("%d", &p->stock);

        printf("Product updated successfully!\n");
    }
}

Product* deleteProduct(struct Product *head, int id) {
    struct Product *temp = head;

    while (temp != NULL && temp->productId != id)
        temp = temp->next;

    if (temp == NULL) {
        printf("Product not found!\n");
        return head;
    }

    if (temp->prev != NULL)
        temp->prev->next = temp->next;
    else
        head = temp->next;

    if (temp->next != NULL)
        temp->next->prev = temp->prev;

    free(temp);
    printf("Product deleted successfully.\n");

    return head;
}

void display() {
    struct Product *temp = head;
    if (temp == NULL) {
        printf("List is empty.\n");
        return;
    }

    while (temp != NULL) {
        printf("ID:%d Name:%s Category:%s Price:%.2f Stock:%d\n",
               temp->productId, temp->name, temp->category,
               temp->price, temp->stock);
        temp = temp->next;
    }
}

int main()
{
    
    return 0;
}
