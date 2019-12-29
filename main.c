//
// Created by Ariane Stephan on 2019-04-02.
//

#include <string.h>
#include <semaphore.h>
//#include "printf.h"
#include <stdlib.h>
#include "msg_file.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <errno.h>


/*********test infini*************/


void test_1(int argc, char *argv[]) { //NORMALEMENT OK
    if (argc == 2 && strcmp(argv[1], "1") == 0) {
        void *test = malloc(sizeof(void) * 5);
        void *tab[] = {"test", "passe"};

        MESSAGE *m1 = msg_connect("testInfini", O_RDWR | O_CREAT, 5, 5);
        if (m1 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");

            while (1) {
                msg_send_multi(m1, tab, 2);

            }
        }

    } else if (argc == 2 && strcmp(argv[1], "2") == 0) {

        MESSAGE *m2 = msg_connect("testInfini", O_RDWR);
        printf("J'ai rejoins file\n");
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            while (1) {
                msg_send(m2, "tata", strlen("tata"));

            }
        }
    } else if (argc == 2 && strcmp(argv[1], "3") == 0) {
        void *test = malloc(sizeof(void) * 5);

        MESSAGE *m2 = msg_connect("testInfini", O_RDWR);
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            void *tab[4];

            tab[0] = malloc(sizeof(char) * 6);
            tab[1] = malloc(sizeof(char) * 6);


            while (1) {
                if (msg_receive(m2, test, 5) > 0) {

                    printf("Voici le message dans le main %s\n", (char *) test);

                    free(test);
                    test = malloc(sizeof(char) * 5);
                }
            }
        }
    } else if (argc == 2 && strcmp(argv[1], "4") == 0) {

        MESSAGE *m2 = msg_connect("testInfini", O_RDWR);
        void *tab[4];

        tab[0] = malloc(sizeof(char) * 6);
        tab[1] = malloc(sizeof(char) * 6);
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            while (1) {
                if (msg_receive_multi(m2, tab, 2, 6) > 0) {

                    printf("Voici le message dans le main %s\n", (char *) tab[0]);

                    printf("Voici le message dans le main %s\n", (char *) tab[1]);

                    memset(tab[0], ' ', 6);
                    memset(tab[1], ' ', 6);
                }
            }
        }
    }

}

/***********test sur mode ouverture lecture et ecriture seule*************/

void test_2(int argc, char *argv[]) { //Normalement ok
    if (argc == 2 && strcmp(argv[1], "1") == 0) {
        void *test = malloc(sizeof(void) * 5);
        void *tab[] = {"Salut", "Bonjour"};

        MESSAGE *m1 = msg_connect("testOuverture", O_RDWR | O_CREAT, 10, 10);
        if (m1 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            msg_send(m1, "Oui", strlen("Oui"));

            sleep(20);

            msg_disconnect(m1);
        }

    } else if (argc == 2 && strcmp(argv[1], "2") == 0) {

        MESSAGE *m2 = msg_connect("testOuverture", O_RDONLY);
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            msg_send(m2, "Super", strlen("Super")); //ne doit pas marcher car O_RDONLY

            sleep(20);

            msg_disconnect(m2);
        }

    } else if (argc == 2 && strcmp(argv[1], "3") == 0) {
        void *test = malloc(sizeof(void) * 5);

        MESSAGE *m2 = msg_connect("testOuverture", O_WRONLY);
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            void *tab[4];

            tab[0] = malloc(sizeof(char) * 6);
            tab[1] = malloc(sizeof(char) * 6);


            if (msg_receive(m2, test, 5) > 0) { //ne doit pas marcher car O_WRONLY

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }

            sleep(20);

            msg_disconnect(m2);
        }

    } else if (argc == 2 && strcmp(argv[1], "4") == 0) {
        void *test = malloc(sizeof(void) * 5);

        MESSAGE *m2 = msg_connect("testOuverture", O_RDWR);
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            if (msg_receive(m2, test, 10) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }


            sleep(20);

            msg_disconnect(m2);
        }

    }
}

/***********test sur O_EXCL empeche execution si file existe***************/

void test_3(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "1") == 0) {
        void *test = malloc(sizeof(void) * 10);
        void *tab[] = {"Salut", "Bonjour"};

        MESSAGE *m1 = msg_connect("testOption", O_RDWR | O_CREAT, 10, 10);
        if (m1 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");

            msg_send(m1, "Oui", strlen("Oui"));

            sleep(20);

            msg_disconnect(m1);
        }

    } else if (argc == 2 && strcmp(argv[1], "2") == 0) {
        void *test = malloc(sizeof(void) * 10);

        MESSAGE *m2 = msg_connect("testOption", O_CREAT | O_RDWR | O_EXCL, 10, 10); //ne doit pas marcher
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");

            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 10);
            }
            sleep(20);

            msg_disconnect(m2);

        }
    } else if (argc == 2 && strcmp(argv[1], "3") == 0) {
        void *test = malloc(sizeof(void) * 5);

        MESSAGE *m2 = msg_connect("testOption", O_RDWR);
        void *tab[4];

        tab[0] = malloc(sizeof(char) * 6);
        tab[1] = malloc(sizeof(char) * 6);

        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            if (msg_receive(m2, test, 10) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }

            sleep(20);

            msg_disconnect(m2);

        }
    }
}

/**********test trySend*****************/


int test_4(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "1") == 0) {

        MESSAGE *m1 = msg_connect("testTry", O_RDWR | O_CREAT, 5, 5);
        if (m1 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            msg_send(m1, "Oui", strlen("Oui"));
            msg_send(m1, "Non", strlen("Non"));
            msg_send(m1, "Sauf", strlen("Sauf"));
            msg_send(m1, "Ah", strlen("Ah"));
            msg_send(m1, "Mais", strlen("Mais"));
            msg_send(m1, "Chat",
                     strlen("Chat")); //doit bloquer car file pleine mais à la fin doit afficher car place se libere

            sleep(20);

            msg_disconnect(m1);
        }

    } else if (argc == 2 && strcmp(argv[1], "2") == 0) { //essaye d'envoyer sans succes

        MESSAGE *m2 = msg_connect("testTry", O_RDWR);
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            if (msg_trysend(m2, "Super", strlen("Super")) == -1) { //ne doit pas bloquer
                fprintf(stderr, "Impossible de send %s\n", strerror(errno));
            }
          //  sleep(20);

            msg_disconnect(m2);
        }

    } else if (argc == 2 && strcmp(argv[1], "3") == 0) { //lit deux messages et libere place
        void *test = malloc(sizeof(void) * 5);

        MESSAGE *m2 = msg_connect("testTry", O_RDWR);
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            void *tab[4];

            tab[0] = malloc(sizeof(char) * 6);
            tab[1] = malloc(sizeof(char) * 6);


            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }

            sleep(20);

            msg_disconnect(m2);
        }

    } else if (argc == 2 && strcmp(argv[1], "4") == 0) { //send avec trysend et lit les messages
        void *test = malloc(sizeof(void) * 5);

        MESSAGE *m2 = msg_connect("testTry", O_RDWR);
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            if (msg_trysend(m2, "Super", strlen("Super")) == -1) { //doit marcher cette fois
                fprintf(stderr, "Impossible de send %s\n", strerror(errno));
            }

            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            sleep(20);

            msg_disconnect(m2);
        }

    }
}


/**********test tryReceive*****************/


int test_5(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "1") == 0) { //envoie un message

        MESSAGE *m1 = msg_connect("testTryReceive", O_RDWR | O_CREAT, 5, 5);
        if (m1 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            msg_send(m1, "Oui", strlen("Oui"));

            sleep(20);

            msg_disconnect(m1);
        }

    } else if (argc == 2 && strcmp(argv[1], "2") == 0) { //premier try receive marche, deuxieme non
        void *test = malloc(sizeof(void) * 5);

        MESSAGE *m2 = msg_connect("testTryReceive", O_RDWR);
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            if (msg_tryreceive(m2, test, 5) == -1) { //doit marcher cette fois
                fprintf(stderr, "Impossible de receive %s\n", strerror(errno));
            } else {
                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);

                test = malloc(sizeof(char) * 5);
            }
        }
        if (msg_tryreceive(m2, test, 5) == -1) { //ne doit pas marcher cette fois
            fprintf(stderr, "Impossible de receive %s\n", strerror(errno));
        } else {
            printf("Voici le message dans le main %s\n", (char *) test);

            free(test);

            test = malloc(sizeof(char) * 5);
        }

        if (msg_receive(m2, test, 5) > 0) { //doit bloquer puis recevoir Oui

            printf("Voici le message dans le main %s\n", (char *) test);

            free(test);
            test = malloc(sizeof(char) * 5);
        }
        sleep(20);

        msg_disconnect(m2);
    } else if (argc == 2 && strcmp(argv[1], "3") == 0) { //envoie un message

        MESSAGE *m1 = msg_connect("testTryReceive", O_RDWR);
        if (m1 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            msg_send(m1, "Oui", strlen("Oui"));

            sleep(20);

            msg_disconnect(m1);
        }

    }

}


/**************test simple plusieurs send plusieurs receive*************/

void test_6(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "1") == 0) {
        void *test = malloc(sizeof(void) * 5);
        void *tab[] = {"test", "passe"};

        MESSAGE *m1 = msg_connect("testSimple", O_RDWR | O_CREAT, 5, 5);
        if (m1 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");

            msg_send(m1, "salut", strlen("salut"));
            printf("J'ai envoyé salut\n");

            msg_send(m1, "mange", strlen("mange"));
            printf("J'ai envoyé mange\n");

            msg_send(m1, "dormi", strlen("dormi"));
            printf("J'ai envoyé dormi\n");

            msg_send(m1, "franc", strlen("franc"));
            printf("J'ai envoyé franc\n");

            msg_send(m1, "toupi", strlen("toupi"));
            printf("J'ai envoyé toupi\n");

            msg_send(m1, "joies", strlen("joies")); //doit bloquer au debut
            printf("J'ai envoyé joies\n");


            sleep(5);

            msg_send(m1, "natte", strlen("natte"));
            printf("J'ai envoyé natte\n");

            msg_send(m1, "tante", strlen("tante"));
            printf("J'ai envoyé tante\n");

            sleep(20);

            msg_disconnect(m1);
        }

    } else if (argc == 2 && strcmp(argv[1], "2") == 0) {
        void *test = malloc(sizeof(void) * 5);

        MESSAGE *m2 = msg_connect("testSimple", O_RDWR);
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            sleep(5);
            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            msg_send(m2, "lutin", strlen("lutin"));
            printf("J'ai envoyé lutin\n");

            sleep(20);

            msg_disconnect(m2);
        }
    } else if (argc == 2 && strcmp(argv[1], "3") == 0) {
        void *test = malloc(sizeof(void) * 5);

        MESSAGE *m2 = msg_connect("testSimple", O_RDWR);
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");


            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            if (msg_receive(m2, test, 5) > 0) { //bloque car plus rien à lire

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 5);
            }
            sleep(20);

            msg_disconnect(m2);
        }

    }
}

/**********test compacter tableau message*******************/


void test_7(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "1") == 0) {

        MESSAGE *m1 = msg_connect("testCompacter", O_RDWR | O_CREAT, 3, 10);
        if (m1 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            printf("Nombre de message max donné à msg connect %zu\n", m1->file->en_tete.capacite);
            printf("Longueur max %zu\n", m1->file->en_tete.longueur_max);
            printf("Place de la taille %zu\n", sizeof(size_t));

            printf("Capacité maximal %zu\n",
                   (m1->file->en_tete.longueur_max + sizeof(size_t)) * m1->file->en_tete.capacite);

            printf("Normalement, seulement 3 messages sur la file maximum\n");

            msg_send(m1, "ta", 2);
            msg_send(m1, "po", 2);
            msg_trysend(m1, "li", 2);
            msg_send(m1, "ce", 2);
            msg_send(m1, "ma", 2);
            printf("Je suis passé jusqu'ici\n");
            msg_trysend(m1, "ar", 2); //ne va pas passer

            msg_send(m1, "te", 2);

            printf("Je suis passé par là\n");

            sleep(20);

            msg_disconnect(m1);


        }

    } else if (argc == 2 && strcmp(argv[1], "2") == 0) {
        void *test = malloc(sizeof(void) * 10);

        MESSAGE *m2 = msg_connect("testCompacter", O_RDWR);
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            if (msg_receive(m2, test, 10) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 10);
            }
            if (msg_tryreceive(m2, test, 10) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 10);
            }
            if (msg_receive(m2, test, 10) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 10);
            }
            if (msg_receive(m2, test, 10) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 10);

            }
            if (msg_receive(m2, test, 10) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 10);

            }
            if (msg_receive(m2, test, 10) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 10);

            }
            sleep(20);

            msg_disconnect(m2);
        }

    }

}

/************teste les tailles buffer******************/

void test_8(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "1") == 0) {

        MESSAGE *m1 = msg_connect("testBuffer", O_RDWR | O_CREAT, 5, 5);
        if (m1 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            msg_send(m1, "tata", 4);

            msg_send(m1, "bonsoir", strlen("bonsoir")); //ne doit pas marcher


            sleep(20);

            msg_disconnect(m1);
        }

    } else if (argc == 2 && strcmp(argv[1], "2") == 0) {
        void *test = malloc(sizeof(void) * 10);

        MESSAGE *m2 = msg_connect("testBuffer", O_RDWR);
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            if (msg_receive(m2, test, 3) > 0) { //taille buffer trop petite

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 10);


            }
            if (msg_receive(m2, test, 5) > 0) { //marche

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 10);


            }
            if (msg_receive(m2, test, 5) > 0) { //marche pas car un seul send est passé

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 10);


            }
            sleep(20);

            msg_disconnect(m2);
        }
    }
}

/*********file anonyme***************/

void test_9(int argc, char *argv[]) {
    void *test = malloc(sizeof(void) * 10);

    MESSAGE *m = msg_connect(NULL, O_RDWR, 5, 5);

    if (m == NULL) {
        printf("Connexion impossible\n");

    } else {
        write(1, "J'ai rejoins file\n", strlen("J'ai rejoins file\n"));

        pid_t pid = fork();
        if (pid == -1) {
            exit(1);
        } else if (pid == 0) {
            msg_send(m, "fils", strlen("fils"));
            msg_send(m, "papa", strlen("papa"));
            sleep(5);
            msg_send(m, "pere", strlen("pere"));

        } else {
            if (msg_receive(m, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 10);


            }
            if (msg_receive(m, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 10);


            }
            if (msg_receive(m, test, 5) > 0) { //ne reçoit rien au debut car fils a pas encore envoyé

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 10);


            }
            waitpid(pid, NULL, 0);
        }
    }
}

/**********test signaux***********/


void test_10(int argc, char *argv[]) { //quatre versions, avec executable 3 et sans, avec annuler enregistrement et sans

    if (argc == 2 && strcmp(argv[1], "1") == 0) { //attend un signal pour lire puis envoie un msg
        void *test = malloc(sizeof(void) * 10);

        MESSAGE *m1 = msg_connect("testSignaux", O_RDWR | O_CREAT, 5, 5);
        if (m1 == NULL) {
            printf("Connexion impossible\n");
        } else {
            if (m1->file->en_tete.capacite_message_s_utilisee == 0) {//file vide

                if (fonction_signalTest(m1) == 0) {

                    while (signal_attente == 0) {
                           /*  if (annuler_enregistrement(m1, getpid()) == 0) { //se de enregistre
                                 break;
                             }*/
                        sleep(3);
                        //   msg_send(m1, "toto", strlen("toto")); //se debloque lui meme
                    }
                    if (signal_attente == 1) {
                        if (msg_receive(m1, test, 5) > 0) {
                            printf("Voici le message dans le main %s\n", (char *) test);

                            free(test);
                            test = malloc(sizeof(char) * 10);
                        }

                    }
                }
            } else {

                if (msg_receive(m1, test, 5) > 0) {

                    printf("Voici le message dans le main %s\n", (char *) test);

                    free(test);
                    test = malloc(sizeof(char) * 10);


                }
            }
            printf("Je vais faire un sleep\n");
            sleep(10);
            msg_send(m1, "toto", strlen("toto"));

            sleep(20);

            msg_disconnect(m1);
        }

    } else if (argc == 2 && strcmp(argv[1], "2") == 0) { //va envoyer un msg, donc un signal, puis attend un signal
        void *test = malloc(sizeof(void) * 10);

        MESSAGE *m2 = msg_connect("testSignaux", O_RDWR);
        printf("J'ai rejoins file\n");
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            if (annuler_enregistrement(m2, getpid()) == -1) { //se de enregistre alors que ce n'est pas lui qui est enregistré
                printf("Je n'ai pas pu annuler l'enregistrement\n");
            }
            msg_send(m2, "tata", strlen("tata"));
            printf("Je vais faire un sleep\n");

            sleep(5);
            if (fonction_signalTest(m2) == 0) {
                while (signal_attente == 0) {
                    sleep(3);
                    printf("sig %d\n", signal_attente);
                }
                if (msg_receive(m2, test, 5) > 0) {

                    printf("Voici le message dans le main %s\n", (char *) test);

                    free(test);
                    test = malloc(sizeof(char) * 10);


                }
            }
            sleep(20);

            msg_disconnect(m2);
        }
    } else if (argc == 2 &&
               strcmp(argv[1], "3") == 0) { //envoie signal non possible car processus attend de recevoir msg
        void *test = malloc(sizeof(void) * 10);

        MESSAGE *m2 = msg_connect("testSignaux", O_RDWR);
        printf("J'ai rejoins file\n");
        if (m2 == NULL) {
            printf("Connexion impossible\n");
        } else {
            printf("J'ai créé/rejoins file\n");
            if (msg_receive(m2, test, 5) > 0) {

                printf("Voici le message dans le main %s\n", (char *) test);

                free(test);
                test = malloc(sizeof(char) * 10);


            }
            sleep(20);

            msg_disconnect(m2);
        }
    }
}

int main(int argc, char *argv[]) {


  //   test_1(argc, argv);
  //  test_2(argc, argv);
    //test_3(argc, argv);
     test_4(argc, argv);
    //  test_5(argc, argv);
  //   test_6(argc, argv);
//    test_7(argc, argv);
    // test_8(argc, argv);
  //  test_9(argc, argv);
   // test_10(argc, argv);

    return 0;

}
