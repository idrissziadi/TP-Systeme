#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "semaphore.h"

// Crée ou récupère un ensemble de sémaphores
int create_semaphore(key_t key, int num_sems, int flags) {
    int sem_id = semget(key, num_sems, flags);
    if (sem_id == -1) {
        perror("Erreur dans create_semaphore");
        exit(EXIT_FAILURE);
    }
    return sem_id;
}

// Initialise un sémaphore spécifique
int initialize_semaphore(int sem_id, int sem_num, int value) {
    if (semctl(sem_id, sem_num, SETVAL, value) == -1) {
        perror("Erreur dans initialize_semaphore");
        exit(EXIT_FAILURE);
    }
    return 0;
}

// Supprime un ensemble de sémaphores
int delete_semaphore(int sem_id) {
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("Erreur dans delete_semaphore");
        return -1;
    }
    return 0;
}

// Primitive P (wait) : Décrémentation du sémaphore
void P(int sem_id, int sem_num) {
    struct sembuf sb = {sem_num, -1, 0};
    if (semop(sem_id, &sb, 1) == -1) {
        perror("Erreur dans P()");
        exit(EXIT_FAILURE);
    }
}

// Primitive V (signal) : Incrémentation du sémaphore
void V(int sem_id, int sem_num) {
    struct sembuf sb = {sem_num, 1, 0};
    if (semop(sem_id, &sb, 1) == -1) {
        perror("Erreur dans V()");
        exit(EXIT_FAILURE);
    }
}
