#ifndef __CLI_TRANS_H__
#define __CLI_TRANS_H__

#include "osapi.h"
#include "list.h"
#include "spie.h"
#include "session.h"

#include "spie_mgmt.h"


enum _client_status{
    CLIENT_STATUS_INIT = 0,
    CLIENT_STATUS_AUTH,
    CLIENT_STATUS_STOR,
};

typedef struct _client{
    session_t *ses;
    int status;
}client_t;

int client_init(client_t *cli, session_t *ses);
int client_fini(client_t *cli);

#define client_check(cli_,status_)  \
    if(cli->status < status_){      \
        return -1;                  \
    }

static inline bool fb_reader(struct cmp_ctx_s *ctx, void *data, size_t limit){
    fixbuffer_t *fb = (fixbuffer_t *)ctx->buf;
    size_t copy;

    copy = fixbuffer_copy(fb, data, limit);
    if(copy == limit){
        fixbuffer_drain(fb, copy);
        return true;
    }

    return false;
}

static inline size_t fb_writer(struct cmp_ctx_s *ctx, const void *data, size_t count){
    fixbuffer_t *fb = (fixbuffer_t *)ctx->buf;

    return fixbuffer_write(fb, data, count);
}

#define cli_request(func_name,req_struct,rsp_struct,req_cmd_code)    \
static inline int cli_request_##func_name(session_t *ses, \
        req_struct##_t *req, rsp_struct##_t **rsp){  \
    fixbuffer_t  *fb;       \
    int rc, ecode;          \
    spie_header_t *ptr;    \
                            \
    fb = fixbuffer_new();   \
    if(fb == NULL){         \
        return -1;          \
    }                       \
                            \
    rc = fixbuffer_reserved(fb, sizeof(spie_header_t), (void**)&ptr);  \
    if(rc != 0){                \
        fixbuffer_free(fb);     \
        return -1;              \
    }                           \
    ptr->cmdcode = req_cmd_code;\
                                \
    rc = req_struct##_mashal(fb, fb_reader, fb_writer, req);  \
    if(rc != 0){                \
        fixbuffer_free(fb);     \
        return -1;              \
    }                           \
                                \
    rc = session_send(ses, fb); \
    if(rc == 0){                \
        fixbuffer_free(fb);     \
        return -1;              \
    }                           \
    fixbuffer_free(fb);         \
    fb = NULL;                  \
                                \
    rc = session_recv(ses, &fb, &ecode);                    \
    if(rc != 0){                \
        return -1;              \
    }                           \
                                \
    if(ecode != 0){             \
        fixbuffer_free(fb);     \
        return -ecode;		\
    }                           \
                                \
    ptr = (spie_header_t*)fixbuffer_start(fb);             \
    if(ptr->size == 0){         \
        fixbuffer_free(fb);     \
        return 0;               \
    }                           \
                                \
    fixbuffer_drain(fb, sizeof(spie_header_t));            \
    rc = rsp_struct##_unmash(fb, fb_reader, fb_writer, rsp);  \
    if(rc != 0){                \
        return -1;              \
    }                           \
    fixbuffer_free(fb);         \
                                \
    return ecode;               \
}

#endif

