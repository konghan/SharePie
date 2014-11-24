
#include "svr-trans.h"

#include "spie.h"
#include "server.h"

#include "osapi.h"
#include "logger.h"

#include "fixbuffer.h"
#include "fixarray.h"

#include "spie_mgmt.h"
#include "spie_stor.h"

#include <event2/buffer.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include <assert.h>
#include <errno.h>

static int header_unpack(struct evbuffer *in, spie_header_t **buf){
    spie_header_t   *pro;
    int16_t   nsize = SPIE_HEADER_LEN;
    int       len;
    int evlen;
    ev_ssize_t es;
    
    evlen = evbuffer_get_length(in);

    if(evlen == 0){
        return -SPIE_ECODE_INVAL;
    }
    
    if(evlen < SPIE_HEADER_LEN){
        log_warn("evbuffer - buffer is to small:%d\n", evlen);
        return -SPIE_ECODE_INVAL;
    }

    // get the payload size
    es = evbuffer_copyout(in, &nsize, sizeof(nsize));
    if(es != sizeof(nsize)){
        log_warn("evbuffer read command size fail!\n");
        return -SPIE_ECODE_INVAL;
    }
    len = ntohs(nsize) + SPIE_HEADER_LEN;

    evlen = evbuffer_get_length(in);
    if(evlen < len){
        return -SPIE_ECODE_INVAL;
    }

    pro = osapi_malloc(len);
    if(pro == NULL){
        log_warn("no enough memory for unpack protocols\n");
        return -SPIE_ECODE_NOMEM;
    }
    memset(pro, 0, len);

    es = evbuffer_copyout(in, pro, len);
    if(es != len){
        log_warn("read whole package from buffer fail:%d", es);
        osapi_free(pro);
        return -SPIE_ECODE_INVAL;
    }
    evbuffer_drain(in, len);
  
    pro->size = ntohs(pro->size);

    pro->sesid = ntohl(pro->sesid);
    pro->seqid = ntohl(pro->seqid);
    pro->cred = ntohl(pro->cred);

    *buf = pro;

    return 0;
}

static int header_pack(spie_header_t *in){

    assert(in != NULL);

    in->size  = htons(in->size);

    in->sesid = htonl(in->sesid);
    in->seqid = htonl(in->seqid);   
    in->cred  = htonl(in->cred);

    return 0;
}

static inline int trans_ack_data(struct evbuffer *out, spie_header_t *pro, struct evbuffer *buf){
    assert((out != NULL) && (pro != NULL));

    if(buf != NULL){
        pro->size = evbuffer_get_length(buf);
    }else{
        pro->size = 0;
    }

    log_info("cmd:%d ecode:%d size:%d %s\n", pro->cmdcode, pro->errcode, pro->size,
	    (buf == NULL)? "NULL":"buf");

    header_pack(pro);

    evbuffer_add(out, pro, sizeof(*pro));

    if(buf != NULL)
        evbuffer_add_buffer(out, buf);

    return 0;
}

static int trans_ack_fail(struct evbuffer *out, spie_header_t *pro, uint8_t errcode){
    int rc;

    assert((out != NULL) && (pro != NULL));

    pro->errcode = errcode;
    pro->size = 0;

    log_info("ack ecode - %d\n", pro->errcode);

    rc = trans_ack_data(out, pro, NULL);

    return rc;
}

static bool fb_reader(struct cmp_ctx_s *ctx, void *data, size_t limit){
    fixbuffer_t *fb = (fixbuffer_t *)ctx->buf;
    size_t copy;

    copy = fixbuffer_copy(fb, data, limit);
    if(copy == limit){
        fixbuffer_drain(fb, copy);
        return true;
    }

    return false;
}

static size_t fb_writer(struct cmp_ctx_s *ctx, const void *data, size_t count){
    fixbuffer_t *fb = (fixbuffer_t *)ctx->buf;

    return fixbuffer_write(fb, data, count);
}

static bool evb_reader(struct cmp_ctx_s *ctx, void *data, size_t limit){
    struct evbuffer *evb = (struct evbuffer *)ctx->buf;
    size_t len;

    len = evbuffer_get_length(evb);
    if(limit > len)
        return false;

    evbuffer_remove(evb, data, limit);
    return true;
}

static size_t evb_writer(struct cmp_ctx_s *ctx, const void *data, size_t count){
    struct evbuffer *evb = (struct evbuffer *)ctx->buf;
    
    if(evbuffer_add(evb, data, count) == 0)
        return count;
    else
        return 0;
}

// auth operation
svr_request(templogin, mgmt_templogin_req, mgmt_templogin_rsp)
svr_request(tempadd, mgmt_tempadd_req, mgmt_tempadd_rsp)

svr_request(login, mgmt_login_req, mgmt_login_rsp)

// user operations
svr_request(adduser, mgmt_adduser_req, mgmt_adduser_rsp)
svr_request(deluser, mgmt_deluser_req, mgmt_deluser_rsp)
svr_request(listuser, mgmt_listuser_req, mgmt_listuser_rsp)
svr_request(updateuser, mgmt_updateuser_req, mgmt_updateuser_rsp)

// system management operations
svr_request(sysinfo, mgmt_sysinfo_req, mgmt_sysinfo_rsp)

// file management operations
svr_request(fstat, stor_fstat_req, stor_fstat_rsp)
svr_request(fopen, stor_fopen_req, stor_fopen_rsp)
svr_request(fclose, stor_fclose_req, stor_fclose_rsp)
svr_request(fread, stor_fread_req, stor_fread_rsp)
svr_request(fwrite, stor_fwrite_req, stor_fwrite_rsp)
svr_request(ftruncate, stor_ftruncate_req, stor_ftruncate_rsp)

svr_request(fcreate, stor_fcreate_req, stor_fcreate_rsp)
svr_request(fdelete, stor_fdelete_req, stor_fdelete_rsp)
svr_request(fmoveto, stor_fmoveto_req, stor_fmoveto_rsp)

svr_request(lookup_create, stor_lookup_create_req, stor_lookup_create_rsp)
svr_request(lookup_destroy, stor_lookup_destroy_req, stor_lookup_destroy_rsp)
svr_request(lookup_next, stor_lookup_next_req, stor_lookup_next_rsp)

svr_request(freadsnap, stor_freadsnap_req, stor_freadsnap_rsp)
svr_request(fwritesnap, stor_fwritesnap_req, stor_fwritesnap_rsp)

int request_dispatch(session_t *ses, struct evbuffer *in, struct evbuffer *out){
    spie_header_t *pro;

    assert((ses != NULL) && (in != NULL) && (out != NULL));

    while(header_unpack(in, &pro) == 0){
        switch(pro->cmdcode){
        case SPIE_CMD_TEMPLOGIN:
            svr_request_templogin(ses, in, out, pro);
            break;

        case SPIE_CMD_TEMPADD:
            svr_request_tempadd(ses, in, out, pro);
            break;

        case SPIE_CMD_LOGIN:
            svr_request_login(ses, in, out, pro);
            break;

        case SPIE_CMD_ADDUSER:
            svr_request_adduser(ses, in, out, pro);
            break;

        case SPIE_CMD_DELUSER:
            svr_request_deluser(ses, in, out, pro);
            break;

        case SPIE_CMD_LISTUSER:
            svr_request_listuser(ses, in, out, pro);
            break;

        case SPIE_CMD_UPDATEUSER:
            svr_request_updateuser(ses, in, out, pro);
            break;

        case SPIE_CMD_SYSINFO:
            svr_request_sysinfo(ses, in, out, pro);
            break;

        case SPIE_CMD_FOPEN:
            svr_request_fopen(ses, in, out, pro);
            break;

        case SPIE_CMD_FCLOSE:
            svr_request_fclose(ses, in, out, pro);
            break;

        case SPIE_CMD_FWRITE:
            svr_request_fwrite(ses, in, out, pro);
            break;

        case SPIE_CMD_FREAD:
            svr_request_fread(ses, in, out, pro);
            break;
    
        case SPIE_CMD_FSTAT:
            svr_request_fstat(ses, in, out, pro);
            break;

        case SPIE_CMD_FTRUNCATE:
            svr_request_ftruncate(ses, in, out, pro);
            break;

        case SPIE_CMD_FMOVETO:
            svr_request_fmoveto(ses, in, out, pro);
            break;

         case SPIE_CMD_FCREATE:
            svr_request_fcreate(ses, in, out, pro);
            break;

        case SPIE_CMD_FDELETE:
            svr_request_fdelete(ses, in, out, pro);
            break;

       case SPIE_CMD_FREADSNAP:
            svr_request_freadsnap(ses, in, out, pro);
            break;

        case SPIE_CMD_FWRITESNAP:
            svr_request_fwritesnap(ses, in, out, pro);
            break;

        case SPIE_CMD_LOOKUP_CREATE:
            svr_request_lookup_create(ses, in, out, pro);
            break;

        case SPIE_CMD_LOOKUP_DESTROY:
            svr_request_lookup_destroy(ses, in, out, pro);
            break;

        case SPIE_CMD_LOOKUP_NEXT:
            svr_request_lookup_next(ses, in, out, pro);
            break;

        default:
            log_warn("received command is unkown:%d\n", pro->cmdcode);
            trans_ack_fail(out, pro, SPIE_ECODE_INVAL);
            break;
        }

        pro = NULL;
    }

    return 0;
}
    
