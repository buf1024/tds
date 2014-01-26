/*
 * lob.c
 *
 *  Created on: 2014-1-26
 *      Author: Luo Guochun
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <getopt.h>

#include "lob.h"
#include "util.h"

#define LOB_NAME    "lob(loadbalance)"
#define LOB_DESC    "lightweight & simple loadbalance"
#define LOB_VERSION "0.01"


lob_conf_t g_conf;
lob_info_t g_info;

int main(int argc, char **argv)
{
    const char* optstr = "c:ehv";
    struct option optlong[] = {
            {"configure", 1, NULL, 'c'},
            {"exclude", 0, NULL, 'e'},
            {"version", 0, NULL, 'v'},
            {"help", 0, NULL, 'h'},
            {NULL, 0, NULL, 0}
    };
    int daemon = LOB_TRUE;
    int opt;

    // no root
    if(is_prog_runas_root() == LOB_FAIL){
        fprintf(stderr, "%s is not allow to run as root!\n", LOB_NAME);
        exit(LOB_FAIL);
    }

    while ((opt = getopt_long(argc, argv, optstr, optlong, NULL)) != -1) {
        switch (opt) {
        case 'c':
            strncpy(g_conf.conf_path, optarg, MAX_PATH_LEN - 1);
            break;
        case 'e':
            daemon = LOB_TRUE;
            break;
        case 'v':
            version();
            exit(LOB_SUCCESS);
            break;
        case 'h':
            usage();
            exit(LOB_SUCCESS);
            break;
        case ':':
        case '?':
            usage();
            exit(LOB_FAIL);
            break;
        default:
            break;
        }
    }

    return 0;
}

void version()
{
    printf("%s version: %s\n", LOB_NAME, LOB_VERSION);
}
void usage()
{
    printf("%s ---- %s\n\n", LOB_NAME, LOB_DESC);
    printf("  -c, --configure=configuration file  Specific the configuration file\n");
    printf("                                      If not specific, use the default setting\n");
    printf("  -e, --exclude                       Don't start as daemon process\n");
    printf("  -v, --version                       Print the program version message\n");
    printf("  -h, --help                          Print this help message\n");
}

int load_conf(const char* file)
{
    return 0;
}
int init_signals()
{
    return 0;
}
