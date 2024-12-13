#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include "semaphore.h" 
#include "segment.h"

enum{splein , svide};
int main(void){
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
    objet_t Mc;
    do
    {
      P(sem_id,splein);
      Mc = *T;
      printf("Le consommateur vient de retirer un objet : %d \n" , Mc.a);
      V(sem_id,svide);
      printf("Consommation ...\n");
      sleep(2);

    } while (1);
    
    detach_shared_memory((char *)T);
    exit(EXIT_SUCCESS);
}