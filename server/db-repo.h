
#ifndef __DBREPO_H__
#define __DBREPO_H__

#include "db-user.h"
#include "db-stor.h"
#include "fixarray.h"
#include "sqlite3.h"

#include <stdio.h>
#include <pthread.h>

/*
** repository
*/
typedef struct  _dbrepo{
    char *rdir_video;
    char *rdir_music;
    char *rdir_photo;
    char *rdir_docs;

    pthread_mutex_t mlock;
    fixmap_t folders;

    dbstor_t dbstor;
    dbuser_t dbuser;
}dbrepo_t;

typedef struct _dbrepo_file{
    fixmap_node_t node;

    int write;
    int64_t fid;
    FILE *handle;
    dbstor_file_t *file;
}rfile_t;

typedef struct _dbrepo_folder{
    fixmap_node_t node;

    int64_t fid;
    int64_t fup;
    char *fname;
}rfolder_t;

int dbrepo_init(dbrepo_t *repo, const char *rdir, const char *rdbs, const char *rdbu);
int dbrepo_fini(dbrepo_t *repo);

int dbrepo_clean(dbrepo_t *repo);

char *dbrepo_build_rpath(dbrepo_t *repo, char *fname, int64_t folder);
char *dbrepo_file_buidpath(dbrepo_t *repo, dbstor_file_t *file);
int dbrepo_is_insubdir(dbrepo_t *repo, const char *fpath);

#endif

