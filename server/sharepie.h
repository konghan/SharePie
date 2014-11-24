
#ifndef __SHAREPIE_H__
#define __SHAREPIE_H__

#include "server.h"

#include <unistd.h>

#define SHAREPIE_DEFAULT_PORT       8086

typedef struct _sharepie{
    int status;

    server_t server;
    
    short port;
    char *rdir;

    int sock_daemon;
    int sock_spie;
}sharepie_t;

enum{
    SPIE_START = 0,
    SPIE_STOP,
    SPIE_STATUS,
    SPIE_ADDWORKER,
    SPIE_RMVWORKER,
};

typedef struct _spie_cmdreq{
    unsigned char cmd;
    unsigned char pad;
    unsigned short param;
}__attribute__((packed)) spie_cmdreq_t;

int spie_init(sharepie_t *sp);
int spie_fini(sharepie_t *sp);

int spie_set_param(sharepie_t *sp, const char *rdir, short port);
int spie_start(sharepie_t *sp, int bg);
int spie_stop(sharepie_t *sp);
int spie_restart(sharepie_t *sp, int bg);
int spie_clean(sharepie_t *sp);
int spie_status(sharepie_t *sp);

#endif // __SHAREPIE_H__

