/*
 * iniconf.c
 *
 *  Created on: 2014-1-22
 *      Author: Luo Guochun
 */

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "iniconf.h"

#define INI_MAX_KEY_LEN  256
#define INI_MAX_VALUE_LEN 1024
#define INI_READ_LINE (INI_MAX_KEY_LEN + INI_MAX_VALUE_LEN + 2)

typedef struct ini_kv_s ini_kv_t;

struct ini_kv_s
{
    char k[INI_MAX_KEY_LEN];
    char v[INI_MAX_VALUE_LEN];
};

struct ini_sec_s
{
    char key[INI_MAX_KEY_LEN];
    slist_t* kv;
};

struct ini_conf_s
{
    slist_t* sec;
};


static int read_line(int fd, char* buf, int size)
{
    int index = 0;
    int rd = 0;
    char ch;

    memset(buf, 0, size);

    while((rd = read(fd, &ch, 1)) > 0){
        if(ch != '\n'){
            if(index <= size){
                buf[index++] = ch;
            }else{
                return -1;
            }
        }else{
            break;
        }
    }
    if(index > 0){
        if(buf[index - 1] == '\r'){
            buf[index - 1] = 0;
            index--;
        }
    }
    if(rd <= 0){
        return -1;
    }

    return index;
}

int ini_destroy(ini_conf_t* ini)
{
    return 0;
}
ini_conf_t* ini_create()
{
    ini_conf_t* ini = (ini_conf_t*)malloc(sizeof(*ini));
    if(!ini) return NULL;

    memset(ini, 0, sizeof(*ini));

    return ini;
}

int ini_load(ini_conf_t* ini, const char* file)
{
    if(!ini || !file) return -1;

    int fd = open(file, O_RDONLY);
    if (fd == -1) return -1;

    char line[INI_READ_LINE + 1] = { 0 };
    char* tmp = 0;
    char key[INI_MAX_KEY_LEN] = { 0 };
    char value[INI_MAX_VALUE_LEN] = { 0 };

    ini_sec_t* cur_sec = 0;

    int len = 0;
    while (1) {
        if ((len = read_line(fd, line, INI_READ_LINE)) < 0) {
            break;
        }
        if (*line == '#' || *line == ';' || len == 0) {
            continue;
        }

        // start section
        memset(key, 0, INI_MAX_KEY_LEN);
        memset(value, 0, INI_MAX_VALUE_LEN);

        tmp = line;
        if (*tmp == '[') {
            const char* key_del = ++tmp;
            while (*tmp != '\0' && *tmp != ']') {
                tmp++;
            }
            if (*tmp == '\0') {
                return -1;
            }
            strncpy(key, key_del, tmp - key_del);
            cur_sec = ini_insert_sec(ini, key);
            continue;
        }
        if (cur_sec) {
            const char* eq_del = tmp;
            while (*eq_del != '\0') {
                if (*eq_del == '=') {
                    break;
                }
                eq_del++;
            }
            if (*eq_del == '\0' || *(eq_del + 1) == '\0') {
                continue;
            }
            strncpy(key, tmp, eq_del - tmp);
            strcpy(value, eq_del + 1);
            ini_insert_val(cur_sec, key, value);

        }
    }
    close(fd);
    return 0;
}
int ini_save(ini_conf_t* ini, const char* file)
{
    return 0;
}

int ini_dump(ini_conf_t* ini, char* buf, size_t len)
{
    return 0;
}

ini_sec_t* ini_insert_sec(ini_conf_t* ini, const char* key)
{
    if (!ini || !key)
        return NULL ;

    if (ini->sec == NULL ) {
        ini->sec = slist_create();
        if (!ini->sec)
            return NULL ;
    }

    ini_sec_t* sec = (ini_sec_t*) malloc(sizeof(*sec));
    if (!sec)
        return NULL ;
    sec->kv = slist_create();
    if (!sec->kv) {
        free(sec);
        return NULL ;
    }
    strncpy(sec->key, key, sizeof(sec->key));

    slist_node_t* node_new = slist_node_create(sec, NULL );
    if (!node_new) {
        slist_destroy(sec->kv);
        free(sec);
        return NULL ;
    }
    if (slist_empty(ini->sec)) {
        slist_insert_head(ini->sec, node_new);
        return sec;
    }

    slist_node_t* node_pre = NULL;
    slist_node_t* node_cur = NULL;
    for (node_cur = slist_first(ini->sec); node_cur; node_cur = slist_next(node_cur)) {
        ini_sec_t* cur_sec = (ini_sec_t*) slist_node_data(node_cur);
        int cmp = strcmp(cur_sec->key, key);
        if (cmp == 0) {
            slist_set_node_data(node_cur, sec);
            slist_destroy(sec->kv);
            sec->kv = cur_sec->kv;
            break;
        }else if(cmp < 0) {
            slist_insert_after(node_cur, node_new);
            break;
        }

        node_pre = node_cur;
    }
    if(node_cur == NULL && node_pre != NULL) {
        slist_insert_after(node_pre, node_new);
    }
    return sec;
}
int ini_delete_sec(ini_config_t* ini, const char* key)
{
    if (!ini || !key)
        return NULL ;

    if (ini->sec == NULL || slist_empty(ini->sec)) {
            return 0 ;
    }

    slist_node_t* node_pre = NULL;
    slist_node_t* node_cur = NULL;
    for (node_cur = slist_first(ini->sec); node_cur; node_cur = slist_next(node_cur)) {
        ini_sec_t* cur_sec = (ini_sec_t*) slist_node_data(node_cur);
        int cmp = strcmp(cur_sec->key, key);
        if (cmp == 0) {
            if(node_pre != NULL) {
                slist_remove_after(node_pre);
            }else{
                slist_remove_head(ini->sec);
            }

            if(!cur_sec->kv && !slist_empty(cur_sec->kv)) {
                for(node_cur = slist_first(cur_sec->kv); node_cur; node_cur = slist_next(node_cur)) {
                    void* kv = slist_node_data(node_cur);
                    free(kv);
                }
            }

            break;
        }

        node_pre = cur_sec;
    }
    return 0;
}
ini_sec_t* ini_get_sec(ini_conf_t* ini, const char* key)
{
    if (!ini || !key)
        return NULL ;

    if (ini->sec == NULL || slist_empty(ini->sec)) {
            return 0 ;
    }

    slist_node_t* node = NULL;
    for (node = slist_first(ini->sec); node; node = slist_next(node)) {
        ini_sec_t* cur_sec = (ini_sec_t*) slist_node_data(node);
        int cmp = strcmp(cur_sec->key, key);
        if (cmp == 0) {
            return cur_sec;
        }
    }
    return NULL;
}

int ini_insert_val(ini_sec_t* sec, const char* key, const char* val)
{
    if (!sec || !key || !val)
        return -1 ;

    ini_kv_t* kv = (ini_kv_t*)malloc(sizeof(*kv));
    if(!kv) return -1;

    strncpy(kv->k, key, sizeof(kv->k));
    strncpy(kv->v, val, sizeof(kv->v));

    slist_node_t* node_new = slist_node_create(kv, NULL);

    slist_node_t* node_pre = NULL;
    slist_node_t* node_cur = NULL;
    for (node_cur = slist_first(sec->kv); node_cur; node_cur = slist_next(node_cur)) {
        ini_kv_t* cur_kv = (ini_kv_t*) slist_node_data(node_cur);
        int cmp = strcmp(cur_kv->k, key);
        if (cmp == 0) {
            strncpy(cur_kv->v, val, sizeof(cur_kv->v));
            slist_node_destroy(node_new);
            free(kv);
            break;
        }else if(cmp < 0) {
            slist_insert_after(node_cur, node_new);
            break;
        }

        node_pre = node_cur;
    }
    if(node_cur == NULL && node_pre != NULL) {
        slist_insert_after(node_pre, node_new);
    }
    return sec;
}
int ini_delete_val(ini_sec_t* sec, const char* key)
{
    if (!sec || !key)
        return NULL ;

    if (sec->kv == NULL || slist_empty(sec->kv)) {
            return 0 ;
    }

    slist_node_t* node_pre = NULL;
    slist_node_t* node_cur = NULL;
    for (node_cur = slist_first(sec->kv); node_cur; node_cur = slist_next(node_cur)) {
        ini_sec_t* cur_sec = (ini_sec_t*) slist_node_data(node_cur);
        int cmp = strcmp(cur_sec->key, key);
        if (cmp == 0) {
            if(node_pre != NULL) {
                slist_remove_after(node_pre);
            }else{
                slist_remove_head(sec->kv);
            }

            if(!cur_sec->kv && !slist_empty(cur_sec->kv)) {
                for(node_cur = slist_first(cur_sec->kv); node_cur; node_cur = slist_next(node_cur)) {
                    void* kv = slist_node_data(node_cur);
                    free(kv);
                }
            }

            break;
        }

        node_pre = cur_sec;
    }
    return 0;
}
const char* ini_get_val(ini_sec_t* sec, const char* key)
{
    if (!sec || !key)
        return NULL ;

    if (sec->kv == NULL || slist_empty(sec->kv)) {
            return 0 ;
    }

    slist_node_t* node = NULL;
    for (node = slist_first(sec->kv); node; node = slist_next(node)) {
        ini_kv_t* cur_kv = (ini_kv_t*) slist_node_data(node);
        int cmp = strcmp(cur_kv->k, key);
        if (cmp == 0) {
            return cur_kv->v;
        }
    }
    return NULL;
}
