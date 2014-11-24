
#ifndef __SESSION_H__
#define __SESSION_H__

#include "fixbuffer.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>

typedef struct _session{
    int status; // status of this session:-1 error, 0 inited, 1 running

    int sock;

    uint32_t seqid_recv;

    uint32_t sesid; // session id
    uint32_t seqid; // request seqence id
    uint32_t cred;  // credential for security

}session_t;

int session_init(session_t *ses);
int session_fini(session_t *ses);

int session_connect(session_t *ses, struct sockaddr *addr, int len);
int session_close(session_t *ses);

int session_setup(session_t *ses, const char *user, const char *cred);
int session_cleanup(session_t *ses);

int session_send(session_t *ses, fixbuffer_t *fb);
int session_recv(session_t *ses, fixbuffer_t **fbout, int *ecode);

#endif

