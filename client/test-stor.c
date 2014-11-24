#include "main.h"

#include "osapi.h"
#include "list.h"

#include "fixbuffer.h"
#include "fixarray.h"

#include "spie_stor.h"
#include "spie_mgmt.h"

#include "cli-mgmt.h"
#include "cli-stor.h"

#include <assert.h>

/*
** comman connect
*/

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

static int cli_setup (){
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

    return 0;
}

static int cli_cleanup(){
    return cli_destroy(&cli);
}

int main(int argc, char *argv[]){
    int rc;

    printf("Test...\n");

    printf("test login...");
    if(cli_setup() != 0){
        printf("setup clean fail\n");
        return -1;
    }

    rc = login_func(&cli, "Administrator", "Administrator", NULL);
    if(rc != 0){
        printf("Fail\n");
        return -1;
    }else{
        printf("Ok\n");
    }

    cli_cleanup();

    return 0;
}


