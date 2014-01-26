/*
 * cmmhdr.h
 *
 *  Created on: 2014-1-26
 *      Author: Luo Guochun
 */

#ifndef __CMMHDR_H__
#define __CMMHDR_H__


#define LB_REG_SIGNAL(signo, func, interupt) do { \
    if(set_signal(signo, func, interupt) == NULL) { \
        printf("set_signal failed: %s\n", strerror(errno)); \
        return -1; \
    }\
}while(0)

#define LB_CONF_READ_STR_MST(sec, key, val) do { \
    const char* v = ini_get_val(sec, key); \
    if(!v) { \
        printf("ini_get_val failed: sec = %s, key = %s\n", \
                ini_sec_name(sec), key); \
        return -1; \
    } \
    strcpy(val, v); \
}while(0)

#define LB_CONF_READ_INT_MST(sec, key, val) do { \
    const char* v = ini_get_val(sec, key); \
    if(!v) { \
        printf("ini_get_val failed: sec = %s, key = %s\n", \
                ini_sec_name(sec), key); \
        return -1; \
    } \
    val = atoi(v); \
}while(0)

#define LB_CONF_READ_STR_OPT(sec, key, val) do { \
    const char* v = ini_get_val(sec, key); \
    if(!v) { \
        printf("ini_get_val failed: sec = %s, key = %s\n", \
                ini_sec_name(sec), key); \
    } \
    strcpy(val, v); \
}while(0)

#define LB_CONF_READ_INT_OPT(sec, key, val) do { \
    const char* v = ini_get_val(sec, key); \
    if(!v) { \
        printf("ini_get_val failed: sec = %s, key = %s\n", \
                ini_sec_name(sec), key); \
    } \
    val = atoi(v); \
}while(0)

enum {
    MAX_PATH_LEN = 256,
};

enum {
    LOB_TRUE    = 1,
    LOB_FALSE   = 0,
};

enum {
    LOB_SUCCESS = 0,
    LOB_FAIL    = -1,
};

typedef struct lob_conf_s lob_conf_t;
typedef struct lob_info_s lob_info_t;
typedef struct lob_stat_s lob_stat_t;

struct lob_conf_s
{
    char conf_path[MAX_PATH_LEN];
    int log_file_lvl;
    char log_file_path[MAX_PATH_LEN];
    int log_term_lvl;

};

struct lob_info_s
{

};

struct lob_stat_s
{

};

#endif /* __CMMHDR_H__ */
