
#include "cstor.h"

#include <time.h>
#include <string.h>
#include <stdio.h>

static cstor_t  _csdb;

static const char *_dbname = "test.sqlite";

static int cstor_test_init_csfile(cstor_file_t *csf, const char *fname){

    csf->cfname = strdup(fname);
    csf->cfpath = strdup("./cctv.a");
    csf->cffrom = strdup("android");

    csf->cflen  = 0;
    csf->cftype = 0;

    csf->cfcreate = (uint64_t)time(NULL);
    csf->cfwrite  = csf->cfcreate;
    csf->cfaccess = csf->cfcreate;

    csf->cfsnaplen = 0;
    csf->cfsnap   = 0;

    return 0;
}

int cstor_test(int argc, char *argv[]){
    cstor_file_t *csf;
    cstor_fetch_t *fetch;

    char cfname[32];
    int i;
    int rc;

    rc = cstor_init(&_csdb, _dbname);
    if(rc != 0){
        printf("init cstor sqlite3 db fail!\n");
        return -1;
    }

    rc = cstor_file_create(&csf);
    if(rc != 0){
        printf("create cstor file instance fail!\n");
        return -1;
    }

    for(i = 0; i < 10; i++){
        snprintf(cfname, 32, "cctv.a%d", i);

        cstor_test_init_csfile(csf, cfname);

        rc = cstor_file_insert(&_csdb, csf);
        if(rc != 0){
            printf("insert item to db fail!\n");
            return -1;
        }

        cstor_file_clean(csf);
    }

    rc = cstor_file_find(&_csdb,  "cctv.a4", 0, csf);
    if(rc != 0){
        printf("can't find item\n");
        return -1;
    }

    rc = cstor_file_delete(&_csdb, csf->cfid);
    if(rc != 0){
        printf("delte item fail!\n");
        return -1;
    }

    cstor_file_clean(csf);

    rc = cstor_fetch_create(&_csdb, "cfname", 1, &fetch);
    if(rc != 0){
        printf("create fetch fail!\n");
        return -1;
    }

    for(i = 0; i < 8; i++){
        rc = cstor_fetch_next(fetch);
        if(rc != 0){
            printf("fetch next call fail!\n");
            return -1;
        }

        rc = cstor_fetch_getfile(fetch, csf);
        if(rc != 0){
            printf("get fetch item fail!\n");
            return -1;
        }

        printf("csfile cfname:%s\n", csf->cfname);
        cstor_file_clean(csf);
    }

    cstor_file_destroy(csf);
    cstor_fetch_destroy(fetch);

    return 0;
}
