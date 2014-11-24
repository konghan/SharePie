
#include "main.h"

#include "osapi.h"
#include "list.h"

#include "fixbuffer.h"
#include "fixarray.h"

#include "spie_stor.h"
#include "spie_mgmt.h"

#include "cli-mgmt.h"
#include "cli-stor.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>


static const char kftypes[][6] = {
    "video",
    "music",
    "photo",
    "docs",
};

static int get_type(const char *type){
    int i;

    for(i = 0; i < 4; i++){
        if(strcmp(type, kftypes[i]) == 0){
            return i;
        }
    }
    return -1;
}

int list_func(client_t *cli, const char *type, const char *folderid, const char *notused){
    int folder = 0;
    int ftype = -1;
    int i;
    int64_t luid;
    fixarray_t *fa;
    rfile_t *rf;
    int num = 20;
    int rc = 0;

    ftype = SPIE_LUTYPE_NAME;

    if(folderid != NULL){
        folder = atoi(folderid);
    }else{
        folder = get_type(type);
        if(folder < 0){
//            usage();
            return -1;
        }
   }

    luid = clis_lookup_create(cli, folder, ftype);
    if(luid < 0){
        printf("create lookup fail!\n");
        return -1;
    }
    printf("create lookup ok\n");

    while(1){
        rc = clis_lookup_next(cli, luid, num, &fa);
        if(rc != 0){
            printf("lookup fetch data fail!\n");
            break;
        }

        for(i = 0; i < fixarray_num(fa); i++){
            rc = fixarray_get(fa, i, (void **)&rf);
            if(rc != 0)
                break;

            printf("name:%s folder:%"PRId64" - id:%"PRId64" - len:%"PRId64"\n",
                    rf->fname, rf->folder, rf->fid, rf->fsize);
            
            // stor_fstat_rsp_free((stor_fstat_rsp_t *)rf);
        }

        if(fixarray_num(fa) != num){
            fixarray_destroy(fa, (fixarray_itemfree)stor_fstat_rsp_free);
            break;
        }
        fixarray_destroy(fa, (fixarray_itemfree)stor_fstat_rsp_free);
        fa = NULL;
    }

    clis_lookup_destroy(cli, luid);
    
    return 0;
}


int push_func(client_t *cli, const char *lofname, const char *refname, const char *notused){
    char *rtype;
    char *rfile;
    char *temp;
    int type;
    int64_t fid;
    FILE *handle;
    int rc;
    int i;
    void *buf;
    size_t len = 4096, rlen;
    off_t offset = 0;

    printf("enter push\n");

    // prepare params
    if(refname == NULL){
//        usage();
        return -1;
    }
    rtype = strdup(refname);
    rfile = strchr(rtype, '/');
    if(rfile == NULL){
        temp = strrchr(lofname, '/');
        if(temp == NULL){
            rfile = (char *)lofname;
        }else{
            rfile = temp + 1;
        }
    }else{
        rfile[0] = 0;
        rfile += 1;
    }

    type = get_type(rtype);
    if(type < 0){
        free(rtype);
//        usage();
        return -1;
    }

    printf("open local file:%s\n", lofname);
    // check local file is exist
    handle = fopen(lofname, "r+b");
    if(handle == NULL){
        printf("local file :%s can't be open!\n", lofname);
        free(rtype);
        return -1;
    }

    printf("call create remote file\n");
    // create file
    fid = clis_file_create(cli, rfile, SPIE_TYPE_NATIVE, type);
    if(fid < 0){
        printf("create remote file:%s fail\n", rfile);
        free(rtype);
        fclose(handle);
        return -1;
    }
    free(rtype);

    printf("open remote file\n");
    rc = clis_file_open(cli, fid);
    if(rc != 0){
        printf("open remote file fail\n");
        return -1;
    }

    buf = malloc(len);
    if(buf == NULL){
        printf("alloc memory fail\n");
        return -1;
    }

    printf("write remote file\n");
    i = 0;
    while(1){
        rlen = fread(buf, 1, len, handle);
        if(rlen > 0){
            rc = clis_file_write(cli, fid, offset, buf, rlen);
            if(rc != rlen){
                printf("write remote file fail\n");
                break;
            }
            offset += rlen;
        }else{
            break;
        }
        
        if((i++%100) == 0)
            printf(".");
    }

    printf("close remote file\n");
    clis_file_close(cli, fid);
    fclose(handle);

    printf("\npush file ok\n");

    return 0;
}

int pull_func(client_t *cli, const char *fidstr, const char *lofname, const char *notused){
    FILE *handle;
    int64_t fid;
    void *buf;
    size_t len = 4096, wlen;
    int i, rc;

    printf("pull file from remote\n");

    fid = atoi(fidstr);
    if(lofname == NULL){
        printf("you must give local file name to receive data!\n");
        return -1;
    }
    
    printf("open local file:%s\n", lofname);

    handle = fopen(lofname, "wb+");
    if(handle == NULL){
        printf("open local file for write fail!\n");
        return -1;
    }
    
    printf("open remote file id:%d\n", (int)fid);
    rc = clis_file_open(cli, fid);
    if(rc != 0){
        printf("open remote file fail\n");
        fclose(handle);
        return -1;
    }

    i = 0;
    buf = malloc(len);
    if(buf == NULL){
        printf("no memory for pull\n");
        return -1;
    }

    while(1){

	printf("read data from remote file\n");
        rc = clis_file_read(cli, fid, i*len, buf, len);
        if(rc < 0){
            printf("read remote file fail:%d\n", rc);
            fclose(handle);
            free(buf);
            return -1;
        }

	printf("write data to remote file:%d\n", (int)rc);
        wlen = fwrite(buf, 1, (size_t)rc, handle);
        if(wlen != (size_t)rc){
            printf("write localfile fail\n");
            return -1;
        }
        i++;

        if((size_t)rc != wlen){
            printf("not all data have been write to local\n");
            break;
        }

        if((i%100) == 0)
            printf(".");

        if(rc != len)
            break;
    }

    fclose(handle);
    clis_file_close(cli, fid);
    
    printf("\npull file ok\n");

    return 0;
}

int move_func(client_t *cli, const char *fidstr, const char *folderidstr,
        const char *new_name){
    int64_t fid;
    int64_t folder;
    char *nname = NULL;

    fid = atoi(fidstr);
    folder = atoi(folderidstr);
    if(new_name != NULL){
        nname = strdup(new_name);
    }

    if(clis_file_moveto(cli, fid, folder, nname) != 0){
        printf("move file fail!\n");
        return -1;
    }

    printf("move file ok\n");

    return 0;
}

int add_func(client_t *cli, const char *lofname, const char *folderstr,
        const char *notused2){
    struct stat st;
    int64_t fid;
    int64_t folder;
    int rc;

    folder = atoi(folderstr);

    rc = stat(lofname, &st);
    if(rc != 0){
        printf("local file is not exist\n");
        return -1;
    }

    if(S_ISDIR(st.st_mode)){
        printf("path %s is directory, not file\n", lofname);
        return -1;
    }
    
    fid = clis_file_create(cli, (char*)lofname, SPIE_TYPE_ROOTFS, folder);
    if(fid < 0){
        printf("add file to sharepie fail\n");
    }else{
        printf("add file to sharepie ok\n");
    }

    return 0;
}

int rmv_func(client_t *cli, const char *fidstr, const char *notused1,
        const char *notused2){
    int64_t fid;

    fid = atoi(fidstr);

    if(clis_file_delete(cli, fid) != 0){
        printf("rm file fail!\n");
        return -1;
    }

    printf("rm file ok\n");

    return 0;
}

