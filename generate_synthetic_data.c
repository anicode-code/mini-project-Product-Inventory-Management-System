#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct
{
    int id;
    char name[50];
    char category[10];
    float price;
    int stock;
} Product;

/* Category codes */
const char *categories[] = {
    "ABC", "ACY", "ELC", "GAM", "NET", "OFF"};

#define CAT_COUNT (sizeof(categories) / sizeof(categories[0]))

Product generateProduct(int id)
{
    Product p;

    strcpy(p.category, categories[rand() % CAT_COUNT]);

    p.id = id;

    /* Name = CATEGORY_ID */
    snprintf(p.name, sizeof(p.name), "%s_%d", p.category, p.id);

    p.price = (rand() % 9000 + 1000) / 100.0f; // 10.00 – 100.00
    p.stock = rand() % 200 + 1;                // 1 – 200

    return p;
}

int main()
{
    int n;
    printf("Enter number of products: ");
    scanf("%d", &n);
    srand(time(NULL));

    // printf("\nGenerated Products\n");
    // printf("-----------------------------------------------\n");

    // for (int i = 0; i < n; i++)
    // {
    //     Product p = generateProduct(1001 + i);

    //     printf("ID:%d | Category:%s | Name:%-12s | Price:%.2f | Stock:%d\n",
    //            p.id, p.category, p.name, p.price, p.stock);
    // }

    FILE *fp;
    fp = fopen("data.txt", "w");
    if (!fp)
    {
        printf("Failed to open file!\n");
        return 1;
    }

    fprintf(fp, "ID,Category,Name,Price,Stock\n");

    for (int i = 0; i < n; i++)
    {
        Product p = generateProduct(1001 + i);

        fprintf(fp, "%d,%s,%s,%.2f,%d\n",
                p.id, p.category, p.name, p.price, p.stock);
    }

    fclose(fp);

    printf("\n%d products stored successfully in data.txt\n", n);

    return 0;
}
