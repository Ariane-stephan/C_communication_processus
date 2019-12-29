//
// Created by Ariane Stephan on 2019-04-02.
//

#include "structure.h"




void initialiser_message(MESSAGE *message, ouverture_e ouverture, file_s *pointeur, char *n){
    message->type_ouverture = ouverture;
    message->file = pointeur;

    message->nom = malloc(sizeof(char)*strlen(n));

    strcpy(message->nom, n);

}
void changer_ouverture(MESSAGE *message, ouverture_e ouverture) {
    message->type_ouverture = ouverture;
}
