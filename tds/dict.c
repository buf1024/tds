/*
 * dict.c
 *
 *  Created on: 2014-1-21
 *      Author: Luo Guochun
 */


#include <stdlib.h>
#include <string.h>
#include "dict.h"
#include "hash.h"

struct dict_kv_s {
    void*  key;
    size_t key_len;
    void*  val;
    size_t val_len;
};

struct dict_ht_s {
    stailq_t** stq;
    unsigned long size;
    unsigned long used;
};


struct dict_s {
    dict_ht_t* ht[2];
    int rehash_idx; /* rehashing not in progress if rehash_idx == -1 */

    void *privdata;

    /*iterator info*/
    dict_ht_t* it_ht;
    int it_idx;
};
#define DICT_INIT_SIZE  4
#define DICT_RESIZE_RATIO  5


#define dict_is_rehashing(d)     ((d)->rehash_idx != -1)
#define dict_is_iterating(d)     ((d)->it_ht != NULL)
#define dict_get_hash(key, key_len, ht) \
    (g_fn_hash((unsigned char*)(key), (key_len)) & ((ht)->size - 1))
#define dict_try_rehash(d) \
    do{if(dict_is_rehashing(d)) dict_rehash(d);}while(0)

#define dict_it_set(d, ht, idx) \
    do{(d)->it_ht = (ht); (d)->it_idx = (idx);}while(0)


/*function hook*/
static dict_alloc_t g_fn_alloc = malloc;
static dict_free_t  g_fn_free = free;
static dict_hash_t  g_fn_hash = murmur;

/*helper function*/
static dict_kv_t* dict_alloc_kv(
        void* key, size_t key_len,
        void* val, size_t val_len)
{
    dict_kv_t* kv = (dict_kv_t*)g_fn_alloc(sizeof(*kv));

    if(!kv) return NULL;

    kv->key_len = key_len;
    kv->key = g_fn_alloc(key_len);
    if(!kv->key) {
        g_fn_free(kv);
        return NULL;
    }
    memcpy(kv->key, key, key_len);

    kv->val_len = val_len;
    if (val_len > 0) {
        kv->val = g_fn_alloc(val_len);
        if (!kv->val) {
            g_fn_free(kv->key);
            g_fn_free(kv);
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
            g_fn_free(kv->key);
        }
        if(kv->val) {
            g_fn_free(kv->val);
        }
        g_fn_free(kv);
    }
}

static int dict_rehash(dict_t* d)
{
    if(!d) {
        return -1;
    }
    if(!dict_is_rehashing(d) || dict_is_iterating(d)) {
        return 0;
    }
    size_t idx = 0;
    dict_ht_t* ht = d->ht[0];

    if(ht->used == 0) {
        d->rehash_idx = -1;

        for(idx = 0; idx < d->ht[0]->size; idx++) {
            stailq_destroy(ht->stq[idx]);
        }
        g_fn_free(ht->stq);

        d->ht[0] = d->ht[1];
        d->ht[1] = NULL;
        return 0;
    }

    while(ht->stq[d->rehash_idx] == NULL) d->rehash_idx++;

    stailq_t* stq = d->ht[0]->stq[d->rehash_idx];

    dict_node_t* snode = NULL;
    for(snode = stailq_first(stq); snode; snode = stailq_next(snode)) {

        size_t hash = dict_get_hash(dict_get_key(snode),
                dict_get_key_len(snode), d->ht[0]);

        if(stailq_empty(d->ht[1]->stq[hash])) {
            stailq_insert_head(d->ht[1]->stq[hash], snode);
        }else{
            stailq_insert_tail(d->ht[1]->stq[hash], snode);
        }
        d->ht[0]->used--;
        d->ht[1]->used++;
    }

    return 0;
}

static int dict_cmp_key(void* k1, size_t k1_len, void* k2, size_t k2_len)
{
    if (k1_len != k2_len)
        return 1;
    if (memcmp(k1, k1, k1_len) != 0)
        return 1;
    return 0;
}

/*user defined data access*/
void* dict_get_key(dict_node_t* node)
{
    return ((dict_kv_t*)(node->data))->key;
}
size_t dict_get_key_len(dict_node_t* node)
{
    return ((dict_kv_t*)(node->data))->key_len;
}
void* dict_get_val(dict_node_t* node)
{
    return ((dict_kv_t*)(node->data))->val;
}
size_t dict_get_val_len(dict_node_t* node)
{
    return ((dict_kv_t*)(node->data))->val_len;
}
void* dict_get_privdata(dict_t* d)
{
    return d->privdata;
}

static int dict_expand(dict_t* d, size_t s)
{
    if(!d) return -1;

    size_t ns = s;
    if(ns < DICT_INIT_SIZE) ns = DICT_INIT_SIZE;

    dict_ht_t* ht = (dict_ht_t*)g_fn_alloc(sizeof(*ht));
    if(!ht) return -1;

    ht->size = ns;
    ht->used = 0;
    ht->stq = (stailq_t**)g_fn_alloc(sizeof(*ht->stq) * ht->size);
    if(!ht->stq) {
        g_fn_free(ht);
        return -1;
    }
    size_t idx = 0;
    for(idx = 0; idx < ns; idx++) {
        ht->stq[idx] = stailq_create();
        if(!ht->stq[idx]) {
            for(idx--; idx >= 0; idx--) {
                stailq_destroy(ht->stq[idx]);
            }
            g_fn_free(ht->stq);
            g_fn_free(ht);
            return -1;
        }
    }

    if(d->ht[0] == NULL){
        d->ht[0] = ht;
        return 0;
    }

    d->ht[1] = ht;
    d->rehash_idx = 0; // rehash

    return 0;
}

dict_t* dict_create(void* privdata)
{
    dict_t* d = (dict_t*)g_fn_alloc(sizeof(*d));
    if(d) {
        d->ht[0] = NULL;
        d->ht[1] = NULL;

        d->privdata = privdata;
        d->rehash_idx = -1;
        d->it_ht = NULL;
        d->it_idx = 0;
    }
    return d;
}
int dict_destroy(dict_t* d)
{
    if(!d) return 0;

    dict_node_t* n = NULL;
    for( n = dict_first(d); n; n = dict_next(d, n)) {
        dict_node_destroy(n);
    }
    dict_ht_t* ht = d->ht[0];
    size_t idx = 0;
    while(ht) {
        for(idx = 0; idx < ht->size; idx++) {
            stailq_destroy(ht->stq[idx]);
        }
        g_fn_free(ht->stq);
        g_fn_free(ht);
        if(dict_is_rehashing(d)) {
            if(ht != d->ht[1]) {
                ht = d->ht[1];
            }else{
                break;
            }
        }
    }
    g_fn_free(d);

    return 0;
}

dict_node_t* dict_node_create(
        void* key, size_t key_len,
        void* val, size_t val_len)
{
    dict_kv_t* kv = dict_alloc_kv(key, key_len, val, val_len);
    if(!kv) return NULL;

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

void dict_set_hook(
        dict_alloc_t fn_alloc, dict_free_t fn_free,
        dict_hash_t fn_hash)
{
    if(fn_alloc) {
        g_fn_alloc = fn_alloc;
    }

    if(fn_free) {
        g_fn_free = fn_free;
    }

    if(fn_hash) {
        g_fn_hash = fn_hash;
    }
}

dict_node_t* dict_first(dict_t* d)
{
    if(!d)  return NULL;

    dict_it_set(d, NULL, 0);

    size_t idx = 0;
    dict_ht_t* ht = d->ht[0];

    // if ht[0].stq is NULL, then ht[1].stq is NULL 2
    if(ht->size > 0) {
        stailq_t* stq = NULL;
        while(ht->size > idx && (stq = ht->stq[idx]) != NULL){
            if(!stailq_empty(stq)) {
                dict_it_set(d, ht, idx);
                return stailq_first(stq);
            }
            idx++;
        }
    }


    return NULL;
}
dict_node_t* dict_next(dict_t* d, dict_node_t* n)
{
    if(!d || !n) return NULL;

    while((n = stailq_next(n)) != NULL) return n;

    d->it_idx++;

    while(d->it_ht->size > d->it_idx) {
        stailq_t* stq = d->it_ht->stq[d->it_idx];
        if(stq != NULL) {
            if (!stailq_empty(stq)) {
                return stailq_first(stq);
            }
        }
        d->it_idx++;

        if (d->it_idx >= d->it_ht->size &&
                dict_is_rehashing(d)) {
            if (d->it_ht == d->ht[1]) {
                dict_it_set(d, NULL, 0);
                return NULL ;
            } else {
                dict_it_set(d, d->ht[1], 0);
            }
        }
    }

    return NULL;
}
int dict_empty(dict_t* d)
{
    if(d) {
        if(d->ht[0]->used == 0) {
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
    if(!d || !key || key_len <= 0) return NULL;

    if(dict_is_rehashing(d)) {
        dict_rehash(d);
    }else{
        if(d->ht[0] == NULL){
            if(dict_expand(d, DICT_INIT_SIZE) != 0) {
                return NULL;
            }
        }else{
            if (d->ht[0]->used >= d->ht[0]->size
                    && d->ht[0]->used / d->ht[0]->size >= DICT_RESIZE_RATIO) {
                if (dict_expand(d, d->ht[0]->used * 2) != 0) {
                    return NULL;
                }
            }
        }
    }

    dict_ht_t* ht = dict_is_rehashing(d) ? d->ht[1] : d->ht[0];
    size_t hash = dict_get_hash(key, key_len, ht);
    stailq_t* stq = ht->stq[hash];

    dict_node_t* node = dict_node_create(key, key_len, val, val_len);
    if(!node) return NULL;

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
    if(!d || !key || key_len <= 0) return NULL;

    dict_try_rehash(d);

    dict_node_t* node = dict_delete(d, key, key_len);
    if(node != NULL) {
        dict_node_destroy(node);

        node = dict_add(d, key, key_len, val, val_len);
    }

    return node;
}

dict_node_t* dict_delete(dict_t* d,
        void* key, size_t key_len)
{
    dict_node_t* node = dict_find(d, key, key_len);
    if(!node) return NULL;

    dict_ht_t* ht = dict_is_rehashing(d) ? d->ht[1]: d->ht[0];
    size_t hash = dict_get_hash(key, key_len, ht);

    stailq_remove(ht->stq[hash], node);

    return node;
}

dict_node_t* dict_find(dict_t* d,
        void* key, size_t key_len)
{
    if(!d || !key || key_len <= 0) return NULL;

    dict_try_rehash(d);

    if(!dict_empty(d)) {
        dict_ht_t* ht = d->ht[0];

        while (1) {
            size_t hash = dict_get_hash(key, key_len, ht);
            stailq_t* stq = ht->stq[hash];

            dict_node_t* node = NULL;
            for (node = stailq_first(stq); node; node = stailq_next(node)) {
                if(dict_cmp_key(dict_get_val(node), dict_get_key_len(node),
                        key, key_len) == 0) {
                    return node;
                }
            }
            if(dict_is_rehashing(d)) {
                ht = d->ht[1];
            }else{
                break;
            }
        }
    }

    return NULL;
}
