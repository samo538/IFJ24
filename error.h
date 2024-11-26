/**
 *  @project IFJ24
 *  @file error.h
 *  @author Samuel Luptak <xluptas00@stud.fit.vutbr.cz>
 */
#ifndef ERROR_H
#define ERROR_H

#include "queue.h"

// Function that does error handling
void throw_error(TokenStoragePtr stoken, int type);
// Function that checks the truthfullness of ret
void check_ret(TokenStoragePtr stoken, bool ret);
// Function that *should* take care of memory leaks
void garbage_collector(TokenStoragePtr stoken, bool err);

#endif
