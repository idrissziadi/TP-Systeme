#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include "semaphore.h" 
#include "segment.h"

#define N 6


enum{mutex};

int main(void){
    key_t key = ftok(".",'A');
    if(key == -1){
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    int sem_id_mutex = create_semaphore(key,1,IPC_CREAT | 0666);
    initialize_semaphore(sem_id_mutex,mutex,1);
    
    key = ftok(".",'B');
    if(key == -1){
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    int sem_id_t = create_semaphore(key,N,IPC_CREAT | 0666);
    for(int i = 0 ; i < N-1 ; i++){
        initialize_semaphore(sem_id_t,i,0);
    }
    initialize_semaphore(sem_id_t,N-1,1);
    
    
    //Creer segment memoire partagé pour tabeau acess
    key = ftok("." , 'C');
    if(key == -1){
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    int shm_id_acces = create_shared_memory(key , sizeof(info_t)*N );
    
    //Creer segment memoire partagé V1 et CPT
    key = ftok("." , 'D');
    if(key == -1){
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    int shm_id_v1 =  create_shared_memory(key,2*sizeof(int));
    int *v1 = (int *)attach_shared_memory(shm_id_v1);
    *v1 = 0;
    int *cpt = v1 +1;
    *cpt = 0;
    detach_shared_memory((char *) v1);

    //Création de 6 processus 
    for(int i = 0 ; i < N ; i++){
        
        
        switch (fork())
        {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        
        case 0:
            execl("./Pgme_processus","./Pgme_processus",NULL);
            exit(EXIT_FAILURE);
        }
    }

    //attente des processus fils
    while (wait(NULL) > 0);
    //Suppression des segments mémoires partagés et segments des sémaphores
    delete_semaphore(sem_id_mutex);
    delete_semaphore(sem_id_t);
    delete_shared_memory(shm_id_acces);
    delete_shared_memory(shm_id_v1);
    exit(EXIT_SUCCESS);
}