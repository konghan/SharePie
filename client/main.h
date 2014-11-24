

#ifndef __MAIN_H__
#define __MAIN_H__

#include "osapi.h"
#include "list.h"

#include "fixbuffer.h"
#include "fixarray.h"

#include "spie_stor.h"
#include "spie_mgmt.h"

#include "cli-mgmt.h"
#include "cli-stor.h"

#define CLI_ADDR   "127.0.0.1"
#define CLI_PORT   8086


void usage();

typedef int (*cmd_func)(client_t *cli, const char *p0, const char *p1, const char *p2); 

struct _cmd{
    char *cmd;
    cmd_func  func;
};

int adduser_func(client_t *cli, const char *p0, const char *p1, const char *p2);
int deluser_func(client_t *cli, const char *p0, const char *p1, const char *p2);
int listuser_func(client_t *cli, const char *p0, const char *p1, const char *p2);
int chpwuser_func(client_t *cli, const char *p0, const char *p1, const char *p2);
int login_func(client_t *cli, const char *p0, const char *p1, const char *p2);


int list_func(client_t *cli, const char *type, const char *folderid, const char *notused);
int push_func(client_t *cli, const char *lofname, const char *refname, const char *notused);
int pull_func(client_t *cli, const char *fidstr, const char *lofname, const char *notused);
int move_func(client_t *cli, const char *fidstr, const char *folderidstr,
        const char *new_name);
int add_func(client_t *cli, const char *lofname, const char *notused1,
        const char *notused2);
int rmv_func(client_t *cli, const char *fidstr, const char *notused1,
        const char *notused2);

#endif

