#include "server.h"
#include "osapi.h"
#include "logger.h"

#include "sharepie.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define WORKER_DEFAULT_HSET_LEN         4

static int server_add_worker(server_t *server, int workers);
static int server_rmv_worker(server_t *server, int workers);

int session_rfile_insert(session_t *ses, int64_t fid, rfile_t *file){
    return fixmap_insert(&ses->fls, fid, &file->node);
}

int session_rfile_remove(session_t *ses, int64_t fid, rfile_t **file){
    fixmap_node_t *n = NULL;
    int rc;

    rc = fixmap_remove(&ses->fls, fid, &n);
    if((n != NULL) && (file != NULL))
        *file = container_of(n, rfile_t, node);

    return rc;
}

int session_rfile_find(session_t *ses, int64_t fid, rfile_t **file){
    fixmap_node_t *n = NULL;
    int rc;

    rc = fixmap_get(&ses->fls, fid, &n);
    if(n != NULL)
        *file = container_of(n, rfile_t, node);

    return rc;
}

int session_lookup_insert(session_t *ses, int64_t luid, dbstor_lookup_t *lu){
    return fixmap_insert(&ses->lus, luid, &lu->node);
}

int session_lookup_remove(session_t *ses, int64_t luid, dbstor_lookup_t **lu){
    fixmap_node_t *node = NULL;
    int rc;

    rc = fixmap_remove(&ses->lus, luid, &node);
    if((node != NULL) && (lu != NULL))
        *lu = container_of(node, dbstor_lookup_t, node);

    return rc;
}

int session_lookup_find(session_t *ses, int64_t luid, dbstor_lookup_t **lu){
    fixmap_node_t *node = NULL;
    int rc;

    rc = fixmap_get(&ses->lus, luid, &node);
    if((node != NULL) && (lu != NULL))
        *lu = container_of(node, dbstor_lookup_t, node);

    return rc;
}

static void session_readcb(struct bufferevent *bev, void *ctx){
    session_t   *ses = ctx;
    struct evbuffer *in, *out;

    assert((bev != NULL) && (ctx != NULL));

    in = bufferevent_get_input(bev);
    out = bufferevent_get_output(bev);

    request_dispatch(ses, in, out);
}

static void session_writecb(struct bufferevent *bev, void *ctx){
}

static void session_destroy(session_t *ses){
    fixmap_node_t *fmn;
    struct rb_node *node;
    rfile_t *file;
    dbstor_lookup_t *lu;

    bufferevent_disable(ses->bev, EV_READ|EV_WRITE);
    bufferevent_free(ses->bev);

    pthread_mutex_lock(&ses->server->lock);
    list_del(&ses->snode);
    pthread_mutex_unlock(&ses->server->lock);

    list_del(&ses->wnode);

    pthread_mutex_lock(&ses->server->repo.mlock);
    fixmap_for_each_remove((&ses->fls.root), node, fmn){
        file = container_of(fmn, rfile_t, node);
        fclose(file->handle);
        dbstor_file_free(file->file);
    }
    pthread_mutex_unlock(&ses->server->repo.mlock);
    
    fixmap_fini(&ses->fls);

    fixmap_for_each_remove((&ses->lus.root), node, fmn){
        lu = container_of(fmn, dbstor_lookup_t, node);
        dbstor_ludestroy(&ses->server->repo.dbstor, lu);
    }
    fixmap_fini(&ses->lus);

    osapi_free(ses);
}

static void session_error(struct bufferevent *bev, short error, void *ctx){
    session_t *ses = (session_t *)ctx;
    evutil_socket_t sock = bufferevent_getfd(bev);

    assert((bev != NULL) && (ctx != NULL));

    if (error & BEV_EVENT_EOF){
        log_warn("connection closed\n");
    }else if (error & BEV_EVENT_ERROR){
        log_warn("some other error\n");
    }else if (error & BEV_EVENT_TIMEOUT){
        log_warn("Timed out\n");
    }

    log_info("close session : %d\n", sock);
    session_destroy(ses);
}

static session_t *session_create(worker_t *worker, int sock){
    session_t   *ses;

    assert(worker != NULL);

    ses = osapi_malloc(sizeof(*ses));
    if(ses == NULL){
        log_warn("session create without memory!\n");
        return NULL;
    }
    memset(ses, 0, sizeof(*ses));

    ses->worker = worker;
    ses->server = worker->server;

    INIT_LIST_HEAD(&ses->snode);
    INIT_LIST_HEAD(&ses->wnode);

    fixmap_init(&ses->fls);
    fixmap_init(&ses->lus);

    evutil_make_socket_nonblocking(sock);
    ses->bev = bufferevent_socket_new(worker->evbase, sock, BEV_OPT_CLOSE_ON_FREE);
    if(ses->bev == NULL){
        log_warn("create buffer event for session fail!\n");
        osapi_free(ses);
        return NULL;
    }

    bufferevent_setcb(ses->bev, session_readcb, session_writecb,
                      session_error, (void*)ses);
    bufferevent_enable(ses->bev, EV_READ|EV_WRITE);

    return ses;
}

static void worker_readcmd(struct bufferevent *bev, void *ctx){
    worker_t   *worker = ctx;
    worker_cmd_t     cmd;
    session_t       *ses;
    session_t       *temp;

    assert((bev != NULL) && (ctx != NULL));

    bufferevent_read(bev, &cmd, sizeof(cmd));

    switch(cmd.cmd){
    case SERVER_WORKER_CMD_ADDSESSION:
        ses = session_create(worker, cmd.sock);

        pthread_mutex_lock(&worker->server->lock);
        ses->sesid = worker->server->sesid++;
        list_add_tail(&ses->snode, &worker->server->seses);
        pthread_mutex_unlock(&worker->server->lock);

        list_add_tail(&ses->wnode, &worker->seses);
        break;

    case SERVER_WORKER_CMD_LISTSESSION:
        break;

    case SERVER_WORKER_CMD_RMVSESSION:
        break;

    case SERVER_WORKER_CMD_TERMINATE:
        event_base_loopexit(worker->evbase, NULL);
        list_for_each_entry_safe(ses, temp, &worker->seses, wnode){
            session_destroy(ses);
        }
        break;

    default:
        break;
    }
}

static void worker_writecmd(struct bufferevent *bev, void *ctx){
}

static void worker_errorcb(struct bufferevent *bev, short error, void *ctx){
    worker_t   *worker = ctx;
    session_t       *ses;
    session_t       *temp;

    if (error & BEV_EVENT_EOF){
        log_warn("connection closed\n");
    }else if (error & BEV_EVENT_ERROR){
        log_warn("some other error\n");
    }else if (error & BEV_EVENT_TIMEOUT){
       log_warn("Timed out\n");
    }

    event_base_loopexit(worker->evbase, NULL);
    list_for_each_entry_safe(ses, temp, &worker->seses, wnode){
        session_destroy(ses);
    }
}

static void *worker_routine(void *data){
    worker_t  *worker = (worker_t *)data;

    log_info("worker is initializing...\n");

    // new event base of this worker
    worker->evbase = event_base_new();
    if(worker->evbase == NULL){
        log_warn("create event base in worker %d fail\n", worker->idx);
        return NULL;
    }

    // listen on command from server
    evutil_make_socket_nonblocking(worker->sock_worker);
    worker->besock = bufferevent_socket_new(worker->evbase, worker->sock_worker,
                                            BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(worker->besock, worker_readcmd, worker_writecmd,
                      worker_errorcb, (void*)worker);
    bufferevent_enable(worker->besock, EV_READ|EV_WRITE);

    log_info("worker %d is running...\n", worker->idx);
    
    worker->status = 1;

    INIT_LIST_HEAD(&worker->seses);
    
    // dispatch event
    event_base_dispatch(worker->evbase);

    log_info("worker %d is stoping...\n", worker->idx);

    // exit worker thread
    worker->status = 0;
 
    // release resource
    bufferevent_free(worker->besock);
    event_free(worker->evserver);
    event_base_free(worker->evbase);

    return NULL;
}

static void worker_cbs(evutil_socket_t worker, short event, void *arg){
}

static void server_accept(evutil_socket_t listener, short event, void *arg){
    struct sockaddr_storage ss;
    socklen_t    slen = sizeof(ss);
    server_t     *server = (server_t *)arg;
    worker_t     *worker;
    int          fd;
    worker_cmd_t cmd;

    fd = accept(listener, (struct sockaddr*)&ss, &slen);
    if (fd < 0){
        log_warn("accept socket fail!\n");
        //FIXME: close server
    }else if (fd > FD_SETSIZE){
        close(fd);
    }else{
        cmd.cmd = SERVER_WORKER_CMD_ADDSESSION;
        cmd.sock = fd;

        if(server->worker_num == 0){
            log_warn("server without wokers!\n");
            close(fd);
        }

        server->worker_pos++;
        if(server->worker_pos >= server->worker_num)
            server->worker_pos = 0;

        worker = &(server->workers[server->worker_pos]);
        send(worker->sock_server, &cmd, sizeof(cmd), 0);

        log_info("new session is connected!\n");
    }
}

static int server_listen_client(server_t *server, short port){
    struct sockaddr_in  addr;
    int sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        log_warn("create listenning sock fail!\n");
        return -1;
    }
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    addr.sin_port = htons(port);

    if(bind(sock, (struct sockaddr*)&addr, sizeof(addr)) != 0){
        log_warn("bind listenning sock fail!\n");
        return -1;
    }

    evutil_make_socket_nonblocking(sock);
    evutil_make_listen_socket_reuseable(sock);

    if (listen(sock, 4)<0){
        log_warn("server listen on %d fail!\n", sock);
        return -EINVAL;
    }

    server->clntev = event_new(server->evbase, sock, EV_READ|EV_PERSIST,
        server_accept, (void*)server);

    if(server->clntev == NULL){
        return -EINVAL;
    }
    
    event_add(server->clntev, NULL);

    return 0;
}

static void mgmt_read(evutil_socket_t listener, short event, void *arg){
    server_t     *server = (server_t *)arg;
    spie_cmdreq_t req;
    int rc;

    rc = recv(listener, &req, sizeof(req), 0);
    if(rc != sizeof(req))
        return ;

    switch(req.cmd){
    case SPIE_START:
        log_warn("command SPIE_START should not send to me\n");
        break;

    case SPIE_STOP:
        log_info("recive SPIE_STOP command request\n");
        server_stop(server);
        break;

    default:
        log_warn("unkown command %d\n", req.cmd);
        break;
    }

    // loop back message
    send(listener, &req, sizeof(req), 0);
}

static int server_listen_mgmt(server_t *server){

    evutil_make_socket_nonblocking(server->sock_spie);
    evutil_make_listen_socket_reuseable(server->sock_spie);

    server->mgmtev = event_new(server->evbase, server->sock_spie,
            EV_READ|EV_PERSIST, mgmt_read, (void*)server);

    if(server->mgmtev == NULL){
        return -EINVAL;
    }
    
    event_add(server->mgmtev, NULL);

    return 0;
}

int server_start(server_t *server, short port){
    int rc;

    log_info("server starting ...\n");

    rc = server_add_worker(server, server->worker_num);
    if(rc != 0){
        return rc;
    }

    rc = server_listen_client(server, port);
    if(rc != 0){
        return rc;
    }

    rc = server_listen_mgmt(server);
    if(rc != 0){
        return rc;
    }

    return event_base_dispatch(server->evbase);
}

int server_stop(server_t *svr){

    log_info("server stop, loopexit\n");

    event_base_loopexit(svr->evbase, NULL);

    server_rmv_worker(svr, svr->worker_num);
    
    return 0;
}

static int server_add_worker(server_t *server, int workers){
    worker_t *worker;
    int socks[2];
    int rc;
    int i;

    for(i = 0; i < workers; i++){
        worker = &(server->workers[i]);

        rc = evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, socks);
        if(rc != 0){
            log_warn("server init : socketpair fail:%d-%d\n", rc, errno);
            break;
        }
        worker->sock_server = socks[0];
        worker->sock_worker = socks[1];
        
        worker->idx = i;
        worker->server = server;

        if(pthread_create(&worker->wid, NULL, worker_routine, worker) != 0){
            log_warn("create thread %d fail\n", i);
            close(socks[0]);
            close(socks[1]);
            break;
        }

        evutil_make_socket_nonblocking(worker->sock_server);
        worker->evserver = event_new(server->evbase, worker->sock_server,
                               EV_READ|EV_PERSIST, worker_cbs, (void*)server);

        event_add(worker->evserver, NULL);
    }

    return 0;
}

static int server_rmv_worker(server_t *server, int workers){
    worker_t *worker;
    worker_cmd_t cmd;
    int start;
    int i;

    start = server->worker_num - workers;

    if(start < 0)
        start = 0;

    for(i = start; i < workers; i++){
        worker = &(server->workers[i]);
        if(worker->status > 0){
            cmd.cmd = SERVER_WORKER_CMD_TERMINATE;
            cmd.sock = 0;
            send(worker->sock_server, &cmd, sizeof(cmd), 0);
            event_del(worker->evserver);
            close(worker->sock_server);
        }else{
            break;
        }
    }

    return 0;
}

int server_init(server_t *server, int sock, int workers){
    assert(server != NULL);

    log_info("server_init ...\n");

    memset(server, 0, sizeof(*server));

    server->evbase = event_base_new();
    if(server->evbase == NULL){
        log_warn("server init new event base instance fail\n");
        osapi_free(server->workers);
        return -EINVAL;
    }

    INIT_LIST_HEAD(&server->seses);
    pthread_mutex_init(&server->lock, NULL);

    server->worker_num = workers;
    server->worker_pos = 0;
    server->sock_spie = sock;

    return 0;
}

int server_fini(server_t *server){

    sleep(1);
    
    log_info("server_fini...\n");

    pthread_mutex_destroy(&server->lock);
    
    server->worker_num = 0;

    return 0;
}

