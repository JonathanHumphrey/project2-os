// LEaving off at:
// Got threads synchornized based off a turn counter that
// TODO:
// - discard,
// - check partner
// - win and close threads



#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// Global Variables and Mutex:


pthread_mutex_t turn;
pthread_mutex_t draw;
/* pthread_mutex_t p1Access;
pthread_mutex_t p2Access;
pthread_mutex_t p3Access;
pthread_mutex_t p4Access; */

pthread_cond_t turnThreshold;
/* pthread_cond_t p1Finished;
pthread_cond_t p2Finished;
pthread_cond_t p3Finished; */

int p1Hand [2];
int p2Hand [2];
int p3Hand [2];
int p4Hand [2];

int turnTracker = 0;


//////////////////////////////////////////////////////////////////////////////////////////
// DEALER OPERATIONS
int Shuffle(long thread){
    
    if(turnTracker == 0){
        printf("Dealer Shuffles\n");
        return rand() % 13 + 1;
    }
    else{
        printf("Player %d", thread);
        
        return rand() % 13 + 1;
    }
    
}
void DealCard(int num, int player[]){
    
    if(turnTracker == 0){
        printf("Dealer picked: %d\n", num);
        player[0] = num;
    }
    else{
        player[1] = num;
    }
}
void *dealerDuty(void *argA){
    
    pthread_mutex_lock(&turn);
    while(turnTracker != 0){
        pthread_cond_wait(&turnThreshold, &turn);
    }
    
    long tid;
    
    tid = (long)argA;
    printf("----------------------------- DEALER %ld\n", argA);

    int selection = Shuffle(tid);
    DealCard(selection, p1Hand);

    selection = Shuffle(tid);
    DealCard(selection, p2Hand);

    selection = Shuffle(tid);
    DealCard(selection, p3Hand);
    
    selection = Shuffle(tid);
    DealCard(selection, p4Hand);
    
    turnTracker++;
    pthread_cond_signal(&turnThreshold);
    pthread_mutex_unlock(&turn);
    
    //pthread_exit(NULL);
    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////
// PLAYER OPERATIONS

int DrawCard(int player[], long thread){
    

    int card = 0;
    card = Shuffle(thread);
    DealCard(card, player);
    //player[1] = card;
    printf(": Draws - %d\n", card);
    
    
    
    return 0;
}

int CheckPartner(int thread){
    int match = 0;
    
    // Odd Team
    if(thread % 2 != 0){
        
        if(thread == 1){
            printf("testing for match on thread: %d\n", thread);
            for(int i = 0; i < 2; i++){
                if(p1Hand[i] == p3Hand[0]){
                    printf("%d\n", p1Hand[i]); 
                    match == p3Hand[0];
                }
            }
            return match;
        }
        else{
           for(int i = 0; i < 2; i++){
                if(p3Hand[i] == p1Hand[0]){
                    match == p1Hand[0];
                    return match;
                }
            }
        }
    }
    return 0;
}


void *playerOne(void *argA){

    pthread_mutex_lock(&turn);
    while(turnTracker != 1){
        pthread_cond_wait(&turnThreshold, &turn);
    }
    
    long tid;
    
    tid = (long)argA;
    printf("----------------------------- PLAYER %ld\n", tid);
    printf("Player 1: Hand - %d\n", p1Hand[0]);

    DrawCard(p1Hand, tid);

    printf("Player 1: Hand - %d", p1Hand[0]);
    printf(", %d\n", p1Hand[1]);

    int match = CheckPartner(tid);
    if(match != 0){
        printf("MATCH %d\n", match);
    }
    
    turnTracker++;
    pthread_cond_signal(&turnThreshold);
    pthread_mutex_unlock(&turn);
    
}
void *playerTwo(void *argA){
      
    pthread_mutex_lock(&turn);
    while(turnTracker != 2){
        pthread_cond_wait(&turnThreshold, &turn);
    }

    long tid;
    
    tid = (long)argA;
    printf("----------------------------- PLAYER %ld\n", tid);
    printf("Player 2: Hand - %d\n", p2Hand[0]);

    DrawCard(p2Hand, tid);

    printf("Player 2: Hand - %d", p2Hand[0]);
    printf(", %d\n", p2Hand[1]); 

    turnTracker++;
    pthread_cond_signal(&turnThreshold);
    pthread_mutex_unlock(&turn);
    
}

 void *playerThree(void *argA){
    
    
    pthread_mutex_lock(&turn);
    while(turnTracker != 3){
        pthread_cond_wait(&turnThreshold, &turn);
    }

    long tid;
    
    tid = (long)argA;
    printf("----------------------------- PLAYER %ld\n", tid);
    printf("Player 3: Hand - %d\n", p3Hand[0]);

    DrawCard(p3Hand, tid);

    printf("Player 3: Hand - %d", p3Hand[0]);
    printf(", %d\n", p3Hand[1]);

    turnTracker++;
    pthread_cond_signal(&turnThreshold);
    pthread_mutex_unlock(&turn);
   
}

void *playerFour(void *argA){
    
    
    pthread_mutex_lock(&turn);
    while(turnTracker != 4){
        pthread_cond_wait(&turnThreshold, &turn);
    }

    long tid;
    
    tid = (long)argA;
    printf("----------------------------- PLAYER %ld\n", tid);
    printf("Player 4: Hand - %d\n", p4Hand[0]); 

    DrawCard(p4Hand, tid);

    printf("Player 4: Hand - %d", p4Hand[0]);
    printf(", %d\n", p4Hand[1]);

    turnTracker = 0;
    pthread_cond_signal(&turnThreshold);
    pthread_mutex_unlock(&turn);
    
} 

int main(int argc, char *argv[]){
    
    unsigned int seed;
    int ret;
    long t0 = 0, t1 = 1, t2 = 2, t3 = 3, t4 = 4;
    pthread_t dealer;
    pthread_t p1;
    pthread_t p2;
    pthread_t p3;
    pthread_t p4;
    pthread_attr_t attr;
    pthread_mutex_init(&turn, NULL);
    pthread_mutex_init(&draw, NULL);
    /* pthread_mutex_init(&p1Access, NULL);
    pthread_mutex_init(&p2Access, NULL);
    pthread_mutex_init(&p3Access, NULL);
    pthread_mutex_init(&p4Access, NULL); */

    pthread_cond_init(&turnThreshold, NULL);
    /* pthread_cond_init(&p1Finished, NULL);
    pthread_cond_init(&p2Finished, NULL);
    pthread_cond_init(&p3Finished, NULL); */

    

    scanf("%u", &seed);
    srand(seed);

    printf("DEALER JOINS\n");
    ret = pthread_create(&dealer, NULL, dealerDuty, (void *)t0);
    if (ret){
        printf("ERROR", ret);
    }
    
    printf("PLAYER 1 JOINS\n");
    ret = pthread_create(&p1, NULL, playerOne, (void *)t1);
    if (ret){
        printf("ERROR", ret);
    }
    
    printf("PLAYER 2 JOINS\n");
    ret = pthread_create(&p2, NULL, playerTwo, (void *)t2);
    if (ret){
        printf("ERROR", ret);
    }

     printf("PLAYER 3 JOINS\n");
    ret = pthread_create(&p3, NULL, playerThree, (void *)t3);
    if (ret){
        printf("ERROR", ret);
    }

    printf("PLAYER 4 JOINS\n");
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
    

    return 0;
}
// Compile: g++ SameFight.c -l pthread -g -o run -fpermissive
// https://hpc-tutorials.llnl.gov/posix/example_using_cond_vars/