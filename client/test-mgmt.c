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
    int id;

    printf("Test...\n");

    // test login default user

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

    // add user

    printf("test add user...");
    if(cli_setup() != 0){
        printf("setup clean fail\n");
        return -1;
    }

    rc = login_func(&cli, "Administrator", "Administrator", NULL);
    if(rc != 0){
        printf("Fail\n");
        return -1;
    }

    rc = adduser_func(&cli, "sharepie", "sharepie", NULL);
    if(rc != 0){
        printf("Fail\n");
        return -1;
    }else{
        printf("add,");
    }

    cli_cleanup();

    if(cli_setup() != 0){
        printf("setup clean fail\n");
        return -1;
    }

    rc = login_func(&cli, "sharepie", "sharepie", NULL);
    if(rc != 0){
        printf("-Fail\n");
        return -1;
    }else{
        printf("Ok\n");
    }

    cli_cleanup();

    // delete user

    printf("test delete user...");
    if(cli_setup() != 0){
        printf("setup clean fail\n");
        return -1;
    }

    rc = login_func(&cli, "sharepie", "sharepie", NULL);
    if(rc != 0){
        printf("login Fail\n");
        return -1;
    }

    rc = deluser_func(&cli, "sharepie", NULL, NULL);
    if(rc != 0){
        printf("del Fail\n");
        return -1;
    }

    cli_cleanup();

    if(cli_setup() != 0){
        printf("setup clean fail\n");
        return -1;
    }

    rc = login_func(&cli, "sharepie", "sharepie", NULL);
    if(rc != 0){
        printf("OK\n");
    }

    cli_cleanup();

    // update user
    
    printf("test update user...");
    if(cli_setup() != 0){
        printf("setup clean fail\n");
        return -1;
    }

    rc = login_func(&cli, "Administrator", "Administrator", NULL);
    if(rc != 0){
        printf("Fail\n");
        return -1;
    }

    rc = adduser_func(&cli, "sharepie", "sharepie", NULL);
    if(rc != 0){
        printf("Fail\n");
        return -1;
    }

    rc = chpwuser_func(&cli, "sharepie", "piebox", NULL);
    if(rc != 0){
        printf("Fail\n");
        return -1;
    }

    cli_cleanup();

    if(cli_setup() != 0){
        printf("setup clean fail\n");
        return -1;
    }

    rc = login_func(&cli, "sharepie", "piebox", NULL);
    if(rc != 0){
        printf("Fail\n");
        return -1;
    }else{
        printf("Ok\n");
    }

    cli_cleanup();

    // create temp user

    printf("test create temp user...");
    if(cli_setup() != 0){
        printf("setup clean fail\n");
        return -1;
    }

    rc = login_func(&cli, "Administrator", "Administrator", NULL);
    if(rc != 0){
        printf("Fail\n");
        return -1;
    }
    
    rc = clis_tempadd(&cli, CLIENT_USER_NORMAL, &id);
    if(rc != 0){
        printf("Fail %d\n", rc);
        return -1;
    }else{
        printf("Ok\n");
    }

    cli_cleanup();

    // temp login

    printf("test temp login ...");
    if(cli_setup() != 0){
        printf("setup clean fail\n");
        return -1;
    }
    
    {
        int type;
        char user[32];
        sprintf(user, "%d", id);
        printf("temp user %s\n", user);

        rc = clis_templogin(&cli, user, &type);
        if(rc != 0){
            printf("Fail\n");
            return -1;
        }

        printf("Ok\n");
    }
    
    cli_cleanup();

    // list user

    if(cli_setup() != 0){
        printf("setup clean fail\n");
        return -1;
    }

    rc = login_func(&cli, "Administrator", "Administrator", NULL);
    if(rc != 0){
        printf("Fail\n");
        return -1;
    }
   
    rc = listuser_func(&cli, NULL, NULL, NULL);
    if(rc != 0){
        printf("\nFail\n");
        return -1;
    }else{
        printf("\nOk\n");
    }

    cli_cleanup();

    return 0;
}


