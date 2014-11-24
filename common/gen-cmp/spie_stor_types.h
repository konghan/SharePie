#ifndef __SPIE_STOR_TYPES_H__
#define __SPIE_STOR_TYPES_H__


/* begin types */

/* struct stor_fopen_req */
struct _stor_fopen_req
{ 
  int64_t fid;
};
typedef struct _stor_fopen_req stor_fopen_req_t;

int stor_fopen_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fopen_req *in);
int stor_fopen_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fopen_req **out);
void stor_fopen_req_free(struct _stor_fopen_req *ptr);
/* struct stor_fopen_rsp */
struct _stor_fopen_rsp
{ 
};
typedef struct _stor_fopen_rsp stor_fopen_rsp_t;

int stor_fopen_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fopen_rsp *in);
int stor_fopen_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fopen_rsp **out);
void stor_fopen_rsp_free(struct _stor_fopen_rsp *ptr);
/* struct stor_fclose_req */
struct _stor_fclose_req
{ 
  int64_t fid;
};
typedef struct _stor_fclose_req stor_fclose_req_t;

int stor_fclose_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fclose_req *in);
int stor_fclose_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fclose_req **out);
void stor_fclose_req_free(struct _stor_fclose_req *ptr);
/* struct stor_fclose_rsp */
struct _stor_fclose_rsp
{ 
};
typedef struct _stor_fclose_rsp stor_fclose_rsp_t;

int stor_fclose_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fclose_rsp *in);
int stor_fclose_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fclose_rsp **out);
void stor_fclose_rsp_free(struct _stor_fclose_rsp *ptr);
/* struct stor_fstat_req */
struct _stor_fstat_req
{ 
  int64_t fid;
  int32_t type;
};
typedef struct _stor_fstat_req stor_fstat_req_t;

int stor_fstat_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fstat_req *in);
int stor_fstat_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fstat_req **out);
void stor_fstat_req_free(struct _stor_fstat_req *ptr);
/* struct stor_fstat_rsp */
struct _stor_fstat_rsp
{ 
  int64_t fid;
  char * fname;
  int64_t folder;
  int32_t type;
  int64_t fatime;
  int64_t fmtime;
  int64_t fctime;
  int64_t fsize;
  uint32_t snap_len;
  void * snap;
  char * origin;
};
typedef struct _stor_fstat_rsp stor_fstat_rsp_t;

int stor_fstat_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fstat_rsp *in);
int stor_fstat_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fstat_rsp **out);
void stor_fstat_rsp_free(struct _stor_fstat_rsp *ptr);
/* struct stor_freadsnap_req */
struct _stor_freadsnap_req
{ 
  int64_t fid;
};
typedef struct _stor_freadsnap_req stor_freadsnap_req_t;

int stor_freadsnap_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_freadsnap_req *in);
int stor_freadsnap_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_freadsnap_req **out);
void stor_freadsnap_req_free(struct _stor_freadsnap_req *ptr);
/* struct stor_freadsnap_rsp */
struct _stor_freadsnap_rsp
{ 
  int64_t fid;
  uint32_t snap_len;
  void * snap;
};
typedef struct _stor_freadsnap_rsp stor_freadsnap_rsp_t;

int stor_freadsnap_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_freadsnap_rsp *in);
int stor_freadsnap_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_freadsnap_rsp **out);
void stor_freadsnap_rsp_free(struct _stor_freadsnap_rsp *ptr);
/* struct stor_fwritesnap_req */
struct _stor_fwritesnap_req
{ 
  int64_t fid;
  uint32_t snap_len;
  void * snap;
};
typedef struct _stor_fwritesnap_req stor_fwritesnap_req_t;

int stor_fwritesnap_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fwritesnap_req *in);
int stor_fwritesnap_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fwritesnap_req **out);
void stor_fwritesnap_req_free(struct _stor_fwritesnap_req *ptr);
/* struct stor_fwritesnap_rsp */
struct _stor_fwritesnap_rsp
{ 
};
typedef struct _stor_fwritesnap_rsp stor_fwritesnap_rsp_t;

int stor_fwritesnap_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fwritesnap_rsp *in);
int stor_fwritesnap_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fwritesnap_rsp **out);
void stor_fwritesnap_rsp_free(struct _stor_fwritesnap_rsp *ptr);
/* struct stor_fread_req */
struct _stor_fread_req
{ 
  int64_t fid;
  int64_t offset;
  int32_t len;
};
typedef struct _stor_fread_req stor_fread_req_t;

int stor_fread_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fread_req *in);
int stor_fread_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fread_req **out);
void stor_fread_req_free(struct _stor_fread_req *ptr);
/* struct stor_fread_rsp */
struct _stor_fread_rsp
{ 
  int64_t fid;
  uint32_t data_len;
  void * data;
};
typedef struct _stor_fread_rsp stor_fread_rsp_t;

int stor_fread_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fread_rsp *in);
int stor_fread_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fread_rsp **out);
void stor_fread_rsp_free(struct _stor_fread_rsp *ptr);
/* struct stor_fwrite_req */
struct _stor_fwrite_req
{ 
  int64_t fid;
  int64_t offset;
  uint32_t data_len;
  void * data;
};
typedef struct _stor_fwrite_req stor_fwrite_req_t;

int stor_fwrite_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fwrite_req *in);
int stor_fwrite_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fwrite_req **out);
void stor_fwrite_req_free(struct _stor_fwrite_req *ptr);
/* struct stor_fwrite_rsp */
struct _stor_fwrite_rsp
{ 
  int32_t len;
};
typedef struct _stor_fwrite_rsp stor_fwrite_rsp_t;

int stor_fwrite_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fwrite_rsp *in);
int stor_fwrite_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fwrite_rsp **out);
void stor_fwrite_rsp_free(struct _stor_fwrite_rsp *ptr);
/* struct stor_ftruncate_req */
struct _stor_ftruncate_req
{ 
  int64_t fid;
  int64_t len;
};
typedef struct _stor_ftruncate_req stor_ftruncate_req_t;

int stor_ftruncate_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_ftruncate_req *in);
int stor_ftruncate_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_ftruncate_req **out);
void stor_ftruncate_req_free(struct _stor_ftruncate_req *ptr);
/* struct stor_ftruncate_rsp */
struct _stor_ftruncate_rsp
{ 
};
typedef struct _stor_ftruncate_rsp stor_ftruncate_rsp_t;

int stor_ftruncate_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_ftruncate_rsp *in);
int stor_ftruncate_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_ftruncate_rsp **out);
void stor_ftruncate_rsp_free(struct _stor_ftruncate_rsp *ptr);
/* struct stor_fcreate_req */
struct _stor_fcreate_req
{ 
  char * fname;
  int64_t folder;
  int32_t type;
  char * fpath;
};
typedef struct _stor_fcreate_req stor_fcreate_req_t;

int stor_fcreate_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fcreate_req *in);
int stor_fcreate_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fcreate_req **out);
void stor_fcreate_req_free(struct _stor_fcreate_req *ptr);
/* struct stor_fcreate_rsp */
struct _stor_fcreate_rsp
{ 
  int64_t fid;
};
typedef struct _stor_fcreate_rsp stor_fcreate_rsp_t;

int stor_fcreate_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fcreate_rsp *in);
int stor_fcreate_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fcreate_rsp **out);
void stor_fcreate_rsp_free(struct _stor_fcreate_rsp *ptr);
/* struct stor_fdelete_req */
struct _stor_fdelete_req
{ 
  int64_t fid;
};
typedef struct _stor_fdelete_req stor_fdelete_req_t;

int stor_fdelete_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fdelete_req *in);
int stor_fdelete_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fdelete_req **out);
void stor_fdelete_req_free(struct _stor_fdelete_req *ptr);
/* struct stor_fdelete_rsp */
struct _stor_fdelete_rsp
{ 
};
typedef struct _stor_fdelete_rsp stor_fdelete_rsp_t;

int stor_fdelete_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fdelete_rsp *in);
int stor_fdelete_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fdelete_rsp **out);
void stor_fdelete_rsp_free(struct _stor_fdelete_rsp *ptr);
/* struct stor_fmoveto_req */
struct _stor_fmoveto_req
{ 
  int64_t fid;
  int64_t folder;
  char * fname;
};
typedef struct _stor_fmoveto_req stor_fmoveto_req_t;

int stor_fmoveto_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fmoveto_req *in);
int stor_fmoveto_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fmoveto_req **out);
void stor_fmoveto_req_free(struct _stor_fmoveto_req *ptr);
/* struct stor_fmoveto_rsp */
struct _stor_fmoveto_rsp
{ 
};
typedef struct _stor_fmoveto_rsp stor_fmoveto_rsp_t;

int stor_fmoveto_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fmoveto_rsp *in);
int stor_fmoveto_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fmoveto_rsp **out);
void stor_fmoveto_rsp_free(struct _stor_fmoveto_rsp *ptr);
/* struct stor_lookup_create_req */
struct _stor_lookup_create_req
{ 
  int64_t folder;
  int32_t type;
};
typedef struct _stor_lookup_create_req stor_lookup_create_req_t;

int stor_lookup_create_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_lookup_create_req *in);
int stor_lookup_create_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_lookup_create_req **out);
void stor_lookup_create_req_free(struct _stor_lookup_create_req *ptr);
/* struct stor_lookup_create_rsp */
struct _stor_lookup_create_rsp
{ 
  int64_t luid;
};
typedef struct _stor_lookup_create_rsp stor_lookup_create_rsp_t;

int stor_lookup_create_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_lookup_create_rsp *in);
int stor_lookup_create_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_lookup_create_rsp **out);
void stor_lookup_create_rsp_free(struct _stor_lookup_create_rsp *ptr);
/* struct stor_lookup_destroy_req */
struct _stor_lookup_destroy_req
{ 
  int64_t luid;
};
typedef struct _stor_lookup_destroy_req stor_lookup_destroy_req_t;

int stor_lookup_destroy_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_lookup_destroy_req *in);
int stor_lookup_destroy_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_lookup_destroy_req **out);
void stor_lookup_destroy_req_free(struct _stor_lookup_destroy_req *ptr);
/* struct stor_lookup_destroy_rsp */
struct _stor_lookup_destroy_rsp
{ 
};
typedef struct _stor_lookup_destroy_rsp stor_lookup_destroy_rsp_t;

int stor_lookup_destroy_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_lookup_destroy_rsp *in);
int stor_lookup_destroy_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_lookup_destroy_rsp **out);
void stor_lookup_destroy_rsp_free(struct _stor_lookup_destroy_rsp *ptr);
/* struct stor_lookup_next_req */
struct _stor_lookup_next_req
{ 
  int64_t luid;
  int32_t num;
};
typedef struct _stor_lookup_next_req stor_lookup_next_req_t;

int stor_lookup_next_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_lookup_next_req *in);
int stor_lookup_next_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_lookup_next_req **out);
void stor_lookup_next_req_free(struct _stor_lookup_next_req *ptr);
/* struct stor_lookup_next_rsp */
struct _stor_lookup_next_rsp
{ 
  int64_t luid;
  fixarray_t * rfiles;
};
typedef struct _stor_lookup_next_rsp stor_lookup_next_rsp_t;

int stor_lookup_next_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_lookup_next_rsp *in);
int stor_lookup_next_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_lookup_next_rsp **out);
void stor_lookup_next_rsp_free(struct _stor_lookup_next_rsp *ptr);
/* constants */

#endif /* SPIE_STOR_TYPES_H */
