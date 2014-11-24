#include "fixmap.h"

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#define NUM_NODES 32

int main()
{
    fixmap_t fm;
    fixmap_node_t *fmn;
    fixmap_node_t *mn[NUM_NODES];
    struct rb_node *node;
    int i, rc;

    fixmap_init(&fm);

    for (i= 0; i < NUM_NODES; i++) {
        mn[i] = (fixmap_node_t *)malloc(sizeof(fixmap_node_t));
        mn[i]->key = i * (random() >> 4);
        printf("insert idx:%d - %" PRId64"\n", i, mn[i]->key);
        rc = fixmap_insert(&fm, mn[i]->key, mn[i]);

        printf("   %s\n", rc ? "fail" : "ok");
    }
    
    /* *search */
    i = 0;
    for (node = rb_first(&fm.root); node; node = rb_next(node)){
        printf("idx %d - %" PRId64"\n", i, rb_entry(node, fixmap_node_t, node)->key);
        i++;
    }

    /* *delete */
    fmn = mn[20];
    if(fixmap_remove(&fm, fmn->key, &fmn) != 0){
        printf("remove node fail\n");
        return -1;
    }
    printf("remove %" PRId64" ok\n", fmn->key);

    if(fixmap_get(&fm, fmn->key, &fmn) == 0){
        printf("remove node still there\n");
    }else{
        printf("get remove node, return fail, right!\n");
    }

    i = 0;
//    for (node = rb_first(&fm.root); node; node = rb_next(node)){

    fixmap_for_each(&fm.root,node,fmn){
        printf("idx:%d key = %" PRId64 "\n", i++, fmn->key);
    }

    return 0;
}
