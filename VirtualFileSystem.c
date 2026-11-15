/**
 * FILENAME: VirtualFileSystem.c
 * DESCRIPTION: Build a Virtual File System (VFS) that simulates an in-memory file storage environment.
 * AUTHOR: Nancy Jain
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BLOCK_SIZE 512
#define MAX_BLOCKS 5000
#define DEFAULT_BLOCKS 1024
#define MAX_NAME 50
#define PROMPT_FULL_PATH 0

typedef enum{ 
    FILE_TYPE, 
    DIR_TYPE 
}NodeType;

typedef struct FreeBlock{
    int index;
    struct FreeBlock *next, *prev;
}FreeBlock;

typedef struct FileNode{
    char name[MAX_NAME + 1];
    NodeType type;
    struct FileNode *parent;
    struct FileNode *next, *prev;
    struct FileNode *children;
    int *blockPointers;
    int blockCount;
    size_t contentSize;
}FileNode;

static unsigned char virtualDisk[MAX_BLOCKS][BLOCK_SIZE];
static FreeBlock *freeHead = NULL, *freeTail = NULL;
static int numBlocks = DEFAULT_BLOCKS;
static int freeCount = 0;
static FileNode *root = NULL;
static FileNode *cwd = NULL;

static void addFreeBlock(int index){
    FreeBlock *b = malloc(sizeof(FreeBlock));
    if(!b){ 
        fprintf(stderr, "Out of memory\n"); 
        exit(EXIT_FAILURE); 
    }
    b->index = index;
    b->next = NULL;
    b->prev = freeTail;
    if (freeTail) freeTail->next = b; else freeHead = b;
    freeTail = b;
    freeCount++;
}

static int allocateBlock(void){
    if(!freeHead) return -1;
    FreeBlock *b = freeHead;
    int index = b->index;
    freeHead = b->next;
    if (freeHead) freeHead->prev = NULL; else freeTail = NULL;
    free(b);
    freeCount--;
    return index;
}

static void releaseBlock(int index){ 
    addFreeBlock(index); 
}

static FileNode *createNode(const char *name, NodeType type){
    FileNode *n = calloc(1, sizeof(FileNode));
    if(!n){ 
        fprintf(stderr, "Out of memory\n"); 
        exit(EXIT_FAILURE); 
    }
    strncpy(n->name, name, MAX_NAME);
    n->name[MAX_NAME] = '\0';
    n->type = type;
    return n;
}

static void insertChild(FileNode *parent, FileNode *node){
    node->parent = parent;
    if(!parent->children){
        node->next = node->prev = node;
        parent->children = node;
    }else{
        FileNode *head = parent->children, *tail = head->prev;
        tail->next = node;
        node->prev = tail;
        node->next = head;
        head->prev = node;
    }
}

static void removeChild(FileNode *parent, FileNode *node){
    if(node->next == node){
        parent->children = NULL;
    }else{
        node->prev->next = node->next;
        node->next->prev = node->prev;
        if(parent->children == node) parent->children = node->next;
    }
    node->next = node->prev = NULL;
    node->parent = NULL;
}

static FileNode *findChild(FileNode *parent, const char *name){
    if(!parent->children) return NULL;
    FileNode *cur = parent->children;
    do{
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }while (cur != parent->children);
    return NULL;
}

static void releaseFileBlocks(FileNode *f){
    for(int i = 0; i < f->blockCount; i++) releaseBlock(f->blockPointers[i]);
    free(f->blockPointers);
    f->blockPointers = NULL;
    f->blockCount = 0;
    f->contentSize = 0;
}

static void freeTree(FileNode *n){
    if(!n) return;
    if(n->type == DIR_TYPE && n->children){
        FileNode *cur = n->children;
        cur->prev->next = NULL;
        while(cur){
            FileNode *nx = cur->next;
            freeTree(cur);
            cur = nx;
        }
    }
    free(n->blockPointers);
    free(n);
}

static void buildPath(FileNode *n, char *out){
    if(n == root){ 
        out[0] = '\0'; 
        return; 
    }
    buildPath(n->parent, out);
    strcat(out, "/");
    strcat(out, n->name);
}

static void displayPrompt(void){
#if PROMPT_FULL_PATH
    if(cwd == root) printf("/ > ");
    else{
        char path[4096] = "";
        buildPath(cwd, path);
        printf("%s > ", path);
    }
#else
    if(cwd == root) printf("/ > ");
    else printf("%s > ", cwd->name);
#endif
    fflush(stdout);
}

static int validName(const char *name, const char *what){
    if(!name || !*name){ 
        printf("Error: %s name required.\n", what); 
        return 0; 
    }
    if(strlen(name) > MAX_NAME){
        printf("Error: name too long (max %d characters).\n", MAX_NAME);
        return 0;
    }
    if(strchr(name, '/') || strchr(name, ' ') || strchr(name, '\t')){
        printf("Error: name cannot contain '/' or spaces.\n");
        return 0;
    }
    if(!strcmp(name, ".") || !strcmp(name, "..")){
        printf("Error: invalid name.\n");
        return 0;
    }
    return 1;
}

static void makeDirectory(const char *name){
    if(!validName(name, "directory")) return;
    if(findChild(cwd, name)){ 
        printf("Error: '%s' already exists.\n", name); 
        return; 
    }
    insertChild(cwd, createNode(name, DIR_TYPE));
    printf("Directory '%s' created successfully.\n", name);
}

static void createFile(const char *name){
    if(!validName(name, "file")) return;
    if(findChild(cwd, name)) { 
        printf("Error: '%s' already exists.\n", name); 
        return; 
    }
    insertChild(cwd, createNode(name, FILE_TYPE));
    printf("File '%s' created successfully.\n", name);
}

static void writeFile(const char *name, const char *content, size_t size){
    FileNode *f = findChild(cwd, name);
    if(!f){ 
        printf("Error: file '%s' not found.\n", name); 
        return; 
    }
    if(f->type != FILE_TYPE){ 
        printf("Error: '%s' is a directory.\n", name); 
        return; 
    }
    int required = (int)((size + BLOCK_SIZE - 1) / BLOCK_SIZE);
    int available = freeCount + f->blockCount;
    if(required > available){
        printf("Error: disk full. Need %d blocks, only %d available.\n",
               required, available);
        return;
    }
    releaseFileBlocks(f);
    if(required > 0){
        f->blockPointers = malloc(sizeof(int) * required);
        if (!f->blockPointers) { fprintf(stderr, "Out of memory\n"); exit(EXIT_FAILURE); }
    }
    size_t written = 0;
    for(int i = 0; i < required; i++){
        int block = allocateBlock();
        f->blockPointers[i] = block;
        f->blockCount++;
        size_t remaining = size - written;
        size_t bytes = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
        memset(virtualDisk[block], 0, BLOCK_SIZE);
        memcpy(virtualDisk[block], content + written, bytes);
        written += bytes;
    }
    f->contentSize = size;
    printf("Data written successfully (size=%zu bytes).\n", size);
}

static void readFile(const char *name){
    if(!name || !*name){ 
        printf("Usage: read <file>\n"); 
        return; 
    }
    FileNode *f = findChild(cwd, name);
    if(!f){ 
        printf("Error: file '%s' not found.\n", name); 
        return; 
    }
    if(f->type != FILE_TYPE){ 
        printf("Error: '%s' is a directory.\n", name); 
        return; 
    }
    if(f->contentSize == 0){ 
        printf("(empty)\n"); 
        return; 
    }
    size_t remaining = f->contentSize;
    for(int i = 0; i < f->blockCount && remaining > 0; i++){
        size_t bytes = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
        fwrite(virtualDisk[f->blockPointers[i]], 1, bytes, stdout);
        remaining -= bytes;
    }
    printf("\n");
}

static void deleteFile(const char *name){
    if(!name || !*name){ 
        printf("Usage: delete <file>\n"); 
        return; 
    }
    FileNode *f = findChild(cwd, name);
    if(!f){ 
        printf("Error: file '%s' not found.\n", name); 
        return; 
    }
    if(f->type != FILE_TYPE){
        printf("Error: '%s' is a directory. Use rmdir.\n", name);
        return;
    }
    releaseFileBlocks(f);
    removeChild(cwd, f);
    free(f);
    printf("File deleted successfully.\n");
}

static void removeDirectory(const char *name){
    if(!name || !*name){ 
        printf("Usage: rmdir <directory>\n"); 
        return; 
    }
    FileNode *d = findChild(cwd, name);
    if(!d){ 
        printf("Error: directory '%s' not found.\n", name); 
        return; 
    }
    if(d->type != DIR_TYPE){
        printf("Error: '%s' is a file. Use delete.\n", name);
        return;
    }
    if(d->children){ 
        printf("Error: directory '%s' is not empty.\n", name); 
        return; 
    }
    removeChild(cwd, d);
    free(d);
    printf("Directory removed successfully.\n");
}

static void listDirectory(void){
    if(!cwd->children){ 
        printf("(empty)\n"); 
        return; 
    }
    FileNode *cur = cwd->children;
    do{
        printf("%s%s\n", cur->name, cur->type == DIR_TYPE ? "/" : "");
        cur = cur->next;
    }while(cur != cwd->children);
}

static void changeDirectory(const char *name){
    if(!name || !*name){ 
        printf("Usage: cd <directory>\n"); 
        return; 
    }
    if(!strcmp(name, "/")) cwd = root;
    else if(!strcmp(name, "..")) {
        if(cwd == root){ 
            printf("Already at root.\n"); 
            return; 
        }
        cwd = cwd->parent;
    }else if(!strcmp(name, ".")) {
    }else{
        FileNode *d = findChild(cwd, name);
        if(!d){ printf("Error: directory '%s' not found.\n", name); return; }
        if(d->type != DIR_TYPE){ printf("Error: '%s' is not a directory.\n", name); return; }
        cwd = d;
    }
    char path[4096] = "";
    buildPath(cwd, path);
    printf("Moved to %s\n", cwd == root ? "/" : path);
}

static void printWorkingDirectory(void){
    char path[4096] = "";
    buildPath(cwd, path);
    printf("%s\n", cwd == root ? "/" : path);
}

static void diskUsage(void){
    int used = numBlocks - freeCount;
    printf("Total Blocks: %d\n", numBlocks);
    printf("Used Blocks: %d\n", used);
    printf("Free Blocks: %d\n", freeCount);
    printf("Disk Usage: %.2f%%\n", 100.0 * used / numBlocks);
}

static void exitSystem(void){
    freeTree(root);
    while(freeHead){
        FreeBlock *n = freeHead->next;
        free(freeHead);
        freeHead = n;
    }
    freeTail = NULL;
    root = cwd = NULL;
    printf("Memory released. Exiting program...\n");
}

static char *skipSpaces(char *p){
    while(*p && isspace((unsigned char)*p)) p++;
    return p;
}

static char *nextToken(char **p){
    char *s = skipSpaces(*p);
    if(!*s){ 
        *p = s; 
        return NULL; 
    }
    char *e = s;
    while(*e && !isspace((unsigned char)*e)) e++;
    if(*e) *e++ = '\0';
    *p = e;
    return s;
}

static char *restOfLine(char *p){ return skipSpaces(p); }

static void handleWrite(char *rest){
    char *name = nextToken(&rest);
    if(!name){ 
        printf("Usage: write <file> \"content\"\n"); 
        return; 
    }
    char *data = restOfLine(rest);
    size_t len = strlen(data);
    if(len >= 2 && (data[0] == '"' || data[0] == '\'') && data[len - 1] == data[0]){
        data[len - 1] = '\0';
        data++;
        len -= 2;
    }
    writeFile(name, data, len);
}

int main(int argc, char **argv){
    if(argc > 1){
        int n = atoi(argv[1]);
        if(n < 1 || n > MAX_BLOCKS){
            fprintf(stderr, "NUM_BLOCKS must be between 1 and %d\n", MAX_BLOCKS);
            return 1;
        }
        numBlocks = n;
    }
    for(int i = 0; i < numBlocks; i++) addFreeBlock(i);
    root = createNode("/", DIR_TYPE);
    cwd = root;
    size_t bufSize = (size_t)numBlocks * BLOCK_SIZE + 256;
    char *input = malloc(bufSize);
    if(!input){ fprintf(stderr, "Out of memory\n"); return 1; }
    printf("Compact VFS - ready. Type 'exit' to quit.\n");
    for(;;){
        displayPrompt();
        if(!fgets(input, (int)bufSize, stdin)){ 
            printf("\n"); 
            break; 
        }
        size_t len = strlen(input);
        if(len > 0 && input[len - 1] != '\n' && !feof(stdin)){
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Error: input line too long.\n");
            continue;
        }
        while(len > 0 && isspace((unsigned char)input[len - 1])) input[--len] = '\0';
        char *rest = input;
        char *cmd = nextToken(&rest);
        if(!cmd) continue;
        if(!strcmp(cmd, "exit")) break;
        else if(!strcmp(cmd, "write"))  handleWrite(rest);
        else if(!strcmp(cmd, "mkdir"))  makeDirectory(restOfLine(rest));
        else if(!strcmp(cmd, "create")) createFile(restOfLine(rest));
        else if(!strcmp(cmd, "read"))   readFile(restOfLine(rest));
        else if(!strcmp(cmd, "delete")) deleteFile(restOfLine(rest));
        else if(!strcmp(cmd, "rmdir"))  removeDirectory(restOfLine(rest));
        else if(!strcmp(cmd, "cd"))     changeDirectory(restOfLine(rest));
        else if(!strcmp(cmd, "ls"))     listDirectory();
        else if(!strcmp(cmd, "pwd"))    printWorkingDirectory();
        else if(!strcmp(cmd, "df"))     diskUsage();
        else{
            printf("Unknown command: %s\n", cmd);
            printf("Available commands: mkdir, create, write, read, delete, rmdir, ls, cd, pwd, df, exit\n");
        }
    }
    free(input);
    exitSystem();
    return 0;
}