
#include "svr-mgmt.h"
#include "db-user.h"
#include "osapi.h"
#include "logger.h"
#include "spie_mgmt.h"
#include "spie.h"

#include "server.h"
#include "md5.h"

int svr_request_templogin_routine(session_t *ses, mgmt_templogin_req_t *req,
        mgmt_templogin_rsp_t **rsp){
    dbrepo_t *repo = &ses->server->repo;
    mgmt_templogin_rsp_t *r;
    int user;
    int rc;

    if(req->user == NULL)
        return -1;

    user = atoi(req->user);

    // rc -1, no; 0, temp; 1 longlive.
    rc = dbuser_tempfind(&repo->dbuser, user);
    if(rc < 0){
        return -SPIE_ECODE_NOENT;
    }

    r = osapi_malloc(sizeof(*r));
    if(r == NULL){
        return -SPIE_ECODE_NOMEM;
    }
    r->type = rc;

    *rsp = r;

    dbuser_tempdel(&repo->dbuser, user);

    ses->from = osapi_strdup("temp", 0);
    ses->priority = rc;
    ses->uid = -1; // not used

    return 0;
}

int svr_request_tempadd_routine(session_t *ses, mgmt_tempadd_req_t *req,
        mgmt_tempadd_rsp_t **rsp){
    dbrepo_t *repo = &ses->server->repo;
    mgmt_tempadd_rsp_t *r;
    int rc;

    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);

    rc = dbuser_tempadd(&repo->dbuser, req->type);
    if(rc < 0){
        log_warn("add temp user fail\n");
        return -1;
    }

    r = osapi_malloc(sizeof(*r));
    if(r == NULL){
        log_warn("no memory for tempadd rsp\n");
        return -1;
    }

    r->code = rc;

    *rsp = r;
    return 0;
}

int svr_request_adduser_routine(session_t *ses, mgmt_adduser_req_t *req,
        mgmt_adduser_rsp_t **rsp){
    dbrepo_t *repo = &ses->server->repo;
    int64_t uid;
    
    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);

    if((req->user == NULL) || (req->cred_len == 0)){
        log_warn("adduse request param invalid\n");
        return -1;
    }

    *rsp = NULL;

    if(req->type > ses->priority)
        req->type = ses->priority;

    uid = dbuser_add(&repo->dbuser, req->user, req->cred_len, req->cred, req->type);
    if(uid < 0){
        log_warn("adduser in database fail\n");
        return -SPIE_ECODE_DBERR;
    }

    return 0;
}

int svr_request_deluser_routine(session_t *ses, mgmt_deluser_req_t *req,
        mgmt_deluser_rsp_t **rsp){
    dbrepo_t *repo = &ses->server->repo;
    int64_t uid;

    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);

    if(req->user == NULL){
        log_warn("deluser invalid param\n");
        return -1;
    }

    *rsp = NULL;

    uid = dbuser_del(&repo->dbuser, req->user);
    if(uid < 0){
        return (int)uid;
    }else{
        return 0;
    }
}

int svr_request_listuser_routine(session_t *ses, mgmt_listuser_req_t *req,
        mgmt_listuser_rsp_t **rsp){
    mgmt_listuser_rsp_t *r;
    fixarray_t *rus, *usrs;
    dbrepo_t *repo = &ses->server->repo;
    user_t  *rspuser;
    ruser_t *ru;
    int i;
    int rc;

    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);

    if(req->len <= 0)
        return -1;

    r = osapi_malloc(sizeof(*r));
    if(r == NULL){
        log_warn("no memory for list rsp\n");
        return -SPIE_ECODE_NOMEM;
    }
    memset(r, 0, sizeof(*r));

    rc = dbuser_list(&repo->dbuser, req->len, &rus);
    if(rc != 0){
        osapi_free(r);
        return rc;
    }

    if(fixarray_num(rus) > 0){
        rc = fixarray_create(fixarray_length(rus), &usrs);
        if(rc != 0){
            log_warn("no memory for list rsp\n");
            fixarray_destroy(rus, (fixarray_itemfree)dbuser_ruser_free);
            return -SPIE_ECODE_NOMEM;
        }

        for(i = 0; i < fixarray_num(rus); i++){
            rspuser = osapi_malloc(sizeof(*rspuser));
            if(rspuser == NULL){
                log_warn("no memory for list\n");
                fixarray_destroy(rus, (fixarray_itemfree)dbuser_ruser_free);
                fixarray_destroy(usrs, (fixarray_itemfree)user_free);
                return -SPIE_ECODE_NOMEM;
            }
        
            fixarray_get(rus, i, (void **)&ru);
            rspuser->user = ru->user;
            ru->user = NULL;
            rspuser->type = ru->type;
            rspuser->status = 0;
            fixarray_set(usrs, i, rspuser);
        }

        r->users = usrs;

        fixarray_destroy(rus, (fixarray_itemfree)dbuser_ruser_free);
    }

    *rsp = r;

    return 0;
}

int svr_request_login_routine(session_t *ses, mgmt_login_req_t *req,
        mgmt_login_rsp_t **rsp){
    mgmt_login_rsp_t *r;
    dbrepo_t *repo = &ses->server->repo;
    ruser_t *ru;
    int rc;
    md5_state_t mds;
    md5_byte_t buffer[16];

    if((req->user == NULL) || (req->cred == NULL) 
            || (req->where == NULL) || (req->seed == NULL))
        return -1;

    if(req->cred_len < 16)
        return -1;

    r = osapi_malloc(sizeof(*r));
    if(r == NULL){
        log_warn("no memory for lookup rsp\n");
        return -SPIE_ECODE_NOMEM;
    }
    memset(r, 0, sizeof(*r));

    rc = dbuser_lookup(&repo->dbuser, req->user, &ru);
    if(rc != 0){
        log_warn("user %s not exist\n", req->user);
        osapi_free(r);
        return rc;
    }
    
    md5_init(&mds);
    md5_append(&mds, ru->cred, ru->cred_len);
    md5_append(&mds, req->seed, req->seed_len);
    md5_finish(&mds, buffer);

    rc = memcmp(buffer, req->cred, 16);

    if(rc != 0){
        *rsp = NULL;
        osapi_free(r);
        rc = -1;
    }else{
        ses->from = req->where;
	req->where = NULL;

        ses->uid = ru->uid;
        ses->priority = ru->type;
        *rsp = r;
    }
    r->ver = 1;

    dbuser_ruser_free(ru);
   
    return rc;
}

int svr_request_updateuser_routine(session_t *ses, mgmt_updateuser_req_t *req,
        mgmt_updateuser_rsp_t **rsp){
    dbrepo_t *repo = &ses->server->repo;
    int64_t uid;
    
    CHECK_PRIORITY(ses, SPIE_PRIORITY_NORMAL);

    if((req->user == NULL) || (req->cred == NULL))
        return -1;

    *rsp = NULL;

    uid = dbuser_update(&repo->dbuser, req->user, req->cred_len, req->cred, req->type);
    if(uid < 0){
        return (int)uid;
    }else{
        return 0;
    }
}

int svr_request_sysinfo_routine(session_t *ses, mgmt_sysinfo_req_t *req,
        mgmt_sysinfo_rsp_t **rsp){
    // not 
    return -1;
}

