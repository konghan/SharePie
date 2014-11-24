
#ifndef __CLI_MGMT_H__
#define __CLI_MGMT_H__

#include "fixarray.h"
#include "cli-trans.h"

#include <unistd.h>

#define CLIENT_USER_NORMAL  0x00000001
#define CLIENT_USER_PUBLIC  0x00000002
#define CLIENT_USER_TEMP    0x00000004

int clis_templogin(client_t *cli, char *user, int *type);
int clis_tempadd(client_t *cli, int type, int *code);

int clis_login(client_t *cli, char *user, char *passwd, char *where);
int clis_login_bycred(client_t *cli, char *user, int cred_len, void *cred, char *where);

int clis_user_add(client_t *cli, char *user, char *passwd, int type);
int clis_user_del(client_t *cli, char *user);
int clis_user_update(client_t *cli, char *user, int cred_len, void *cred, int type);
int clis_user_list(client_t *cli, int len, fixarray_t **users);

int clis_user_cred(char *user, char *passwd, void **cred, int *len);

struct _mgmt_sysinfo_rsp;
typedef struct _mgmt_sysinfo_rsp cli_sysinfo_t;

int clis_sysinfo(client_t *cli, cli_sysinfo_t **sta);

#endif

