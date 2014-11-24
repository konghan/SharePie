
#include "spie_mgmt_types.h"

int mgmt_templogin_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_templogin_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  if(in->user == NULL){
      cmp_write_str(&cmp, in->user, 0);
  }else{
      cmp_write_str(&cmp, in->user, strlen(in->user));
  }

    return 0;
}

int mgmt_templogin_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_templogin_req **out){
  struct _mgmt_templogin_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _mgmt_templogin_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      o->user = NULL;
  }else{
      o->user = (char*)osapi_malloc(size+1);
      if(o->user == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, o->user, size);
      o->user[size] = 0;
  }
}


    *out = o;
    return 0;
}

void mgmt_templogin_req_free(struct _mgmt_templogin_req *ptr){

  if(ptr->user != NULL){
      osapi_free(ptr->user);
      ptr->user = NULL;
  }

    osapi_free(ptr);
}

int mgmt_templogin_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_templogin_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

   cmp_write_sint(&cmp, in->type);
    return 0;
}

int mgmt_templogin_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_templogin_rsp **out){
  struct _mgmt_templogin_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _mgmt_templogin_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

   cmp_read_int(&cmp, &o->type);

    *out = o;
    return 0;
}

void mgmt_templogin_rsp_free(struct _mgmt_templogin_rsp *ptr){

    osapi_free(ptr);
}

int mgmt_tempadd_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_tempadd_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

   cmp_write_sint(&cmp, in->type);
    return 0;
}

int mgmt_tempadd_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_tempadd_req **out){
  struct _mgmt_tempadd_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _mgmt_tempadd_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

   cmp_read_int(&cmp, &o->type);

    *out = o;
    return 0;
}

void mgmt_tempadd_req_free(struct _mgmt_tempadd_req *ptr){

    osapi_free(ptr);
}

int mgmt_tempadd_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_tempadd_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

   cmp_write_sint(&cmp, in->code);
    return 0;
}

int mgmt_tempadd_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_tempadd_rsp **out){
  struct _mgmt_tempadd_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _mgmt_tempadd_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

   cmp_read_int(&cmp, &o->code);

    *out = o;
    return 0;
}

void mgmt_tempadd_rsp_free(struct _mgmt_tempadd_rsp *ptr){

    osapi_free(ptr);
}

int mgmt_login_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_login_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  if(in->user == NULL){
      cmp_write_str(&cmp, in->user, 0);
  }else{
      cmp_write_str(&cmp, in->user, strlen(in->user));
  }

  cmp_write_bin(&cmp, in->seed, in->seed_len);
  cmp_write_bin(&cmp, in->cred, in->cred_len);
  if(in->where == NULL){
      cmp_write_str(&cmp, in->where, 0);
  }else{
      cmp_write_str(&cmp, in->where, strlen(in->where));
  }

    return 0;
}

int mgmt_login_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_login_req **out){
  struct _mgmt_login_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _mgmt_login_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      o->user = NULL;
  }else{
      o->user = (char*)osapi_malloc(size+1);
      if(o->user == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, o->user, size);
      o->user[size] = 0;
  }
}

  cmp_read_bin_size(&cmp, &o->seed_len);
  if(o->seed_len == 0){
      o->seed = NULL;
  }else{
      o->seed = osapi_malloc(o->seed_len);
      if(o->seed == NULL){ 
            mgmt_login_req_free(o);
            return -ENOMEM;
        }
      reader(&cmp, o->seed, o->seed_len);
  }
  cmp_read_bin_size(&cmp, &o->cred_len);
  if(o->cred_len == 0){
      o->cred = NULL;
  }else{
      o->cred = osapi_malloc(o->cred_len);
      if(o->cred == NULL){ 
            mgmt_login_req_free(o);
            return -ENOMEM;
        }
      reader(&cmp, o->cred, o->cred_len);
  }

{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      o->where = NULL;
  }else{
      o->where = (char*)osapi_malloc(size+1);
      if(o->where == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, o->where, size);
      o->where[size] = 0;
  }
}


    *out = o;
    return 0;
}

void mgmt_login_req_free(struct _mgmt_login_req *ptr){

  if(ptr->user != NULL){
      osapi_free(ptr->user);
      ptr->user = NULL;
  }
  if(ptr->seed != NULL){
     osapi_free(ptr->seed);
     ptr->seed = NULL;
     ptr->seed_len = 0;
  }
  if(ptr->cred != NULL){
     osapi_free(ptr->cred);
     ptr->cred = NULL;
     ptr->cred_len = 0;
  }

  if(ptr->where != NULL){
      osapi_free(ptr->where);
      ptr->where = NULL;
  }

    osapi_free(ptr);
}

int mgmt_login_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_login_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

   cmp_write_sint(&cmp, in->ver);
    return 0;
}

int mgmt_login_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_login_rsp **out){
  struct _mgmt_login_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _mgmt_login_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

   cmp_read_int(&cmp, &o->ver);

    *out = o;
    return 0;
}

void mgmt_login_rsp_free(struct _mgmt_login_rsp *ptr){

    osapi_free(ptr);
}

int mgmt_adduser_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_adduser_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  if(in->user == NULL){
      cmp_write_str(&cmp, in->user, 0);
  }else{
      cmp_write_str(&cmp, in->user, strlen(in->user));
  }

  cmp_write_bin(&cmp, in->cred, in->cred_len);
   cmp_write_sint(&cmp, in->type);
    return 0;
}

int mgmt_adduser_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_adduser_req **out){
  struct _mgmt_adduser_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _mgmt_adduser_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      o->user = NULL;
  }else{
      o->user = (char*)osapi_malloc(size+1);
      if(o->user == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, o->user, size);
      o->user[size] = 0;
  }
}

  cmp_read_bin_size(&cmp, &o->cred_len);
  if(o->cred_len == 0){
      o->cred = NULL;
  }else{
      o->cred = osapi_malloc(o->cred_len);
      if(o->cred == NULL){ 
            mgmt_adduser_req_free(o);
            return -ENOMEM;
        }
      reader(&cmp, o->cred, o->cred_len);
  }
   cmp_read_int(&cmp, &o->type);

    *out = o;
    return 0;
}

void mgmt_adduser_req_free(struct _mgmt_adduser_req *ptr){

  if(ptr->user != NULL){
      osapi_free(ptr->user);
      ptr->user = NULL;
  }
  if(ptr->cred != NULL){
     osapi_free(ptr->cred);
     ptr->cred = NULL;
     ptr->cred_len = 0;
  }

    osapi_free(ptr);
}

int mgmt_adduser_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_adduser_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

    return 0;
}

int mgmt_adduser_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_adduser_rsp **out){
  struct _mgmt_adduser_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _mgmt_adduser_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


    *out = o;
    return 0;
}

void mgmt_adduser_rsp_free(struct _mgmt_adduser_rsp *ptr){

    osapi_free(ptr);
}

int mgmt_deluser_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_deluser_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  if(in->user == NULL){
      cmp_write_str(&cmp, in->user, 0);
  }else{
      cmp_write_str(&cmp, in->user, strlen(in->user));
  }

    return 0;
}

int mgmt_deluser_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_deluser_req **out){
  struct _mgmt_deluser_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _mgmt_deluser_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      o->user = NULL;
  }else{
      o->user = (char*)osapi_malloc(size+1);
      if(o->user == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, o->user, size);
      o->user[size] = 0;
  }
}


    *out = o;
    return 0;
}

void mgmt_deluser_req_free(struct _mgmt_deluser_req *ptr){

  if(ptr->user != NULL){
      osapi_free(ptr->user);
      ptr->user = NULL;
  }

    osapi_free(ptr);
}

int mgmt_deluser_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_deluser_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

    return 0;
}

int mgmt_deluser_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_deluser_rsp **out){
  struct _mgmt_deluser_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _mgmt_deluser_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


    *out = o;
    return 0;
}

void mgmt_deluser_rsp_free(struct _mgmt_deluser_rsp *ptr){

    osapi_free(ptr);
}

int mgmt_listuser_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_listuser_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

   cmp_write_sint(&cmp, in->len);
    return 0;
}

int mgmt_listuser_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_listuser_req **out){
  struct _mgmt_listuser_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _mgmt_listuser_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

   cmp_read_int(&cmp, &o->len);

    *out = o;
    return 0;
}

void mgmt_listuser_req_free(struct _mgmt_listuser_req *ptr){

    osapi_free(ptr);
}

int user_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _user *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  if(in->user == NULL){
      cmp_write_str(&cmp, in->user, 0);
  }else{
      cmp_write_str(&cmp, in->user, strlen(in->user));
  }

   cmp_write_sint(&cmp, in->type);
   cmp_write_sint(&cmp, in->status);
    return 0;
}

int user_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _user **out){
  struct _user *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _user*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      o->user = NULL;
  }else{
      o->user = (char*)osapi_malloc(size+1);
      if(o->user == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, o->user, size);
      o->user[size] = 0;
  }
}

   cmp_read_int(&cmp, &o->type);
   cmp_read_int(&cmp, &o->status);

    *out = o;
    return 0;
}

void user_free(struct _user *ptr){

  if(ptr->user != NULL){
      osapi_free(ptr->user);
      ptr->user = NULL;
  }

    osapi_free(ptr);
}

int mgmt_listuser_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_listuser_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  {
    user_t *obj;
    int i; 
    fixarray_zip(in->users);
    cmp_write_array(&cmp, fixarray_num(in->users));
    for (i = 0; i < fixarray_num(in->users); i++){
        if(fixarray_get(in->users, i, (void **)&obj) != 0){
            return -1;
        }
      if(obj->user == NULL){
          cmp_write_str(&cmp, obj->user, 0);
      }else{
          cmp_write_str(&cmp, obj->user, strlen(obj->user));
      }

     cmp_write_sint(&cmp, obj->type);
     cmp_write_sint(&cmp, obj->status);
  }
 }

    return 0;
}

int mgmt_listuser_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_listuser_rsp **out){
  struct _mgmt_listuser_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _mgmt_listuser_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  {
    user_t *obj;
    uint32_t size;
    int i;
    cmp_read_array(&cmp, &size);
    fixarray_create(size, &o->users);
    for (i = 0; i < size; i++){
      obj = osapi_malloc(sizeof(*obj));
      if(obj == NULL) {
        osapi_free(o);
        return -ENOMEM; 
  }

{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      obj->user = NULL; 
}else{
      obj->user = (char*)osapi_malloc(size+1);
      if(obj->user == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, obj->user, size);
      obj->user[size] = 0;
  }
}

   cmp_read_int(&cmp, &obj->type);
   cmp_read_int(&cmp, &obj->status);
      fixarray_set(o->users, i, obj);
  }
 }


    *out = o;
    return 0;
}

void mgmt_listuser_rsp_free(struct _mgmt_listuser_rsp *ptr){
    if(ptr->users != NULL){
        fixarray_destroy(ptr->users, (fixarray_itemfree)user_free);
        ptr->users = NULL;
    }

    osapi_free(ptr);
}

int mgmt_updateuser_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_updateuser_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  if(in->user == NULL){
      cmp_write_str(&cmp, in->user, 0);
  }else{
      cmp_write_str(&cmp, in->user, strlen(in->user));
  }

  cmp_write_bin(&cmp, in->cred, in->cred_len);
   cmp_write_sint(&cmp, in->type);
    return 0;
}

int mgmt_updateuser_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_updateuser_req **out){
  struct _mgmt_updateuser_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _mgmt_updateuser_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      o->user = NULL;
  }else{
      o->user = (char*)osapi_malloc(size+1);
      if(o->user == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, o->user, size);
      o->user[size] = 0;
  }
}

  cmp_read_bin_size(&cmp, &o->cred_len);
  if(o->cred_len == 0){
      o->cred = NULL;
  }else{
      o->cred = osapi_malloc(o->cred_len);
      if(o->cred == NULL){ 
            mgmt_updateuser_req_free(o);
            return -ENOMEM;
        }
      reader(&cmp, o->cred, o->cred_len);
  }
   cmp_read_int(&cmp, &o->type);

    *out = o;
    return 0;
}

void mgmt_updateuser_req_free(struct _mgmt_updateuser_req *ptr){

  if(ptr->user != NULL){
      osapi_free(ptr->user);
      ptr->user = NULL;
  }
  if(ptr->cred != NULL){
     osapi_free(ptr->cred);
     ptr->cred = NULL;
     ptr->cred_len = 0;
  }

    osapi_free(ptr);
}

int mgmt_updateuser_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_updateuser_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

    return 0;
}

int mgmt_updateuser_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_updateuser_rsp **out){
  struct _mgmt_updateuser_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _mgmt_updateuser_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


    *out = o;
    return 0;
}

void mgmt_updateuser_rsp_free(struct _mgmt_updateuser_rsp *ptr){

    osapi_free(ptr);
}

int mgmt_sysinfo_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_sysinfo_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

   cmp_write_sint(&cmp, in->type);
    return 0;
}

int mgmt_sysinfo_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_sysinfo_req **out){
  struct _mgmt_sysinfo_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _mgmt_sysinfo_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

   cmp_read_int(&cmp, &o->type);

    *out = o;
    return 0;
}

void mgmt_sysinfo_req_free(struct _mgmt_sysinfo_req *ptr){

    osapi_free(ptr);
}

int nif_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _nif *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  if(in->name == NULL){
      cmp_write_str(&cmp, in->name, 0);
  }else{
      cmp_write_str(&cmp, in->name, strlen(in->name));
  }

  if(in->mac == NULL){
      cmp_write_str(&cmp, in->mac, 0);
  }else{
      cmp_write_str(&cmp, in->mac, strlen(in->mac));
  }

  if(in->addr == NULL){
      cmp_write_str(&cmp, in->addr, 0);
  }else{
      cmp_write_str(&cmp, in->addr, strlen(in->addr));
  }

    return 0;
}

int nif_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _nif **out){
  struct _nif *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _nif*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      o->name = NULL;
  }else{
      o->name = (char*)osapi_malloc(size+1);
      if(o->name == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, o->name, size);
      o->name[size] = 0;
  }
}


{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      o->mac = NULL;
  }else{
      o->mac = (char*)osapi_malloc(size+1);
      if(o->mac == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, o->mac, size);
      o->mac[size] = 0;
  }
}


{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      o->addr = NULL;
  }else{
      o->addr = (char*)osapi_malloc(size+1);
      if(o->addr == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, o->addr, size);
      o->addr[size] = 0;
  }
}


    *out = o;
    return 0;
}

void nif_free(struct _nif *ptr){

  if(ptr->name != NULL){
      osapi_free(ptr->name);
      ptr->name = NULL;
  }

  if(ptr->mac != NULL){
      osapi_free(ptr->mac);
      ptr->mac = NULL;
  }

  if(ptr->addr != NULL){
      osapi_free(ptr->addr);
      ptr->addr = NULL;
  }

    osapi_free(ptr);
}

int mgmt_sysinfo_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _mgmt_sysinfo_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

   cmp_write_sint(&cmp, in->capacity_used);
   cmp_write_sint(&cmp, in->capacity_free);
   cmp_write_sint(&cmp, in->battery);
  {
    nif_t *obj;
    int i; 
    fixarray_zip(in->nifs);
    cmp_write_array(&cmp, fixarray_num(in->nifs));
    for (i = 0; i < fixarray_num(in->nifs); i++){
        if(fixarray_get(in->nifs, i, (void **)&obj) != 0){
            return -1;
        }
      if(obj->name == NULL){
          cmp_write_str(&cmp, obj->name, 0);
      }else{
          cmp_write_str(&cmp, obj->name, strlen(obj->name));
      }

      if(obj->mac == NULL){
          cmp_write_str(&cmp, obj->mac, 0);
      }else{
          cmp_write_str(&cmp, obj->mac, strlen(obj->mac));
      }

      if(obj->addr == NULL){
          cmp_write_str(&cmp, obj->addr, 0);
      }else{
          cmp_write_str(&cmp, obj->addr, strlen(obj->addr));
      }

  }
 }

    return 0;
}

int mgmt_sysinfo_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _mgmt_sysinfo_rsp **out){
  struct _mgmt_sysinfo_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _mgmt_sysinfo_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

   cmp_read_int(&cmp, &o->capacity_used);
   cmp_read_int(&cmp, &o->capacity_free);
   cmp_read_int(&cmp, &o->battery);
  {
    nif_t *obj;
    uint32_t size;
    int i;
    cmp_read_array(&cmp, &size);
    fixarray_create(size, &o->nifs);
    for (i = 0; i < size; i++){
      obj = osapi_malloc(sizeof(*obj));
      if(obj == NULL) {
        osapi_free(o);
        return -ENOMEM; 
  }

{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      obj->name = NULL; 
}else{
      obj->name = (char*)osapi_malloc(size+1);
      if(obj->name == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, obj->name, size);
      obj->name[size] = 0;
  }
}


{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      obj->mac = NULL; 
}else{
      obj->mac = (char*)osapi_malloc(size+1);
      if(obj->mac == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, obj->mac, size);
      obj->mac[size] = 0;
  }
}


{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      obj->addr = NULL; 
}else{
      obj->addr = (char*)osapi_malloc(size+1);
      if(obj->addr == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, obj->addr, size);
      obj->addr[size] = 0;
  }
}

      fixarray_set(o->nifs, i, obj);
  }
 }


    *out = o;
    return 0;
}

void mgmt_sysinfo_rsp_free(struct _mgmt_sysinfo_rsp *ptr){
    if(ptr->nifs != NULL){
        fixarray_destroy(ptr->nifs, (fixarray_itemfree)nif_free);
        ptr->nifs = NULL;
    }

    osapi_free(ptr);
}

/* constants */

