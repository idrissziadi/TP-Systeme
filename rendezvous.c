#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include "semaphore.h"
//idriss
#define NOMBRE_PROCESSUS 2
#define NOMBRE_SEMAPHORE 2

enum { RDV1=0, RDV2=1 };

int main() {
    // Clé unique pour les sémaphores
    key_t key = ftok("monfichier", 'A');
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }


    // Création d'un ensemble de sémaphores (nombre_semaphore sémaphore)
    int sem_id = create_semaphore(key, NOMBRE_SEMAPHORE, IPC_CREAT | 0666);




    // Initialisation du sémaphores à 1
    for( int i = 0 ; i < NOMBRE_SEMAPHORE ; i++){
        initialize_semaphore(sem_id, i, 0);    
    }
    //************************* //

    printf("Création de 2 processus concurrents...\n");
   
    switch (fork())
    {
    case -1:
        perror("fork1");
        exit(EXIT_FAILURE);
    case 0:
        printf("Processus 1 : Début de la tâche...\n");
        sleep(1); // Simule une tâche
        printf("Processus 1 : Fin de la tâche. Signal au processus 2.\n");
        V(sem_id,RDV2);
        P(sem_id,RDV1);
        printf("Processus 1 : Rendez-vous terminé.\n");
        exit(EXIT_SUCCESS); 
    }
    
    switch (fork())
    {
    case -1: 
        perror("fork2");
        exit(EXIT_FAILURE);
    case 0:
        printf("Processus 2 : Début de la tâche...\n");
        sleep(10); // Simule une tâche (plus long pour simuler un délai)
        printf("Processus 2 : Fin de la tâche. Signal au processus 1.\n");
        V(sem_id ,RDV1);
        P(sem_id ,RDV2);
        printf("Processus 2 : Rendez-vous terminé.\n");
        exit(EXIT_SUCCESS);
    }  
    //************************* //  
    // Attendre la fin de tous les processus fils
    for (int i = 0; i < NOMBRE_PROCESSUS; i++) {
        wait(NULL);
    }

    // Suppression de l'ensemble des sémaphores
    delete_semaphore(sem_id);

    printf("Tous les processus sont terminés.\n");

    exit(EXIT_SUCCESS);
}
