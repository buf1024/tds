/*
 * dict.h
 *
 *  Created on: 2014-1-21
 *      Author: Luo Guochun
 */

#ifndef __DICT_H__
#define __DICT_H__

#include <stddef.h>
#include "myqueue.h"

typedef struct dict_s      dict_t;
typedef stailq_node_t      dict_node_t;

typedef struct dict_ht_s   dict_ht_t;
typedef struct dict_kv_s   dict_kv_t;

/*function hook*/
typedef void* (*dict_alloc_t)(size_t);
typedef void (*dict_free_t)(void*);
typedef unsigned int (*dict_hash_t)(unsigned char*, size_t);

/*user defined data access*/
void* dict_get_key(dict_node_t* node);
size_t dict_get_key_len(dict_node_t* node);
void* dict_get_val(dict_node_t* node);
size_t dict_get_val_len(dict_node_t* node);
void* dict_get_privdata(dict_t* d);

/*dict / dict node construct / destruct*/
dict_t* dict_create(void* privdata);
int dict_destroy(dict_t* d);
dict_node_t* dict_node_create(
        void* key, size_t key_len,
        void* val, size_t val_len);
int dict_node_destroy(dict_node_t* node);

/*dict global attribute*/
void dict_set_hook(
        dict_alloc_t fn_alloc, dict_free_t fn_free,
        dict_hash_t fn_hash);

/*dict iterator interface*/
dict_node_t* dict_first(dict_t* d);
dict_node_t* dict_next(dict_t* d, dict_node_t* n);

/*dict general interface*/
dict_node_t* dict_add(dict_t* d,
        void* key, size_t key_len,
        void* val, size_t val_len);
dict_node_t* dict_replace(dict_t* d,
        void* key, size_t key_len,
        void* val, size_t val_len);
dict_node_t* dict_delete(dict_t* d,
        void* key, size_t key_len);
dict_node_t* dict_find(dict_t* d,
        void* key, size_t key_len);
int dict_empty(dict_t* d);

#endif /* __DICT_H__ */
