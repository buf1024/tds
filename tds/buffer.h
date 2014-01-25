/*
 * buffer.h
 *
 *  Created on: 2014-1-24
 *      Author: Luo Guochun
 */

#ifndef __BUFFER_H__
#define __BUFFER_H__

typedef struct buffer_s buffer_t;


buffer_t* buffer_create(int size);
void buffer_destroy(buffer_t* buf);


int buffer_size(buffer_t* buf);
int buffer_free_size(buffer_t* buf);
int buffer_write_size(buffer_t* buf);

int buffer_get(buffer_t* buf, char* dst, int size);
int buffer_write(buffer_t* buf, char* src, int size);

#endif /* __BUFFER_H__ */
