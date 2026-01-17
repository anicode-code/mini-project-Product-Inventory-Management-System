#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Product
{
    struct Product *prev, *next;
    int id;
    char name[100];
    char category[100];
    float price;
    int stock;
} Product;

Product *head = NULL, *tail = NULL;
FILE *fp = NULL;

void displayEachProduct(Product *product)
{
    printf("Product ID : %d\n", product->id);
    printf("Name       : %s\n", product->name);
    printf("Category   : %s\n", product->category);
    printf("Price      : %g\n", product->price);
    printf("Stock      : %d\n\n", product->stock);
}

void traverseForward()
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
        displayEachProduct(temp);
        temp = temp->next;
    }
}

void traverseBackward()
{
    Product *temp = tail;
    if (tail == NULL)
    {
        printf("\nNo products available.\n");
        return;
    }
    printf("\nProducts available:");
    printf("\n```````````````````\n");
    while (temp != NULL)
    {
        displayEachProduct(temp);
        temp = temp->prev;
    }
}

Product *create()
{
    Product *newnode = (Product *)malloc(sizeof(Product));
    newnode->prev = NULL;
    newnode->next = NULL;
    return newnode;
}

void insertProduct()
{
    Product *newnode = create();

    printf("\n\nEnter ID: ");
    scanf("%d", &newnode->id);
    printf("Enter name: ");
    scanf(" %99[^\n]", newnode->name);
    printf("Enter category: ");
    scanf(" %99[^\n]", newnode->category);
    printf("Enter price: ");
    scanf("%f", &newnode->price);
    printf("Enter stock: ");
    scanf("%d", &newnode->stock);

    if (head == NULL)
    {
        head = tail = newnode;
    }
    else
    {
        tail->next = newnode;
        newnode->prev = tail;
        tail = newnode;
    }
}

void insertProductFromFile()
{

    if (fp == NULL)
    {
        fp = fopen("data.csv", "r");
        if (!fp)
        {
            printf("\nUnable to open data.csv\n");
            return;
        }
        char header[256];
        fgets(header, sizeof(header), fp);
    }

    Product *newnode = create();

    if (fscanf(fp, "%d,%99[^,],%99[^,],%f,%d",
               &newnode->id,
               newnode->category,
               newnode->name,
               &newnode->price,
               &newnode->stock) != 5)
    {
        printf("\nNo more records in file.\n");
        free(newnode);
        fclose(fp);
        fp = NULL;
        return;
    }

    if (head == NULL)
    {
        head = tail = newnode;
    }
    else
    {
        tail->next = newnode;
        newnode->prev = tail;
        tail = newnode;
    }

    printf("\nProduct inserted from file successfully.\n");
}

void reverseDLL()
{
    if (!head || !head->next)
    {
        printf("\nDLL reversed successfully!\n");
        return;
    }
    Product *temp = NULL, *curr = head;
    tail = head;
    while (curr != NULL)
    {
        temp = curr->prev;
        curr->prev = curr->next;
        curr->next = temp;
        curr = curr->prev;
    }
    head = temp->prev;
    printf("\nDLL reversed successfully!\n");
}

Product *searchProduct(int id)
{
    Product *temp = head;
    while (temp != NULL)
    {
        if (temp->id == id)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

void updateProduct(int id)
{
    Product *p = searchProduct(id);
    if (p == NULL)
    {
        printf("\nProduct not found!\n");
    }
    else
    {
        printf("Product Found: %s\n", p->name);
        printf("Enter new price: ");
        scanf("%f", &p->price);
        printf("Enter new stock: ");
        scanf("%d", &p->stock);
        printf("Product updated successfully!\n");
    }
}

void deleteProduct(int id)
{
    Product *temp = head;

    while (temp != NULL && temp->id != id)
        temp = temp->next;

    if (temp == NULL)
    {
        printf("\nProduct not found!\n");
        return;
    }

    if (temp->prev != NULL)
        temp->prev->next = temp->next;
    else
        head = temp->next;

    if (temp->next != NULL)
        temp->next->prev = temp->prev;
    else
        tail = temp->prev;

    free(temp);
    printf("\nProduct deleted successfully.\n");
}

void accept(int *id)
{
    printf("\nEnter Product ID: ");
    scanf("%d", id);
}

int main()
{
    int choice, id;
    do
    {
        printf("\nPRODUCT INVENTORY MENU\n");
        printf("1. Insert Random Product From File\n");
        printf("2. Insert Product\n");
        printf("3. Delete Product\n");
        printf("4. Update Price/Stock\n");
        printf("5. Search Product\n");
        printf("6. Traverse Forward\n");
        printf("7. Traverse Backward\n");
        printf("8. Reverse DLL\n");
        printf("9. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            insertProductFromFile();
            break;
        case 2:
            insertProduct();
            break;
        case 3:
            accept(&id);
            deleteProduct(id);
            break;
        case 4:
            accept(&id);
            updateProduct(id);
            break;
        case 5:
        {
            accept(&id);
            Product *p = searchProduct(id);
            if (p)
            {
                printf("Product found.\n");
                displayEachProduct(p);
            }
            else
                printf("\nProduct not found!\n");
            break;
        }
        case 6:
            traverseForward();
            break;
        case 7:
            traverseBackward();
            break;
        case 8:
            reverseDLL();
            break;
        case 9:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice!\n");
        }
    } while (choice != 9);
    return 0;
}
