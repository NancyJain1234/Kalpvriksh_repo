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

typedef struct BlockList{
    FreeBlock *head, *tail;
    int count;
}BlockList;

typedef struct FileNode{
    char name[MAX_NAME + 1];
    NodeType type;
    struct FileNode *parent;
    struct FileNode *next, *prev;
    struct FileNode *children;
    BlockList blocks;
    size_t contentSize;
}FileNode;

static unsigned char virtualDisk[MAX_BLOCKS][BLOCK_SIZE];
static BlockList freeList = { NULL, NULL, 0 };
static int numBlocks = DEFAULT_BLOCKS;
static FileNode *root = NULL;
static FileNode *cwd = NULL;

static void listPush(BlockList *l, FreeBlock *b){
    b->next = NULL;
    b->prev = l->tail;
    if(l->tail) l->tail->next = b;
    else l->head = b;
    l->tail = b;
    l->count++;
}

static FreeBlock *listPop(BlockList *l){
    FreeBlock *b = l->head;
    if(!b) return NULL;
    l->head = b->next;
    if(l->head) l->head->prev = NULL;
    else l->tail = NULL;
    b->next = b->prev = NULL;
    l->count--;
    return b;
}

static void freeBlockList(BlockList *l){
    FreeBlock *b;
    while((b = listPop(l))) free(b);
}

static int addFreeBlock(int index){
    FreeBlock *b = malloc(sizeof(FreeBlock));
    if(!b) return 0;
    b->index = index;
    listPush(&freeList , b);
    return 1;
}

static FileNode *createNode(const char *name, NodeType type){
    FileNode *n = calloc(1, sizeof(FileNode));
    if(!n) return NULL;
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
    FreeBlock *b;
    while((b = listPop(&f->blocks))) listPush(&freeList, b);
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
    freeBlockList(&n->blocks);
    free(n);
}

static void printPath(const FileNode *n){
    if(!n->parent){
        putchar('/');
        return;
    }
    if(n->parent->parent) printPath(n->parent);
    printf("/%s", n->name);
}

static void displayPrompt(void){
#if PROMPT_FULL_PATH
    printPath(cwd);
    printf(" > ");
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

static void addNode(const char *name, NodeType type){
    const char *what = type == DIR_TYPE ? "directory" : "file";
    if(!validName(name, what)) return;
    if(findChild(cwd, name)){ 
        printf("Error: '%s' already exists.\n", name); 
        return; 
    }
    FileNode *n = createNode(name, type);
    if(!n){
        printf("Error: out of memory, %s not created.\n", what);
        return;
    }
    insertChild(cwd, n);
    printf("%s '%s' created successfully.\n", type == DIR_TYPE ? "Directory" : "File", name);
}

static void makeDirectory(const char *name){ 
    addNode(name, DIR_TYPE); 
}

static void createFile(const char *name){ 
    addNode(name, FILE_TYPE);
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
    int available = freeList.count + f->blocks.count;
    if(required > available){
        printf("Error: disk full. Need %d blocks, only %d available.\n", required, available);
        return;
    }
    releaseFileBlocks(f);
    size_t written = 0;
    for(int i = 0; i < required; i++){
        FreeBlock *b = listPop(&freeList);
        listPush(&f->blocks, b);
        size_t remaining = size - written;
        size_t bytes = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
        memset(virtualDisk[b->index], 0, BLOCK_SIZE);
        memcpy(virtualDisk[b->index], content + written, bytes);
        written += bytes;
    }
    f->contentSize = size;
    printf("Data written successfully (size=%zu bytes).\n", size);
}

static void readFile(const char *name){
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
    for(FreeBlock *b = f->blocks.head; b && remaining > 0; b = b->next){
        size_t bytes = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
        fwrite(virtualDisk[b->index], 1, bytes, stdout);
        remaining -= bytes;
    }
    printf("\n");
}

static void deleteFile(const char *name){
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
    if(!strcmp(name, "/")) cwd = root;
    else if(!strcmp(name, "..")){
        if(cwd == root){ 
            printf("Already at root.\n"); 
            return; 
        }
        cwd = cwd->parent;
    }else if(!strcmp(name, ".")){
    }else{
        FileNode *d = findChild(cwd, name);
        if(!d){ 
            printf("Error: directory '%s' not found.\n", name); 
            return; 
        }
        if(d->type != DIR_TYPE){ 
            printf("Error: '%s' is not a directory.\n", name); 
            return; 
        }
        cwd = d;
    }
    printf("Moved to ");
    printPath(cwd);
    printf("\n");
}

static void printWorkingDirectory(void){
    printPath(cwd);
    printf("\n");
}

static void diskUsage(void){
    int used = numBlocks - freeList.count;
    printf("Total Blocks: %d\n", numBlocks);
    printf("Used Blocks: %d\n", used);
    printf("Free Blocks: %d\n", freeList.count);
    printf("Disk Usage: %.2f%%\n", 100.0 * used / numBlocks);
}

static void releaseAll(void){
    freeTree(root);
    freeBlockList(&freeList);
    root = cwd = NULL;
}

static void exitSystem(void){
    releaseAll();
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

static int parseOneArg(char *rest, const char *usage, char **arg){
    char *a = nextToken(&rest);
    if(!a || nextToken(&rest)){
        printf("%s\n", usage);
        return 0;
    }
    *arg = a;
    return 1;
}

static int parseNoArgs(char *rest, const char *usage){
    if(nextToken(&rest)){
        printf("%s\n", usage);
        return 0;
    }
    return 1;
}

static void handleWrite(char *rest){
    static const char *usage = "Usage: write <file> \"content\"";
    char *name = nextToken(&rest);
    if(!name){ 
        printf("%s\n", usage); 
        return; 
    }
    char *data = skipSpaces(rest);
    size_t len = strlen(data);
    char q = data[0];
    if(len < 2 || (q != '"' && q != '\'') || data[len - 1] != q || memchr(data + 1, q, len - 2)){
        printf("Error: content must be fully enclosed in matching quotes.\n");
        printf("%s\n", usage);
        return;
    }
    data[len - 1] = '\0';
    data++;
    len -= 2;
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
    for(int i = 0; i < numBlocks; i++){
        if(!addFreeBlock(i)){
            fprintf(stderr , "Out of memory\n");
            releaseAll();
            return 1;
        }
    }
    root = createNode("/", DIR_TYPE);
    if(!root){
        fprintf(stderr , "Out of memory\n");
        releaseAll();
        return 1;
    }
    cwd = root;
    size_t bufSize = (size_t)numBlocks * BLOCK_SIZE + 256;
    char *input = malloc(bufSize);
    if(!input){ 
        fprintf(stderr, "Out of memory\n");
        releaseAll();
        return 1;
    }
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
        char *arg;
        if(!cmd) continue;
        if(!strcmp(cmd, "exit")){
            if(parseNoArgs(rest, "Usage: exit")) break;
        }else if(!strcmp(cmd, "write")){
            handleWrite(rest);
        }else if(!strcmp(cmd, "mkdir")){
            if(parseOneArg(rest, "Usage: mkdir <directory>", &arg)) makeDirectory(arg);
        }else if(!strcmp(cmd, "create")){
            if(parseOneArg(rest, "Usage: create <file>", &arg)) createFile(arg);
        }else if(!strcmp(cmd, "read")){
            if(parseOneArg(rest, "Usage: read <file>", &arg)) readFile(arg);
        }else if(!strcmp(cmd, "delete")){
            if(parseOneArg(rest, "Usage: delete <file>", &arg)) deleteFile(arg);
        }else if(!strcmp(cmd, "rmdir")){
            if(parseOneArg(rest, "Usage: rmdir <directory>", &arg)) removeDirectory(arg);
        }else if(!strcmp(cmd, "cd")){
            if(parseOneArg(rest, "Usage: cd <directory>", &arg)) changeDirectory(arg);
        }else if(!strcmp(cmd, "ls")){
            if(parseNoArgs(rest, "Usage: ls")) listDirectory();
        }else if(!strcmp(cmd, "pwd")){
            if(parseNoArgs(rest, "Usage: pwd")) printWorkingDirectory();
        }else if(!strcmp(cmd, "df")){
            if(parseNoArgs(rest, "Usage: df")) diskUsage();
        }else{
            printf("Unknown command: %s\n", cmd);
            printf("Available commands: mkdir, create, write, read, delete, rmdir, ls, cd, pwd, df, exit\n");
        }
    }
    free(input);
    exitSystem();
    return 0;
}