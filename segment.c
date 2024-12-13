#include "segment.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stddef.h>
// Fonction de création de la mémoire partagée
int create_shared_memory(key_t key, size_t size) {
    int shmid = shmget(key, size, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Erreur dans shmget");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

// Fonction d'attachement à la mémoire partagée
char* attach_shared_memory(int shmid) {
    char* segment = (char*) shmat(shmid, NULL, 0);
    if (segment == (void*) -1) {
        perror("Erreur dans shmat");
        exit(EXIT_FAILURE);
    }
    return segment;
}

// Fonction de détachement de la mémoire partagée
void detach_shared_memory(char* segment) {
    if (shmdt(segment) == -1) {
        perror("Erreur dans shmdt");
        exit(EXIT_FAILURE);
    }
}

// Fonction de suppression de la mémoire partagée
void delete_shared_memory(int shmid) {
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Erreur dans shmctl");
        exit(EXIT_FAILURE);
    }
}

// Fonction de dépôt dans le tampon
void deposer(file_t f, data_t d) {
    f.buf[*(f.fin)] = d;
    *(f.fin) = (*(f.fin) + 1) % TAILLE_TAMPON;
}

// Fonction de retrait du tampon
data_t retirer(file_t f) {
    data_t d = f.buf[*(f.debut)];
    *(f.debut) = (*(f.debut) + 1) % TAILLE_TAMPON;
    return d;
}