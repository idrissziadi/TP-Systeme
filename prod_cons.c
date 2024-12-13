#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include "semaphore.h"

#define NOMBRE_SEMAPHORES 2
#define TAILLE_TAMPON 5 // Nombre maximum d'éléments dans le tampon
#define TAILLE_BUFFER 5
// Tampon circulaire
char tampon[TAILLE_TAMPON][TAILLE_BUFFER];
int debut = 0; // Indice du prochain élément à consommer
int fin = 0;   // Indice du prochain emplacement à produire

void produire(char *produit) {
    snprintf(produit, TAILLE_BUFFER, "Produit_%d", rand() % 100);
}

void deposer(char *produit) {
    strcpy(tampon[fin], produit);
    fin = (fin + 1) % TAILLE_TAMPON; // Tampon circulaire
}

void retirer(char *produit) {
    strcpy(produit, tampon[debut]);
    debut = (debut + 1) % TAILLE_TAMPON; // Tampon circulaire
}

void consommer(char *produit) {
    printf("Consommation : %s\n", produit);
}

int main() {
    srand(getpid());

    // Clé unique pour les sémaphores
    key_t key = ftok("monfichier", 'A');
    if (key == -1) {
        perror("Erreur dans ftok");
        return EXIT_FAILURE;
    }

    // Création des sémaphores : 0 = plein, 1 = vide
    int sem_id = create_semaphore(key, NOMBRE_SEMAPHORES, IPC_CREAT | 0666);

    // Initialisation des sémaphores
    initialize_semaphore(sem_id, 0, 0);          // plein (nombre d'éléments disponibles) = 0
    initialize_semaphore(sem_id, 1, TAILLE_TAMPON); // vide (nombre de places disponibles) = taille du tampon

    // Création des processus
    pid_t pid1 = fork(); // Producteur
    if (pid1 == 0) {
        char produit[TAILLE_BUFFER];
        while (1) {
            produire(produit);
            P(sem_id, 1); // Attendre un espace vide
            deposer(produit);
            printf("[Producteur] Produit déposé : %s\n", produit);
            V(sem_id, 0); // Signaler qu'un produit est disponible
            sleep(1); // Simulation du temps de production
        }
        exit(0);
    }

    pid_t pid2 = fork(); // Consommateur
    if (pid2 == 0) {
        char produit[TAILLE_BUFFER];
        while (1) {
            P(sem_id, 0); // Attendre un produit disponible
            retirer(produit);
            consommer(produit);
            V(sem_id, 1); // Signaler qu'un espace vide est disponible
            sleep(2); // Simulation du temps de consommation
        }
        exit(0);
    }

    // Attente de la fin des processus
    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }

    // Suppression des sémaphores
    delete_semaphore(sem_id);

    printf("Tous les processus sont terminés.\n");
    return EXIT_SUCCESS;
}
