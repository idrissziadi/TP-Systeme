#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include "semaphore.h" 
#include "segment.h"

enum{splein , svide};
int main(void){
    srand(time(NULL));
    key_t key = ftok("." , 'A');
    if( key == -1){
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    int sem_id = create_semaphore( key,  2, 0);

    key = ftok("." , 'B');
    if( key == -1){
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int shm_id = create_shared_memory(key,sizeof(objet_t));
    objet_t *T   = (objet_t *) attach_shared_memory(shm_id);
    objet_t Mp;
    do
    {
      printf("Production ...\n");  
      Mp.a = rand() % 20; //produire
      sleep(2);
      P(sem_id,svide);
      *T = Mp;
      printf("Le producteur vient de déposer un objet : %d \n" , Mp.a);
      V(sem_id,splein);

    } while (1);
    
    detach_shared_memory((char *)T);
    exit(EXIT_SUCCESS);
}