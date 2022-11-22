// Leaving off at:
// 
// TODO:



#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SameFight.h"
// Global Variables and Mutex:

pthread_mutex_t turn;
pthread_mutex_t shuffle;
pthread_mutex_t look;
pthread_mutex_t win;

pthread_cond_t turnThreshold;
pthread_cond_t shuffleFinish;
pthread_cond_t playerCanLook;

bool winner = false;

int p1Hand [2];
int p2Hand [2];
int p3Hand [2];
int p4Hand [2];

int turnTracker = 0;
int roundTracker;
bool shuffled = false;

FILE *gameLog;

unsigned int seed;

int deckTemplate[13] = {1,2,3,4,5,6,7,8,9,10,11,12,13};

//////////////////////////////////////////////////////////////////////////////////////////
// DEALER OPERATIONS
// Initializes the deck data structure
bool Shuffle(long thread){
    pthread_mutex_lock(&shuffle);
    printf("\n");
    fprintf(gameLog, "Dealer Shuffles\n");
    for(int i = 0; i < 52; i++){
        insert(deckTemplate[i % 13]);
    }
     
    for(int times = 0; times < 2; times++){
        for(int j = 0; j < 52; j++){
            int random = (rand() % 52);

            int temp = deck[random];
            deck[j] = deck[temp];
            deck[temp] = temp;
        }
    }
    for(int k = 0; k < 52; k++){
        printf("%d, ", deck[k]);
    }
    printf("\n");
    
    shuffled = true;
    return shuffled;
    pthread_cond_signal(&shuffleFinish);
    pthread_mutex_unlock(&shuffle);
}
void DealCard(int player[]){
    
    int card = removeData();
    fprintf(gameLog, "Dealer picked: %d\n", card);
    player[0] = card;
}

void *dealerDuty(void *argA){
    
    srand(seed);
    pthread_mutex_lock(&turn);
    while(turnTracker != 0){
        pthread_cond_wait(&turnThreshold, &turn);
    }
    long tid;
    
    tid = (long)argA;
    fprintf(gameLog, "----------------------------- DEALER %ld\n", argA);

    bool done = Shuffle(tid);
    printf("Deck Shuffled\n");
    if(done){
        DealCard(p1Hand);
        DealCard(p2Hand);
        DealCard(p3Hand);
        DealCard(p4Hand);   
    }
    
    
    if(roundTracker == 0){
        turnTracker++;
    }
    else{
        turnTracker = 2;
    }
    pthread_cond_signal(&turnThreshold);
    pthread_mutex_unlock(&turn);
    
    
    //pthread_exit(NULL);
    //return 0;
}
///////////////////////////////////////////////////////////////////////////////////////
// PLAYER OPERATIONS

void endRound(long thread){
    if(thread % 2 == 0){
        printf("Player 2 Hand:(%d, %d)\n", p2Hand[0], p2Hand[1]);
        printf("Player 4 Hand:(%d, %d)\n", p4Hand[0], p4Hand[1]);
        printf("Even Wins!\n");
    }
    else{
        printf("Player 1 Hand:(%d, %d)\n", p1Hand[0], p1Hand[1]);
        printf("Player 3 Hand:(%d, %d)\n", p3Hand[0], p3Hand[1]);
        printf("Odd Wins!\n");
    }
    printf("Players Exit the round\n");
    p1Hand[0] = 0;
    p1Hand[1] = 0;

    p2Hand[0] = 0;
    p2Hand[1] = 0;

    p3Hand[0] = 0;
    p3Hand[1] = 0;

    p4Hand[0] = 0;
    p4Hand[1] = 0;

    pthread_mutex_lock(&win);
    winner = true;
    pthread_mutex_unlock(&win);
    
}
void drawCard(long thread, int player[]){
    pthread_mutex_lock(&look);

    /* while(shuffled == false){
        pthread_cond_wait(&shuffleFinish, &shuffle);
    } */
    if(shuffled == true){
        int card = removeData();
        fprintf(gameLog, "Player %d: Draws - %d\n", thread, deck[card]);
        player[1] = card;
    }
    
    // pthread_cond_signal(&playerCanLook);
    pthread_mutex_unlock(&look); 
    
}
void checkPartner(long thread, int playerA[], int playerB[]){
    int teammate = 0;
    
    if(thread % 2 == 0){
        if(thread == 2){
            teammate = 4;
        }
        else{
            teammate = 2;
        }
    }
    else{
        if(thread == 1){
            teammate = 3;
        }
        else{
            teammate = 1;
        }
    }
    if(playerA[0] == playerB[0] || playerA[1] == playerB[0]){
        fprintf(gameLog, "Match Found: Player %d matched with Player %d's - %d\n", thread, teammate,  playerB[0]);
        printf("Match Found: Player %d matched with Player %d's - %d\n", thread, teammate, playerB[0]);
        endRound(thread);
    }
    else{
        int discard;
        discard = (rand() % 2);
        fprintf(gameLog, "Player %d: discards - %d at random\n", thread, playerA[discard]);
        printf("Player %d: discards - %d at random\n", thread, playerA[discard]);
        insert(playerA[discard]);
        playerA[discard] = 0;
    }
}


void *playerOne(void *argA){
     
    
    
    pthread_mutex_lock(&turn);
    while(turnTracker != 1 || !shuffled){
        pthread_cond_wait(&turnThreshold, &turn);
    }
    
    long tid;
    
    tid = (long)argA;
    fprintf(gameLog, "----------------------------- PLAYER %ld\n", tid);
    fprintf(gameLog, "Player 1: Hand - %d\n", p1Hand[0]);

   
    drawCard(tid, p1Hand);

    fprintf(gameLog, "Player 1: Hand - %d", p1Hand[0]);
    fprintf(gameLog, ", %d\n", p1Hand[1]);

    checkPartner(tid, p1Hand, p3Hand);


    turnTracker++;
    pthread_cond_signal(&turnThreshold);
    pthread_mutex_unlock(&turn);
    
}
void *playerTwo(void *argA){
   

    pthread_mutex_lock(&turn);
    while(turnTracker != 2 || !shuffled){
        pthread_cond_wait(&turnThreshold, &turn);
    }

    long tid;
    
    tid = (long)argA;
    fprintf(gameLog, "----------------------------- PLAYER %ld\n", tid);
    fprintf(gameLog, "Player 2: Hand - %d\n", p2Hand[0]);

    
    drawCard(tid, p2Hand);

    fprintf(gameLog, "Player 2: Hand - %d", p2Hand[0]);
    fprintf(gameLog, ", %d\n", p2Hand[1]); 

    checkPartner(tid, p2Hand, p4Hand);
    
    
    
    turnTracker++;
    pthread_cond_signal(&turnThreshold);
    pthread_mutex_unlock(&turn);
    
}

 void *playerThree(void *argA){
   
    
    pthread_mutex_lock(&turn);
    while(turnTracker != 3 || !shuffled){
        pthread_cond_wait(&turnThreshold, &turn);
    }

    long tid;
    
    tid = (long)argA;
    fprintf(gameLog, "----------------------------- PLAYER %ld\n", tid);
    fprintf(gameLog, "Player 3: Hand - %d\n", p3Hand[0]);

    
    drawCard(tid, p3Hand);

    fprintf(gameLog, "Player 3: Hand - %d", p3Hand[0]);
    fprintf(gameLog, ", %d\n", p3Hand[1]);

    checkPartner(tid, p3Hand, p1Hand);
    
    

    turnTracker++;
    pthread_cond_signal(&turnThreshold);
    pthread_mutex_unlock(&turn);
   
}

void *playerFour(void *argA){
    
    
    pthread_mutex_lock(&turn);
    while(turnTracker != 4 || !shuffled){
        pthread_cond_wait(&turnThreshold, &turn);
    }

    long tid;
    
    tid = (long)argA;
    fprintf(gameLog, "----------------------------- PLAYER %ld\n", tid);
    fprintf(gameLog, "Player 4: Hand - %d\n", p4Hand[0]); 

    drawCard(tid, p4Hand);

    fprintf(gameLog, "Player 4: Hand - %d", p4Hand[0]);
    fprintf(gameLog, ", %d\n", p4Hand[1]);

    checkPartner(tid, p4Hand, p2Hand);
    

    if(roundTracker == 0){
        turnTracker = 0;
    }
    else{
        turnTracker = 1;
    }
    pthread_cond_signal(&turnThreshold);
    pthread_mutex_unlock(&turn);
    
} 

int main(int argc, char *argv[]){
    
    
    int ret;
    long t0 = 0, t1 = 1, t2 = 2, t3 = 3, t4 = 4;
    // Thread initialization
    pthread_t dealer;
    pthread_t p1;
    pthread_t p2;
    pthread_t p3;
    pthread_t p4;
    
    // Mutex lock init
    pthread_mutex_init(&turn, NULL);
    pthread_mutex_init(&shuffle, NULL);
    pthread_mutex_init(&look, NULL);
    pthread_mutex_init(&win, NULL);

    // Lock conditions init
    pthread_cond_init(&turnThreshold, NULL);
    pthread_cond_init(&shuffleFinish, NULL);
    pthread_cond_init(&playerCanLook, NULL);
    

    gameLog = fopen("GameLog.txt", "w");
    
    scanf("%u", &seed);
    
    for(roundTracker = 0; roundTracker < 2; roundTracker++){
        winner = false;
        
        
        fprintf(gameLog, "----------------------------------------------- ROUND %d\n", roundTracker + 1);
        fprintf(gameLog, "DEALER JOINS\n");
        ret = pthread_create(&dealer, NULL, dealerDuty, (void *)t0);
        if (ret){
            printf("ERROR", ret);
        }
        
        fprintf(gameLog, "PLAYER 1 JOINS\n");
        ret = pthread_create(&p1, NULL, playerOne, (void *)t1);
        if (ret){
            printf("ERROR", ret);
        }
        
        fprintf(gameLog, "PLAYER 2 JOINS\n");
        ret = pthread_create(&p2, NULL, playerTwo, (void *)t2);
        if (ret){
            printf("ERROR", ret);
        }

        fprintf(gameLog, "PLAYER 3 JOINS\n");
        ret = pthread_create(&p3, NULL, playerThree, (void *)t3);
        if (ret){
            printf("ERROR", ret);
        }

        fprintf(gameLog, "PLAYER 4 JOINS\n");
        ret = pthread_create(&p4, NULL, playerFour, (void *)t4);
        if (ret){
            printf("ERROR", ret);
        } 


        // Joins all threads
        pthread_join(dealer, NULL);
        pthread_join(p1, NULL); 
        pthread_join(p2, NULL);
        pthread_join(p3, NULL);
        pthread_join(p4, NULL);
        
   }

}
// Compile: g++ SameFight.c -l pthread -g -o run -fpermissive
// https://hpc-tutorials.llnl.gov/posix/example_using_cond_vars/