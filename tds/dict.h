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

struct dict_kv_s {
    void*  key;
    size_t key_len;
    void*  val;
    size_t val_len;
};

struct dict_ht_s {
    stailq_t** table;
    unsigned long size;
    unsigned long used;
};

struct dict_s {
    void *privdata;
    dict_ht_t ht[2];
    int rehashidx; /* rehashing not in progress if rehashidx == -1 */
};


#define dict_get_key(node)   (((dict_kv_t*)((node)->data))->key)
#define dict_get_key_len(node)   (((dict_kv_t*)((node)->data))->key_len)
#define dict_get_val(node)   (((dict_kv_t*)((node)->data))->val)
#define dict_get_val_len(node)   (((dict_kv_t*)((node)->data))->val_len)

#define dict_get_privdata(d) ((d)->privdata)

dict_t* dict_create(void* privdata);
int dict_destroy(dict_t* d);

dict_node_t* dict_node_create(
        void* key, size_t key_len,
        void* val, size_t val_len);
int dict_node_destroy(dict_node_t* node);

dict_node_t* dict_first(dict_t* d);
dict_node_t* dict_next(dict_node_t* n);
int dict_empty(dict_t* d);

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

#endif /* __DICT_H__ */
