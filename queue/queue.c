#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "queue_fill.h"

#include "../lexer/lexer.h"

void queue_add_token(Queue *queue, TokenPtr token){
    queue->Queue_act = realloc(queue->Queue_act, sizeof(TokenPtr) * (queue->Queue_size + 1));
    if (queue->Queue_act == NULL){
        //TODO error
    }
    queue->Queue_act[queue->Queue_size] = token;
    queue->Queue_size++;
}

TokenPtr queue_next_token(Queue *queue){
    static int counter = 0;
    if (queue->Queue_size <= counter){
        printf("invalid read\n"); //TODO real error
        return NULL;
    }
    TokenPtr tmp = queue->Queue_act[counter];
    counter++;
    return tmp;
}

void queue_free(Queue *queue){
    free(queue->Queue_act);
    free(queue);
}

Queue *queue_init(){
    Queue * new = malloc(sizeof(Queue));
    if (new == NULL){
        //TODO error
    }
    new->Queue_act = NULL;
    new->Queue_size = 0;
    return new;
}

void queue_fill(TokenStoragePtr stoken){
    TokenPtr tmp;
    while ((tmp = next_token())->type != END_OF_FILE){
        if (tmp->type == PUB){
            stoken->SToken = tmp;
            fn_def_q(stoken);
            queue_add_token(stoken->queue, stoken->SToken); // Add the last token
            if (stoken->SToken->type == END_OF_FILE){
                // Invalid syntax return err
            }
        }
        else{
            queue_add_token(stoken->queue, tmp);
        }
    }
    queue_add_token(stoken->queue, tmp);
}
