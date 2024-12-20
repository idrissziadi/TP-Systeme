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
    //Ouvir l'ensemble de semaphore mutex
    key_t key = ftok(".",'A');
    if(key == -1){
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    int sem_id_mutex = create_semaphore(key,1,0);
    //Ouvir l'ensemble des semaphores t
    key = ftok(".",'B');
    if(key == -1){
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    int sem_id_t = create_semaphore(key,N,0);
    //Ouvir le segment memoire partagé pour tabeau acess
    key = ftok("." , 'C');
    if(key == -1){
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    int shm_id_acces = create_shared_memory(key , sizeof(info_t)*N );
    info_t * access_v1 = (info_t * ) attach_shared_memory(shm_id_acces);
    //COuvrir segment memoire partagé V1 et CPT
    key = ftok("." , 'D');
    if(key == -1){
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    int shm_id_v1 =  create_shared_memory(key,2*sizeof(int));
    int *v1 = (int *) attach_shared_memory(shm_id_v1);
    int *cpt = v1 + 1;

    //code 
    int j, i ;
    info_t element;
    P(sem_id_mutex,mutex);
    j = *cpt;// ordre d'arrivée du processus
    *cpt = *cpt +1;
    sleep(2);
    printf("Processus N %d de PID : %d\n",j,getpid());
    V(sem_id_mutex,mutex);  
    P(sem_id_t,j);// P attend son tour d'accès à la variable v1
    *v1 = *v1 + j;// Opération sur V1
    element.pid = getpid();
    element.rang = j;
    element.val = *v1;
    access_v1[j] = element;//mettre les informations d'accès à la variable v& du processus dans le tableau Accès_V1 dans la case j
    V(sem_id_t,(j-1+N)% N);// Réveiller le procesus de rang j-1
    if(j == 0){
        printf("Les résultats d'acees à la variable v1 :\n");
        for(i = N-1 ; i >= 0 ; i--){
            sleep(2);
            printf("Acess N  : %d par le processus de pid = %d de rang = %d , v1 = %d\n",N-i,access_v1[i].pid,access_v1[i].rang,access_v1[i].val);
        }
    }

    detach_shared_memory((char *)access_v1);
    detach_shared_memory((char *) v1);

}