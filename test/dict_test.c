/*
 * dict_test.c
 *
 *  Created on: 2014-1-21
 *      Author: Luo Guochun
 */

#include "dict.h"
#include "ctest.h"

TEST(mydict, dict)
{
    dict_t* d = dict_create((void*)123);

    int k, v;
    dict_node_t* node = NULL;

    int i = 0;

    int sum = 0;
    for (i = 0; i < 10000; i++) {
        k = i, v = i;
        node = dict_add(d, &k, sizeof(k), &v, sizeof(v));
        ASSERT_TRUE(node != NULL);

        sum += i;
    }
    for (i = 0; i < 10000; i++) {
        node = dict_find(d, &k, sizeof(k));
        ASSERT_EQ((long)dict_get_val(node), i);
    }

    i = 0;
    for(node = dict_first(d); node; node = dict_next(d, node)) {
        i += (long)dict_get_val(node);
    }
    ASSERT_EQ(i, sum);

    for (i = 0; i < 10000; i++) {
        k = i;
        node = dict_delete(d, &k, sizeof(k));
        ASSERT_EQ((long)dict_get_val(node), i);
    }
    ASSERT_TRUE(dict_empty(d));

    dict_destroy(d);
}
