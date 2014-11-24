#include "db-user.h"

#include "db-repo.h"
#include "spie.h"
#include "md5.h"
#include "logger.h"
#include "osapi.h"

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

static const char *sql_create = "CREATE TABLE IF NOT EXISTS dbuser( \
        uid INTEGER PRIMARY KEY ASC,    \
        user TEXT,                      \
        cred BLOB,                      \
        type INTEGER);";

static const char *sql_select =  "SELECT * FROM dbuser;";

static const char *sql_lookup = "SELECT * FROM dbuser WHERE user = ?1;";

static const char *sql_insert = "INSERT INTO dbuser VALUES(?1, ?2, ?3, ?4);";

static const char *sql_delete = "DELETE FROM dbuser WHERE user = ?1;";

static const char *sql_update = "REPLACE INTO dbuser VALUES(?1, ?2, ?3, ?4);";

static void dbuser_bootstrap(dbuser_t *dbu){
    static char *user = "Administrator";
    md5_state_t mds;
    md5_byte_t buffer[16];
    
    md5_init(&mds);
    md5_append(&mds, (md5_byte_t*)user, strlen(user));
    md5_finish(&mds, buffer);

    dbuser_add(dbu, user, 16, buffer, SPIE_PRIORITY_NORMAL);
}

int dbuser_init(dbuser_t *dbu, const char *dbuser){
    sqlite3_stmt *stmt;
    int rc;

    assert((dbu != NULL) && (dbuser != NULL));

    rc = sqlite3_open_v2(dbuser, &dbu->dbuser, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, NULL);
    if(rc != 0){
        log_warn("open dbuser fail:%s\n", rc);
        return -EINVAL;
    }

    rc = sqlite3_prepare_v2(dbu->dbuser, sql_create, strlen(sql_create), &stmt, NULL);
    if(rc != SQLITE_OK){
        log_warn("prepare dbuser create stmt fail:%d\n", rc);
        sqlite3_close(dbu->dbuser);
        return -EINVAL;
    }

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        log_warn("execute create dbuser fail:%d\n", rc);
        sqlite3_finalize(stmt);
        sqlite3_close(dbu->dbuser);
        
        return -EINVAL;
    }

    sqlite3_finalize(stmt);

    dbuser_bootstrap(dbu);

    pthread_mutex_init(&dbu->mtx, NULL);

    return 0;
}

int dbuser_fini(dbuser_t *dbu){
    if(dbu != NULL){
        sqlite3_close(dbu->dbuser);
        dbu->dbuser = NULL;
    }

    pthread_mutex_destroy(&dbu->mtx);

    return 0;
}

static int64_t dbuser_find(dbuser_t *dbu, char *user){
    sqlite3_stmt *stmt;
    int64_t uid;
    int rc;

    rc = sqlite3_prepare_v2(dbu->dbuser, sql_lookup, strlen(sql_lookup), &stmt, NULL);
    if(rc != SQLITE_OK){
        return -1;
    }

    sqlite3_bind_text(stmt, 1, user, strlen(user), SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_ROW){
        sqlite3_finalize(stmt);
        return -SPIE_ECODE_NOENT;
    }

    uid = sqlite3_column_int64(stmt, 0);

    sqlite3_finalize(stmt);
    
    return uid;
}

int64_t dbuser_add(dbuser_t *dbu, char *user, int cred_len, void *cred, int type){
    sqlite3_stmt *stmt;
    int64_t uid;
    int   rc;

    uid = dbuser_find(dbu, user);
    if(uid >= 0){
        log_warn("user:%s already exist!\n", user);
        return -SPIE_ECODE_EXIST;
    }

    rc = sqlite3_prepare_v2(dbu->dbuser, sql_insert, strlen(sql_insert), &stmt, NULL);
    if(rc != SQLITE_OK){
        log_warn("prepare add user fail\n");
        return -1;
    }

    sqlite3_bind_null(stmt, 1);
    sqlite3_bind_text(stmt, 2, user, strlen(user), SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 3, cred, cred_len, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, type);

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        log_warn("inser user:%s fail!\n", user);
        sqlite3_finalize(stmt);
        return -1;
    }

    uid  = sqlite3_last_insert_rowid(dbu->dbuser);

    sqlite3_finalize(stmt);

    return uid;
}

int64_t dbuser_del(dbuser_t *dbu, char *user){
    sqlite3_stmt    *stmt;
    int64_t uid;
    int rc = 0;

    uid = dbuser_find(dbu, user);
    if(uid < 0){
        log_warn("user:%s not exist!\n", user);
        return -SPIE_ECODE_NOENT;
    }

    rc = sqlite3_prepare_v2(dbu->dbuser, sql_delete, strlen(sql_delete), &stmt, NULL);
    if(rc != SQLITE_OK){
        log_warn("prepare for del user fail\n");
        return -SPIE_ECODE_DBERR;
    }

    sqlite3_bind_text(stmt, 1, user, strlen(user), SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        uid = -1;
    }

    sqlite3_finalize(stmt);

    return uid;
}

int64_t dbuser_update(dbuser_t *dbu, char *user, int cred_len, void *cred, int type){
    sqlite3_stmt    *stmt;
    int64_t uid;
    int   rc;

    uid = dbuser_find(dbu, user);
    if(uid < 0){
        log_warn("user:%s not exist!\n", user);
        return -ENOENT;
    }

    rc = sqlite3_prepare_v2(dbu->dbuser, sql_update, strlen(sql_update), &stmt, NULL);
    if(rc != SQLITE_OK){
        return -1;
    }

    sqlite3_bind_int64(stmt, 1, uid);
    sqlite3_bind_text(stmt, 2, user, strlen(user), SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 3, cred, cred_len, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, type);

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        uid = -1;
    }

    sqlite3_finalize(stmt);

    return uid;
}

void dbuser_ruser_free(ruser_t *ru){
    if(ru->user != NULL){
        osapi_free(ru->user);
        ru->user = NULL;
    }

    if(ru->cred != NULL){
        osapi_free(ru->cred);
        ru->cred = NULL;
        ru->cred_len = 0;
    }

    osapi_free(ru);
}

static int dbuser_ruser_from_stmt(sqlite3_stmt *stmt, ruser_t **ru){
    ruser_t *r;
    char *temp;
    int len;
    int rc;

    r = osapi_malloc(sizeof(*r));
    if(r == NULL){
        return -SPIE_ECODE_NOMEM;
    }
    memset(r, 0, sizeof(*r));

    r->uid = sqlite3_column_int64(stmt, 0);

    len = sqlite3_column_bytes(stmt, 1);
    if(len == 0){
        rc = -SPIE_ECODE_DBERR;
        goto err_exit;
    }
    r->user = osapi_malloc(len + 1);
    if(r->user == NULL){
        rc = -SPIE_ECODE_NOMEM;
        goto err_exit;
    }
    temp = (char *)sqlite3_column_text(stmt, 1);
    strncpy(r->user, temp, len);
    r->user[len] = 0;

    r->cred_len = sqlite3_column_bytes(stmt, 2);
    if(r->cred_len == 0){
        rc = -SPIE_ECODE_DBERR;
        goto err_exit;
    }
    r->cred = osapi_malloc(r->cred_len);
    if(r->cred == NULL){
        rc = -SPIE_ECODE_NOMEM;
        goto err_exit;
    }
    temp = (char *)sqlite3_column_blob(stmt, 2);
    memcpy(r->cred, temp, r->cred_len);

    r->type = sqlite3_column_int(stmt, 3);

    *ru = r;
    return 0;

err_exit:
    dbuser_ruser_free(r);
    return rc;
}

int dbuser_lookup(dbuser_t *dbu, char *user, ruser_t **ru){
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(dbu->dbuser, sql_lookup, strlen(sql_lookup), &stmt, NULL);
    if(rc != SQLITE_OK){
        return -1;
    }

    sqlite3_bind_text(stmt, 1, user, strlen(user), SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_ROW){
        sqlite3_finalize(stmt);
        return -SPIE_ECODE_NOENT;
    }

    rc = dbuser_ruser_from_stmt(stmt, ru);
    
    sqlite3_finalize(stmt);

    return rc;
}

int dbuser_list(dbuser_t *dbu, int len, fixarray_t **users){
    sqlite3_stmt *stmt;
    fixarray_t *fa;
    ruser_t *ru;
    int rc;
    int i;

    rc = fixarray_create(len, &fa);
    if(rc != 0){
        return rc;
    }

    rc = sqlite3_prepare_v2(dbu->dbuser, sql_select, strlen(sql_select), &stmt, NULL);
    if(rc != SQLITE_OK){
        fixarray_destroy(fa, (fixarray_itemfree)dbuser_ruser_free);
        return -EINVAL;
    }

    for(i = 0; i < len; i++){
        rc = sqlite3_step(stmt);
        if(rc == SQLITE_DONE){
            rc = 0;
            break;
        }

        if(rc != SQLITE_ROW){
            rc = -1;
            break;
        }

        rc = dbuser_ruser_from_stmt(stmt, &ru);
        if(rc != 0){
            break;
        }

        fixarray_set(fa, i, (void *)ru);
    }

    sqlite3_finalize(stmt);

    *users = fa;

    return rc;
}

int dbuser_tempadd(dbuser_t *dbu, int type){
    int rd;
    int i;


    pthread_mutex_lock(&dbu->mtx);
    if(dbu->user_num >= DBUSER_TEMP_MAX){
        log_warn("too much temp user\n");
        pthread_mutex_unlock(&dbu->mtx);
        return -1;
    }
    
    for(i = 0; i < DBUSER_TEMP_MAX; i++){
        if(dbu->users[i] <= 0){
            rd = (int)random() % 10000;
            if(type){
                dbu->users[i] = rd | (5 << 16);
            }else{
                dbu->users[i] = rd | (1 << 16);
            }
            dbu->user_num ++;
            pthread_mutex_unlock(&dbu->mtx);
            return rd;
        }
    }

    pthread_mutex_unlock(&dbu->mtx);
    return -1;
}

int dbuser_tempfind(dbuser_t *dbu, int user){
    int code;
    int i;

    pthread_mutex_lock(&dbu->mtx);
    for(i = 0; i < DBUSER_TEMP_MAX; i++){
        code = dbu->users[i] & 0x0000FFFF;
        if(code == user){
            pthread_mutex_unlock(&dbu->mtx);
            return 0;
        }
    }

    pthread_mutex_unlock(&dbu->mtx);
    return -1;
}

int dbuser_tempdel(dbuser_t *dbu, int user){
    int code;
    int i;

    pthread_mutex_lock(&dbu->mtx);
    for(i = 0; i < DBUSER_TEMP_MAX; i++){
        code = dbu->users[i] & 0x0000FFFF;
        if(code == user){
            dbu->users[i] = -1;
            dbu->user_num --;
            pthread_mutex_unlock(&dbu->mtx);
            return 0;
        }
    }

    pthread_mutex_unlock(&dbu->mtx);
    return -1;
}

