#ifndef __SPIE_MGMT_TYPES_H__
#define __SPIE_MGMT_TYPES_H__


/* begin types */

/* struct mgmt_templogin_req */
struct _mgmt_templogin_req
{ 
  char * user;
};
typedef struct _mgmt_templogin_req mgmt_templogin_req_t;

int mgmt_templogin_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_templogin_req *in);
int mgmt_templogin_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_templogin_req **out);
void mgmt_templogin_req_free(struct _mgmt_templogin_req *ptr);
/* struct mgmt_templogin_rsp */
struct _mgmt_templogin_rsp
{ 
  int32_t type;
};
typedef struct _mgmt_templogin_rsp mgmt_templogin_rsp_t;

int mgmt_templogin_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_templogin_rsp *in);
int mgmt_templogin_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_templogin_rsp **out);
void mgmt_templogin_rsp_free(struct _mgmt_templogin_rsp *ptr);
/* struct mgmt_tempadd_req */
struct _mgmt_tempadd_req
{ 
  int32_t type;
};
typedef struct _mgmt_tempadd_req mgmt_tempadd_req_t;

int mgmt_tempadd_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_tempadd_req *in);
int mgmt_tempadd_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_tempadd_req **out);
void mgmt_tempadd_req_free(struct _mgmt_tempadd_req *ptr);
/* struct mgmt_tempadd_rsp */
struct _mgmt_tempadd_rsp
{ 
  int32_t code;
};
typedef struct _mgmt_tempadd_rsp mgmt_tempadd_rsp_t;

int mgmt_tempadd_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_tempadd_rsp *in);
int mgmt_tempadd_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_tempadd_rsp **out);
void mgmt_tempadd_rsp_free(struct _mgmt_tempadd_rsp *ptr);
/* struct mgmt_login_req */
struct _mgmt_login_req
{ 
  char * user;
  uint32_t seed_len;
  void * seed;
  uint32_t cred_len;
  void * cred;
  char * where;
};
typedef struct _mgmt_login_req mgmt_login_req_t;

int mgmt_login_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_login_req *in);
int mgmt_login_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_login_req **out);
void mgmt_login_req_free(struct _mgmt_login_req *ptr);
/* struct mgmt_login_rsp */
struct _mgmt_login_rsp
{ 
  int32_t ver;
};
typedef struct _mgmt_login_rsp mgmt_login_rsp_t;

int mgmt_login_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_login_rsp *in);
int mgmt_login_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_login_rsp **out);
void mgmt_login_rsp_free(struct _mgmt_login_rsp *ptr);
/* struct mgmt_adduser_req */
struct _mgmt_adduser_req
{ 
  char * user;
  uint32_t cred_len;
  void * cred;
  int32_t type;
};
typedef struct _mgmt_adduser_req mgmt_adduser_req_t;

int mgmt_adduser_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_adduser_req *in);
int mgmt_adduser_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_adduser_req **out);
void mgmt_adduser_req_free(struct _mgmt_adduser_req *ptr);
/* struct mgmt_adduser_rsp */
struct _mgmt_adduser_rsp
{ 
};
typedef struct _mgmt_adduser_rsp mgmt_adduser_rsp_t;

int mgmt_adduser_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_adduser_rsp *in);
int mgmt_adduser_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_adduser_rsp **out);
void mgmt_adduser_rsp_free(struct _mgmt_adduser_rsp *ptr);
/* struct mgmt_deluser_req */
struct _mgmt_deluser_req
{ 
  char * user;
};
typedef struct _mgmt_deluser_req mgmt_deluser_req_t;

int mgmt_deluser_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_deluser_req *in);
int mgmt_deluser_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_deluser_req **out);
void mgmt_deluser_req_free(struct _mgmt_deluser_req *ptr);
/* struct mgmt_deluser_rsp */
struct _mgmt_deluser_rsp
{ 
};
typedef struct _mgmt_deluser_rsp mgmt_deluser_rsp_t;

int mgmt_deluser_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_deluser_rsp *in);
int mgmt_deluser_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_deluser_rsp **out);
void mgmt_deluser_rsp_free(struct _mgmt_deluser_rsp *ptr);
/* struct mgmt_listuser_req */
struct _mgmt_listuser_req
{ 
  int32_t len;
};
typedef struct _mgmt_listuser_req mgmt_listuser_req_t;

int mgmt_listuser_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_listuser_req *in);
int mgmt_listuser_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_listuser_req **out);
void mgmt_listuser_req_free(struct _mgmt_listuser_req *ptr);
/* struct user */
struct _user
{ 
  char * user;
  int32_t type;
  int32_t status;
};
typedef struct _user user_t;

int user_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _user *in);
int user_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _user **out);
void user_free(struct _user *ptr);
/* struct mgmt_listuser_rsp */
struct _mgmt_listuser_rsp
{ 
  fixarray_t * users;
};
typedef struct _mgmt_listuser_rsp mgmt_listuser_rsp_t;

int mgmt_listuser_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_listuser_rsp *in);
int mgmt_listuser_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_listuser_rsp **out);
void mgmt_listuser_rsp_free(struct _mgmt_listuser_rsp *ptr);
/* struct mgmt_updateuser_req */
struct _mgmt_updateuser_req
{ 
  char * user;
  uint32_t cred_len;
  void * cred;
  int32_t type;
};
typedef struct _mgmt_updateuser_req mgmt_updateuser_req_t;

int mgmt_updateuser_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_updateuser_req *in);
int mgmt_updateuser_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_updateuser_req **out);
void mgmt_updateuser_req_free(struct _mgmt_updateuser_req *ptr);
/* struct mgmt_updateuser_rsp */
struct _mgmt_updateuser_rsp
{ 
};
typedef struct _mgmt_updateuser_rsp mgmt_updateuser_rsp_t;

int mgmt_updateuser_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_updateuser_rsp *in);
int mgmt_updateuser_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_updateuser_rsp **out);
void mgmt_updateuser_rsp_free(struct _mgmt_updateuser_rsp *ptr);
/* struct mgmt_sysinfo_req */
struct _mgmt_sysinfo_req
{ 
  int32_t type;
};
typedef struct _mgmt_sysinfo_req mgmt_sysinfo_req_t;

int mgmt_sysinfo_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_sysinfo_req *in);
int mgmt_sysinfo_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_sysinfo_req **out);
void mgmt_sysinfo_req_free(struct _mgmt_sysinfo_req *ptr);
/* struct nif */
struct _nif
{ 
  char * name;
  char * mac;
  char * addr;
};
typedef struct _nif nif_t;

int nif_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _nif *in);
int nif_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _nif **out);
void nif_free(struct _nif *ptr);
/* struct mgmt_sysinfo_rsp */
struct _mgmt_sysinfo_rsp
{ 
  int32_t capacity_used;
  int32_t capacity_free;
  int32_t battery;
  fixarray_t * nifs;
};
typedef struct _mgmt_sysinfo_rsp mgmt_sysinfo_rsp_t;

int mgmt_sysinfo_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_sysinfo_rsp *in);
int mgmt_sysinfo_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_sysinfo_rsp **out);
void mgmt_sysinfo_rsp_free(struct _mgmt_sysinfo_rsp *ptr);
/* constants */

#endif /* SPIE_MGMT_TYPES_H */
