//
// Created by Ariane Stephan on 2019-04-09.
//


#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <string.h>
#include <semaphore.h>
//#include "printf.h"
#include <stdlib.h>


typedef struct file_s file_s;
typedef struct en_tete_s en_tete_s;
typedef struct message_texte_s message_texte_s;
typedef struct MESSAGE MESSAGE;
typedef enum ouverture_e ouverture_e;


enum ouverture_e {
    READ,
    WRITE,
    REWR,
    DECO
};

struct MESSAGE {
    ouverture_e type_ouverture;
    file_s *file;
    char *nom;

};


struct message_texte_s {
    int nbOctet;
    void *texte;
};

struct en_tete_s {

    size_t longueur_max;
    size_t capacite;
    size_t nombre_message;
    size_t capacite_message_s_utilisee;
    int first;
    int last;
    sem_t semaphore_lecture;
    sem_t semaphore_ecriture;
    sem_t semaphore_memoire;
    sem_t semaphore_signal;
    sem_t semaphore_info_processus;


    int nombre_processus;
    pid_t pid_signal;
    int signal;
    int recu;
};

struct file_s {
    en_tete_s en_tete;

};

void initialiser_message(MESSAGE *message, ouverture_e ouverture, file_s *pointeur, char *n);
void changer_ouverture(MESSAGE *message, ouverture_e ouverture);


#endif
