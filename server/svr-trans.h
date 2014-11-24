
#ifndef __SVR_TRANS_H__
#define __SVR_TRANS_H__

#include "spie.h"

#define svr_request(func_name, req_struct, rsp_struct) \
extern int svr_request_##func_name##_routine(session_t *ses, req_struct##_t *req, \
        rsp_struct##_t **rsp); \
int svr_request_##func_name(session_t *ses, struct evbuffer *in,    \
        struct evbuffer *out, spie_header_t *pro){                   \
    fixbuffer_t fb;                 \
    struct evbuffer *evb;           \
    req_struct##_t *req;     \
    rsp_struct##_t *rsp = NULL;     \
    int rc;                         \
                                    \
    fb.buffer = (void *)pro;       \
    fb.len = sizeof(*pro) + pro->size;                                \
    fb.start = sizeof(*pro);       \
    fb.end = fb.len;                \
                                    \
    if(pro->size == 0){            \
        trans_ack_fail(out,pro,SPIE_ECODE_INVAL);              \
        osapi_free(pro);            \
        return -1;                  \
    }                               \
    pro->errcode = 0;		    \
                                    \
    rc = req_struct##_unmash(&fb, fb_reader, fb_writer, &req);   \
    if(rc != 0){                    \
        trans_ack_fail(out,pro,SPIE_ECODE_INVAL);              \
        osapi_free(pro);            \
        return -1;                  \
    }                               \
                                    \
    rc = svr_request_##func_name##_routine(ses,req,&rsp);  \
    if(rc != 0){                    \
        trans_ack_fail(out,pro, (uint8_t)-rc);              \
        osapi_free(pro);           \
        req_struct##_free(req);      \
        return -1;                          \
    }                                       \
    req_struct##_free(req);                 \
                                            \
    if(rsp == NULL){                        \
        trans_ack_fail(out, pro, SPIE_ECODE_SUCCESS);  \
        osapi_free(pro);                    \
        return 0;                           \
    }                                       \
                                            \
    evb = evbuffer_new();                   \
    if(evb == NULL){                        \
        trans_ack_fail(out,pro,SPIE_ECODE_NOMEM);              \
        osapi_free(pro);                    \
        rsp_struct##_free(rsp);      \
        return -1;                          \
    }                                       \
                                            \
    rc = rsp_struct##_mashal(evb, evb_reader, evb_writer, rsp);   \
    if(rc != 0){                            \
        trans_ack_fail(out,pro,SPIE_ECODE_INVAL);    \
        osapi_free(pro);                    \
        rsp_struct##_free(rsp);      \
        return -1;                          \
    }                                       \
                                            \
    trans_ack_data(out, pro, evb);        \
                                            \
    evbuffer_free(evb);                     \
    rsp_struct##_free(rsp);          \
                                            \
    osapi_free(pro);                        \
    return 0;                               \
}

#endif // __SVR_TRANS_H__

