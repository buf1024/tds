/*
 * buffer.c
 *
 *  Created on: 2014-1-24
 *      Author: Luo Guochun
 */

#include "buffer.h"
#include <stdlib.h>
#include <string.h>

#define BUFFER_MIN_SIZE 128

struct buffer_s
{
    char* buf;

    int size;
    int wpos;
};

buffer_t* buffer_create(int size)
{
    buffer_t* buf = NULL;
    if(size <= BUFFER_MIN_SIZE) {
        size = BUFFER_MIN_SIZE;
    }
    buf = (buffer_t*)malloc(sizeof(*buf));
    if(!buf) return buf;

    buf->size = size;
    buf->wpos = 0;

    buf->buf = (char*)malloc(size);
    if(!buf->buf) {
        free(buf);
        return NULL;
    }

    return buf;
}
void buffer_destroy(buffer_t* buf)
{
    free(buf->buf);
    free(buf);
}


int buffer_size(buffer_t* buf)
{
    return buf->size;
}
int buffer_free_size(buffer_t* buf)
{
    return buf->size - buf->wpos;
}
int buffer_write_size(buffer_t* buf)
{
    return buf->wpos;
}

int buffer_get(buffer_t* buf, char* dst, int size)
{
    int wsize = buffer_write_size(buf);
    int cpsize = size >= wsize ? wsize : size;

    memcpy(dst, buf->buf, cpsize);

    if(cpsize < wsize) {
        int pos = cpsize;
        int start = 0;
        for(; pos <= wsize; pos++) {
            buf->buf[start++] = buf->buf[pos++];
        }
        buf->wpos = start;
    }else{
        buf->wpos = 0;
    }

    return cpsize;
}
int buffer_write(buffer_t* buf, char* src, int size)
{
    int free_size = buffer_free_size(buf);
    if(size > free_size) return -1;

    memcpy(buf->buf + buf->wpos, src, size);

    buf->wpos += size;

    return 0;
}
