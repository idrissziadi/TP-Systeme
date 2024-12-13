#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#ifndef SEGMENT_H
#define SEGMENT_H

#define TAILLE_TAMPON 5  // Définition de la taille du tampon

typedef struct {
    int a;
} objet_t;

typedef struct {
    objet_t *buf; // Pointeur vers le tampon
    int *debut;  // Pointeur vers l'indice de début
    int *fin;    // Pointeur vers l'indice de fin
} file_t;

// Déclarations des fonctions
int create_shared_memory(key_t key, size_t size);
char* attach_shared_memory(int shmid);
void detach_shared_memory(char* segment);
void delete_shared_memory(int shmid);
void deposer(file_t f, objet_t d);
objet_t retirer(file_t f);

#endif // SEGMENT_H
