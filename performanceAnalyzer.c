/**
 * FILENAME: performanceAnalyzer.c
 * DESCRIPTION: Design and implement a data management and analysis system for ICC ODI player statistics that organizes and processes data using Searching, Sorting and Linked list concepts.
 * AUTHOR: Nancy Jain
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Players_data.h"
#define TEAMCOUNT 10
#define MIN_PLAYERID 1
#define MAX_PLAYERID 1000
#define MIN_PLAYER 11
#define MAX_PLAYER 50
#define NAME_LENGTH 51

typedef struct ODIPlayer{
    int PlayerID;
    char Name[NAME_LENGTH];
    char TeamName[NAME_LENGTH];
    int Role;
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
    if(p->Role == 1)      return (p->BattingAverage * p->StrikeRate) / 100.0;
    else if(p->Role == 2) return (p->Wickets * 2) + (100 - p->EconomyRate);
    else if(p->Role == 3) return ((p->BattingAverage * p->StrikeRate) / 100.0) + (p->Wickets * 2);
    return 0;
}

int findTeamByName(Team teamList[] , const char* name){
    int low = 0 , high = TEAMCOUNT - 1;
    while(low <= high){
        int mid = low + (high - low) / 2;
        int result = strcmp(teamList[mid].Name , name);
        if(result == 0) return mid;
        else if( result < 0) low = mid + 1;
        else high = mid - 1;
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
    if(strcmp(role , "Batsman") == 0)         return 1;
    else if(strcmp(role , "Bowler") == 0)     return 2;
    else if(strcmp(role, "All-rounder") == 0) return 3;
    return 0;
}

void calculateTeamAverage(Team *team){
    float sum = 0;
    int count = 0;
    ODIPlayer* temp = team->head;
    while(temp){
        if(temp->Role == 1 || temp->Role == 3){
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

void addPlayer(Team teamList[] , int TeamID){
    int index = findTeamByID(teamList , TeamID);
    if(index == -1){
        printf("Team not found.\n");
        return;
    }
    if(teamList[index].TotalPlayers >= MAX_PLAYER){
        printf("Team already has maximum players.\n");
        return;
    }
    ODIPlayer* newplayer = malloc(sizeof(ODIPlayer));
    if(!newplayer){
        printf("Dynamic memory allocation failed.\n");
        return;
    }
    printf("Enter Player Details:\n");
    printf("Player ID: ");
    if(scanf("%d",&newplayer->PlayerID)!= 1){
        printf("Invalid Player ID.\n");
        free(newplayer);
        return;
    }
    if(newplayer->PlayerID < MIN_PLAYERID || newplayer->PlayerID > MAX_PLAYERID){
        printf("Player ID must be between 1 and 1000.\n");
        free(newplayer);
        return;
    }
    for(int i = 0 ; i < TEAMCOUNT ; i++){
        ODIPlayer* temp = teamList[i].head;
        while(temp){
            if(temp->PlayerID == newplayer->PlayerID){
                printf("Player ID already exists.\n");
                free(newplayer);
                return;
            }
            temp = temp->next;
        }
    }
    printf("Name: ");
    if(scanf(" %50[^\n]", newplayer->Name) != 1){
        printf("Invalid name.\n");
        free(newplayer);
        return;
    }
    printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    if(scanf("%d", &newplayer->Role) != 1 || newplayer->Role < 1 || newplayer->Role > 3){
        printf("Invalid role.\n");
        free(newplayer);
        return;
    }
    printf("Total Runs: ");
    if(scanf("%d", &newplayer->TotalRuns) != 1 || newplayer->TotalRuns < 0){
        printf("Invalid total runs.\n");
        free(newplayer);
        return;
    }
    printf("Batting Average: ");
    if(scanf("%f", &newplayer->BattingAverage) != 1 || !isfinite(newplayer->BattingAverage) || newplayer->BattingAverage < 0){
        printf("Invalid batting average.\n");
        free(newplayer);
        return;
    }
    printf("Strike Rate: ");
    if(scanf("%f", &newplayer->StrikeRate) != 1 ||  !isfinite(newplayer->StrikeRate) || newplayer->StrikeRate < 0){
        printf("Invalid strike rate.\n");
        free(newplayer);
        return;
    }
    printf("Wickets: ");

    if(scanf("%d", &newplayer->Wickets) != 1 || newplayer->Wickets < 0){
        printf("Invalid wickets.\n");
        free(newplayer);
        return;
    }
    printf("Economy Rate: ");
    if(scanf("%f", &newplayer->EconomyRate) != 1 ||  !isfinite(newplayer->EconomyRate) || newplayer->EconomyRate < 0){
        printf("Invalid economy rate.\n");
        free(newplayer);
        return;
    }
    strcpy(newplayer->TeamName, teamList[index].Name);
    newplayer->PerformanceIndex = PerformanceIndex(newplayer);
    newplayer->next = teamList[index].head;
    teamList[index].head = newplayer;
    insertRoleSorted(&teamList[index], newplayer);
    teamList[index].TotalPlayers++;
    calculateTeamAverage(&teamList[index]);
    printf("\nPlayer added successfully to Team %s!\n", teamList[index].Name);
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
        strcpy(newnode->TeamName, players[i].team);
        newnode->Role = getRole(players[i].role);
        if(newnode->Role == 0){
            free(newnode);
            continue;
        }
        newnode->TotalRuns = players[i].totalRuns;
        newnode->BattingAverage = players[i].battingAverage;
        newnode->StrikeRate = players[i].strikeRate;
        newnode->Wickets = players[i].wickets;
        newnode->EconomyRate = players[i].economyRate;
        newnode->PerformanceIndex = PerformanceIndex(newnode);
        newnode->next = teamList[index].head;
        teamList[index].head = newnode;
        insertRoleSorted(&teamList[index], newnode);
        teamList[index].TotalPlayers++;
    }
}

void playerDetail(ODIPlayer *player){
    printf("%-5d %-22.22s %-12s %6d %6.1f %6.1f %5d %5.1f %10.2f\n",
        player->PlayerID, player->Name, player->Role == 1 ? "Batsman" : player->Role == 2 ? "Bowler" : "All-rounder", player->TotalRuns, player->BattingAverage,
        player->StrikeRate, player->Wickets, player->EconomyRate, player->PerformanceIndex
    );
}

void displayKPlayers(Team teamList[] , int teamID , int role , int k){
    int index = findTeamByID(teamList, teamID);
    if(index == -1){
        printf("Team not found.\n");
        return;
    }
    if(role < 1 || role > 3){
        printf("Invalid role.\n");
        return;
    }
    if(k <= 0){
        printf("K must be greater than 0.\n");
        return;
    }
    ODIPlayer *temp = teamList[index].roleHead[role];
    printf("\nTop %d %s of Team %s:\n", k , role == 1 ? "Batsmen" : role == 2 ? "Bowlers" : "All-rounders", teamList[index].Name);
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
    if(role < 1 || role > 3){
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
    printf("\n%s of all teams:\n", role == 1 ? "Batsmen" : role == 2 ? "Bowlers" : "All-rounders");
    printf("\n%-5s %-22s %-13s %-12s %6s %6s %6s %5s %5s %10s\n", "ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    while(heap.size > 0){
        HeapNode maxNode = extractMax(&heap);
        ODIPlayer *p = maxNode.player;
        printf("%-5d %-22.22s %-13.13s %-12s %6d %6.1f %6.1f %5d %5.1f %10.2f\n", p->PlayerID, p->Name, p->TeamName,
        p->Role == 1 ? "Batsman" : p->Role == 2 ? "Bowler" : "All-Rounder", p->TotalRuns, p->BattingAverage, p->StrikeRate,
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
    printf("\nEnter your choice:");
    scanf("%d" , &choice);
    return choice;
}

int main(){
    Team teamList[TEAMCOUNT] = {0};
    initializeTeams(teamList);
    initializePlayers(teamList);
    for(int i = 0 ; i < TEAMCOUNT ; i++) calculateTeamAverage(&teamList[i]);
    while(1){
        int choice = menu();
        if(choice == 6){
            printf("Exiting Program.");
            break;
        }
        switch(choice){
            case 1:{
                printf("Enter Team ID to add player: ");
                int teamID;
                if(scanf("%d", &teamID) != 1){
                    printf("Invalid Team ID.\n");
                    break;
                }
                addPlayer(teamList, teamID);
                break;
            }
            case 2:{
                int teamID;
                printf("Enter Team ID: ");
                if(scanf("%d", &teamID) != 1){
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
                int teamID, role, k;
                printf("Enter Team ID: ");
                if(scanf("%d", &teamID) != 1){
                    printf("Invalid Team ID.\n");
                    break;
                }
                printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
                if(scanf("%d", &role) != 1){
                    printf("Invalid role.\n");
                    break;
                }
                printf("Enter number of players: ");
                if(scanf("%d", &k) != 1){
                    printf("Invalid k.\n");
                    break;
                }
                displayKPlayers(teamList, teamID, role, k);
                break;
            }
            case 5:{
                int role;
                printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
                if(scanf("%d", &role) != 1){
                    printf("Invalid role.\n");
                    break;
                }
                displayPlayersByRole(teamList , role);
                break;
            }
            default:
                printf("Invalid choice. Enter a number between 1 and 6.\n");
        }
    }
    return 0;
}