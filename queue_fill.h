/**
 *  @project IFJ24
 *  @file queue_fill.h
 *  @author Samuel Luptak <xluptas00@stud.fit.vutbr.cz>
 */
#ifndef QUEUE_FILL
#define QUEUE_FILL

#include "queue.h"

// The function that gets called by queue.c to initiate function header parsing
bool fn_def_q(TokenStoragePtr stoken, Elem_id **token);

#endif
