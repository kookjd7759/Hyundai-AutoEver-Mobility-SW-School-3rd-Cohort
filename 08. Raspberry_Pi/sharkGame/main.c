#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MIN(a,b) ((a)<(b)?(a):(b))
#define PLAYER_SIZE 2
#define BOARD_SIZE 20

struct Player {
    char name[10];
    int pos;
    int coin;
    int state; // (1)Alive (2)pass (3)die
};

int board[BOARD_SIZE]; // (0)None (1~6)Coin
struct Player player[PLAYER_SIZE];
int shark, alive_cnt;

int roll_dice() { return rand() % 6 + 1; }

void reset() { 
    memset(board, 0, sizeof(board)); 
    for (int i = 1; i < BOARD_SIZE; ++i){ // 0번째 칸에는 코인 없음 보장
        int isCoin = rand() % 2; // 1/2 확률로 코인 존재
        if (isCoin) continue;
        board[i] = roll_dice();  // 코인은 1원 부터 6원
    }
    
    shark = 0; 
    alive_cnt = PLAYER_SIZE;
}
void init() {
    printf("--<[Shark Game]>--\n");
    srand((unsigned)time(NULL));
    reset();

    for (int i = 0; i < PLAYER_SIZE; ++i) {
        printf("Enter player %dth nickname >> ", i + 1);
        scanf("%9s", player[i].name);
        player[i].pos  = 0;
        player[i].coin = 0;
        player[i].state = 1;
    }
    printf("------------------\n");
}

void print_state() {
    printf("[Current state]\n");
    // board
    printf(" - Board\n|");
    for (int i = 0; i < BOARD_SIZE; ++i)
        printf("%d|", board[i]);
    printf("\n");

    // player
    printf(" - Player\n");
    for (int i = 0; i < PLAYER_SIZE; ++i) {
        if (player[i].state == 2) {
            printf("Player <%s>, COIN: %d, ESCAPE !!\n", player[i].name, player[i].coin);
            continue;
        }
        if (player[i].state == 3) {
            printf("Player <%s>, COIN: %d, DIE ...\n", player[i].name, player[i].coin);
            continue;
        }
        printf("Player <%s>, COIN: %d, ALIVE\n", player[i].name, player[i].coin);

        printf("|");
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (player[i].pos == j) printf("■|");
            else                    printf("□|");
        }
        printf("\n");
    }

    // Shark
    printf(" - Shark\n|");
    for (int i = 0; i < BOARD_SIZE; ++i){
        if (shark == i) printf("\033[31m■\033[0m|");
        else                    printf("□|");
    }
    printf("\n");
}

void player_move(int idx, int step){
    printf("\t%s moves %d !\n", player[idx].name, step);
    player[idx].pos += step;
    if (player[idx].pos >= BOARD_SIZE) {
        player[idx].state = 2;
        alive_cnt--;
        return; 
    }
    if (board[player[idx].pos]) {
        player[idx].coin += board[player[idx].pos];
        board[player[idx].pos] = 0; // coin 제거
    }
}

void capture(){
    for (int i = 0; i < PLAYER_SIZE; ++i){
        if (player[i].state != 1) continue;
        if (player[i].pos <= shark) {
            player[i].state = 3;
            printf("\t%s is 냠냠 from 무시무시 shark ..\n", player[i].name);
            alive_cnt--;
        }
    }
}

void run(){
    init();

    int round = 1;
    while(1){
        print_state();
        printf("[round - %d]\n", round);
        // player move
        for (int i = 0; i < PLAYER_SIZE; ++i){
            if (player[i].state != 1) continue;
            printf("\t[turn - %s]\n", player[i].name);
            while(1) {
                printf("\tEnter 'r' for roll the dice >> ");
                char input; scanf("%c", &input);
                if (input == 'r' || input == 'R') break;
            }
            player_move(i, roll_dice());
        }

        // shark move
        int shark_step = roll_dice();
        printf("Shark moves .. %d !\n", shark_step);
        shark = MIN(shark + shark_step, BOARD_SIZE - 1);

        capture();

        round++;

        if (alive_cnt <= 0) break;
    }

    printf("Game over !!\n[Ranking]\n");
    for (int i = 0; i < PLAYER_SIZE; ++i){
        printf("Player <%s>, COIN: %d - ", player[i].name, player[i].coin);
        if (player[i].state == 2) printf("\033[32mALIVE !!\033[0m\n");
        else                      printf("\033[31mDIE ....\033[0m\n");
    }
}

int main(void) {
    run();

    return 0;
}
