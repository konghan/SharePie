#include "svr-stor.h"

#include "spie.h"

#include "db-repo.h"
#include "server.h"
#include "spie_stor.h"
#include "osapi.h"
#include "logger.h"
 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static int svr_fstat_rsp_from_dbfile(dbrepo_t *repo, dbstor_file_t *file,
        stor_fstat_rsp_t **fstat){
    stor_fstat_rsp_t *r;
    char *rpath;
    struct stat sb;
    int rc;

    r = osapi_malloc(sizeof(*r));
    if(r == NULL){
        log_warn("no memory for fstat rsp\n");
        return -SPIE_ECODE_NOMEM;
    }
    memset(r, 0, sizeof(*r));

    r->fid = file->fid;
    r->fname = file->fname;
    r->folder = file->folder;
    file->fname = NULL;
    
    r->type = file->ftype;
    r->snap_len = file->fsnap_len;
    r->snap = file->fsnap;
    r->origin = file->fwhere;
    file->fsnap_len = 0;
    file->fsnap = NULL;
    file->fwhere = NULL;

    if(file->ftype == SPIE_TYPE_ROOTFS){
        rpath = file->fpath;
        file->fpath = NULL;
    }else{
        rpath = dbrepo_build_rpath(repo, r->fname, r->folder);
        if(rpath == NULL){
            stor_fstat_rsp_free(r);
            return -SPIE_ECODE_NOMEM;
        }
    }

    rc = stat(rpath, &sb);
    osapi_free(rpath);
    if(rc != 0){
        stor_fstat_rsp_free(r);
        return -1;
    }

    if(!(file->ftype != SPIE_TYPE_FOLDER)){
        r->fsize = (int64_t)sb.st_size;
    }else{
        r->fsize = 0;
    }

#ifdef __MACH__
    r->fatime = (int64_t)sb.st_atimespec.tv_sec;   /* time of last access */
    r->fmtime = (int64_t)sb.st_mtimespec.tv_sec;   /* time of last modification */
    r->fctime = (int64_t)sb.st_ctimespec.tv_sec;   /* time of last status change */
#else
#ifdef ANDROID_NDK
    r->fatime = (int64_t)sb.st_atime;   /* time of last access */
    r->fmtime = (int64_t)sb.st_mtime;   /* time of last modification */
    r->fctime = (int64_t)sb.st_ctime;   /* time of last status change */
#else
    r->fatime = (int64_t)sb.st_atim.tv_sec;   /* time of last access */
    r->fmtime = (int64_t)sb.st_mtim.tv_sec;   /* time of last modification */
    r->fctime = (int64_t)sb.st_ctim.tv_sec;   /* time of last status change */
#endif
#endif
    *fstat = r;

    return 0;
}

// file management operations
int svr_request_fstat_routine(session_t *ses, stor_fstat_req_t *req,
        stor_fstat_rsp_t **rsp){
    dbrepo_t *repo = &ses->server->repo;
    dbstor_file_t *file;
    int rc;

    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);

    rc = dbstor_flookup(&repo->dbstor, req->fid, &file, 0);
    if(rc != 0){
        log_warn("file not exist in dbstor\n");
        return -1;
    }
    
    rc = svr_fstat_rsp_from_dbfile(repo, file, rsp);
    dbstor_file_free(file);
    if(rc != 0){
        log_warn("stat file fail\n");
        return -1;
    }
    
    return 0;
}

int svr_request_fopen_routine(session_t *ses, stor_fopen_req_t *req,
        stor_fopen_rsp_t **rsp){
    dbrepo_t *repo = &ses->server->repo;
    dbstor_file_t *file;
    rfile_t *rf;
    char *rpath;
    int rc;
 
    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);
  
    rc = session_rfile_find(ses, req->fid, &rf);
    if(rc == 0){
        return 0;
    }
 
    rc = dbstor_flookup(&repo->dbstor, req->fid, &file, 0);
    if(rc != 0){
        return -SPIE_ECODE_NOENT;
    }

    if(file->ftype == SPIE_TYPE_FOLDER){
        dbstor_file_free(file);
        return -1;
    }

    rf = osapi_malloc(sizeof(*rf));
    if(rf == NULL){
        dbstor_file_free(file);
        return -SPIE_ECODE_NOMEM;
    }
    memset(rf, 0, sizeof(*rf));
  
    rf->fid = req->fid;
    rf->file = file;

    if(file->ftype == SPIE_TYPE_NATIVE){
        rpath = dbrepo_build_rpath(repo, file->fname, file->folder);
        rf->handle = fopen(rpath, "r+b");
        osapi_free(rpath);
    }else{
        rpath = file->fpath;
        rf->handle = fopen(rpath, "r+b");
    }

    if(rf->handle == NULL){
        dbstor_file_free(file);
        osapi_free(rf);
        return -1;
    }

    rf->write = 0;
    fixmap_node_init(&rf->node);
    session_rfile_insert(ses, req->fid, rf);

    return 0;
}

int svr_request_fclose_routine(session_t *ses, stor_fclose_req_t *req,
        stor_fclose_rsp_t **rsp){
    dbrepo_t *repo = &ses->server->repo;
    rfile_t *rf;
    int rc;

    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);

    rc = session_rfile_remove(ses, req->fid, &rf);
    if(rc != 0){
        return -1;
    }
   
    // update database
    if(rf->write){
        dbstor_update_access(&repo->dbstor, rf->file, 1);
    }else{
        dbstor_update_access(&repo->dbstor, rf->file, 0);
    }

    // no referent, release resource
    fclose(rf->handle);
    dbstor_file_free(rf->file);
    osapi_free(rf);
    
    return 0;
}

int svr_request_fread_routine(session_t *ses, stor_fread_req_t *req,
        stor_fread_rsp_t **rsp){
    stor_fread_rsp_t *r;
    rfile_t *rf;
    int rc;

    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);

    rc = session_rfile_find(ses, req->fid, &rf);
    if(rc != 0){
        log_warn("file not exist\n");
        return -SPIE_ECODE_NOENT;
    }

    r = osapi_malloc(sizeof(*r));
    if(r == NULL){
        log_warn("no memory for fread rsp\n");
        return -SPIE_ECODE_NOMEM;
    }
    memset(r, 0, sizeof(*r));

    r->data = osapi_malloc(req->len);
    if(r->data == NULL){
	log_warn("no memory for fread data\n");
        osapi_free(r);
        return -SPIE_ECODE_NOMEM;
    }

    clearerr(rf->handle);
    fseek(rf->handle, req->offset, SEEK_SET);
    r->data_len = fread(r->data, 1, req->len, rf->handle);
    if(r->data_len == 0){
	if(feof(rf->handle)){
	    rc = -SPIE_ECODE_EOF;
	}else{
	    rc = -SPIE_ECODE_INVAL;
	}

	osapi_free(r->data);
        osapi_free(r);

        return rc;
    }else{
	log_info("read routine size:%d\n", (int)r->data_len);
        *rsp = r;
        return 0;
    }
}

int svr_request_fwrite_routine(session_t *ses, stor_fwrite_req_t *req,
        stor_fwrite_rsp_t **rsp){
    stor_fwrite_rsp_t *r;
    rfile_t *rf;
    int rc;

    log_info("fwrite at:%d size:%d\n", (int)req->offset,req->data_len);

    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);
    if(req->data_len <= 0){
        log_warn("fwrite data len is invalid:%d\n", req->data_len);
        return -1;
    }

    rc = session_rfile_find(ses, req->fid, &rf);
    if(rc != 0){
        log_warn("fwrite file not opened\n");
        return -SPIE_ECODE_NOENT;
    }

    fseek(rf->handle, req->offset, SEEK_SET);

    r = osapi_malloc(sizeof(*r));
    if(r == NULL){
        log_warn("no memory for fread rsp\n");
        return -SPIE_ECODE_NOMEM;
    }
    memset(r, 0, sizeof(*r));
   
    r->len = fwrite(req->data, 1, req->data_len, rf->handle);
    if(r->len <= 0){
        log_warn("write data to file fail\n");
        osapi_free(r);
        return -SPIE_ECODE_INVAL;
    }else{
        log_info("data size %d have write to file at:%d\n", r->len, (int)req->offset);
        rf->write = 1;
        *rsp = r;
        return 0;
    }
}

int svr_request_ftruncate_routine(session_t *ses, stor_ftruncate_req_t *req,
        stor_ftruncate_rsp_t **rsp){
    rfile_t *rf;
    int rc;

    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);

    rc = session_rfile_find(ses, req->fid, &rf);
    if(rc != 0){
        log_warn("file have opened!\n");
        return -SPIE_ECODE_NOENT;
    }

    rc = ftruncate(fileno(rf->handle), req->len);
    if(rc != 0){
        log_warn("truncate file fail:%d\n", rc);
        return -SPIE_ECODE_MEDIAERR;
    }
    
    rf->write = 1;

    return 0;
}

static int svr_stat_file(const char *path, dbstor_file_t *file){
    struct stat fs;
    int rc;

    rc = stat(path, &fs);
    if(rc != 0){
        return -1;
    }

#ifdef __MACH__
    file->fatime = (int64_t)fs.st_atimespec.tv_sec;
    file->fctime = (int64_t)fs.st_ctimespec.tv_sec;
    file->fmtime = (int64_t)fs.st_mtimespec.tv_sec;
#else
#ifdef ANDROID_NDK
    file->fatime = (int64_t)fs.st_atime;   /* time of last access */
    file->fmtime = (int64_t)fs.st_mtime;   /* time of last modification */
    file->fctime = (int64_t)fs.st_ctime;   /* time of last status change */
#else
    file->fatime = (int64_t)fs.st_atim.tv_sec;
    file->fctime = (int64_t)fs.st_ctim.tv_sec;
    file->fmtime = (int64_t)fs.st_mtim.tv_sec;
#endif
#endif

    file->fsize  = (int64_t)fs.st_size;

    return 0;
}

int svr_request_fcreate_routine(session_t *ses, stor_fcreate_req_t *req,
        stor_fcreate_rsp_t **rsp){
    stor_fcreate_rsp_t *r;
    dbstor_file_t *file, df;
    dbrepo_t *repo = &ses->server->repo;
    char *rpath;
    FILE *handle;
    int rc;

    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);

    if((req->fname == NULL) || (req->folder < 0))
        return -1;

    rc = dbstor_flookup_byname(&repo->dbstor, req->fname, req->folder, &file, 0);
    if(rc == 0){
        log_warn("file already exist");
        dbstor_file_free(file);
        return -SPIE_ECODE_EXIST;
    }

    df.folder = req->folder;
    df.fowner = session_uid(ses);

    df.fname = req->fname;
    df.fwhere = session_where(ses);
    df.fsnap_len = 0;
    df.fsnap = NULL;

    if(req->type == SPIE_TYPE_ROOTFS){
        rc = svr_stat_file(req->fpath, &df);
        if(rc != 0){
            return -1;
        }

        // in repo or not
        if(dbrepo_is_insubdir(repo, req->fpath) >= 0){
            rpath = dbrepo_build_rpath(repo, req->fname, req->folder);
            if(rpath == NULL){
                return -1;
            }

            rc = rename(req->fpath, rpath);
            osapi_free(rpath);
            if(rc != 0){
                return -1;
            }
            df.ftype = SPIE_TYPE_NATIVE;
            df.fpath = NULL;
        }else{

            df.fpath = req->fpath;
            df.ftype = req->type;
        }
    }else{
        df.fctime = (int64_t)time(NULL);
        df.fmtime = (int64_t)time(NULL);
        df.fatime = (int64_t)time(NULL);
        df.fsize  = 0;

        df.ftype = req->type;
        df.fpath = NULL;
    }

    rc = dbstor_finsert(&repo->dbstor, &df);
    if(rc != 0){
        log_warn("delete file in db fail\n");
        return -1;
    }

    if(df.ftype == SPIE_TYPE_FOLDER){
        rpath = dbrepo_build_rpath(repo, df.fname, df.folder);
        if(rpath == NULL){
            return -1;
        }

        mkdir(rpath, 0777);
        osapi_free(rpath);

    }else if(df.ftype == SPIE_TYPE_NATIVE){
 
        rpath = dbrepo_build_rpath(repo, df.fname, df.folder);
        if(rpath == NULL){
            return -1;
        }
        handle = fopen(rpath,"wb+");
        osapi_free(rpath);
        if(handle == NULL){
            log_warn("create file fail\n");
            return -1;
        }
        fclose(handle);
    }

    r = osapi_malloc(sizeof(*r));
    if(r == NULL){
        log_warn("no memory for fread rsp\n");
        return -SPIE_ECODE_NOMEM;
    }
    memset(r, 0, sizeof(*r));
    r->fid = df.fid;
 
    *rsp = r;
    
    return 0;
}

int svr_request_fdelete_routine(session_t *ses, stor_fdelete_req_t *req,
        stor_fdelete_rsp_t **rsp){
    dbstor_file_t *file;
    dbrepo_t *repo = &ses->server->repo;
    rfile_t *rf;
    char *rpath;
    int rc;

    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);

    rc = session_rfile_find(ses, req->fid, &rf);
    if(rc == 0){
        log_warn("file still opened\n");
        return -EEXIST;
    }

    rc = dbstor_flookup(&repo->dbstor, req->fid, &file, 0);
    if(rc != 0){
        log_warn("file not in db\n");
        return -SPIE_ECODE_NOENT;
    }

    rc = dbstor_fdelete(&repo->dbstor, req->fid);
     if(rc != 0){
        log_warn("delete file in db fail\n");
        dbstor_file_free(file);
        return -1;
    }

    if(file->ftype == SPIE_TYPE_ROOTFS){
        rpath = file->fpath;
        file->fpath = NULL;
    }else{
        rpath = dbrepo_build_rpath(repo, file->fname, file->folder);
    }

    if(rpath == NULL){
        log_warn("build real path fail\n");
        dbstor_file_free(file);
        return -1;
    }

    dbstor_file_free(file);

    rc = remove(rpath);
    osapi_free(rpath);
    if(rc != 0){
        log_warn("remove file fail!\n");
        return -1;
    }
    
    return 0;
}

int svr_request_fmoveto_routine(session_t *ses, stor_fmoveto_req_t *req,
        stor_fmoveto_rsp_t **rsp){
    dbstor_file_t *file, *folder, df;
    dbrepo_t *repo = &ses->server->repo;
    rfile_t *rf;
    char *rpath_src, *rpath_dst;
    int rc;

    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);

    if(req->fname == NULL)
        return -1;

    rc = session_rfile_find(ses, req->fid, &rf);
    if(rc == 0){
        log_warn("file still opened\n");
        return -EEXIST;
    }

    rc = dbstor_flookup(&repo->dbstor, req->fid, &file, 0);
    if(rc != 0){
        log_warn("file not in db\n");
        return -SPIE_ECODE_NOENT;
    }

    rc = dbstor_flookup(&repo->dbstor, req->folder, &folder, 0);
    if(rc != 0){
        log_warn("dst folder not exist\n");
        dbstor_file_free(file);
        return -SPIE_ECODE_NOENT;
    }
    dbstor_file_free(folder);

    df.fid = file->fid;
    df.folder = req->folder;
    df.fowner = file->fowner;
    df.ftype = file->ftype;

    df.fname = req->fname;
    df.fwhere = file->fwhere;
    df.fpath = file->fpath;
    
    df.fctime = file->fctime;
    df.fmtime = file->fmtime;
    df.fatime = file->fatime;

    df.fsnap_len = file->fsnap_len;
    df.fsnap = file->fsnap;
    
    if(df.ftype != SPIE_TYPE_ROOTFS){
        rpath_src = dbrepo_build_rpath(repo, file->fname, file->folder);
        if(rpath_src == NULL){
            dbstor_file_free(file);
            return -1;
        }

        rpath_dst = dbrepo_build_rpath(repo, df.fname, df.folder);
        if(rpath_dst == NULL){
            dbstor_file_free(file);
            osapi_free(rpath_src);
            return -1;
        }

        rc = rename(rpath_src, rpath_dst);
        if(rc != 0){
            log_warn("rename file fail\n");
            osapi_free(rpath_src);
            osapi_free(rpath_dst);
            dbstor_file_free(file);
            return -1;
        }
   
        rc = dbstor_fupdate(&repo->dbstor, file->fid, &df);
        if(rc != 0){
            rename(rpath_dst, rpath_src);
            dbstor_file_free(file);
            osapi_free(rpath_src);
            osapi_free(rpath_dst);
            log_warn("update db fail!\n");
            return -1;
        }

        osapi_free(rpath_src);
        osapi_free(rpath_dst);
    }else{
        rc = dbstor_fupdate(&repo->dbstor, file->fid, &df);
        if(rc != 0){
            dbstor_file_free(file);
            log_warn("update db fail!\n");
            return -1;
        }
    }

    dbstor_file_free(file);
    
    return 0;
}


int svr_request_freadsnap_routine(session_t *ses, stor_freadsnap_req_t *req,
        stor_freadsnap_rsp_t **rsp){
    dbrepo_t *repo = &ses->server->repo;
    dbstor_file_t *file;
    stor_freadsnap_rsp_t *r;
    rfile_t *rf;
    int rc;
   
    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);

    rc = session_rfile_find(ses, req->fid, &rf);
    if(rc == 0){
        return -SPIE_ECODE_OPENED;
    }
 
    rc = dbstor_flookup(&repo->dbstor, req->fid, &file, 1);
    if(rc != 0){
        return -SPIE_ECODE_NOENT;
    }

    if(file->ftype == SPIE_TYPE_FOLDER){
        dbstor_file_free(file);
        return -SPIE_ECODE_NOTFILE;
    }

    r = osapi_malloc(sizeof(*r));
    if(r == NULL){
        dbstor_file_free(file);
        return -SPIE_ECODE_NOMEM;
    }
    memset(r, 0, sizeof(*r));
  
    r->fid = req->fid;
    r->snap_len = file->fsnap_len;
    r->snap = file->fsnap;
    file->fsnap_len = 0;
    file->fsnap = NULL;

    dbstor_file_free(file);
    
    *rsp = r;

    return 0;
}

int svr_request_fwritesnap_routine(session_t *ses, stor_fwritesnap_req_t *req,
        stor_fwritesnap_rsp_t **rsp){
    dbrepo_t *repo = &ses->server->repo;
    dbstor_file_t *file;
    rfile_t *rf;
    int rc;
   
    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);
    if(req->snap_len <= 0)
        return -1;

    rc = session_rfile_find(ses, req->fid, &rf);
    if(rc == 0){
        return -SPIE_ECODE_OPENED;
    }
 
    rc = dbstor_flookup(&repo->dbstor, req->fid, &file, 0);
    if(rc != 0){
        return -SPIE_ECODE_NOENT;
    }

    if(file->ftype == SPIE_TYPE_FOLDER){
        dbstor_file_free(file);
        return -SPIE_ECODE_NOTFILE;
    }

    file->fsnap_len = req->snap_len;
    file->fsnap = req->snap;

    rc = dbstor_fupdate(&repo->dbstor, req->fid, file);

    dbstor_file_free(file);

    return rc;
}


int svr_request_lookup_create_routine(session_t *ses, stor_lookup_create_req_t *req,
        stor_lookup_create_rsp_t **rsp){
    stor_lookup_create_rsp_t *r;
    dbrepo_t *repo = &ses->server->repo;
    dbstor_lookup_t *lu;
    int rc;

    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);

    rc = dbstor_lucreate(&repo->dbstor, req->folder, req->type, &lu);
    if(rc != 0){
        log_warn("create lookup fail:%d\n", rc);
        return -1;
    }

    session_lookup_insert(ses, lu->luid, lu);

    r = osapi_malloc(sizeof(*r));
    if(r == NULL){
        log_warn("no memory for fread rsp\n");
        return -SPIE_ECODE_NOMEM;
    }
    memset(r, 0, sizeof(*r));
 
    r->luid = lu->luid;
    *rsp = r;
    
    return 0;
}

int svr_request_lookup_destroy_routine(session_t *ses, stor_lookup_destroy_req_t *req,
        stor_lookup_destroy_rsp_t **rsp){
    dbrepo_t *repo = &ses->server->repo;
    dbstor_lookup_t *lu;
    int rc;

    rc = session_lookup_remove(ses, req->luid, &lu);
    if(rc != 0){
        log_warn("lookup not exist\n");
        return -SPIE_ECODE_NOENT;
    }

    dbstor_ludestroy(&repo->dbstor, lu);
    
    return 0;
}

int svr_request_lookup_next_routine(session_t *ses, stor_lookup_next_req_t *req,
        stor_lookup_next_rsp_t **rsp){
    stor_lookup_next_rsp_t *r;
    dbrepo_t *repo = &ses->server->repo;
    dbstor_lookup_t *lu;
    dbstor_file_t *file;
    stor_fstat_rsp_t *fstat;
    fixarray_t *fa;
    int i;
    int rc;

    rc = session_lookup_find(ses, req->luid, &lu);
    if(rc != 0){
        log_warn("lookup not exist\n");
        return -SPIE_ECODE_NOENT;
    }

    rc = dbstor_lunext(&repo->dbstor, lu, req->num, &fa);
    if(rc != 0){
        log_warn("lookup call lunext fail\n");

        //for(i = 0; i < fixarray_num(fa); i++){
        //    fixarray_get(fa, i, (void **)&file);
        //    dbstor_file_free(file);
        //}

        fixarray_destroy(fa, (fixarray_itemfree)dbstor_file_free);

        return -1;
    }

    r = osapi_malloc(sizeof(*r));
    if(r == NULL){
        log_warn("no memory for fread rsp\n");
        return -SPIE_ECODE_NOMEM;
    }
    memset(r, 0, sizeof(*r));

    for(i = 0; i < fixarray_num(fa); i++){
        fixarray_get(fa, i, (void**)&file);
        
        fstat = NULL;
        svr_fstat_rsp_from_dbfile(repo, file, &fstat);
        fixarray_set(fa, i, fstat);

        dbstor_file_free(file);
    }

    r->rfiles = fa;
    *rsp = r;
    
    return 0;
}

