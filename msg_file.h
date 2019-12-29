//
// Created by Ariane Stephan on 2019-04-09.
//

#ifndef PROJET_MSG_FILE_H
#define PROJET_MSG_FILE_H
#include "structure.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include<errno.h>
#include<stdarg.h>
#include <fcntl.h>
//#include <zconf.h>
#include <sys/errno.h>


volatile sig_atomic_t signal_attente;
int fonction_signalTest(MESSAGE *m);

static void handler_change_variable_signal(int sig);
MESSAGE *msg_connect(const char *nom, int options, ...);

int msg_disconnect(MESSAGE *file);

int msg_unlink(const char *nom);

int msg_send(MESSAGE *file, const void *msg, size_t len);

int msg_trysend(MESSAGE *file, const void *msg, size_t len);

int msg_send_multi(MESSAGE *file, void *msg[], int nb_msg);

ssize_t msg_receive(MESSAGE *file, void *msg, size_t len);

ssize_t msg_tryreceive(MESSAGE *file, void *msg, size_t len);

ssize_t msg_receive_multi(MESSAGE *file, void *msg[], int nb_msg, size_t len);

int annuler_enregistrement(MESSAGE *file, pid_t pid);

size_t msg_message_size(MESSAGE *);

size_t msg_capacite(MESSAGE *);

size_t msg_nb(MESSAGE *);

int fonction_signal(MESSAGE *m, void *test);



#endif //PROJET_MSG_FILE_H
