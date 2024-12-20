#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdbool.h>


#ifndef SEGMENT_H
#define SEGMENT_H



// Définition des structures
typedef struct {
    int pid;
    int rang;
    int val;
} info_t;








// Déclarations des fonctions
int create_shared_memory(key_t key, size_t size);
char* attach_shared_memory(int shmid);
void detach_shared_memory(char* segment);
void delete_shared_memory(int shmid);

#endif // SEGMENT_H