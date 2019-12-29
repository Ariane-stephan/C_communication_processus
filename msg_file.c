//
// Created by Ariane Stephan on 2019-04-09.
//



#include "msg_file.h"

/*******************************msg_connect******************************/

MESSAGE *msg_connect(const char *nom, int options, ...) {
    signal_attente = 0;
    char *nom_message;
    /*   sigset_t set;
       sigfillset(&set);
       sigdelset(&set, SIGUSR1);
       sigprocmask(SIG_BLOCK,&set, NULL); */ //pas tres bonne idee
    en_tete_s en_tete;
    file_s *file;


    MESSAGE *message = malloc(sizeof(MESSAGE));
    ouverture_e ouverture;

    if (nom == NULL) {
        ouverture = REWR;
        nom_message = malloc(sizeof(char));
        strcpy(nom_message, "");
        va_list ap; //création du pointeur
        va_start(ap, options);
        size_t nb_msg = va_arg(ap, size_t);
        size_t len_max = va_arg(ap, size_t);
        va_end(ap);

        en_tete.longueur_max = len_max;
        en_tete.capacite = nb_msg;
        en_tete.nombre_message = 0;
        en_tete.nombre_processus = 1;

        en_tete.capacite_message_s_utilisee = 0;
        en_tete.pid_signal = 0;


        /***********SEMAPHORES************/

        sem_destroy(&en_tete.semaphore_lecture);
        sem_destroy(&en_tete.semaphore_ecriture);
        sem_destroy(&en_tete.semaphore_memoire);
        sem_destroy(&en_tete.semaphore_signal);

        sem_init(&en_tete.semaphore_ecriture, 1, 1);
        sem_init(&en_tete.semaphore_memoire, 1, 1);
        sem_init((&en_tete.semaphore_lecture), 1, 1);
        sem_init((&en_tete.semaphore_signal), 1, 1);



        /**********shm_open************/


        /************mmap**************/

        void *mmaped = mmap(0, sizeof(en_tete_s) + (len_max + sizeof(size_t)) * nb_msg, PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        printf("Adresse attachement %p\n", mmaped);

        if (mmaped == MAP_FAILED) {
            perror("processus: mmap");
            exit(1);
        }


        file = (file_s *) mmaped;


        file->en_tete = en_tete;


        file->en_tete.last = sizeof(en_tete);
        file->en_tete.first = sizeof(en_tete);
        file->en_tete.recu = 0;


    } else {
        nom_message = malloc(sizeof(char) * (strlen(nom)) + 1);

        strcpy(nom_message, "/");
        strcat(nom_message, nom);
        if ((options & O_RDWR) != 0 && (options & O_RDONLY) == 0 && (options & O_WRONLY) == 0) {
            printf("REWR\n");
            ouverture = REWR; //2
            printf("2\n");
        } else if ((options & O_RDONLY) == 0 && (options & O_RDWR) == 0 && (options & O_WRONLY) == 0) {
            printf("READ\n");
            ouverture = READ; //0
            printf("3 %d\n", ouverture);

        } else if ((options & O_WRONLY) != 0 && (options & O_RDONLY) == 0 && (options & O_RDWR) == 0) {
            printf("WRITE\n");
            ouverture = WRITE; //1
            printf("4\n");

        } else {
            perror("Mauvais augument pour msg_connect");
            exit(1);
        }


        if ((options & O_CREAT) != 0) { //Creation de la file

            va_list ap; //création du pointeur
            va_start(ap, options);
            size_t nb_msg = va_arg(ap, size_t);
            size_t len_max = va_arg(ap, size_t);
            va_end(ap);

            en_tete.longueur_max = len_max;
            en_tete.capacite = nb_msg;
            en_tete.nombre_message = 0;
            en_tete.nombre_processus = 1;

            en_tete.capacite_message_s_utilisee = 0;


            /***********SEMAPHORES************/

            sem_destroy(&en_tete.semaphore_lecture);
            sem_destroy(&en_tete.semaphore_ecriture);
            sem_destroy(&en_tete.semaphore_memoire);
            sem_destroy(&en_tete.semaphore_signal);
            sem_destroy(&en_tete.semaphore_info_processus);

            sem_init(&en_tete.semaphore_ecriture, 1, 1);
            sem_init(&en_tete.semaphore_memoire, 1, 1);
            sem_init((&en_tete.semaphore_lecture), 1, 1);
            sem_init((&en_tete.semaphore_signal), 1, 1);
            sem_init((&en_tete.semaphore_info_processus), 1, 1);



            /**********shm_open************/

            int shm;
            if ((options & O_EXCL) != 0) {
                shm = shm_open(nom_message, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

            } else {
                shm = shm_open(nom_message, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
            }

            if (shm == -1) {
                perror("processus: shm_open");
                return NULL;
              //  exit(1);
            }
            if (ftruncate(shm, sizeof(en_tete_s) + (len_max + sizeof(size_t)) * nb_msg) == -1) {
                if (errno == EINVAL)
                    perror("processus: ftruncate");
                return NULL;
            }

            /************mmap**************/

            void *mmaped = mmap(NULL, sizeof(en_tete_s) + (len_max + sizeof(size_t)) * nb_msg, PROT_WRITE | PROT_READ,
                                MAP_SHARED, shm,
                                0);
            printf("Adresse attachement %p\n", mmaped);

            if (mmaped == MAP_FAILED) {
                perror("processus: mmap");
                return NULL;
            }


            file = (file_s *) mmaped;


            file->en_tete = en_tete;


            file->en_tete.last = sizeof(en_tete);
            file->en_tete.first = sizeof(en_tete);
            file->en_tete.recu = 0;
            file->en_tete.pid_signal = 0;


        } else { //si on rejoint la file

            /*********shm_open***********/


            int shm = shm_open(nom_message, O_RDWR, S_IRUSR | S_IWUSR);

            if (shm == -1) {
                perror("processus: shm_open");
                return NULL;
            }

            struct stat statbuf;
            if (fstat(shm, &statbuf) == -1) {
                perror("erreur de fstat");
                return NULL;
            }

            size_t length = (size_t) statbuf.st_size;

            /**************mmap********/

            void *mmaped = mmap(NULL, length, PROT_WRITE | PROT_READ, MAP_SHARED, shm, 0);

            //   printf("Adresse attachement %p\n", mmaped);

            if (mmaped == MAP_FAILED) {
                perror("processus: mmap");
                return NULL;
            }

            file = (file_s *) mmaped;

            if (sem_wait(&file->en_tete.semaphore_info_processus) < 0) {
                perror("Erreur de sem_wait");
                return NULL;
            }
            file->en_tete.nombre_processus++;

            if (sem_post(&file->en_tete.semaphore_info_processus) < 0) {
                perror("Erreur de sem_wait");
                return NULL;
            }
        }

    }
    initialiser_message(message, ouverture, file, nom_message);


    return message;
}


int msg_disconnect(MESSAGE *file) {
    if (sem_wait(&file->file->en_tete.semaphore_info_processus) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }


    if (file->file->en_tete.nombre_processus <= 0) {
        if (sem_post(&file->file->en_tete.semaphore_info_processus) < 0) {
            perror("Erreur de sem_wait");
            exit(1);
        }
        return -1;
    }

    file->file->en_tete.nombre_processus--;
    ouverture_e o = DECO;
    changer_ouverture(file, o);
    if (file->file->en_tete.nombre_processus == 0) {
        msg_unlink(file->nom);
    }
    if (sem_post(&file->file->en_tete.semaphore_info_processus) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }
    return 0;
}


int msg_unlink(const char *nom) {
    if (shm_unlink(nom) == -1) {
        //METTRE ERREUR
        return -1;
    }
    return 0;
}

/*************************msg_send***************************************/


int msg_send(MESSAGE *file, const void *msg, size_t len) {

    printf("Je vais SEND\n");

    if (file->type_ouverture == READ || file->type_ouverture == DECO) { // rdwr ou write //SI C'EST EGAL A 0 OU READ
        printf("Je ne suis pas autorisé à ecrire\n");
        printf("Je n'ai pas pu SEND\n");

        return -1;
    }




    /************Test si notre message n'est pas plus grand que la longueur maximal d'un message******/

    if (len > file->file->en_tete.longueur_max) {

        errno = EMSGSIZE;
        printf("Je n'ai pas pu SEND\n");

        return -1;

    }
    if (sem_wait(&file->file->en_tete.semaphore_ecriture) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }



    while (file->file->en_tete.capacite * (file->file->en_tete.longueur_max + sizeof(size_t)) <
           file->file->en_tete.capacite_message_s_utilisee + len + sizeof(size_t)) {
    }


    if (sem_wait(&file->file->en_tete.semaphore_memoire) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }


    file->file->en_tete.nombre_message++;
    file->file->en_tete.capacite_message_s_utilisee += (len + sizeof(size_t));


    char *p1 = (char *) (file->file) + file->file->en_tete.last;
    char *p = (char *) (file->file) + file->file->en_tete.last + sizeof(size_t);


    file->file->en_tete.last = (int) (file->file->en_tete.last + len + sizeof(size_t));



    // printf("Adresse de copie %p\n", p);

    memcpy(p1, &len, sizeof(size_t));

    memcpy(p, msg, len);


    msync(file->file,
          sizeof(en_tete_s) + (file->file->en_tete.longueur_max + sizeof(size_t)) * file->file->en_tete.capacite,
          MS_SYNC);


    if (sem_post(&file->file->en_tete.semaphore_memoire) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }

    if (sem_post(&file->file->en_tete.semaphore_ecriture) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }



    if (sem_wait(&file->file->en_tete.semaphore_signal) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }
    if (file->file->en_tete.pid_signal != 0 && sem_trywait(&file->file->en_tete.semaphore_lecture) == 0) {

        kill(file->file->en_tete.pid_signal, file->file->en_tete.signal);
        if (sem_post(&file->file->en_tete.semaphore_lecture) < 0) {
            perror("Erreur de sem_wait");
            exit(1);
        }
    }


    if (sem_post(&file->file->en_tete.semaphore_signal) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }


    printf("J'ai SEND\n");

    return 0;
}


int msg_trysend(MESSAGE *file, const void *msg, size_t len) { // à finir avec les semaphores

    printf("Je vais SEND\n");


    if (file->type_ouverture == READ || file->type_ouverture == DECO) { // rdwr ou write //SI C'EST EGAL A 0 OU READ
        printf("Je ne suis pas autorisé à ecrire\n");
        printf("Je n'ai pas pu SEND\n");

        return -1;
    }


    /************Test si notre message n'est pas plus grand que la longueur maximal d'un message******/

    if (len > file->file->en_tete.longueur_max) {

        errno = EMSGSIZE;
        printf("Je n'ai pas pu SEND\n");


        return -1;

    }
    if (sem_wait(&file->file->en_tete.semaphore_memoire) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }
    if (file->file->en_tete.capacite * (file->file->en_tete.longueur_max + sizeof(size_t)) <
        file->file->en_tete.capacite_message_s_utilisee + len + sizeof(size_t)) {
        errno = EAGAIN;

        if (sem_post(&file->file->en_tete.semaphore_memoire) < 0) {
            perror("Erreur de sem_wait");
            exit(1);
        }
        printf("Je n'ai pas pu SEND\n");

        return -1;
    }
    if (sem_wait(&file->file->en_tete.semaphore_ecriture) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }


    /*********on augmente le nombre de message et la capacité ************/


    file->file->en_tete.nombre_message++;
    file->file->en_tete.capacite_message_s_utilisee += (len + sizeof(size_t));


    /********on initialise notre first après la structure en tete*********/





/*******On copie le message***********/



    char *p1 = (char *) (file->file) + file->file->en_tete.last;
    char *p = (char *) (file->file) + file->file->en_tete.last + sizeof(size_t);
    file->file->en_tete.last = (int) (file->file->en_tete.last + len + sizeof(size_t));



    if (sem_post(&file->file->en_tete.semaphore_ecriture) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }


    memcpy(p1, &len, sizeof(size_t));
    memcpy(p, msg, len);


    msync(file->file,
          sizeof(en_tete_s) + (file->file->en_tete.longueur_max + sizeof(size_t)) * file->file->en_tete.capacite,
          MS_SYNC);

    if (sem_post(&file->file->en_tete.semaphore_memoire) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }

    if (sem_wait(&file->file->en_tete.semaphore_signal) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }
    if (file->file->en_tete.pid_signal != 0 && sem_trywait(&file->file->en_tete.semaphore_lecture) == 0) {

        kill(file->file->en_tete.pid_signal, file->file->en_tete.signal);
        if (sem_post(&file->file->en_tete.semaphore_lecture) < 0) {
            perror("Erreur de sem_wait");
            exit(1);
        }
    }


    if (sem_post(&file->file->en_tete.semaphore_signal) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }


    printf("J'ai SEND\n");


    return 0;
}

int msg_send_multi(MESSAGE *file, void *msg[], int nb_msg) { //à finir avec les semaphores

    printf("Je vais SEND\n");


    if (file->type_ouverture == READ || file->type_ouverture == DECO) { // rdwr ou write //SI C'EST EGAL A 0 OU READ
        printf("Je ne suis pas autorisé à ecrire\n");
        printf("Je n'ai pas pu SEND\n");

        return -1;
    }

    int i;
    size_t capacite = 0;
    for (i = 0; i < nb_msg; i++) {
        if (strlen(msg[i]) > file->file->en_tete.longueur_max) {

            errno = EMSGSIZE;
            printf("Je n'ai pas pu SEND\n");

            return -1;

        }
        capacite += strlen(msg[i]) + sizeof(size_t);

    }




    if (sem_wait(&file->file->en_tete.semaphore_ecriture) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }


    /************Test si notre message n'est pas plus grand que la longueur maximal d'un message******/


    if (file->file->en_tete.capacite * (file->file->en_tete.longueur_max + sizeof(size_t)) <
        file->file->en_tete.capacite_message_s_utilisee + capacite) {

        errno = EAGAIN;
        if (sem_post(&file->file->en_tete.semaphore_ecriture) < 0) {
            perror("Erreur de sem_wait");
            exit(1);
        }

        printf("Je n'ai pas pu SEND\n");

        return -1;
    }

    if (sem_wait(&file->file->en_tete.semaphore_memoire) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }



    /*********on augmente le nombre de message et la capacité ************/


    file->file->en_tete.nombre_message += nb_msg;
    file->file->en_tete.capacite_message_s_utilisee += capacite;




/*******On copie le message***********/




    for (i = 0; i < nb_msg; i++) {
        char *p1 = (char *) (file->file) + file->file->en_tete.last;
        char *p = (char *) (file->file) + file->file->en_tete.last + sizeof(size_t);
        file->file->en_tete.last = (int) (file->file->en_tete.last + strlen(msg[i]) + sizeof(size_t));

        size_t taille = strlen(msg[i]);

        memcpy(p1, &taille, sizeof(size_t));
        memcpy(p, msg[i], taille);


        msync(file->file,
              sizeof(en_tete_s) + (file->file->en_tete.longueur_max + sizeof(size_t)) * file->file->en_tete.capacite,
              MS_SYNC);


    }

    if (sem_post(&file->file->en_tete.semaphore_ecriture) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }

    if (sem_post(&file->file->en_tete.semaphore_memoire) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }





    if (sem_wait(&file->file->en_tete.semaphore_signal) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }
    if (file->file->en_tete.pid_signal != 0 && sem_trywait(&file->file->en_tete.semaphore_lecture) == 0) {

        kill(file->file->en_tete.pid_signal, file->file->en_tete.signal);
        if (sem_post(&file->file->en_tete.semaphore_lecture) < 0) {
            perror("Erreur de sem_wait");
            exit(1);
        }
    }


    if (sem_post(&file->file->en_tete.semaphore_signal) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }


    printf("J'ai SEND\n");

    return 0;
}


/*************************msg_receive / msg_tryreceive**********************/


ssize_t msg_receive(MESSAGE *file, void *msg, size_t len) {

    printf("Je vais RECEIVE\n");


    if (file->type_ouverture == WRITE || file->type_ouverture == DECO) {
        printf("Je ne suis pas autorisé à lire\n");
        printf("Je n'ai pas pu RECEIVE\n");

        return -1;
    }



    if (len < file->file->en_tete.longueur_max) { //PEUT ETRE ECHANGER AVEC WHILE

        errno = EMSGSIZE;


        printf("Je n'ai pas pu RECEIVE\n");

        return -1;

    }

    if (sem_wait(&file->file->en_tete.semaphore_lecture) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }




    while (file->file->en_tete.nombre_message == 0 || file->file->en_tete.capacite_message_s_utilisee == 0) {
    }


    if (sem_wait(&file->file->en_tete.semaphore_memoire) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }

    file->file->en_tete.nombre_message--;

    char *p1 = ((char *) (file->file) + file->file->en_tete.first);
    char *p = ((char *) (file->file) + file->file->en_tete.first + sizeof(size_t));



    size_t taille;




    memcpy(&taille, p1, sizeof(size_t));
    file->file->en_tete.capacite_message_s_utilisee -= (taille + sizeof(size_t));


    memcpy(msg, p, taille);


    memmove((char *) (file->file) + file->file->en_tete.first,
            (char *) (file->file) + file->file->en_tete.first + (taille + sizeof(size_t)),
            file->file->en_tete.capacite_message_s_utilisee);


    file->file->en_tete.last = sizeof(en_tete_s) + file->file->en_tete.capacite_message_s_utilisee;



    if (sem_post(&file->file->en_tete.semaphore_memoire) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }

    if (sem_post(&file->file->en_tete.semaphore_lecture) < 0) { //ICI
        perror("Erreur de sem_wait");
        exit(1);
    }

    printf("J'ai RECEIVE\n");
    if(signal_attente == 1){
        signal_attente = 0;
        file->file->en_tete.pid_signal = 0;
    }

    return taille;
}


ssize_t msg_tryreceive(MESSAGE *file, void *msg, size_t len) {

    printf("Je vais RECEIVE\n");

    if (file->type_ouverture == WRITE || file->type_ouverture == DECO) {
        printf("Je ne suis pas autorisé à lire\n");
        printf("Je n'ai pas pu RECEIVE\n");

        return -1;
    }


    if (sem_wait(&file->file->en_tete.semaphore_lecture) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }

    if (len < file->file->en_tete.longueur_max) {
        errno = EMSGSIZE;
        if (sem_post(&file->file->en_tete.semaphore_lecture) < 0) {
            perror("Erreur de sem_wait");
            exit(1);
        }
        printf("Je n'ai pas pu RECEIVE\n");

        return -1;

    }

    if (file->file->en_tete.capacite_message_s_utilisee == 0) {

        errno = EAGAIN;
        if (sem_post(&file->file->en_tete.semaphore_lecture) < 0) {
            perror("Erreur de sem_wait");
            exit(1);
        }
        printf("Je n'ai pas pu RECEIVE\n");

        return -1;
    }

    file->file->en_tete.nombre_message--;


    char *p1 = ((char *) (file->file) + file->file->en_tete.first);
    char *p = ((char *) (file->file) + file->file->en_tete.first + sizeof(size_t));

    if (sem_post(&file->file->en_tete.semaphore_lecture) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }
    size_t taille;

    if (sem_wait(&file->file->en_tete.semaphore_memoire) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }

    memcpy(&taille, p1, sizeof(size_t));
    file->file->en_tete.capacite_message_s_utilisee -= (taille + sizeof(size_t));


    memcpy(msg, p, taille);


    file->file->en_tete.last -= (taille + sizeof(size_t));

    memmove((char *) (file->file) + file->file->en_tete.first,
            (char *) (file->file) + file->file->en_tete.first + (taille + sizeof(size_t)),
            sizeof(en_tete_s) + (file->file->en_tete.longueur_max + sizeof(size_t)) * file->file->en_tete.capacite);

    file->file->en_tete.last = sizeof(en_tete_s) + file->file->en_tete.capacite_message_s_utilisee;




    if (sem_post(&file->file->en_tete.semaphore_memoire) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }
    printf("J'ai RECEIVE\n");

    return taille;
}

ssize_t msg_receive_multi(MESSAGE *file, void *msg[], int nb_msg, size_t len) {

    printf("Je vais RECEIVE\n");


    if (file->type_ouverture == WRITE || file->type_ouverture == DECO) {
        printf("Je ne suis pas autorisé à lire\n");
        printf("Je n'ai pas pu RECEIVE\n");

        return -1;
    }


    if (sem_wait(&file->file->en_tete.semaphore_lecture) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }


    size_t capacite = 0;
    if (len < file->file->en_tete.longueur_max) {

        errno = EMSGSIZE;
        if (sem_post(&file->file->en_tete.semaphore_lecture) < 0) {
            perror("Erreur de sem_wait");
            exit(1);
        }
        printf("Je n'ai pas pu RECEIVE\n");

        return -1;

    }

    if (sem_wait(&file->file->en_tete.semaphore_memoire) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }
    int i;
    for (i = 0; i < nb_msg; i++) {

        if (file->file->en_tete.nombre_message == 0 || file->file->en_tete.capacite_message_s_utilisee == 0) { //modif faite dimanche
            if (sem_post(&file->file->en_tete.semaphore_lecture) < 0) {
                perror("Erreur de sem_wait");
                exit(1);
            }
            if (sem_post(&file->file->en_tete.semaphore_memoire) < 0) {
                perror("Erreur de sem_wait");
                exit(1);
            }

            printf("Cpa %zu\n", capacite);
            return capacite;
        }
        file->file->en_tete.nombre_message--;

        char *p1 = ((char *) (file->file) + file->file->en_tete.first);
        char *p = ((char *) (file->file) + file->file->en_tete.first + sizeof(size_t));


        size_t taille;

        memcpy(&taille, p1, sizeof(size_t));
        file->file->en_tete.capacite_message_s_utilisee -= (taille + sizeof(size_t));

        capacite += taille + sizeof(size_t);
        memcpy(msg[i], p, taille);

        file->file->en_tete.last -= (taille + sizeof(size_t));

        memmove((char *) (file->file) + file->file->en_tete.first,
                (char *) (file->file) + file->file->en_tete.first + (taille + sizeof(size_t)),
                sizeof(en_tete_s) + (file->file->en_tete.longueur_max + sizeof(size_t)) * file->file->en_tete.capacite);

        file->file->en_tete.last = sizeof(en_tete_s) + file->file->en_tete.capacite_message_s_utilisee;
    }


    if (sem_post(&file->file->en_tete.semaphore_lecture) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }
    if (sem_post(&file->file->en_tete.semaphore_memoire) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }
    printf("J'ai RECEIVE\n");


    return capacite;
}


int annuler_enregistrement(MESSAGE *file, pid_t pid) {

    if (sem_wait(&file->file->en_tete.semaphore_signal) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }
    if(pid == file->file->en_tete.pid_signal) {

        file->file->en_tete.pid_signal = 0;
        if (sem_post(&file->file->en_tete.semaphore_signal) < 0) {
            perror("Erreur de sem_post");
            exit(1);
        }
        return 0;
    }
    if (sem_post(&file->file->en_tete.semaphore_signal) < 0) {
        perror("Erreur de sem_post");
        exit(1);
    }
    return -1;

}



int fonction_signalTest(MESSAGE *m){

    if (sem_wait(&m->file->en_tete.semaphore_signal) < 0) {
        perror("Erreur de sem_wait");
        exit(1);
    }
    if(m->file->en_tete.pid_signal == 0){
        m->file->en_tete.pid_signal = getpid();
        m->file->en_tete.signal = SIGUSR1;
        struct sigaction act;
        act.sa_handler = handler_change_variable_signal;
        sigfillset(&act.sa_mask);
        act.sa_flags = 0;
        if(sigaction(m->file->en_tete.signal, &act, NULL) == -1)
            return -1;


        if (sem_post(&m->file->en_tete.semaphore_signal) < 0) {
            perror("Erreur de sem_post");
            exit(1);
        }
        printf("J'ai demandé un signal\n");
        return 0;

    }
    if (sem_post(&m->file->en_tete.semaphore_signal) < 0) {
        perror("Erreur de sem_post");
        exit(1);
    }
    return -1;
}

static void handler_change_variable_signal(int sig){
    printf("J'ai reçu un signal\n");
    signal_attente = 1;
}


/***************************methode etat file***************************/

size_t msg_message_size(MESSAGE *m) {

    if (m->type_ouverture == DECO) {
        return (size_t) -1;
    }

    return m->file->en_tete.longueur_max;
}


size_t msg_capacite(MESSAGE *m) {
    if (m->type_ouverture == DECO) {
        return (size_t) -1;
    }
    return m->file->en_tete.capacite;
}


size_t msg_nb(MESSAGE *m) {
    if (m->type_ouverture == DECO) {
        return (size_t) -1;
    }
    return m->file->en_tete.nombre_message;

}
