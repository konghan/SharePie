#include "db-repo.h"
#include "spie.h"

#include "osapi.h"
#include "logger.h"
#include "sqlite3.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define SUBDIR_LEN      8

static const char dbrepo_default_basedir[][SUBDIR_LEN] = {
    "/video",
    "/music",
    "/photo",
    "/docs",
    ""
};

int dbrepo_init(dbrepo_t *repo, const char *rdir, const char *dbstor, const char *dbuser){
    char *db;
    int rc;

    assert((repo != NULL) && (rdir != NULL) && (dbstor != NULL) && (dbuser != NULL));

    memset(repo, 0, sizeof(*repo));

    repo->rdir_video = osapi_strdup(rdir, SUBDIR_LEN);
    if(repo->rdir_video == NULL){
        log_warn("init repo rdir or rdb fail!\n");
        return -1;
    }
    strcat(repo->rdir_video, dbrepo_default_basedir[0]);
    mkdir(repo->rdir_video, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
    
    repo->rdir_music = osapi_strdup(rdir, SUBDIR_LEN);
    if(repo->rdir_music == NULL){
        log_warn("init repo rdir or rdb fail!\n");
        goto fail_exit;
    }
    strcat(repo->rdir_music, dbrepo_default_basedir[1]);
    mkdir(repo->rdir_music, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
    
    repo->rdir_photo = osapi_strdup(rdir, SUBDIR_LEN);
    if(repo->rdir_photo == NULL){
        log_warn("init repo rdir or rdb fail!\n");
        goto fail_exit;
    }
    strcat(repo->rdir_photo, dbrepo_default_basedir[2]);
    mkdir(repo->rdir_photo, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
    
    repo->rdir_docs = osapi_strdup(rdir, SUBDIR_LEN);
    if(repo->rdir_docs == NULL){
        log_warn("init repo rdir or rdb fail!\n");
        goto fail_exit;
    }
    strcat(repo->rdir_docs, dbrepo_default_basedir[3]);
    mkdir(repo->rdir_docs, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
    
    db = osapi_strdup(rdir, strlen(dbuser) + 1);
    if(db == NULL){
        goto fail_exit;
    }
    strcat(db, dbuser);

    rc =  dbuser_init(&repo->dbuser, db);
    osapi_free(db);
    if(rc != 0){
        log_warn("init dbuser fail\n");
        goto fail_exit;
    }

    db = osapi_strdup(rdir, strlen(dbstor) + 1);
    if(db == NULL){
        dbuser_fini(&repo->dbuser);
        goto fail_exit;
    }
    strcat(db, dbstor);

    rc = dbstor_init(&repo->dbstor, db);
    osapi_free(db);
    if(rc != 0){
        log_warn("repo init open rdb fail!\n");
        dbuser_fini(&repo->dbuser);
        goto fail_exit;
    }
    
    fixmap_init(&repo->folders);

    pthread_mutex_init(&repo->mlock, NULL);

    return 0;

fail_exit:
    if(repo->rdir_video != NULL)
        osapi_free(repo->rdir_video);

    if(repo->rdir_music != NULL)
        osapi_free(repo->rdir_music);

    if(repo->rdir_photo != NULL)
        osapi_free(repo->rdir_photo);

    if(repo->rdir_docs != NULL)
        osapi_free(repo->rdir_docs);

    return -1;
}

int dbrepo_fini(dbrepo_t *repo){
    fixmap_node_t *fmn;
    struct rb_node *node;
    rfolder_t *folder;

    assert(repo != NULL);

    dbuser_fini(&repo->dbuser);
    dbstor_fini(&repo->dbstor);
    
    osapi_free(repo->rdir_video);
    osapi_free(repo->rdir_music);
    osapi_free(repo->rdir_photo);
    osapi_free(repo->rdir_docs);

    pthread_mutex_destroy(&repo->mlock);

    fixmap_for_each((&repo->folders.root), node, fmn){
        folder = container_of(fmn, rfolder_t, node);
        osapi_free(folder->fname);
        osapi_free(folder);
    }

    return 0;
}

char *dbrepo_build_folder_path(dbrepo_t *repo, int64_t folder){
    fixmap_node_t *node = NULL;
    dbstor_file_t *rf;
    rfolder_t *rfd;
    char *rpath = NULL;
    char *temp = NULL;
    int len;
    char *rdir;
    int rc;

    if(folder < 0)
        return NULL;

    // 0 ~ SPIE_FOLDER_DOCS is root folder of defent type
    while(folder > SPIE_FOLDER_DOCS){

        pthread_mutex_lock(&repo->mlock);
        rc = fixmap_get(&repo->folders, folder, &node);
        pthread_mutex_unlock(&repo->mlock);

        if(rc == 0){
            rfd = container_of(node, rfolder_t, node);
        }else{
            rc = dbstor_flookup(&repo->dbstor, folder, &rf, 0);
            if(rc != 0){
                return NULL;
            }
        
            rfd = osapi_malloc(sizeof(*rfd));
            if(rfd == NULL){
                dbstor_file_free(rf);
                return NULL;
            }
        
            fixmap_node_init(&rfd->node);

            rfd->fname =  rf->fname;
            rf->fname = NULL;
            rfd->fup = rf->folder;

            dbstor_file_free(rf);

            pthread_mutex_lock(&repo->mlock);
            rc = fixmap_insert(&repo->folders, folder, &rfd->node);
            pthread_mutex_unlock(&repo->mlock);
       
            if(rc != 0){
                osapi_free(rfd->fname);
                osapi_free(rfd);
                return NULL;
            }
        }

        folder = rfd->fup;

        rpath = osapi_strdup(temp, strlen(rfd->fname) + 2);
        if(rpath == NULL){
            if(temp != NULL)
                osapi_free(temp);
            return NULL;
        }

        strcpy(rpath, "/");
        strcat(rpath, rfd->fname);

        if(temp != NULL){
            strcat(rpath, temp);
            osapi_free(temp);
        }

        temp = rpath;
    }

    if(rpath != NULL)
        len = strlen(rpath);
    else
        len = 1;

    switch(folder){
    case SPIE_FOLDER_VIDEO:
        rdir = repo->rdir_video;
        rpath = osapi_malloc(len + strlen(repo->rdir_video) + 1);
        break;

    case SPIE_FOLDER_MUSIC:
        rdir = repo->rdir_music;
        rpath = osapi_malloc(len + strlen(repo->rdir_music) + 1);
        break;

    case SPIE_FOLDER_PHOTO:
        rdir = repo->rdir_photo;
        rpath = osapi_malloc(len + strlen(repo->rdir_photo) + 1);
        break;

    case SPIE_FOLDER_DOCS:
        rdir = repo->rdir_docs;
        rpath = osapi_malloc(len + strlen(repo->rdir_docs) + 1);
        break;

    default:
        osapi_free(temp);
        return NULL;
    }

    if(rpath == NULL){
        if(temp != NULL)
            osapi_free(temp);
        return NULL;
    }

    strcpy(rpath, rdir);

    if(temp != NULL){
        strcat(rpath, temp);
        osapi_free(temp);
    }

    return rpath;
}

char *dbrepo_build_rpath(dbrepo_t *repo, char *fname, int64_t folder){
    char *rpath;
    char *temp;

    temp = dbrepo_build_folder_path(repo, folder);
    if(temp == NULL)
        return NULL;

    rpath = osapi_strdup(temp, strlen(fname) + 1);
    if(rpath == NULL){
        osapi_free(temp);
        return NULL;
    }

    strcat(rpath, "/");
    strcat(rpath, fname);

    osapi_free(temp);

    return rpath;
}

char *dbrepo_file_buidpath(dbrepo_t *repo, dbstor_file_t *file){
    return dbrepo_build_rpath(repo, file->fname, file->folder);
}

static char *dbrepo_rootpath(dbrepo_t *repo, int folder){
    
    switch(folder){
    case SPIE_FOLDER_VIDEO:
        return repo->rdir_video;

    case SPIE_FOLDER_MUSIC:
        return repo->rdir_music;

    case SPIE_FOLDER_PHOTO:
        return repo->rdir_photo;

    case SPIE_FOLDER_DOCS:
        return repo->rdir_docs;

    default:
        return NULL;
    }

    return NULL;
}

int dbrepo_is_insubdir(dbrepo_t *repo, const char *fpath){
    char *rpath = dbrepo_rootpath(repo, SPIE_FOLDER_VIDEO);
    if(osapi_is_inpath(rpath, fpath))
        return SPIE_FOLDER_VIDEO;
    
    rpath = dbrepo_rootpath(repo, SPIE_FOLDER_MUSIC);
    if(osapi_is_inpath(rpath, fpath))
        return SPIE_FOLDER_MUSIC;
    
    rpath = dbrepo_rootpath(repo, SPIE_FOLDER_PHOTO);
    if(osapi_is_inpath(rpath, fpath))
        return SPIE_FOLDER_PHOTO;
    
    rpath = dbrepo_rootpath(repo, SPIE_FOLDER_DOCS);
    if(osapi_is_inpath(rpath, fpath))
        return SPIE_FOLDER_DOCS;

    return -1;
}

int dbrepo_clean(dbrepo_t *repo){
    return 0;
}

