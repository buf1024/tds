/*
 * hash.h
 *
 *  Created on: 2014-1-21
 *      Author: Luo Guochun
 */

#ifndef __HASH_H__
#define __HASH_H__

#include <stddef.h>

unsigned int murmur(unsigned char *data, size_t len);
unsigned int djb(unsigned char* data, size_t len);

#endif /* __HASH_H__ */
