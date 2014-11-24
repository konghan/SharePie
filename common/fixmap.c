/*
 * Copyright (c) 2014, Konghan. All rights reserved.
 * Distributed under the GNU v3 license, see the LICENSE file.
 */

#include "fixmap.h"

#include <errno.h>
#include <assert.h>

int fixmap_init(fixmap_t *fm){
    fm->root = RB_ROOT;
    fm->num = 0;
    return 0;
}

int fixmap_fini(fixmap_t *fm){
    if(fm->num != 0)
        return fm->num;

    fm->root = RB_ROOT;
    return 0;
}

int fixmap_get(fixmap_t *fm, int64_t key, fixmap_node_t **node){
    struct rb_node *rbn = fm->root.rb_node;
    fixmap_node_t *fmn;

    assert((fm != NULL) && (node != NULL));

    while(rbn){
        fmn = container_of(rbn, fixmap_node_t, node);
        if(key < fmn->key){
            rbn = rbn->rb_left;
        }else if(key > fmn->key){
            rbn = rbn->rb_right;
        }else{
            *node = fmn;
            return 0;
        }
    }
    
    return -ENOENT;
}

int fixmap_insert(fixmap_t *fm, int64_t key, fixmap_node_t *node){
    struct rb_node **new = &(fm->root.rb_node);
    struct rb_node *parent = NULL;
    fixmap_node_t *fmn;

    node->key = key;

    /* Figure out where to put new node */
    while (*new) {
        fmn = container_of(*new, fixmap_node_t, node);
        parent = *new;
        if(key < fmn->key){
            new = &((*new)->rb_left);
        }else if(key > fmn->key){
            new = &((*new)->rb_right);
        }else{
            return -EEXIST;
        }
    }

    fm->num++;
    /* Add new node and rebalance tree. */
    rb_link_node(&node->node, parent, new);
    rb_insert_color(&node->node, &fm->root);

    return 0;
}

int fixmap_remove(fixmap_t *fm, int64_t key, fixmap_node_t **node){
    fixmap_node_t *fn;

    if(fixmap_get(fm, key, &fn) != 0){
        return -ENOENT;
    }

    fm->num--;
    rb_erase(&fn->node, &fm->root);

    if(node != NULL)
        *node = fn;

    return 0;
}

