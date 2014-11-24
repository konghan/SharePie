
#ifndef __SERVER_H__
#define __SERVER_H__

#include "list.h"
#include "fixmap.h"
#include "db-repo.h"

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <netinet/in.h>

#include <pthread.h>

#define SERVER_DEFAULT_PORT     8086
#define SERVER_REPO_MAX         16
#define SERVER_WORKER_MAX       16

enum {
    SERVER_WORKER_CMD_ADDSESSION = 0,
    SERVER_WORKER_CMD_RMVSESSION,
    SERVER_WORKER_CMD_LISTSESSION,
    SERVER_WORKER_CMD_TERMINATE,
};

typedef struct _worker_cmd{
    unsigned int cmd;
    union {
        int sock;
        int retcode;
        void *data;
    };
}worker_cmd_t;

struct _worker;
struct _server;

typedef struct _session{
    struct _server *server;
    struct _worker *worker; 

    struct bufferevent *bev; // bufferevent wrapped client sock

    int sesid;      // current session id
    
    int64_t uid;    // current user id
    char *from;     // this session request device
    int priority;   // user priority

    struct list_head snode; // link to server
    struct list_head wnode; // link to worker

    fixmap_t fls;   // opened files
    fixmap_t lus;   // opened lookups
}session_t;

typedef struct _worker{
    int status;     // 0-initialized, 1-running

    // init by caller
    pthread_t wid;      // pthread id
    int sock_server;    // server -> worker
    int sock_worker;    // worker -> server

    int idx;    // index in workers
    
    struct _server *server;

    struct list_head  seses;    // session by this worker
    
    // init by worker routine
    struct event_base *evbase;      // libevent base
    struct event *evserver;         // wrap of sock_server
    struct bufferevent *besock;     // bufferevent wrapped sock_worker
}worker_t;

typedef struct _server{
    struct event_base *evbase;

    int status;     // 0-inited, 1-running
    int sock_spie;  // socket communicate with management

    struct event *clntev;   // socket for recieve client connecting... 
    struct event *mgmtev;   // for management

    pthread_mutex_t lock;

    int worker_num;     // number of worker
    int worker_pos;     // current worker pos for process incoming client
    worker_t workers[SERVER_WORKER_MAX];

    int sesid;  // session id for new incomming
    struct list_head seses; // all session

    dbrepo_t repo;
}server_t;

int server_init(server_t *svr, int sock, int workers);
int server_fini(server_t *svr);

int server_start(server_t *svr, short port);
int server_stop(server_t *svr);

int session_rfile_insert(session_t *ses, int64_t fid, rfile_t *file);
int session_rfile_remove(session_t *ses, int64_t fid, rfile_t **file);
int session_rfile_find(session_t *ses, int64_t fid, rfile_t **file);

int session_lookup_insert(session_t *ses, int64_t luid, dbstor_lookup_t *lu);
int session_lookup_remove(session_t *ses, int64_t luid, dbstor_lookup_t **lu);
int session_lookup_find(session_t *ses, int64_t luid, dbstor_lookup_t **lu);

static inline int64_t session_uid(session_t *ses){
    return ses->uid;
}

static inline char *session_where(session_t *ses){
   return ses->from;
}

#define CHECK_PRIORITY(ses_,op_)    \
    if(ses_->priority < op_){     \
        return -SPIE_ECODE_PRIORITY;                  \
    }

// implement in protocol.c
int request_dispatch(session_t *ses, struct evbuffer *in, struct evbuffer *out);

#endif // __SERVER_H__

