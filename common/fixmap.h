/*
 * Copyright (c) 2014, Konghan. All rights reserved.
 * Distributed under the GNU v3 license, see the LICENSE file.
 */

#ifndef __FIXMAP_H__
#define __FIXMAP_H__

#include "rbtree.h"

#include <stdint.h>

typedef struct _fixmap_node{
    int64_t key;
    struct  rb_node node;
}fixmap_node_t;

typedef struct _fixmap{
    struct rb_root root;
    int num;
}fixmap_t;

int fixmap_init(fixmap_t *fm);
int fixmap_fini(fixmap_t *fm);

int fixmap_get(fixmap_t *fm, int64_t key, fixmap_node_t **node);

int fixmap_insert(fixmap_t *fm, int64_t key, fixmap_node_t *node);
int fixmap_remove(fixmap_t *fm, int64_t key, fixmap_node_t **node);

/*
 * fmroot - struct rb_root *
 * rbnode - sgruct rb_node *
 * fnode - fixmap_node *
 */
#define fixmap_for_each(fmroot,rbnode,fnode)  \
    for(rbnode = rb_first(fmroot), fnode = rb_entry(rbnode, fixmap_node_t, node);   \
            rbnode != NULL;     \
            rbnode = rb_next(rbnode), fnode = rb_entry(rbnode, fixmap_node_t, node))

#define fixmap_for_each_remove(fmroot,rbnode,fnode)   \
    for(rbnode = rb_first(fmroot), fnode = rb_entry(rbnode, fixmap_node_t, node);   \
            rbnode != NULL;     \
            rbnode = rb_first(fmroot), fnode = rb_entry(rbnode, fixmap_node_t, node))

static inline void fixmap_node_init(fixmap_node_t *node){
    node->key = 0;
    rb_init_node(&node->node);
}

static inline int fixmap_num(fixmap_t *fm){
    return fm->num;
}

#endif // __FIXMAP_H__

