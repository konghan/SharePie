
#ifndef __DBSTOR_H__
#define __DBSTOR_H__

#include "fixarray.h"
#include "fixmap.h"

#include "sqlite3.h"

#include <stdint.h>
#include <pthread.h>

typedef struct _dbstor_file{
    int64_t fid;
    int64_t folder;
    int64_t fowner;

    int ftype;      // SPIE_TYPE_xxx  

    char *fname;
    char *fpath;
    char *fwhere;

    int64_t fctime;
    int64_t fmtime;
    int64_t fatime;

    int64_t fsize;

    int fsnap_len;
    void *fsnap;
}dbstor_file_t;

typedef struct _dbstor_lookup{
    fixmap_node_t node;

    int64_t luid;
    sqlite3_stmt *stmt;
}dbstor_lookup_t;

typedef struct _dbstor{
    pthread_mutex_t mtx;
    sqlite3 *dbstor;
}dbstor_t;

int dbstor_init(dbstor_t *dbs, const char *dbname);
int dbstor_fini(dbstor_t *dbs);

int  dbstor_file_new(dbstor_file_t **file);
void dbstor_file_free(dbstor_file_t *file);

int dbstor_finsert(dbstor_t *dbs, dbstor_file_t *file);
int dbstor_fdelete(dbstor_t *dbs, int64_t fid);
int dbstor_fupdate(dbstor_t *dbs, int64_t fid, dbstor_file_t *file);
int dbstor_flookup(dbstor_t *dbs, int64_t fid, dbstor_file_t **file, int withsnap);
int dbstor_flookup_byname(dbstor_t *dbs, char *fname, int64_t folder,
        dbstor_file_t **file, int withsnap);

int dbstor_update_access(dbstor_t *repo, dbstor_file_t *file, int withmtime);

int dbstor_lucreate(dbstor_t *dbs, int64_t folder, int type, dbstor_lookup_t **lu);
int dbstor_ludestroy(dbstor_t *dbs, dbstor_lookup_t *lu);
int dbstor_lunext(dbstor_t *dbs, dbstor_lookup_t *lu, int len, fixarray_t **fa);

#endif

