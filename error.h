/**
 *  @file error.h
 *  @author Samuel Luptak <xluptas00@stud.fit.vutbr.cz>
 */
#ifndef ERROR_H
#define ERROR_H

#include "queue.h"

void throw_error(TokenStoragePtr stoken, int type);
void check_ret(TokenStoragePtr stoken, bool ret);
void garbage_collector(TokenStoragePtr stoken, bool err);

#endif
