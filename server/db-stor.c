
#include "db-stor.h"

#include "spie.h"
#include "logger.h"
#include "osapi.h"

#include <assert.h>
#include <string.h>
#include <errno.h>

static const char *sql_fcreate = "CREATE TABLE IF NOT EXISTS dbstor( \
        fid INTEGER PRIMARY KEY ASC,    \
        folder INTEGER,                 \
        fowner INTEGER,                 \
        ftype INTEGER,                  \
        fname TEXT,                     \
        fpath TEXT,                     \
        fwhere TEXT,                    \
        fctime INTEGER,                 \
        fmtime INTEGER,                 \
        fatime INTEGER,                 \
        fsize INTEGER,                  \
        fsnap BLOB);";

static const char *sql_finsert = "INSERT INTO dbstor VALUES(?1, ?2, ?3, ?4, \
                                        ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12);";
static const char *sql_fdelete = "DELETE FROM dbstor WHERE fid = ?1;";

static const char *sql_flookup = "SELECT * FROM dbstor WHERE fid = ?1;";

static const char *sql_flookup_byname  = "SELECT * FROM dbstor WHERE fname = ?1 \
                                AND folder = ?2;";

static const char *sql_fupdate = "REPLACE INTO dbstor VALUES(?1, ?2, ?3, \
                                    ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12);";

static char *sql_lookup_name =  "SELECT * FROM dbstor \
                        WHERE folder = ?1 ORDER BY fname;";

static char *sql_lookup_size =  "SELECT * FROM dbstor \
                        WHERE folder = ?1 ORDER BY fsize;";

static char *sql_lookup_atime =  "SELECT * FROM dbstor \
                        WHERE folder = ?1 ORDER BY fatime;";

static char *sql_lookup_ctime =  "SELECT * FROM dbstor \
                        WHERE folder = ?1 ORDER BY fctime;";


static int dbstor_init_root_folder(dbstor_t *dbs, int64_t folder){
    sqlite3_stmt *stmt;
    int rc;

    rc = dbstor_flookup(dbs, folder, NULL, 0);
    if(rc == 0){
        // folder alread exist;
        return 0;
    }

    rc = sqlite3_prepare_v2(dbs->dbstor, sql_finsert, strlen(sql_finsert), &stmt, NULL);
    if(rc != SQLITE_OK){
        return -SPIE_ECODE_DBERR;
    }

    sqlite3_bind_int64(stmt, 1, folder); // id is given by folder
    sqlite3_bind_int64(stmt, 2, -1); // root folder's container is not exist!
    sqlite3_bind_int64(stmt, 3, 0);
    sqlite3_bind_int(stmt, 4, SPIE_TYPE_FOLDER);

    sqlite3_bind_text(stmt, 5, "/", 1, SQLITE_STATIC);
    sqlite3_bind_null(stmt, 6);
    sqlite3_bind_null(stmt, 7);

    // root folder's ctime, mtime, atime is 0
    sqlite3_bind_int64(stmt, 8, 0);
    sqlite3_bind_int64(stmt, 9, 0);
    sqlite3_bind_int64(stmt, 10, 0);
    sqlite3_bind_int64(stmt, 11, 0);

    sqlite3_bind_null(stmt, 12);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if(rc != SQLITE_DONE){
        return -SPIE_ECODE_DBERR;
    }

    return 0;
}

int dbstor_init(dbstor_t *dbs, const char *dbname){
    sqlite3_stmt *stmt;
    int rc;

    assert((dbname != NULL) && (dbs != NULL));

    if(sqlite3_open_v2(dbname, &dbs->dbstor, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, NULL) != 0){
        log_warn("open dbstor:%s fail\n", dbname);
        return -SPIE_ECODE_INVAL;
    }

    // create table of dbstor
    rc = sqlite3_prepare_v2(dbs->dbstor, sql_fcreate, strlen(sql_fcreate), &stmt, NULL);
    if(rc != SQLITE_OK){
        sqlite3_close(dbs->dbstor);
        return -SPIE_ECODE_INVAL;
    }

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        sqlite3_finalize(stmt);
        sqlite3_close(dbs->dbstor);
        return -SPIE_ECODE_INVAL;
    }

    sqlite3_finalize(stmt);
    
    // create root dir
    dbstor_init_root_folder(dbs, SPIE_FOLDER_VIDEO);
    dbstor_init_root_folder(dbs, SPIE_FOLDER_MUSIC);
    dbstor_init_root_folder(dbs, SPIE_FOLDER_PHOTO);
    dbstor_init_root_folder(dbs, SPIE_FOLDER_DOCS);

    pthread_mutex_init(&dbs->mtx, NULL);

    return 0;
}

int dbstor_fini(dbstor_t *dbs){
    if(dbs->dbstor != NULL){
        sqlite3_close(dbs->dbstor);
        dbs->dbstor = NULL;
    }

    pthread_mutex_destroy(&dbs->mtx);

    return 0;
}

int dbstor_file_new(dbstor_file_t **file){
    dbstor_file_t *f;

    f = osapi_malloc(sizeof(*f));
    if(f == NULL){
        return -SPIE_ECODE_NOMEM;
    }
    memset(f, 0, sizeof(*f));

    *file = f;
    return 0;
}

void dbstor_file_free(dbstor_file_t *file){
    if(file->fname != NULL){
        osapi_free(file->fname);
        file->fname = NULL;
    }

    if(file->fpath != NULL){
        osapi_free(file->fpath);
        file->fpath = NULL;
    }

    if(file->fwhere != NULL){
        osapi_free(file->fwhere);
        file->fwhere = NULL;
    }

    if(file->fsnap_len != 0){
        osapi_free(file->fsnap);
        file->fsnap = NULL;
        file->fsnap_len = 0;
    }

    osapi_free(file);
}

int dbstor_finsert(dbstor_t *dbs, dbstor_file_t *file){
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(dbs->dbstor, sql_finsert, strlen(sql_finsert), &stmt, NULL);
    if(rc != SQLITE_OK){
        return -SPIE_ECODE_DBERR;
    }

    sqlite3_bind_null(stmt, 1);
    sqlite3_bind_int64(stmt, 2, file->folder);
    sqlite3_bind_int64(stmt, 3, file->fowner);
    sqlite3_bind_int(stmt, 4, file->ftype);   
    
    sqlite3_bind_text(stmt, 5, file->fname, strlen(file->fname), SQLITE_STATIC);

    if(file->fpath != NULL){
        sqlite3_bind_text(stmt, 6, file->fpath, strlen(file->fpath), SQLITE_STATIC);
    }else{
        sqlite3_bind_null(stmt, 7);
    }

    sqlite3_bind_text(stmt, 7, file->fwhere, strlen(file->fwhere), SQLITE_STATIC);

    sqlite3_bind_int64(stmt, 8, file->fctime);
    sqlite3_bind_int64(stmt, 9, file->fmtime);
    sqlite3_bind_int64(stmt, 10, file->fatime);
    sqlite3_bind_int64(stmt, 11, file->fsize);

    if(file->fsnap_len == 0){
        sqlite3_bind_null(stmt, 12);
    }else{
        sqlite3_bind_blob(stmt, 12, file->fsnap, file->fsnap_len, SQLITE_STATIC);
    }

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        sqlite3_finalize(stmt);
        return -1;
    }

    file->fid = sqlite3_last_insert_rowid(dbs->dbstor);

    sqlite3_finalize(stmt);

    return 0;
}

int dbstor_fdelete(dbstor_t * dbs, int64_t fid){
    sqlite3_stmt *stmt;
    int rc = 0;

    rc = sqlite3_prepare_v2(dbs->dbstor, sql_fdelete, strlen(sql_fdelete), &stmt, NULL);
    if(rc != SQLITE_OK){
        return -1;
    }

    sqlite3_bind_int64(stmt, 1, fid);

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        rc = -SPIE_ECODE_DBERR;
    }else{
        rc = 0;
    }

    sqlite3_finalize(stmt);

    return rc;
}

int dbstor_fupdate(dbstor_t * dbs, int64_t fid, dbstor_file_t *file){
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(dbs->dbstor, sql_fupdate, strlen(sql_fupdate), &stmt, NULL);
    if(rc != SQLITE_OK){
        return -SPIE_ECODE_DBERR;
    }

    sqlite3_bind_int64(stmt, 1, fid);
    sqlite3_bind_int64(stmt, 2, file->folder);
    sqlite3_bind_int64(stmt, 3, file->fowner);
    sqlite3_bind_int(stmt, 4, file->ftype);

    if(file->fname != NULL){
        sqlite3_bind_text(stmt, 5, file->fname, strlen(file->fname), SQLITE_STATIC);
    }else{
        log_info("fname is null\n");
        sqlite3_bind_null(stmt, 5);
    }

    if(file->fpath != NULL){
        sqlite3_bind_text(stmt, 6, file->fpath, strlen(file->fpath), SQLITE_STATIC);
    }else{
        log_info("fpath is null\n");
        sqlite3_bind_null(stmt, 6);
    }

    if(file->fwhere != NULL){
        sqlite3_bind_text(stmt, 7, file->fwhere, strlen(file->fwhere), SQLITE_STATIC);
    }else{
        log_info("where is null\n");
        sqlite3_bind_null(stmt, 7);
    }

    sqlite3_bind_int64(stmt, 8, file->fctime);
    sqlite3_bind_int64(stmt, 9, file->fmtime);
    sqlite3_bind_int64(stmt, 10, file->fatime);

    sqlite3_bind_int64(stmt, 11, file->fsize);
    
    if(file->fsnap_len == 0){
        sqlite3_bind_null(stmt, 12);
    }else{
        sqlite3_bind_blob(stmt, 12, file->fsnap, file->fsnap_len, SQLITE_STATIC);
    }

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        rc = -SPIE_ECODE_DBERR;
    }else{
        rc = 0;
    }

    sqlite3_finalize(stmt);

    return rc;
}

static int dbstor_file_from_stmt(sqlite3_stmt *stmt, dbstor_file_t *file, int withsnap){
    char *temp;
    int len;

    assert((stmt != NULL) && (file != NULL));

    memset(file, 0, sizeof(*file));

    file->fid = sqlite3_column_int64(stmt, 0);

    file->folder  = sqlite3_column_int64(stmt, 1);
    file->fowner  = sqlite3_column_int64(stmt, 2);
    file->ftype = sqlite3_column_int(stmt, 3);

    len = sqlite3_column_bytes(stmt, 4);
    if(len != 0){
        temp = (char *)sqlite3_column_text(stmt, 4);
        file->fname = osapi_malloc(len + 1);
        if(file->fname == NULL)
            return -SPIE_ECODE_NOMEM;

        strncpy(file->fname, temp, len);
        file->fname[len] = 0;
    }else{
        file->fname =  NULL;
    }

    len = sqlite3_column_bytes(stmt, 5);
    if(len != 0){
        temp = (char *)sqlite3_column_text(stmt, 5);
        file->fpath = osapi_malloc(len + 1);
        if(file->fpath == NULL){
            osapi_free(file->fname);
            file->fname = NULL;
            return -SPIE_ECODE_NOMEM;
        }

        strncpy(file->fpath, temp, len);
        file->fpath[len] = 0;
    }else{
        file->fpath =  NULL;
    }

    len = sqlite3_column_bytes(stmt, 6);
    if(len != 0){
        temp = (char *)sqlite3_column_text(stmt, 6);
        file->fwhere = osapi_malloc(len + 1);
        if(file->fwhere == NULL){
            osapi_free(file->fname);
            osapi_free(file->fpath);
            file->fname = NULL;
            file->fpath = NULL;
            return -SPIE_ECODE_NOMEM;
        }

        strncpy(file->fwhere, temp, len);
        file->fwhere[len] = 0;
    }else{
        file->fwhere =  NULL;
    }

    file->fctime = sqlite3_column_int64(stmt, 7);    
    file->fmtime = sqlite3_column_int64(stmt, 8);
    file->fatime = sqlite3_column_int64(stmt, 9);
   
    file->fsize = sqlite3_column_int64(stmt, 10);
    
    if(withsnap){
        file->fsnap_len = sqlite3_column_bytes(stmt, 11);
        if(file->fsnap_len > 0){
            file->fsnap = osapi_malloc(file->fsnap_len);
            if(file->fsnap == NULL){
                osapi_free(file->fname);
                osapi_free(file->fpath);
                osapi_free(file->fwhere);
                file->fname = NULL;
                file->fpath = NULL;
                file->fwhere = NULL;
                return -SPIE_ECODE_NOMEM;
            }
            memcpy(file->fsnap, sqlite3_column_blob(stmt, 11), file->fsnap_len);
        }else{
            file->fsnap_len = 0;
            file->fsnap = NULL;
        }
    }

    return 0;
}

int dbstor_flookup(dbstor_t * dbs, int64_t fid, dbstor_file_t **file, int withsnap){
    sqlite3_stmt *stmt;
    dbstor_file_t *f;
    int rc;

    rc = sqlite3_prepare_v2(dbs->dbstor, sql_flookup, strlen(sql_flookup), &stmt, NULL);
    if(rc != SQLITE_OK){
        return -SPIE_ECODE_DBERR;
    }

    sqlite3_bind_int64(stmt, 1, fid);

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_ROW){
        sqlite3_finalize(stmt);
        return -SPIE_ECODE_DBERR;
    }

    if(file == NULL){
        sqlite3_finalize(stmt);
        return 0;
    }else{
        if(dbstor_file_new(&f) != 0){
            sqlite3_finalize(stmt);
            return -SPIE_ECODE_NOMEM;
        }

        rc = dbstor_file_from_stmt(stmt, f, withsnap);
        if(rc != 0){
            dbstor_file_free(f);
            rc = -SPIE_ECODE_INVAL;
        }else{
            *file = f;
            rc = 0;
        }

        sqlite3_finalize(stmt);
    }

    return rc;
}

int dbstor_update_access(dbstor_t *dbs, dbstor_file_t *file, int withmtime){
    dbstor_file_t *fl;
    int rc;

    rc = dbstor_flookup(dbs, file->fid, &fl, 1);
    if(rc != 0){
        return rc;
    }

    fl->fatime = (int64_t)time(NULL);
    if(withmtime)
        fl->fmtime = fl->fatime;

    rc = dbstor_fupdate(dbs, fl->fid, fl);
    dbstor_file_free(fl);

    return rc;
}

int dbstor_flookup_byname(dbstor_t *dbs, char *fname, int64_t folder,
        dbstor_file_t **file, int withsnap){
    sqlite3_stmt *stmt;
    dbstor_file_t *f;
    int rc;

    rc = sqlite3_prepare_v2(dbs->dbstor, sql_flookup_byname,
            strlen(sql_flookup_byname), &stmt, NULL);
    if(rc != SQLITE_OK){
        return -SPIE_ECODE_DBERR;
    }

    sqlite3_bind_text(stmt, 1, fname, strlen(fname), SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 2, folder);

    rc = sqlite3_step(stmt);
    if(rc != SQLITE_ROW){
        sqlite3_finalize(stmt);
        return -SPIE_ECODE_DBERR;
    }

    if(dbstor_file_new(&f) != 0){
        sqlite3_finalize(stmt);
        return -SPIE_ECODE_DBERR;
    }

    rc = dbstor_file_from_stmt(stmt, f, withsnap);
    if(rc != 0){
        dbstor_file_free(f);
        rc = -SPIE_ECODE_INVAL;
    }else{
        *file = f;
        rc = 0;
    }

    sqlite3_finalize(stmt);

    return rc;
}

int dbstor_lucreate(dbstor_t * dbs, int64_t folder, int type, dbstor_lookup_t **lu){
    dbstor_lookup_t *l;
    char *sql;
    int rc;

    switch(type){
    case SPIE_LUTYPE_NAME:
        sql = sql_lookup_name;
        break;

    case SPIE_LUTYPE_SIZE:
        sql = sql_lookup_size;
        break;
        
    case SPIE_LUTYPE_CTIME:
        sql = sql_lookup_ctime;
        break;

    case SPIE_LUTYPE_ATIME:
        sql = sql_lookup_atime;
        break;

    default:
        return -SPIE_ECODE_INVAL;
    }

    assert((dbs != NULL) && (lu != NULL));

    l = osapi_malloc(sizeof(*l));
    if(l == NULL){
        return -ENOMEM;
    }
    memset(l, 0, sizeof(*l));
    l->luid = (int64_t)l; // address as his id

    rc = sqlite3_prepare_v2(dbs->dbstor, sql, strlen(sql), &l->stmt, NULL);
    if(rc != SQLITE_OK){
        osapi_free(l);
        return -SPIE_ECODE_INVAL;
    }

    sqlite3_bind_int64(l->stmt, 1, folder);

    *lu = l;

    return 0;
}

int dbstor_ludestroy(dbstor_t *dbs, dbstor_lookup_t *lu){
    assert((lu != NULL) && (lu->stmt != NULL));

    sqlite3_finalize(lu->stmt);
    osapi_free(lu);

    return 0;
}

int dbstor_lunext(dbstor_t *dbs, dbstor_lookup_t *lu, int len, fixarray_t **fa){
    fixarray_t *f;
    dbstor_file_t *file;
    int i;
    int rc;

    assert((lu != NULL) && (fa != NULL));

    rc = fixarray_create(len, &f);
    if(rc != 0){
        return rc;
    }

    for(i = 0; i < len; i++){
        rc = sqlite3_step(lu->stmt);
        switch(rc){
        case SQLITE_DONE:
            /* no more record in db */
            rc = 0;
            break;

        case SQLITE_ROW:
            rc = dbstor_file_new(&file);
            if(rc != 0){
                break;
            }

            rc = dbstor_file_from_stmt(lu->stmt, file, 0);
            if(rc != 0){
                dbstor_file_free(file);
                break;
            }

            fixarray_set(f, i, (void *)file);
            continue;

        default:
            rc = -1;
            break;
        }
        break;
    }

    *fa = f;

    return rc;
}


