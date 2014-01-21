/*
 * myqueue_test.c
 *
 *  Created on: 2014-1-19
 *      Author: Luo Guochun
 */

#include "myqueue.h"
#include "ctest.h"

TEST(myqueue, slist)
{
    slist_t* slist = slist_create();
    slist_node_t* node = slist_node_create((void*)1, NULL);
    slist_insert_head(slist, node);

    node = slist_node_create((void*)2, NULL);
    slist_insert_head(slist, node);

    for(node = slist_first(slist); node; node = slist_next(node)) {
        printf("node : %ld\n", (long)node->data);
    }

    slist_destroy(slist);

}
TEST(myqueue, stailq)
{
    stailq_t* stailq = stailq_create();
    stailq_node_t* node = stailq_node_create((void*)1, NULL);
    stailq_insert_tail(stailq, node);

    node = stailq_node_create((void*)2, NULL);
    stailq_insert_tail(stailq, node);

    for(node = stailq_first(stailq); node; node = stailq_next(node)) {
        printf("node : %ld\n", (long)node->data);
    }

    stailq_destroy(stailq);
}

TEST(myqueue, list)
{
    list_t* list = list_create();

    list_node_t* node = list_node_create((void*) 1, NULL );

    list_insert_head(list, node);

    list_node_t* node2 = list_node_create((void*) 2, NULL );

    list_insert_after(node, node2);

    list_node_t* node3 = list_node_create((void*) 3, NULL );

    list_insert_after(node, node3);

    for (node = list_first(list); node; node = list_next(node)) {
        printf("node : %ld\n", (long) node->data);
    }
}
TEST(myqueue, tailq)
{
    tailq_t* tailq = tailq_create();
    tailq_node_t* node = tailq_node_create((void*)1, NULL);
    tailq_insert_tail(tailq, node);

    node = tailq_node_create((void*)2, NULL);
    tailq_insert_tail(tailq, node);

    for(node = tailq_first(tailq); node; node = tailq_next(node)) {
        printf("node : %ld\n", (long)node->data);
    }

    tailq_destroy(tailq);

}

