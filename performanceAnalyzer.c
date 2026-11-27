/**
 * FILENAME: performanceAnalyzer.c
 * DESCRIPTION: Design and implement a data management and analysis system for ICC ODI player statistics that organizes and processes data using Searching, Sorting and Linked list concepts.
 * AUTHOR: Nancy Jain
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "Players_data.h"

#define TEAMCOUNT 10
#define MIN_PLAYERID 1
#define MAX_PLAYERID 1000
#define MIN_PLAYER 11
#define MAX_PLAYER 50
#define NAME_LENGTH 51

typedef enum Role{
    ROLE_INVALID = 0,
    BATSMAN,
    BOWLER,
    ALL_ROUNDER
} Role;

typedef struct ODIPlayer{
    int PlayerID;
    char Name[NAME_LENGTH];
    char TeamName[NAME_LENGTH];
    Role Role;
    int TotalRuns;
    float BattingAverage;
    float StrikeRate;
    int Wickets;
    float EconomyRate;
    float PerformanceIndex;
    struct ODIPlayer* next;
    struct ODIPlayer* roleNext;
} ODIPlayer;

typedef struct Team{
    int TeamID;
    char Name[NAME_LENGTH];
    int TotalPlayers;
    float AverageBattingStrikeRate;
    ODIPlayer *head;
    ODIPlayer *roleHead[4];
} Team;

typedef struct HeapNode{
    ODIPlayer* player;
    int teamIndex;
} HeapNode;

typedef struct MaxHeap{
    HeapNode arr[TEAMCOUNT];
    int size;
} MaxHeap;

static int readLine(char *buffer , size_t size){
    if(fgets(buffer , (int)size , stdin) == NULL) return 0;
    size_t len = strlen(buffer);
    if(len > 0 && buffer[len - 1] == '\n'){
        buffer[len - 1] = '\0';
    }else if(!feof(stdin)){
        int ch;
        while((ch = getchar()) != '\n' && ch != EOF){}
        return -1;
    }
    return 1;
}

static void trim(char *text){
    char *start = text;
    while(isspace((unsigned char)*start)) start++;
    if(start != text) memmove(text , start , strlen(start) + 1);
    size_t len = strlen(text);
    while(len > 0 && isspace((unsigned char)text[len - 1])) text[--len] = '\0';
}

static int readInt(const char *prompt , int *out){
    char buffer[128];
    printf("%s" , prompt);
    fflush(stdout);
    int rc = readLine(buffer , sizeof(buffer));
    if(rc == 0) return -1;
    if(rc < 0) return 0;
    trim(buffer);
    char *end;
    long value = strtol(buffer , &end , 10);
    if(end == buffer || *end != '\0' || value != (int)value) return 0;
    *out = (int)value;
    return 1;
}

static int readFloat(const char *prompt , float *out){
    char buffer[128];
    printf("%s" , prompt);
    fflush(stdout);
    int rc = readLine(buffer , sizeof(buffer));
    if(rc == 0) return -1;
    if(rc < 0) return 0;
    trim(buffer);
    char *end;
    float value = strtof(buffer , &end);
    if(end == buffer || *end != '\0' || !isfinite(value)) return 0;
    *out = value;
    return 1;
}

static int readName(const char *prompt , char *out){
    char buffer[128];
    printf("%s" , prompt);
    fflush(stdout);
    int rc = readLine(buffer , sizeof(buffer));
    if(rc == 0) return -1;
    if(rc < 0) return 0;
    trim(buffer);
    if(buffer[0] == '\0' || strlen(buffer) >= NAME_LENGTH) return 0;
    strcpy(out , buffer);
    return 1;
}

const char* roleName(Role role , int plural){
    switch(role){
        case BATSMAN: return plural ? "Batsmen" : "Batsman";
        case BOWLER:  return plural ? "Bowlers" : "Bowler";
        case ALL_ROUNDER: return plural ? "All-rounders" : "All-rounder";
        default:  return "Unknown";
    }
}

void swapHeapNodes(HeapNode* a , HeapNode* b){
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void insertHeap(MaxHeap *heap , HeapNode node){
    int i = heap->size;
    heap->arr[i] = node;
    heap->size++;
    while(i > 0){
        int parent = (i - 1)/2;
        if(heap->arr[i].player->PerformanceIndex >heap->arr[parent].player->PerformanceIndex){
            swapHeapNodes(&heap->arr[i] , &heap->arr[parent]);
            i = parent;
        }else{
            break;
        }
    }
}

HeapNode extractMax(MaxHeap *heap){
    HeapNode maxNode = heap->arr[0];
    heap->arr[0] = heap->arr[heap->size - 1];
    heap->size--;
    int i = 0;
    while(1){
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int largest = i;
        if(left < heap->size && heap->arr[left].player->PerformanceIndex > heap->arr[largest].player->PerformanceIndex){
            largest = left;
        }
        if(right < heap->size && heap->arr[right].player->PerformanceIndex > heap->arr[largest].player->PerformanceIndex){
            largest = right;
        }
        if(largest == i) break;
        swapHeapNodes(&heap->arr[i] , &heap->arr[largest]);
        i = largest;
    }
    return maxNode;
}

float PerformanceIndex(ODIPlayer* p){
    if(p->Role == BATSMAN)      return (p->BattingAverage * p->StrikeRate) / 100.0;
    else if(p->Role == BOWLER) return (p->Wickets * 2) + (100 - p->EconomyRate);
    else if(p->Role == ALL_ROUNDER) return ((p->BattingAverage * p->StrikeRate) / 100.0) + (p->Wickets * 2);
    return 0;
}

int findTeamByName(Team teamList[] , const char* name){
    for(int i = 0 ; i < TEAMCOUNT ; i++){
        if(strcmp(teamList[i].Name , name) == 0) return i;
    }
    return -1;
}

int findTeamByID(Team teamList[] , int id){
    int low = 0 , high = TEAMCOUNT - 1;
    while(low <= high){
        int mid = low + (high - low)/2;
        if(teamList[mid].TeamID == id) return mid;
        else if(teamList[mid].TeamID < id) low = mid + 1;
        else high = mid -1;
    }
    return -1;
}

void initializeTeams(Team teamList[]){
    for(int i = 0 ; i < TEAMCOUNT ; i++){
        teamList[i].TeamID = i + 1;
        strcpy(teamList[i].Name , teams[i]);
        teamList[i].TotalPlayers = 0;
        teamList[i].AverageBattingStrikeRate = 0;
        teamList[i].head = NULL;
        for(int j = 0; j < 4; j++){
            teamList[i].roleHead[j] = NULL;
        }
    }
}

int getRole(const char* role){
    if(strcmp(role , "Batsman") == 0)         return BATSMAN;
    else if(strcmp(role , "Bowler") == 0)     return BOWLER;
    else if(strcmp(role, "All-rounder") == 0) return ALL_ROUNDER;
    return ROLE_INVALID;
}

void calculateTeamAverage(Team *team){
    float sum = 0;
    int count = 0;
    ODIPlayer* temp = team->head;
    while(temp){
        if(temp->Role == BATSMAN || temp->Role == ALL_ROUNDER){
            sum += temp->StrikeRate;
            count++;
        }
        temp = temp->next;
    }
    if(count > 0) team->AverageBattingStrikeRate = sum / count;
    else team->AverageBattingStrikeRate = 0;
}

void insertRoleSorted(Team *team , ODIPlayer* player){
    int role = player->Role;
    ODIPlayer **current = &team->roleHead[role];
    while(*current != NULL && (*current)->PerformanceIndex >= player->PerformanceIndex){
        current = &(*current)->roleNext;
    }
    player->roleNext = *current;
    *current = player;
}

void insertPlayer(Team *team , ODIPlayer *player){
    strcpy(player->TeamName , team->Name);
    player->PerformanceIndex = PerformanceIndex(player);
    player->next = team->head;
    team->head = player;
    insertRoleSorted(team , player);
    team->TotalPlayers++;
}

void freeAllPlayers(Team teamList[]){
    for(int i = 0 ; i < TEAMCOUNT ; i++){
        ODIPlayer* temp = teamList[i].head;
        while(temp){
            ODIPlayer* next = temp->next;
            free(temp);
            temp = next;
        }
        teamList[i].head = NULL;
        for(int j = 0 ; j < 4 ; j++) teamList[i].roleHead[j] = NULL;
        teamList[i].TotalPlayers = 0;
    }
}

int addPlayer(Team teamList[] , int TeamID){
    int index = findTeamByID(teamList , TeamID);
    if(index == -1){
        printf("Team not found.\n");
        return 0;
    }
    if(teamList[index].TotalPlayers >= MAX_PLAYER){
        printf("Team already has maximum players.\n");
        return 0;
    }
    ODIPlayer p;
    int role , rc;
    printf("Enter Player Details:\n");

    rc = readInt("Player ID: " , &p.PlayerID);
    if(rc != 1){
        if(rc == 0) printf("Invalid Player ID.\n");
        return rc;
    }
    if(p.PlayerID < MIN_PLAYERID || p.PlayerID > MAX_PLAYERID){
        printf("Player ID must be between 1 and 1000.\n");
        return 0;
    }
    for(int i = 0 ; i < TEAMCOUNT ; i++){
        ODIPlayer* temp = teamList[i].head;
        while(temp){
            if(temp->PlayerID == p.PlayerID){
                printf("Player ID already exists.\n");
                return 0;
            }
            temp = temp->next;
        }
    }
    rc = readName("Name: " , p.Name);
    if(rc != 1){
        if(rc == 0) printf("Invalid name.\n");
        return rc;
    }
    rc = readInt("Role (1-Batsman, 2-Bowler, 3-All-rounder): " , &role);
    if(rc != 1 || role < BATSMAN || role > ALL_ROUNDER){
        if(rc == -1) return -1;
        printf("Invalid role.\n");
        return 0;
    }
    p.Role = (Role)role;

    rc =readInt("Total Runs: " , &p.TotalRuns);
    if(rc != 1 || p.TotalRuns < 0){
        if(rc == -1) return -1;
        printf("Invalid total runs.\n");
        return 0;
    }
    rc = readFloat("Batting Average: " , &p.BattingAverage);
    if(rc != 1 || p.BattingAverage < 0){
        if(rc == -1) return -1;
        printf("Invalid batting average.\n");
        return 0;
    }
    rc = readFloat("Strike Rate: " , &p.StrikeRate);
    if(rc != 1 || p.StrikeRate < 0){
        if(rc == -1) return -1;
        printf("Invalid strike rate.\n");
        return 0;
    }
    rc = readInt("Wickets: " , &p.Wickets);
    if(rc != 1 || p.Wickets < 0){
        if(rc == -1) return -1;
        printf("Invalid wickets.\n");
        return 0;
    }
    rc = readFloat("Economy Rate: " , &p.EconomyRate);
    if(rc != 1 || p.EconomyRate < 0){
        if(rc == -1) return -1;
        printf("Invalid economy rate.\n");
        return 0;
    }
    ODIPlayer* newplayer = malloc(sizeof(ODIPlayer));
    if(!newplayer){
        printf("Dynamic memory allocation failed.\n");
        return 0;
    }
    *newplayer = p;
    insertPlayer(&teamList[index] , newplayer);
    calculateTeamAverage(&teamList[index]);
    printf("\nPlayer added successfully to Team %s!\n", teamList[index].Name);
    return 1;
}

void initializePlayers(Team teamList[]){
    for(int i = 0 ; i < playerCount ; i++){
        int index = findTeamByName(teamList , players[i].team);
        if(index == -1){
            printf("Team not found.\n");
            continue;
        }
        ODIPlayer* newnode = malloc(sizeof(ODIPlayer));
        if(newnode == NULL){
            printf("Dynamic memory allocation failed.\n");
            return;
        }
        newnode->PlayerID = players[i].id;
        strcpy(newnode->Name, players[i].name);
        newnode->Role = getRole(players[i].role);
        if(newnode->Role == ROLE_INVALID){
            free(newnode);
            continue;
        }
        newnode->TotalRuns = players[i].totalRuns;
        newnode->BattingAverage = players[i].battingAverage;
        newnode->StrikeRate = players[i].strikeRate;
        newnode->Wickets = players[i].wickets;
        newnode->EconomyRate = players[i].economyRate;
        insertPlayer(&teamList[index] , newnode);
    }
}

void playerDetail(ODIPlayer *player){
    printf("%-5d %-22.22s %-12s %6d %6.1f %6.1f %5d %5.1f %10.2f\n",
        player->PlayerID, player->Name, roleName((player->Role) , 0), player->TotalRuns, player->BattingAverage,
        player->StrikeRate, player->Wickets, player->EconomyRate, player->PerformanceIndex
    );
}

void displayKPlayers(Team teamList[] , int teamID , int role , int k){
    int index = findTeamByID(teamList, teamID);
    if(index == -1){
        printf("Team not found.\n");
        return;
    }
    if(role < BATSMAN || role > ALL_ROUNDER){
        printf("Invalid role.\n");
        return;
    }
    if(k <= 0){
        printf("K must be greater than 0.\n");
        return;
    }
    ODIPlayer *temp = teamList[index].roleHead[role];
    printf("\nTop %d %s of Team %s:\n", k , roleName((Role)role , 1) , teamList[index].Name);
    printf("\n%-5s %-22s %-12s %6s %6s %6s %5s %5s %10s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    int count = 0;
    while(temp && count < k){
        playerDetail(temp);
        temp = temp->roleNext;
        count++;
    }
    if(count == 0){
        printf("No players found.\n");
    }
}

void displayPlayersByRole(Team teamList[] , int role){
    if(role < BATSMAN || role > ALL_ROUNDER){
        printf("Invalid role.\n");
        return;
    }
    MaxHeap heap;
    heap.size = 0;
    for(int i = 0 ; i < TEAMCOUNT ; i++){
        ODIPlayer *first = teamList[i].roleHead[role];
        if(first != NULL){
            HeapNode node;
            node.player = first;
            node.teamIndex = i;
            insertHeap(&heap , node);
        }
    }
    printf("\n%s of all teams:\n", roleName((Role)role , 1));
    printf("\n%-5s %-22s %-13s %-12s %6s %6s %6s %5s %5s %10s\n", "ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    while(heap.size > 0){
        HeapNode maxNode = extractMax(&heap);
        ODIPlayer *p = maxNode.player;
        printf("%-5d %-22.22s %-13.13s %-12s %6d %6.1f %6.1f %5d %5.1f %10.2f\n", p->PlayerID, p->Name, p->TeamName,
        roleName(p->Role , 0), p->TotalRuns, p->BattingAverage, p->StrikeRate,
        p->Wickets, p->EconomyRate, p->PerformanceIndex);
        ODIPlayer *nextPlayer = maxNode.player->roleNext;
        if(nextPlayer){
            HeapNode newnode;
            newnode.player = nextPlayer;
            newnode.teamIndex = maxNode.teamIndex;
            insertHeap(&heap , newnode);
        }
    }
}

void displayTeamPlayers(Team teamList[], int teamID){
    int index = findTeamByID(teamList , teamID);
    if(index == -1){
        printf("Team not found.\n");
        return;
    }
    ODIPlayer *temp = teamList[index].head;
    printf("Players of Team %s\n" , teamList[index].Name);
    printf("%-5s %-22s %-12s %6s %7s %7s %5s %6s %10s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    while(temp){
        playerDetail(temp);
        temp = temp->next;
    }
    printf("\nTotal Players: %d\n", teamList[index].TotalPlayers);
    printf("Average Batting Strike Rate: %.2f\n", teamList[index].AverageBattingStrikeRate);
}

void displayTeamsByASR(Team teamList[]){
    Team *sortedTeams[TEAMCOUNT];
    for(int i = 0 ; i < TEAMCOUNT ; i++){
        sortedTeams[i] = &teamList[i];
    }
    for(int i = 0 ; i < TEAMCOUNT - 1 ; i++){
        int maxIndex = i;
        for(int j = i + 1 ; j < TEAMCOUNT ; j++){
            if(sortedTeams[j]->AverageBattingStrikeRate > sortedTeams[maxIndex]->AverageBattingStrikeRate){
                maxIndex = j;
            }
        }
        if(maxIndex != i){
            Team* temp = sortedTeams[i];
            sortedTeams[i] = sortedTeams[maxIndex];
            sortedTeams[maxIndex] = temp;
        }
    }
    printf("\nTeams Sorted by Average Batting Strike Rate\n");
    printf("%-4s %-15s %10s %14s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    for(int i = 0; i < TEAMCOUNT; i++){
        printf("%-4d %-15s %10.1f %14d\n", sortedTeams[i]->TeamID , sortedTeams[i]->Name , sortedTeams[i]->AverageBattingStrikeRate , sortedTeams[i]->TotalPlayers);
    }
}

int menu(){
    int choice;
    for(int i = 0; i < 70; i++)    printf("=");
    printf("\nICC ODI Player Performance Analyzer\n");
    for(int i = 0; i < 70; i++)    printf("=");
    printf("\n");
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display all Players of specific role Across All Teams by performance index\n");
    printf("6. Exit\n");
    for(int i = 0; i < 70; i++)    printf("=");
    int rc = readInt("\nEnter your choice:" , &choice);
    if(rc == -1) return -1;
    if(rc == 0) return 0;
    return choice;
}

int main(){
    Team teamList[TEAMCOUNT] = {0};
    initializeTeams(teamList);
    initializePlayers(teamList);
    for(int i = 0 ; i < TEAMCOUNT ; i++) calculateTeamAverage(&teamList[i]);
    while(1){
        int choice = menu();
        if(choice == -1 || choice == 6){
            printf("Exiting Program.\n");
            break;
        }
        int eof = 0;
        switch(choice){
            case 1:{
                int teamID;
                int rc = readInt("Enter Team ID to add player: " , &teamID);
                if(rc != 1){
                    if(rc == -1){
                        eof = 1;
                        break;
                    }
                    printf("Invalid Team ID.\n");
                    break;
                }
                if(addPlayer(teamList, teamID) == -1) eof = 1;
                break;
            }
            case 2:{
                int teamID;
                int rc = readInt("Enter Team ID: " , &teamID);
                if(rc != 1){
                    if(rc == -1){
                        eof = 1;
                        break;
                    }
                    printf("Invalid Team ID.\n");
                    break;
                }
                displayTeamPlayers(teamList , teamID);
                break;
            }
            case 3:
                displayTeamsByASR(teamList);
                break;
            case 4:{
                int teamID, role, k , rc;
                rc = readInt("Enter Team ID: " , &teamID);
                if(rc != 1){
                    if(rc == -1){
                        eof = 1;
                        break;
                    }
                    printf("Invalid Team ID.\n");
                    break;
                }
                rc = readInt("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ", &role);
                if(rc != 1){
                    if(rc == -1){
                        eof = 1;
                        break;
                    }
                    printf("Invalid role.\n");
                    break;
                }
                rc = readInt("Enter number of players: ", &k);
                if(rc != 1){
                    if(rc == -1){
                        eof = 1;
                        break;
                    }
                    printf("Invalid k.\n");
                    break;
                }
                displayKPlayers(teamList, teamID, role, k);
                break;
            }
            case 5:{
                int role;
                int rc = readInt("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): " , &role);
                if(rc != 1){
                    if(rc == -1){
                        eof = 1;
                        break;
                    }
                    printf("Invalid role.\n");
                    break;
                }
                displayPlayersByRole(teamList , role);
                break;
            }
            default:
                printf("Invalid choice. Enter a number between 1 and 6.\n");
        }
        if(eof){
            printf("\nEnd of input. Exiting Program.\n");
            break;
        }
    }
    freeAllPlayers(teamList);
    return 0;
}