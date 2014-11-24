
#ifndef __DB_USER_H__
#define __DB_USER_H__

#include "fixarray.h"
#include "fixmap.h"

#include "sqlite3.h"

#include <stdint.h>
#include <pthread.h>

typedef struct _repo_user{

    int64_t uid;

    char *user;
    int cred_len;
    void *cred;
    int type;

    // don't store in db
    char *from;
}ruser_t;

#define DBUSER_TEMP_MAX     8

typedef struct _dbuser{
    pthread_mutex_t mtx;
    sqlite3 *dbuser;

    int user_num;
    int users[DBUSER_TEMP_MAX];
}dbuser_t;

void dbuser_ruser_free(ruser_t *ru);

int dbuser_init(dbuser_t *dbu, const char *dbname);
int dbuser_fini(dbuser_t *dbu);

int dbuser_lookup(dbuser_t *dbu, char *user, ruser_t **ru);
int64_t dbuser_add(dbuser_t *dbu, char *user, int cred_len, void *cred, int type);
int64_t dbuser_del(dbuser_t *dbu, char *user);
int64_t dbuser_update(dbuser_t *dbu, char *user, int cred_len, void *cred, int type);
int dbuser_list(dbuser_t *dbu, int len, fixarray_t **users);

int dbuser_tempadd(dbuser_t *dbu, int user);
int dbuser_tempfind(dbuser_t *dbu, int user);
int dbuser_tempdel(dbuser_t *dbu, int user);

#endif // __DB_USER_H__

