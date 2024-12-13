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
    int sem_id = create_semaphore( key,  2, IPC_CREAT | 0666);
    initialize_semaphore(sem_id,splein,0);
    initialize_semaphore(sem_id,svide,1);

    key = ftok("." , 'B');
    if( key == -1){
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int shm_id = create_shared_memory(key,sizeof(objet_t));
    //****************** *//

    switch (fork()){
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    
    case 0:
        execl("./Pgme_Producter","./Pgme_Producter",NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }

    switch (fork()){
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    
    case 0:
        execl("./Pgme_Consommateur","./Pgme_Consommateur",NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }

    if (wait(NULL) == -1){
        perror("wait");
        exit(EXIT_FAILURE);
    }

    if (wait(NULL) == -1){
        perror("wait");
        exit(EXIT_FAILURE);
    }

    delete_semaphore(sem_id);
    delete_shared_memory(shm_id);
    exit(EXIT_SUCCESS);
}