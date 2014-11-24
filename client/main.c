#include "main.h"

#include "osapi.h"
#include "list.h"

#include "fixbuffer.h"
#include "fixarray.h"

#include "spie_stor.h"
#include "spie_mgmt.h"

#include "cli-mgmt.h"
#include "cli-stor.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

static client_t cli;
static session_t ses;

static int cli_create_remote(struct sockaddr *addr, int len){
    int rc;

    rc = session_init(&ses);
    if(rc != 0){
        return rc;
    }
    
    rc = session_connect(&ses, addr, len);
    if(rc != 0){
        return rc;
    }

    client_init(&cli, &ses);

    return 0;
}

static int cli_destroy(client_t *cli){
    assert(cli != NULL);

    session_cleanup(cli->ses);
    session_fini(cli->ses);

    client_fini(cli);

    return 0;
}


static int setup_cli(){
    struct sockaddr_in  sin;
    int rc;
    
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(CLI_ADDR);
    sin.sin_port = htons(CLI_PORT);

    rc = cli_create_remote((struct sockaddr*)&sin, sizeof(sin));
    if(rc != 0){
        printf("init remote cli fail:%d\n", rc);
        return -1;
    }

    rc = login_func(&cli, "Administrator", "Administrator", NULL);
    if(rc != 0){
        printf("default user Administrator login fail\n");
        return -1;
    }

    return 0;
}

void usage(){
    printf("usage:  program command param ...\n");
    printf("command:\n");
    printf("    list {video music photo docs} [folder-id]\n");
    printf("    push local-filename {video music photo docs} [remote-filename]\n");
    printf("    pull file-id local-filename\n");
    printf("    move file-id folder-id [new-name]\n");
    printf("    add  local-filename\n");
    printf("    rmv   file-id\n");
    printf("    adduser username passwd\n");
    printf("    deluser username\n");
    printf("    chpwuser username passwd\n");
    printf("    listuser \n");
    printf("    login username passwd\n");
}

struct _cmd  _cmds[] = {
    {"list", list_func},
    {"push", push_func},
    {"pull", pull_func},
    {"move", move_func},
    {"add",  add_func},
    {"rmv",  rmv_func},
    {"adduser", adduser_func},
    {"deluser", deluser_func},
    {"listuser", listuser_func},
    {"chpwuser", chpwuser_func},
    {"login", login_func}
};

int main(int argc, char *argv[]){
    char *cmd;
    char *p0=NULL, *p1=NULL, *p2=NULL;
    int  i;
    struct _cmd *c;

    if(argc < 2){
        usage();
        return -1;
    }

    cmd = argv[1];
    switch(argc){
        case 4: 
            p2 = argv[4];

        case 3:
            p1 = argv[3];

        case 2:
            p0 = argv[2];
            break;

        default :
            usage();
            return -1;
    }

    if(setup_cli() != 0){
        return -1;
    }

    printf("setup client ok\n");

    for(i = 0; i < sizeof(_cmds)/sizeof(struct _cmd); i++){
        c = &(_cmds[i]);
        if(strcmp(c->cmd, cmd) == 0){
            return c->func(&cli, p0, p1, p2);
        }
    }

    usage();
    return -1;
}


