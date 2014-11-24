#include "main.h"

#include "osapi.h"
#include "list.h"

#include "fixbuffer.h"
#include "fixarray.h"

#include "spie_stor.h"
#include "spie_mgmt.h"

#include "cli-mgmt.h"
#include "cli-stor.h"

#define LIST_USER_MAX           32
#define DEFAULT_USER_TYPE       SPIE_PRIORITY_NORMAL

int login_func(client_t *cli, const char *user, const char *passwd, const char *notused){
    static const char *where = "cli-posix";
    int rc = -EINVAL;

    if((cli == NULL) || (user == NULL) || (passwd == NULL)){
        printf("invalid input param\n");
        return rc;
    }

    rc = clis_login(cli, (char *)user, (char*)passwd, (char *)where);
    if(rc != 0){
        printf("login fail %d\n", rc);
    }else{
        printf("login ok\n");
    }
    
    return rc;
}

int adduser_func(client_t *cli, const char *user, const char *passwd, const char *notused){
    int rc;

    if((cli == NULL) || (user == NULL) || (passwd == NULL)){
        printf("invalid input param\n");
        return -1;
    }

    rc = clis_user_add(cli, (char *)user, (char *)passwd, DEFAULT_USER_TYPE);
    if(rc != 0){
        printf("add user fail:%d\n", rc);
    }else{
        printf("add user ok\n");
    }

    return rc;
}

int deluser_func(client_t *cli, const char *user, const char *notused0, const char *notused1){
    int rc;

    if((cli == NULL) || (user == NULL)){
        printf("invalid input param\n");
        return -1;
    }

    rc = clis_user_del(cli, (char *)user);
    if(rc != 0){
        printf("del user fail:%d\n", rc);
    }else{
        printf("del user ok\n");
    }

    return rc;
}

int listuser_func(client_t *cli, const char *notused0, const char *notused1, const char *notused2){
    fixarray_t *aru;
    user_t *usr;
    int len = LIST_USER_MAX;
    int i;
    int rc;

    rc = clis_user_list(cli, len, &aru);
    if(rc != 0){
        printf("list user fail:%d\n", rc);
        return -1;
    }

    for(i = 0; i < fixarray_num(aru); i++){
        fixarray_get(aru, i, (void **)&usr);
        printf("    user: %s type: %d status: %d\n", usr->user, usr->type, usr->status);
    }

    fixarray_destroy(aru, (fixarray_itemfree)user_free);

    return 0;
}

int chpwuser_func(client_t *cli, const char *user, const char *passwd, const char *notused){
    void *cred;
    int cred_len;
    int rc;

    if((cli == NULL) || (user == NULL) || (passwd == NULL)){
        printf("invalid input param\n");
        return -1;
    }

    rc = clis_user_cred((char *)user, (char *)passwd, &cred, &cred_len);
    if(rc != 0){
        printf("create cred for user fail:%d\n", rc);
        return -1;
    }

    rc = clis_user_update(cli, (char *)user, cred_len, cred, DEFAULT_USER_TYPE);
    osapi_free(cred);
    if(rc != 0){
        printf("update user fail:%d\n", rc);
    }else{
        printf("update user ok\n");
    }

    return rc;
}

