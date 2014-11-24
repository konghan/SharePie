
#ifndef __CLI_STOR_H__
#define __CLI_STOR_H__

#include "fixarray.h"
#include "cli-trans.h"

#include <stdint.h>

struct _stor_fstat_rsp;
typedef struct _stor_fstat_rsp rfile_t;

int64_t clis_lookup_create(client_t *cli, int64_t folder, int type);
int clis_lookup_destroy(client_t *cli, int64_t luid);
int clis_lookup_next(client_t *cli, int64_t luid, int num, fixarray_t **items);

int clis_file_stat(client_t *cli, int64_t fid, rfile_t **rf);
int clis_file_open(client_t *cli, int64_t fid);
int clis_file_close(client_t *cli, int64_t fid);
int clis_file_truncate(client_t *cli, int64_t fid, int64_t len);
int clis_file_read(client_t *cli, int64_t fid, int64_t offset, void *buf, int len);
int clis_file_write(client_t *cli, int64_t fid, int64_t offset, void *buf, int len);

int clis_file_readsnap(client_t *cli, int64_t fid, void **buf, int *len);
int clis_file_writesnap(client_t *cli, int64_t fid, void *buf, int len);

int64_t clis_file_create(client_t *cli, char *file, int type, int64_t folder);
int clis_file_delete(client_t *cli, int64_t fid);

int clis_file_moveto(client_t *cli, int64_t fid, int64_t folder, char *new_name);

#endif

