#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

// Création ou récupération d'un ensemble de sémaphores
int create_semaphore(key_t key, int num_sems, int flags);

// Initialisation d'un sémaphore spécifique
int initialize_semaphore(int sem_id, int sem_num, int value);

// Suppression d'un ensemble de sémaphores
int delete_semaphore(int sem_id);

// Primitives P et V pour la gestion des sémaphores
void P(int sem_id, int sem_num);
void V(int sem_id, int sem_num);

#endif
