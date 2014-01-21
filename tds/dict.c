/*
 * dict.c
 *
 *  Created on: 2014-1-21
 *      Author: Luo Guochun
 */

#include "dict.h"
#include <stdlib.h>

#define dict_is_rehashing(d)     ((d)->rehashidx != -1)
#define dict_get_ht(d) \
    (dict_is_rehashing((d)) ? (d)->ht[0] : (d)->ht[1])
#define dict_get_hash(key, key_len, ht) \
    (murmur((unsigned char*)(key), (key_len)) & ((ht)->size - 1))
#define dict_try_rehash(d) \
    do{if(dict_is_rehashing(d)) dict_rehash(d);}while(0)

enum {
    DICT_INIT_SIZE = 4,
    DICT_RESIZE_RATIO = 5,
};

static dict_kv_t* dict_alloc_kv(
        void* key, size_t key_len,
        void* val, size_t val_len)
{
    dict_kv_t* kv = (dict_kv_t*)malloc(sizeof(*kv));
    if(!kv) {
        return NULL;
    }

    kv->key_len = key_len;

    kv->key = malloc(key_len);
    if(!kv->key) {
        free(kv);
        return NULL;
    }

    memcpy(kv->key, key, key_len);


    kv->val_len = val_len;

    if (val_len > 0) {
        kv->val = malloc(val_len);
        if (!kv->val) {
            free(kv->key);
            free(kv);
            return NULL ;
        }

        memcpy(kv->val, val, val_len);
    }else{
        kv->val = NULL;
    }





    return kv;
}

static void dict_free_kv(dict_kv_t* kv)
{
    if(kv) {
        if(kv->key) {
            free(kv->key);
        }
        if(kv->val) {
            free(kv->val);
        }
        free(kv);
    }
}

static void dict_reset(dict_ht_t* ht)
{
    ht->table = NULL;
    ht->size = 0;
    ht->used = 0;
}

static int dict_rehash(dict_t* d)
{
    if(!d) {
        return -1;
    }
    if(!dict_is_rehashing(d)) {
        return 0;
    }

    if(d->ht[0].used == 0) {
        d->rehashidx = -1;
        d->ht[0] = d->ht[1];
        dict_reset(d->ht[1]);
        return 0;
    }

    while(d->ht[0].table[d->rehashidx] == NULL) {
        d->rehashidx++;
    }

/*    dict_ht_node_t* ht_node = d.ht[0].table[d.rehashidx];

    dict_node_t* snode = NULL;
    for(snode = stailq_first(ht_node); snode; snode = stailq_next(snode)) {
        void* key = dict_get_key(snode);
        size_t key_len = dict_get_key_len(snode);

        size_t hash = murmur(key, key_len);

        ht_node = d->ht[1].table[hash];
    }*/

    return 0;
}

static int dict_expand(dict_t* d)
{
    return 0;
}

dict_t* dict_create(void* privdata)
{
    dict_t* d = (dict_t*)malloc(sizeof(*d));
    if(d) {
        dict_reset(d->ht[0]);
        dict_reset(d->ht[1]);

        d->privdata = privdata;
        d->rehashidx = -1;
    }
    return d;
}
int dict_destroy(dict_t* d)
{
    return 0;
}

dict_node_t* dict_node_create(
        void* key, size_t key_len,
        void* val, size_t val_len)
{
    dict_kv_t* kv = dict_alloc_kv(key, key_len, val, val_len);
    if(!kv) {
        return NULL;
    }

    dict_node_t* node = stailq_node_create((void*)kv, NULL);
    if(!node) {
        dict_free_kv(kv);
        return NULL;
    }
    return node;
}
int dict_node_destroy(dict_node_t* node)
{
    if(node) {
        if(node->data) {
            dict_free_kv((dict_kv_t*)node->data);
        }
        stailq_node_destroy(node);
    }
    return 0;
}

dict_node_t* dict_first(dict_t* d)
{
    if(!d) {
        return NULL;
    }

    dict_try_rehash(d);

    size_t index = 0;
    stailq_t* stq = NULL;

    while(index < d->ht[0].size || index < d->ht[1].size) {
        if (d->ht[0].size > 0) {
            stq = d->ht[0].table[index];
            if (stq != NULL && !stailq_empty(stq)) {
                return stailq_first(stq);
            }
        }
        if(dict_is_rehashing(d)) {
            if (d->ht[1].size > 0) {
                stq = d->ht[1].table[index];
                if (stq != NULL && !stailq_empty(stq)) {
                    return stailq_first(stq);
                }
            }
        }
        index++;
    }

    return NULL;
}
dict_node_t* dict_next(dict_node_t* n)
{
    if(!n) {
        return NULL;
    }

    void* key = dict_get_key(n);
    size_t key_len = dict_get_key_len(n);

    while(n) {
        n = stailq_next(n);
        if(n) {
            return n;
        }
    }

    //size_t hash = dict_get_hash(key, key_len, NULL);

    size_t index = 0;
    stailq_t* stq = NULL;


    return NULL;
}
int dict_empty(dict_t* d)
{
    if(d) {
        if(d->ht[0].used == 0 && d->ht.used == 0) {
            return 1;
        }
        return 0;
    }
    return 1;
}

dict_node_t* dict_add(dict_t* d,
        void* key, size_t key_len,
        void* val, size_t val_len)
{
    if(!dict || !key || key_len <= 0) {
        return NULL;
    }

    if(dict_is_rehashing(d)) {
        dict_rehash(d);
    }else{
        if(d->ht[0].size == 0){
            if(dict_expand(d) != 0) {
                return -1;
            }
        }else{
            if (d->ht[0].used >= d->ht[0].size
                    && d->ht[0].used / d->ht[0].size >= DICT_RESIZE_RATIO) {
                if (dict_expand(d) != 0) {
                    return -1;
                }
            }
        }
    }

    dict_ht_t* ht = dict_is_rehashing(d) ? d->ht[1] : d->ht[0];

    size_t idx = murmur(key, key_len) & (ht->size - 1);
    stailq_t* stq = ht->table[idx];

    dict_node_t* node = dict_node_create(key, key_len, val, val_len);
    if(!node) {
        return NULL;
    }

    if(stailq_empty(stq)) {
        stailq_insert_head(stq, node);
    }else{
        stailq_insert_tail(stq, node);
    }

    return node;
}
dict_node_t* dict_replace(dict_t* d,
        void* key, size_t key_len,
        void* val, size_t val_len)
{
    return NULL;
}

dict_node_t* dict_delete(dict_t* d,
        void* key, size_t key_len)
{
    dict_node_t* node = dict_find(d, key, key_len);
    if(!node) {
        return NULL;
    }

    dict_ht_t* ht = dict_get_ht(d);
    size_t hash = dict_get_hash(key, key_len, ht);

    stailq_remove(ht->table[hash], node);

    return node;
}

dict_node_t* dict_find(dict_t* d,
        void* key, size_t key_len)
{
    if(!dict || !key || key_len <= 0) {
        return NULL;
    }

    if(dict_is_rehashing(d)) {
        dict_rehash(d);
    }

    return NULL;
}
