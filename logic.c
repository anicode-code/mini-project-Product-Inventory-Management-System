#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#define DATA_FILE "data.json"
#define CHAIN_FILE "blockchain.json"

// --- SHA-256 Implementation ---
typedef struct {
    uint8_t data[64];
    uint32_t datalen;
    uint64_t bitlen;
    uint32_t state[8];
} SHA256_CTX;

#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))
#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

static const uint32_t k[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

void sha256_transform(SHA256_CTX *ctx, const uint8_t data[]) {
    uint32_t a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];
    for (i = 0, j = 0; i < 16; ++i, j += 4)
        m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
    for (; i < 64; ++i)
        m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];
    a = ctx->state[0]; b = ctx->state[1]; c = ctx->state[2]; d = ctx->state[3];
    e = ctx->state[4]; f = ctx->state[5]; g = ctx->state[6]; h = ctx->state[7];
    for (i = 0; i < 64; ++i) {
        t1 = h + EP1(e) + CH(e, f, g) + k[i] + m[i];
        t2 = EP0(a) + MAJ(a, b, c);
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }
    ctx->state[0] += a; ctx->state[1] += b; ctx->state[2] += c; ctx->state[3] += d;
    ctx->state[4] += e; ctx->state[5] += f; ctx->state[6] += g; ctx->state[7] += h;
}

void sha256_init(SHA256_CTX *ctx) {
    ctx->datalen = 0;
    ctx->bitlen = 0;
    ctx->state[0] = 0x6a09e667; ctx->state[1] = 0xbb67ae85; ctx->state[2] = 0x3c6ef372; ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f; ctx->state[5] = 0x9b05688c; ctx->state[6] = 0x1f83d9ab; ctx->state[7] = 0x5be0cd19;
}

void sha256_update(SHA256_CTX *ctx, const uint8_t data[], size_t len) {
    uint32_t i;
    for (i = 0; i < len; ++i) {
        ctx->data[ctx->datalen] = data[i];
        ctx->datalen++;
        if (ctx->datalen == 64) {
            sha256_transform(ctx, ctx->data);
            ctx->bitlen += 512;
            ctx->datalen = 0;
        }
    }
}

void sha256_final(SHA256_CTX *ctx, uint8_t hash[]) {
    uint32_t i = ctx->datalen;
    if (ctx->datalen < 56) {
        ctx->data[i++] = 0x80;
        while (i < 56) ctx->data[i++] = 0x00;
    } else {
        ctx->data[i++] = 0x80;
        while (i < 64) ctx->data[i++] = 0x00;
        sha256_transform(ctx, ctx->data);
        memset(ctx->data, 0, 56);
    }
    ctx->bitlen += ctx->datalen * 8;
    ctx->data[63] = ctx->bitlen;
    ctx->data[62] = ctx->bitlen >> 8;
    ctx->data[61] = ctx->bitlen >> 16;
    ctx->data[60] = ctx->bitlen >> 24;
    ctx->data[59] = ctx->bitlen >> 32;
    ctx->data[58] = ctx->bitlen >> 40;
    ctx->data[57] = ctx->bitlen >> 48;
    ctx->data[56] = ctx->bitlen >> 56;
    sha256_transform(ctx, ctx->data);
    for (i = 0; i < 4; ++i) {
        hash[i] = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 4] = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 8] = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
    }
}

void hash_string(const char *str, char *outputBuffer) {
    unsigned char hash[32];
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (unsigned char*)str, strlen(str));
    sha256_final(&ctx, hash);
    for(int i = 0; i < 32; i++) sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    outputBuffer[64] = 0;
}

// --- Structures ---

typedef struct Product {
    int id;
    char name[100];
    char category[50];
    float price;
    int stock;
    struct Product *prev;
    struct Product *next;
} Product;

typedef struct Block {
    int index;
    char timestamp[30];
    char data[256];
    char prev_hash[65];
    char hash[65];
    char state_hash[65]; 
    int nonce;
    struct Block *next;
} Block;

Product *head = NULL;
Product *tail = NULL;
Block *blockchain_head = NULL;
Block *blockchain_tail = NULL;

// --- Prototypes ---
void load_data();
void save_data();
void add_to_list(int id, char *name, char *category, float price, int stock);
void init_blockchain();
void add_block(char *data);
void save_blockchain();
void load_blockchain();
void calculate_block_hash(Block *b, char *dest);
void calculate_current_state_hash(char *dest);
int check_ledger_integrity(char *buffer_reason);

// --- Helpers ---

void get_timestamp(char *buffer) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, 30, "%Y-%m-%d %H:%M:%S", t);
}

void calculate_current_state_hash(char *dest) {
    SHA256_CTX ctx;
    sha256_init(&ctx);
    if(!head) {
        sha256_update(&ctx, (unsigned char*)"EMPTY", 5);
    } else {
        Product *curr = head;
        while(curr) {
            char buffer[512];
            sprintf(buffer, "%d|%s|%s|%.2f|%d;", curr->id, curr->name, curr->category, curr->price, curr->stock);
            sha256_update(&ctx, (unsigned char*)buffer, strlen(buffer));
            curr = curr->next;
        }
    }
    unsigned char hash[32];
    sha256_final(&ctx, hash);
    for(int i = 0; i < 32; i++) sprintf(dest + (i * 2), "%02x", hash[i]);
    dest[64] = 0;
}

void calculate_block_hash(Block *b, char *dest) {
    char input[1024];
    sprintf(input, "%d%s%s%s%s%d", b->index, b->timestamp, b->data, b->prev_hash, b->state_hash, b->nonce);
    hash_string(input, dest);
}

void init_blockchain() {
    load_blockchain();
    if (!blockchain_head) {
        Block *genesis = (Block *)malloc(sizeof(Block));
        genesis->index = 0;
        get_timestamp(genesis->timestamp);
        strcpy(genesis->data, "Genesis Block");
        strcpy(genesis->prev_hash, "0");
        calculate_current_state_hash(genesis->state_hash);
        genesis->nonce = 0;
        genesis->next = NULL;
        calculate_block_hash(genesis, genesis->hash);
        
        blockchain_head = blockchain_tail = genesis;
        save_blockchain();
    }
}

void add_block(char *data) {
    if (!blockchain_tail) init_blockchain();
    
    Block *new_block = (Block *)malloc(sizeof(Block));
    new_block->index = blockchain_tail->index + 1;
    get_timestamp(new_block->timestamp);
    strcpy(new_block->data, data);
    strcpy(new_block->prev_hash, blockchain_tail->hash);
    
    calculate_current_state_hash(new_block->state_hash);
    
    new_block->nonce = 0; 
    new_block->next = NULL;
    
    calculate_block_hash(new_block, new_block->hash);
    
    blockchain_tail->next = new_block;
    blockchain_tail = new_block;
    
    save_blockchain();
}

void save_blockchain() {
    FILE *fp = fopen(CHAIN_FILE, "w");
    if (!fp) return;
    fprintf(fp, "[\n");
    Block *curr = blockchain_head;
    while(curr) {
        fprintf(fp, "  {\"index\":%d,\"timestamp\":\"%s\",\"data\":\"%s\",\"prev_hash\":\"%s\",\"hash\":\"%s\",\"state_hash\":\"%s\",\"nonce\":%d}",
            curr->index, curr->timestamp, curr->data, curr->prev_hash, curr->hash, curr->state_hash, curr->nonce);
        if(curr->next) fprintf(fp, ",\n");
        else fprintf(fp, "\n");
        curr = curr->next;
    }
    fprintf(fp, "]");
    fclose(fp);
}

void load_blockchain() {
    FILE *fp = fopen(CHAIN_FILE, "r");
    if (!fp) return;
    
    char line[4096]; 
    while(fgets(line, sizeof(line), fp)) {
        if(strstr(line, "\"index\":")) {
            Block *b = (Block *)malloc(sizeof(Block));
            sscanf(strstr(line, "\"index\":") + 8, "%d", &b->index);
            
            char *p;
            p = strstr(line, "\"timestamp\":\""); if(p) sscanf(p+13, "%[^\"]", b->timestamp);
            p = strstr(line, "\"data\":\""); if(p) sscanf(p+8, "%[^\"]", b->data);
            p = strstr(line, "\"prev_hash\":\""); if(p) sscanf(p+13, "%[^\"]", b->prev_hash);
            p = strstr(line, "\"hash\":\""); if(p) sscanf(p+8, "%[^\"]", b->hash);
            p = strstr(line, "\"state_hash\":\""); 
            if(p) sscanf(p+14, "%[^\"]", b->state_hash);
            else strcpy(b->state_hash, "0");
            
            p = strstr(line, "\"nonce\":"); if(p) sscanf(p+8, "%d", &b->nonce);
            
            b->next = NULL;
            if(!blockchain_head){
                blockchain_head = blockchain_tail = b;
            } else {
                blockchain_tail->next = b;
                blockchain_tail = b;
            }
        }
    }
    fclose(fp);
}

// Returns 1 if valid, 0 if invalid. Fills buffer_reason if invalid.
int check_ledger_integrity(char *buffer_reason) {
    if (!blockchain_head) return 1; // Empty chain is arguably valid or needs genesis. treat as valid for now.

    Block *curr = blockchain_head;
    while(curr) {
        char calc_hash[65];
        calculate_block_hash(curr, calc_hash);
        
        if (strcmp(curr->hash, calc_hash) != 0) {
             sprintf(buffer_reason, "Block %d hash mismatch. Chain corrupted.", curr->index);
             return 0;
        }
        
        if (curr->next) {
            if (strcmp(curr->hash, curr->next->prev_hash) != 0) {
                 sprintf(buffer_reason, "Block %d broken link to %d.", curr->index, curr->next->index);
                 return 0;
            }
        }
        
        if (curr->next == NULL) {
            char current_actual_hash[65];
            calculate_current_state_hash(current_actual_hash);
            if (strcmp(curr->state_hash, current_actual_hash) != 0) {
                sprintf(buffer_reason, "Data State Mismatch! data.json tampered.");
                return 0;
            }
        }
        curr = curr->next;
    }
    return 1;
}

void verify_chain() {
    char reason[256];
    if (check_ledger_integrity(reason)) {
        printf("{\"status\":\"valid\", \"message\":\"Blockchain & Data Integrity Verified\"}");
    } else {
        printf("{\"status\":\"invalid\", \"message\":\"%s\"}", reason);
    }
}

void print_chain() {
    printf("[");
    Block *curr = blockchain_head;
    while(curr) {
        printf("{\"index\":%d,\"timestamp\":\"%s\",\"data\":\"%s\",\"prev_hash\":\"%s\",\"hash\":\"%s\",\"state_hash\":\"%s\"}",
            curr->index, curr->timestamp, curr->data, curr->prev_hash, curr->hash, curr->state_hash);
        if(curr->next) printf(",");
        curr = curr->next;
    }
    printf("]");
}

// --- Product Logic ---
void add_to_list(int id, char *name, char *category, float price, int stock) {
    Product *new_node = (Product *)malloc(sizeof(Product));
    new_node->id = id;
    strcpy(new_node->name, name);
    strcpy(new_node->category, category);
    new_node->price = price;
    new_node->stock = stock;
    new_node->prev = NULL;
    new_node->next = NULL;

    if (!head) {
        head = tail = new_node;
    } else {
        tail->next = new_node;
        new_node->prev = tail;
        tail = new_node;
    }
}

void free_list() {
    Product *current = head;
    while (current) {
        Product *next = current->next;
        free(current);
        current = next;
    }
    Block *b = blockchain_head;
    while(b) {
        Block *n = b->next;
        free(b);
        b = n;
    }
}

void load_data() {
    FILE *fp = fopen(DATA_FILE, "r");
    if (!fp) return; 

    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        int id;
        char name[100], category[50];
        float price;
        int stock;
        char *id_ptr = strstr(line, "\"id\":");
        if (id_ptr) {
            sscanf(id_ptr, "\"id\":%d", &id);
            char *p;
            p = strstr(line, "\"name\":\""); if(p) sscanf(p+8, "%[^\"]", name);
            p = strstr(line, "\"category\":\""); if(p) sscanf(p+12, "%[^\"]", category);
            p = strstr(line, "\"price\":"); if(p) sscanf(p+8, "%f", &price);
            p = strstr(line, "\"stock\":"); if(p) sscanf(p+8, "%d", &stock);
            add_to_list(id, name, category, price, stock);
        }
    }
    fclose(fp);
}

void save_data() {
    FILE *fp = fopen(DATA_FILE, "w");
    if (!fp) return;
    fprintf(fp, "[\n");
    Product *curr = head;
    while (curr) {
        fprintf(fp, "  {\"id\":%d,\"name\":\"%s\",\"category\":\"%s\",\"price\":%.2f,\"stock\":%d}", 
                curr->id, curr->name, curr->category, curr->price, curr->stock);
        if (curr->next) fprintf(fp, ",\n");
        else fprintf(fp, "\n");
        curr = curr->next;
    }
    fprintf(fp, "]");
    fclose(fp);
}

// WRAPPERS WITH INTEGRITY CHECKS

void add_product_command(int id, char *name, char *category, float price, int stock) {
    char reason[256];
    if (!check_ledger_integrity(reason)) {
        printf("{\"status\":\"error\", \"message\":\"Blockchain Integrity Failed: %s\"}", reason);
        return;
    }

    Product *curr = head;
    while (curr) {
        if (curr->id == id) {
             printf("{\"status\":\"error\", \"message\":\"Product ID already exists\"}");
             return; 
        }
        curr = curr->next;
    }
    add_to_list(id, name, category, price, stock);
    save_data();
    
    char log[256];
    sprintf(log, "ADDED Product %d (%s) - Stock: %d", id, name, stock);
    add_block(log);
    
    printf("{\"status\":\"success\", \"message\":\"Product added\"}");
}

void update_product_command(int id, float price, int stock) {
    char reason[256];
    if (!check_ledger_integrity(reason)) {
        printf("{\"status\":\"error\", \"message\":\"Blockchain Integrity Failed: %s\"}", reason);
        return;
    }

    Product *curr = head;
    int found = 0;
    while (curr) {
        if (curr->id == id) {
            curr->price = price;
            curr->stock = stock;
            found = 1;
            break;
        }
        curr = curr->next;
    }
    if (found) {
        save_data();
        char log[256];
        sprintf(log, "UPDATED Product %d - Price: %.2f, Stock: %d", id, price, stock);
        add_block(log);
        printf("{\"status\":\"success\", \"message\":\"Product updated\"}");
    } else {
        printf("{\"status\":\"error\", \"message\":\"Product not found\"}");
    }
}

void delete_product_command(int id) {
    char reason[256];
    if (!check_ledger_integrity(reason)) {
        printf("{\"status\":\"error\", \"message\":\"Blockchain Integrity Failed: %s\"}", reason);
        return;
    }

    Product *curr = head;
    int found = 0;
    while (curr) {
        if (curr->id == id) {
            if (curr->prev) curr->prev->next = curr->next;
            else head = curr->next;
            if (curr->next) curr->next->prev = curr->prev;
            else tail = curr->prev;
            free(curr);
            found = 1;
            break;
        }
        curr = curr->next;
    }
    if (found) {
        save_data();
        char log[256];
        sprintf(log, "DELETED Product %d", id);
        add_block(log);
        printf("{\"status\":\"success\", \"message\":\"Product deleted\"}");
    } else {
        printf("{\"status\":\"error\", \"message\":\"Product not found\"}");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    load_data();
    init_blockchain();

    if (strcmp(argv[1], "read") == 0) {
        printf("[");
        Product *curr = head;
        while (curr) {
           printf("{\"id\":%d,\"name\":\"%s\",\"category\":\"%s\",\"price\":%.2f,\"stock\":%d}", 
               curr->id, curr->name, curr->category, curr->price, curr->stock);
           if(curr->next) printf(",");
           curr = curr->next;
        }
        printf("]");
    } else if (strcmp(argv[1], "read_reverse") == 0) {
        printf("[");
        Product *curr = tail;
        while (curr) {
           printf("{\"id\":%d,\"name\":\"%s\",\"category\":\"%s\",\"price\":%.2f,\"stock\":%d}", 
               curr->id, curr->name, curr->category, curr->price, curr->stock);
           if(curr->prev) printf(",");
           curr = curr->prev;
        }
        printf("]");
    } else if (strcmp(argv[1], "add") == 0 && argc == 7) {
        add_product_command(atoi(argv[2]), argv[3], argv[4], atof(argv[5]), atoi(argv[6]));
    } else if (strcmp(argv[1], "update") == 0 && argc == 5) {
        update_product_command(atoi(argv[2]), atof(argv[3]), atoi(argv[4]));
    } else if (strcmp(argv[1], "delete") == 0 && argc == 3) {
        delete_product_command(atoi(argv[2]));
    } else if (strcmp(argv[1], "search") == 0 && argc == 3) {
         printf("[");
        Product *curr = head;
        int first = 1;
        while (curr) {
            if (strstr(curr->name, argv[2]) || strstr(curr->category, argv[2])) {
                if (!first) printf(",");
                printf("{\"id\":%d,\"name\":\"%s\",\"category\":\"%s\",\"price\":%.2f,\"stock\":%d}", 
               curr->id, curr->name, curr->category, curr->price, curr->stock);
                first = 0;
            }
            curr = curr->next;
        }
        printf("]");
    } else if (strcmp(argv[1], "view_chain") == 0) {
        print_chain();
    } else if (strcmp(argv[1], "verify_chain") == 0) {
        verify_chain();
    }

    free_list();
    return 0;
}
