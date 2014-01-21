/*
 * hash.c
 *
 *  Created on: 2014-1-21
 *      Author: Luo Guochun
 */

#include "hash.h"

unsigned int murmur(unsigned char *data, size_t len)
{
    unsigned int  h, k;

    h = 0 ^ len;

    while (len >= 4) {
        k  = data[0];
        k |= data[1] << 8;
        k |= data[2] << 16;
        k |= data[3] << 24;

        k *= 0x5bd1e995;
        k ^= k >> 24;
        k *= 0x5bd1e995;

        h *= 0x5bd1e995;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len) {
    case 3:
        h ^= data[2] << 16;
        h ^= data[1] << 8;
        h ^= data[0];
        h *= 0x5bd1e99;
        break;
    case 2:
        h ^= data[1] << 8;
        h ^= data[0];
        h *= 0x5bd1e99;
        break;
    case 1:
        h ^= data[0];
        h *= 0x5bd1e995;
        break;
    }

    h ^= h >> 13;
    h *= 0x5bd1e995;
    h ^= h >> 15;

    return h;
}

unsigned int djb(unsigned char* data, int len)
{
    unsigned int hash = 5381;

    while (len--)
        hash = ((hash << 5) + hash) + (*data++); /* hash * 33 + c */
    return hash;
}
