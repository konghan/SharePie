#include "cli-trans.h"

int client_init(client_t *cli, session_t *ses){
    cli->ses = ses;
    cli->status = CLIENT_STATUS_INIT;

    return 0;
}

int client_fini(client_t *cli){
    cli->ses = NULL;
    cli->status = -1;

    return 0;
}


