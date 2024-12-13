#include "segment.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "semaphore.h"

#define NOMBRE_SEMAPHORE 2
#define NOMBRE_PROCESSUS 2


enum { VIDE=0, PLEIN=1 };
int main() {
    //creation des semaphores
    key_t key = ftok("monfichier", 'A');
    int sem_id = create_semaphore(key, NOMBRE_SEMAPHORE, IPC_CREAT | 0666);    // Exemple d'utilisation des fonctions
    initialize_semaphore(sem_id , VIDE , 1);
    initialize_semaphore(sem_id , PLEIN , 0);
    
    
    key_t key = ftok("monfichier", 'A');
    int shmid = create_shared_memory(key, sizeof(data_t) * TAILLE_TAMPON + sizeof(int) * 2);
    char* segment = attach_shared_memory(shmid);

    // Initialisation de la structure `file_t`
    file_t f;
    f.buf = (data_t *)segment;
    f.debut = (int *)(segment + sizeof(data_t) * TAILLE_TAMPON);
    f.fin = f.debut + 1;
    *(f.debut) = 0;
    *(f.fin) = 0;

    //*****************************//
    switch (fork()) 
    {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
       data_t d = { 1 , 2 , "ziad"};
        P(sem_id , VIDE);
        deposer(f,d);
        V(sem_id,PLEIN);
    default:
        break;
    }

        switch (fork()) 
    {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0:
         data_t d ;
        P(sem_id , PLEIN);
        d = retirer(f);
        V(sem_id,VIDE);
    default:
        break;
    }



    // Détachement et suppression de la mémoire partagée après utilisation
    detach_shared_memory(segment);
    delete_shared_memory(shmid);

    return 0;
}
