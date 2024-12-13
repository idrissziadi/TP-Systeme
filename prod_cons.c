#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "semaphore.h"

#define NOMBRE_SEMAPHORES 2
#define TAILLE_TAMPON 5

typedef struct {
    int a;
    int b;
    char mot[5];
} data_t;

typedef struct {
    data_t *buf; // Pointeur vers le tampon
    int *debut;  // Pointeur vers l'indice de début
    int *fin;    // Pointeur vers l'indice de fin
} file_t;

void deposer(file_t f, data_t d) {
    // Dépôt des données dans le tampon
    f.buf[*(f.fin)] = d;
    *(f.fin) = (*(f.fin) + 1) % TAILLE_TAMPON; // Tampon circulaire
}

data_t retirer(file_t f) {
    // Retrait des données du tampon
    data_t d = f.buf[*(f.debut)];
    *(f.debut) = (*(f.debut) + 1) % TAILLE_TAMPON; // Tampon circulaire
    return d;
}

int main() {
    key_t key = ftok("monfichier", 'A');
    if (key == -1) {
        perror("Erreur dans ftok");
        return EXIT_FAILURE;
    }

    // Création de la mémoire partagée
    int shmid = shmget(key, sizeof(data_t) * TAILLE_TAMPON + sizeof(int) * 2, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Erreur dans shmget");
        return EXIT_FAILURE;
    }

    // Attachement de la mémoire partagée
    char *segment = (char *)shmat(shmid, NULL, 0);
    if (segment == (void *)-1) {
        perror("Erreur dans shmat");
        return EXIT_FAILURE;
    }

    // Initialisation de la structure `file_t`
    file_t f;
    f.buf = (data_t *)segment;                           // Tampon
    f.debut = (int *)(segment + sizeof(data_t) * TAILLE_TAMPON); // Début
    f.fin = f.debut + 1;                                  // Fin

    *(f.debut) = 0; // Initialisation de début
    *(f.fin) = 0;   // Initialisation de fin

    // Création et initialisation des sémaphores
    int sem_id = create_semaphore(key, NOMBRE_SEMAPHORES, IPC_CREAT | 0666);
    initialize_semaphore(sem_id, 0, 0);          // plein (produits disponibles)
    initialize_semaphore(sem_id, 1, TAILLE_TAMPON); // vide (emplacements disponibles)

    // Création des processus
    pid_t pid = fork();
    if (pid == -1) {
        perror("Erreur dans fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        // Processus fils (producteur)
        for (int i = 0; i < 10; i++) {
            data_t d;
            d.a = i;
            d.b = i * 2;
            snprintf(d.mot, sizeof(d.mot), "P%d", i);

            P(sem_id, 1); // Attendre un emplacement vide
            deposer(f, d);
            printf("[Producteur] Déposé : %d, %d, %s\n", d.a, d.b, d.mot);
            V(sem_id, 0); // Signaler un produit disponible

            sleep(1); // Simulation de temps de production
        }
        exit(EXIT_SUCCESS);
    } else {
        // Processus parent (consommateur)
        for (int i = 0; i < 10; i++) {
            P(sem_id, 0); // Attendre un produit disponible
            data_t d = retirer(f);
            printf("[Consommateur] Retiré : %d, %d, %s\n", d.a, d.b, d.mot);
            V(sem_id, 1); // Signaler un emplacement vide

            sleep(2); // Simulation de temps de consommation
        }

        // Attendre la fin du processus fils
        wait(NULL);

        // Détachement et suppression de la mémoire partagée
        shmdt(segment);
        shmctl(shmid, IPC_RMID, NULL);

        // Suppression des sémaphores
        delete_semaphore(sem_id);

        printf("Simulation terminée.\n");
    }

    return EXIT_SUCCESS;
}
