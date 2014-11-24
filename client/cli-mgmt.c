#include "cli-mgmt.h"

#include "spie_mgmt.h"
#include "fixarray.h"
#include "session.h"
#include "md5.h"

#include <unistd.h>


cli_request(templogin, mgmt_templogin_req, mgmt_templogin_rsp, SPIE_CMD_TEMPLOGIN)
cli_request(tempadd, mgmt_tempadd_req, mgmt_tempadd_rsp, SPIE_CMD_TEMPADD)

cli_request(user_add, mgmt_adduser_req, mgmt_adduser_rsp,SPIE_CMD_ADDUSER)
cli_request(user_del, mgmt_deluser_req, mgmt_deluser_rsp, SPIE_CMD_DELUSER)
cli_request(user_list, mgmt_listuser_req, mgmt_listuser_rsp, SPIE_CMD_LISTUSER)
cli_request(user_update, mgmt_updateuser_req, mgmt_updateuser_rsp, SPIE_CMD_UPDATEUSER)

cli_request(login, mgmt_login_req, mgmt_login_rsp, SPIE_CMD_LOGIN);

cli_request(sysinfo, mgmt_sysinfo_req, mgmt_sysinfo_rsp, SPIE_CMD_SYSINFO)



int clis_templogin(client_t *cli, char *user, int *type){
    mgmt_templogin_req_t  req;
    mgmt_templogin_rsp_t  *rsp = NULL;
    int rc = 0;
    
    if(cli->status < CLIENT_STATUS_INIT){
        return -EINVAL;
    }

    if(user == NULL)
        return -EINVAL;

    req.user = user;

    rc = cli_request_templogin(cli->ses, &req, &rsp);
    if(rc != 0){
        return rc;
    }

    cli->status = CLIENT_STATUS_AUTH;

    if(rsp != NULL){
        *type = rsp->type;
        mgmt_templogin_rsp_free(rsp);
    }

    return rc;
}

int clis_tempadd(client_t *cli, int type, int *code){
    mgmt_tempadd_req_t  req;
    mgmt_tempadd_rsp_t  *rsp;
    int rc = 0;

    if(cli->status < CLIENT_STATUS_INIT){
        return -EINVAL;
    }
    req.type = type;

    rc = cli_request_tempadd(cli->ses, &req, &rsp);
    if(rc != 0){
        return rc;
    }

    if(code != NULL)
        *code = rsp->code;

    mgmt_tempadd_rsp_free(rsp);

    return rc;
}

int clis_login(client_t *cli, char *user, char *passwd, char *where){
    md5_state_t md;
    md5_byte_t digest[16];

    if(cli->status < CLIENT_STATUS_INIT){
        return -EINVAL;
    }

    if((user == NULL) || (passwd == NULL) || (where == NULL))
        return -EINVAL;

    // md5
    md5_init(&md);
    md5_append(&md, (md5_byte_t*)passwd, strlen(passwd));
    md5_finish(&md, digest);

    return clis_login_bycred(cli, user, 16, digest, where);
}

int clis_user_cred(char *user, char *passwd, void **cred, int *len){
    md5_state_t md;
    md5_byte_t *digest;

    if((user == NULL) || (passwd == NULL) || (cred == NULL))
        return -EINVAL;

    digest = osapi_malloc(sizeof(md5_byte_t)*16);
    if(digest == NULL)
        return -1;

    // md5
    md5_init(&md);
    md5_append(&md, (md5_byte_t*)passwd, strlen(passwd));
    md5_finish(&md, digest);

    *cred = digest;
    if(len != NULL)
        *len = 16;

    return 0;
}


int clis_login_bycred(client_t *cli, char *user, int cred_len, void *cred, char *where){
    mgmt_login_req_t  req;
    mgmt_login_rsp_t  *rsp;
    int rc = 0;
    md5_state_t md;
    md5_byte_t digest[16];
    md5_byte_t buffer[16];

    if(cli->status < CLIENT_STATUS_INIT){
        return -EINVAL;
    }

    snprintf((char*)buffer, 16, "rand:%d\n", (int)random());
    
    // md5
    md5_init(&md);
    md5_append(&md, (md5_byte_t*)cred, cred_len);
    md5_append(&md, (md5_byte_t*)buffer, 16);
    md5_finish(&md, digest);

    // authen user
    req.user = user;
    req.where = where;
    req.cred_len = 16;
    req.cred = digest;
    req.seed_len = 16;
    req.seed = buffer;

    rc = cli_request_login(cli->ses, &req, &rsp);
    if(rc != 0){
        return rc;
    }

    cli->status = CLIENT_STATUS_AUTH;
    mgmt_login_rsp_free(rsp);

    return rc;
}

int clis_user_add(client_t *cli, char *user, char *passwd, int type){
    mgmt_adduser_req_t req;
    mgmt_adduser_rsp_t *rsp = NULL;
    int rc;
    md5_state_t md;
    md5_byte_t digest[16];

    if(cli->status < CLIENT_STATUS_INIT){
        return -EINVAL;
    }

    if((user == NULL) || (passwd == NULL))
        return -EINVAL;

    // md5
    md5_init(&md);
    md5_append(&md, (md5_byte_t*)passwd, strlen(passwd));
    md5_finish(&md, digest);

    req.user = user;
    req.cred_len = 16;
    req.cred = digest;
    req.type = type;

    rc = cli_request_user_add(cli->ses, &req, &rsp);
    if(rc != 0){
        return rc;
    }

    if(rsp != NULL)
        mgmt_adduser_rsp_free(rsp);

    return rc;
}

int clis_user_del(client_t *cli, char *user){
    mgmt_deluser_req_t req;
    mgmt_deluser_rsp_t *rsp = NULL;
    int rc;

    req.user = user;

    rc = cli_request_user_del(cli->ses, &req, &rsp);
    if(rc != 0){
        return rc;
    }

    if(rsp != NULL)
        mgmt_deluser_rsp_free(rsp);

    return rc;
}

int clis_user_update(client_t *cli, char *user, int cred_len, void *cred, int type){
    mgmt_updateuser_req_t req;
    mgmt_updateuser_rsp_t *rsp = NULL;
    int rc;

    req.user = user; 
    req.cred_len = cred_len;
    req.cred = cred;
    req.type = type;

    rc = cli_request_user_update(cli->ses, &req, &rsp);
    if(rc != 0){
        return rc;
    }

    if(rsp != NULL)
        mgmt_updateuser_rsp_free(rsp);

    return rc;
}

int clis_user_list(client_t *cli, int len, fixarray_t **users){
    mgmt_listuser_req_t req;
    mgmt_listuser_rsp_t *rsp;
    int rc;

    req.len = len;

    rc = cli_request_user_list(cli->ses, &req, &rsp);
    if(rc != 0){
        return rc;
    }

    *users = rsp->users;
    rsp->users = NULL;

    mgmt_listuser_rsp_free(rsp);

    return rc;
}

int clis_sysinfo(client_t *cli, cli_sysinfo_t **sta){
    mgmt_sysinfo_req_t req;
    mgmt_sysinfo_rsp_t *rsp = NULL;
    int rc;

    req.type =  0;

    rc = cli_request_sysinfo(cli->ses, &req, &rsp);
    if(rc != 0){
        return rc;
    }

    *sta = rsp;

    return rc;
}

