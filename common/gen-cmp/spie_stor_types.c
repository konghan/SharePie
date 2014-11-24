
#include "spie_stor_types.h"

int stor_fopen_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fopen_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->fid);
    return 0;
}

int stor_fopen_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fopen_req **out){
  struct _stor_fopen_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fopen_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->fid);

    *out = o;
    return 0;
}

void stor_fopen_req_free(struct _stor_fopen_req *ptr){

    osapi_free(ptr);
}

int stor_fopen_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fopen_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

    return 0;
}

int stor_fopen_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fopen_rsp **out){
  struct _stor_fopen_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fopen_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


    *out = o;
    return 0;
}

void stor_fopen_rsp_free(struct _stor_fopen_rsp *ptr){

    osapi_free(ptr);
}

int stor_fclose_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fclose_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->fid);
    return 0;
}

int stor_fclose_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fclose_req **out){
  struct _stor_fclose_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fclose_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->fid);

    *out = o;
    return 0;
}

void stor_fclose_req_free(struct _stor_fclose_req *ptr){

    osapi_free(ptr);
}

int stor_fclose_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fclose_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

    return 0;
}

int stor_fclose_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fclose_rsp **out){
  struct _stor_fclose_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fclose_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


    *out = o;
    return 0;
}

void stor_fclose_rsp_free(struct _stor_fclose_rsp *ptr){

    osapi_free(ptr);
}

int stor_fstat_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fstat_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->fid);
   cmp_write_sint(&cmp, in->type);
    return 0;
}

int stor_fstat_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fstat_req **out){
  struct _stor_fstat_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fstat_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->fid);
   cmp_read_int(&cmp, &o->type);

    *out = o;
    return 0;
}

void stor_fstat_req_free(struct _stor_fstat_req *ptr){

    osapi_free(ptr);
}

int stor_fstat_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fstat_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->fid);
  if(in->fname == NULL){
      cmp_write_str(&cmp, in->fname, 0);
  }else{
      cmp_write_str(&cmp, in->fname, strlen(in->fname));
  }

  cmp_write_sint(&cmp, in->folder);
   cmp_write_sint(&cmp, in->type);
  cmp_write_sint(&cmp, in->fatime);
  cmp_write_sint(&cmp, in->fmtime);
  cmp_write_sint(&cmp, in->fctime);
  cmp_write_sint(&cmp, in->fsize);
  cmp_write_bin(&cmp, in->snap, in->snap_len);
  if(in->origin == NULL){
      cmp_write_str(&cmp, in->origin, 0);
  }else{
      cmp_write_str(&cmp, in->origin, strlen(in->origin));
  }

    return 0;
}

int stor_fstat_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fstat_rsp **out){
  struct _stor_fstat_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fstat_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->fid);

{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      o->fname = NULL;
  }else{
      o->fname = (char*)osapi_malloc(size+1);
      if(o->fname == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, o->fname, size);
      o->fname[size] = 0;
  }
}

  cmp_read_long(&cmp,&o->folder);
   cmp_read_int(&cmp, &o->type);
  cmp_read_long(&cmp,&o->fatime);
  cmp_read_long(&cmp,&o->fmtime);
  cmp_read_long(&cmp,&o->fctime);
  cmp_read_long(&cmp,&o->fsize);
  cmp_read_bin_size(&cmp, &o->snap_len);
  if(o->snap_len == 0){
      o->snap = NULL;
  }else{
      o->snap = osapi_malloc(o->snap_len);
      if(o->snap == NULL){ 
            stor_fstat_rsp_free(o);
            return -ENOMEM;
        }
      reader(&cmp, o->snap, o->snap_len);
  }

{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      o->origin = NULL;
  }else{
      o->origin = (char*)osapi_malloc(size+1);
      if(o->origin == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, o->origin, size);
      o->origin[size] = 0;
  }
}


    *out = o;
    return 0;
}

void stor_fstat_rsp_free(struct _stor_fstat_rsp *ptr){

  if(ptr->fname != NULL){
      osapi_free(ptr->fname);
      ptr->fname = NULL;
  }
  if(ptr->snap != NULL){
     osapi_free(ptr->snap);
     ptr->snap = NULL;
     ptr->snap_len = 0;
  }

  if(ptr->origin != NULL){
      osapi_free(ptr->origin);
      ptr->origin = NULL;
  }

    osapi_free(ptr);
}

int stor_freadsnap_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_freadsnap_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->fid);
    return 0;
}

int stor_freadsnap_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_freadsnap_req **out){
  struct _stor_freadsnap_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_freadsnap_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->fid);

    *out = o;
    return 0;
}

void stor_freadsnap_req_free(struct _stor_freadsnap_req *ptr){

    osapi_free(ptr);
}

int stor_freadsnap_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_freadsnap_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->fid);
  cmp_write_bin(&cmp, in->snap, in->snap_len);
    return 0;
}

int stor_freadsnap_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_freadsnap_rsp **out){
  struct _stor_freadsnap_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_freadsnap_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->fid);
  cmp_read_bin_size(&cmp, &o->snap_len);
  if(o->snap_len == 0){
      o->snap = NULL;
  }else{
      o->snap = osapi_malloc(o->snap_len);
      if(o->snap == NULL){ 
            stor_freadsnap_rsp_free(o);
            return -ENOMEM;
        }
      reader(&cmp, o->snap, o->snap_len);
  }

    *out = o;
    return 0;
}

void stor_freadsnap_rsp_free(struct _stor_freadsnap_rsp *ptr){
  if(ptr->snap != NULL){
     osapi_free(ptr->snap);
     ptr->snap = NULL;
     ptr->snap_len = 0;
  }

    osapi_free(ptr);
}

int stor_fwritesnap_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fwritesnap_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->fid);
  cmp_write_bin(&cmp, in->snap, in->snap_len);
    return 0;
}

int stor_fwritesnap_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fwritesnap_req **out){
  struct _stor_fwritesnap_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fwritesnap_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->fid);
  cmp_read_bin_size(&cmp, &o->snap_len);
  if(o->snap_len == 0){
      o->snap = NULL;
  }else{
      o->snap = osapi_malloc(o->snap_len);
      if(o->snap == NULL){ 
            stor_fwritesnap_req_free(o);
            return -ENOMEM;
        }
      reader(&cmp, o->snap, o->snap_len);
  }

    *out = o;
    return 0;
}

void stor_fwritesnap_req_free(struct _stor_fwritesnap_req *ptr){
  if(ptr->snap != NULL){
     osapi_free(ptr->snap);
     ptr->snap = NULL;
     ptr->snap_len = 0;
  }

    osapi_free(ptr);
}

int stor_fwritesnap_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fwritesnap_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

    return 0;
}

int stor_fwritesnap_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fwritesnap_rsp **out){
  struct _stor_fwritesnap_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fwritesnap_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


    *out = o;
    return 0;
}

void stor_fwritesnap_rsp_free(struct _stor_fwritesnap_rsp *ptr){

    osapi_free(ptr);
}

int stor_fread_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fread_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->fid);
  cmp_write_sint(&cmp, in->offset);
   cmp_write_sint(&cmp, in->len);
    return 0;
}

int stor_fread_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fread_req **out){
  struct _stor_fread_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fread_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->fid);
  cmp_read_long(&cmp,&o->offset);
   cmp_read_int(&cmp, &o->len);

    *out = o;
    return 0;
}

void stor_fread_req_free(struct _stor_fread_req *ptr){

    osapi_free(ptr);
}

int stor_fread_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fread_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->fid);
  cmp_write_bin(&cmp, in->data, in->data_len);
    return 0;
}

int stor_fread_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fread_rsp **out){
  struct _stor_fread_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fread_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->fid);
  cmp_read_bin_size(&cmp, &o->data_len);
  if(o->data_len == 0){
      o->data = NULL;
  }else{
      o->data = osapi_malloc(o->data_len);
      if(o->data == NULL){ 
            stor_fread_rsp_free(o);
            return -ENOMEM;
        }
      reader(&cmp, o->data, o->data_len);
  }

    *out = o;
    return 0;
}

void stor_fread_rsp_free(struct _stor_fread_rsp *ptr){
  if(ptr->data != NULL){
     osapi_free(ptr->data);
     ptr->data = NULL;
     ptr->data_len = 0;
  }

    osapi_free(ptr);
}

int stor_fwrite_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fwrite_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->fid);
  cmp_write_sint(&cmp, in->offset);
  cmp_write_bin(&cmp, in->data, in->data_len);
    return 0;
}

int stor_fwrite_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fwrite_req **out){
  struct _stor_fwrite_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fwrite_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->fid);
  cmp_read_long(&cmp,&o->offset);
  cmp_read_bin_size(&cmp, &o->data_len);
  if(o->data_len == 0){
      o->data = NULL;
  }else{
      o->data = osapi_malloc(o->data_len);
      if(o->data == NULL){ 
            stor_fwrite_req_free(o);
            return -ENOMEM;
        }
      reader(&cmp, o->data, o->data_len);
  }

    *out = o;
    return 0;
}

void stor_fwrite_req_free(struct _stor_fwrite_req *ptr){
  if(ptr->data != NULL){
     osapi_free(ptr->data);
     ptr->data = NULL;
     ptr->data_len = 0;
  }

    osapi_free(ptr);
}

int stor_fwrite_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fwrite_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

   cmp_write_sint(&cmp, in->len);
    return 0;
}

int stor_fwrite_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fwrite_rsp **out){
  struct _stor_fwrite_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fwrite_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

   cmp_read_int(&cmp, &o->len);

    *out = o;
    return 0;
}

void stor_fwrite_rsp_free(struct _stor_fwrite_rsp *ptr){

    osapi_free(ptr);
}

int stor_ftruncate_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_ftruncate_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->fid);
  cmp_write_sint(&cmp, in->len);
    return 0;
}

int stor_ftruncate_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_ftruncate_req **out){
  struct _stor_ftruncate_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_ftruncate_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->fid);
  cmp_read_long(&cmp,&o->len);

    *out = o;
    return 0;
}

void stor_ftruncate_req_free(struct _stor_ftruncate_req *ptr){

    osapi_free(ptr);
}

int stor_ftruncate_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_ftruncate_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

    return 0;
}

int stor_ftruncate_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_ftruncate_rsp **out){
  struct _stor_ftruncate_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_ftruncate_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


    *out = o;
    return 0;
}

void stor_ftruncate_rsp_free(struct _stor_ftruncate_rsp *ptr){

    osapi_free(ptr);
}

int stor_fcreate_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fcreate_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  if(in->fname == NULL){
      cmp_write_str(&cmp, in->fname, 0);
  }else{
      cmp_write_str(&cmp, in->fname, strlen(in->fname));
  }

  cmp_write_sint(&cmp, in->folder);
   cmp_write_sint(&cmp, in->type);
  if(in->fpath == NULL){
      cmp_write_str(&cmp, in->fpath, 0);
  }else{
      cmp_write_str(&cmp, in->fpath, strlen(in->fpath));
  }

    return 0;
}

int stor_fcreate_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fcreate_req **out){
  struct _stor_fcreate_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fcreate_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      o->fname = NULL;
  }else{
      o->fname = (char*)osapi_malloc(size+1);
      if(o->fname == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, o->fname, size);
      o->fname[size] = 0;
  }
}

  cmp_read_long(&cmp,&o->folder);
   cmp_read_int(&cmp, &o->type);

{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      o->fpath = NULL;
  }else{
      o->fpath = (char*)osapi_malloc(size+1);
      if(o->fpath == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, o->fpath, size);
      o->fpath[size] = 0;
  }
}


    *out = o;
    return 0;
}

void stor_fcreate_req_free(struct _stor_fcreate_req *ptr){

  if(ptr->fname != NULL){
      osapi_free(ptr->fname);
      ptr->fname = NULL;
  }

  if(ptr->fpath != NULL){
      osapi_free(ptr->fpath);
      ptr->fpath = NULL;
  }

    osapi_free(ptr);
}

int stor_fcreate_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fcreate_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->fid);
    return 0;
}

int stor_fcreate_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fcreate_rsp **out){
  struct _stor_fcreate_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fcreate_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->fid);

    *out = o;
    return 0;
}

void stor_fcreate_rsp_free(struct _stor_fcreate_rsp *ptr){

    osapi_free(ptr);
}

int stor_fdelete_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fdelete_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->fid);
    return 0;
}

int stor_fdelete_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fdelete_req **out){
  struct _stor_fdelete_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fdelete_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->fid);

    *out = o;
    return 0;
}

void stor_fdelete_req_free(struct _stor_fdelete_req *ptr){

    osapi_free(ptr);
}

int stor_fdelete_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fdelete_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

    return 0;
}

int stor_fdelete_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fdelete_rsp **out){
  struct _stor_fdelete_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fdelete_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


    *out = o;
    return 0;
}

void stor_fdelete_rsp_free(struct _stor_fdelete_rsp *ptr){

    osapi_free(ptr);
}

int stor_fmoveto_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fmoveto_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->fid);
  cmp_write_sint(&cmp, in->folder);
  if(in->fname == NULL){
      cmp_write_str(&cmp, in->fname, 0);
  }else{
      cmp_write_str(&cmp, in->fname, strlen(in->fname));
  }

    return 0;
}

int stor_fmoveto_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fmoveto_req **out){
  struct _stor_fmoveto_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fmoveto_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->fid);
  cmp_read_long(&cmp,&o->folder);

{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      o->fname = NULL;
  }else{
      o->fname = (char*)osapi_malloc(size+1);
      if(o->fname == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, o->fname, size);
      o->fname[size] = 0;
  }
}


    *out = o;
    return 0;
}

void stor_fmoveto_req_free(struct _stor_fmoveto_req *ptr){

  if(ptr->fname != NULL){
      osapi_free(ptr->fname);
      ptr->fname = NULL;
  }

    osapi_free(ptr);
}

int stor_fmoveto_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_fmoveto_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

    return 0;
}

int stor_fmoveto_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_fmoveto_rsp **out){
  struct _stor_fmoveto_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_fmoveto_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


    *out = o;
    return 0;
}

void stor_fmoveto_rsp_free(struct _stor_fmoveto_rsp *ptr){

    osapi_free(ptr);
}

int stor_lookup_create_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_lookup_create_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->folder);
   cmp_write_sint(&cmp, in->type);
    return 0;
}

int stor_lookup_create_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_lookup_create_req **out){
  struct _stor_lookup_create_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_lookup_create_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->folder);
   cmp_read_int(&cmp, &o->type);

    *out = o;
    return 0;
}

void stor_lookup_create_req_free(struct _stor_lookup_create_req *ptr){

    osapi_free(ptr);
}

int stor_lookup_create_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_lookup_create_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->luid);
    return 0;
}

int stor_lookup_create_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_lookup_create_rsp **out){
  struct _stor_lookup_create_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_lookup_create_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->luid);

    *out = o;
    return 0;
}

void stor_lookup_create_rsp_free(struct _stor_lookup_create_rsp *ptr){

    osapi_free(ptr);
}

int stor_lookup_destroy_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_lookup_destroy_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->luid);
    return 0;
}

int stor_lookup_destroy_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_lookup_destroy_req **out){
  struct _stor_lookup_destroy_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_lookup_destroy_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->luid);

    *out = o;
    return 0;
}

void stor_lookup_destroy_req_free(struct _stor_lookup_destroy_req *ptr){

    osapi_free(ptr);
}

int stor_lookup_destroy_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_lookup_destroy_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

    return 0;
}

int stor_lookup_destroy_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_lookup_destroy_rsp **out){
  struct _stor_lookup_destroy_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_lookup_destroy_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 


    *out = o;
    return 0;
}

void stor_lookup_destroy_rsp_free(struct _stor_lookup_destroy_rsp *ptr){

    osapi_free(ptr);
}

int stor_lookup_next_req_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_lookup_next_req *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->luid);
   cmp_write_sint(&cmp, in->num);
    return 0;
}

int stor_lookup_next_req_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_lookup_next_req **out){
  struct _stor_lookup_next_req *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_lookup_next_req*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->luid);
   cmp_read_int(&cmp, &o->num);

    *out = o;
    return 0;
}

void stor_lookup_next_req_free(struct _stor_lookup_next_req *ptr){

    osapi_free(ptr);
}

int stor_lookup_next_rsp_mashal(void *ptr, cmp_reader reader, cmp_writer writer, const struct _stor_lookup_next_rsp *in){
  cmp_ctx_t cmp; 
  cmp_init(&cmp, ptr, reader, writer);

  cmp_write_sint(&cmp, in->luid);
  {
    stor_fstat_rsp_t *obj;
    int i; 
    fixarray_zip(in->rfiles);
    cmp_write_array(&cmp, fixarray_num(in->rfiles));
    for (i = 0; i < fixarray_num(in->rfiles); i++){
        if(fixarray_get(in->rfiles, i, (void **)&obj) != 0){
            return -1;
        }
      cmp_write_sint(&cmp, obj->fid);
      if(obj->fname == NULL){
          cmp_write_str(&cmp, obj->fname, 0);
      }else{
          cmp_write_str(&cmp, obj->fname, strlen(obj->fname));
      }

      cmp_write_sint(&cmp, obj->folder);
     cmp_write_sint(&cmp, obj->type);
      cmp_write_sint(&cmp, obj->fatime);
      cmp_write_sint(&cmp, obj->fmtime);
      cmp_write_sint(&cmp, obj->fctime);
      cmp_write_sint(&cmp, obj->fsize);
      cmp_write_bin(&cmp, obj->snap, obj->snap_len);
      if(obj->origin == NULL){
          cmp_write_str(&cmp, obj->origin, 0);
      }else{
          cmp_write_str(&cmp, obj->origin, strlen(obj->origin));
      }

  }
 }

    return 0;
}

int stor_lookup_next_rsp_unmash(void *ptr, cmp_reader reader, cmp_writer writer, struct _stor_lookup_next_rsp **out){
  struct _stor_lookup_next_rsp *o;
  cmp_ctx_t cmp; 

  cmp_init(&cmp, ptr, reader, writer);
  o = (struct _stor_lookup_next_rsp*)osapi_malloc(sizeof(*o));
  if(o == NULL) return -ENOMEM; 
  memset(o, 0, sizeof(*o)); 

  cmp_read_long(&cmp,&o->luid);
  {
    stor_fstat_rsp_t *obj;
    uint32_t size;
    int i;
    cmp_read_array(&cmp, &size);
    fixarray_create(size, &o->rfiles);
    for (i = 0; i < size; i++){
      obj = osapi_malloc(sizeof(*obj));
      if(obj == NULL) {
        osapi_free(o);
        return -ENOMEM; 
  }
  cmp_read_long(&cmp,&obj->fid);

{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      obj->fname = NULL; 
}else{
      obj->fname = (char*)osapi_malloc(size+1);
      if(obj->fname == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, obj->fname, size);
      obj->fname[size] = 0;
  }
}

  cmp_read_long(&cmp,&obj->folder);
   cmp_read_int(&cmp, &obj->type);
  cmp_read_long(&cmp,&obj->fatime);
  cmp_read_long(&cmp,&obj->fmtime);
  cmp_read_long(&cmp,&obj->fctime);
  cmp_read_long(&cmp,&obj->fsize);
  cmp_read_bin_size(&cmp, &obj->snap_len);
  if(obj->snap_len == 0){
      obj->snap = NULL;  }else{
      obj->snap = osapi_malloc(obj->snap_len);
      if(obj->snap == NULL){ 
            return -ENOMEM;
        }
      reader(ptr, obj->snap, obj->snap_len);
  }

{
  uint32_t size;
  cmp_read_str_size(&cmp, &size);
  if(size == 0){
      obj->origin = NULL; 
}else{
      obj->origin = (char*)osapi_malloc(size+1);
      if(obj->origin == NULL){
          osapi_free(o); 
          return -ENOMEM;
      }
      reader(&cmp, obj->origin, size);
      obj->origin[size] = 0;
  }
}

      fixarray_set(o->rfiles, i, obj);
  }
 }


    *out = o;
    return 0;
}

void stor_lookup_next_rsp_free(struct _stor_lookup_next_rsp *ptr){
    if(ptr->rfiles != NULL){
        fixarray_destroy(ptr->rfiles, (fixarray_itemfree)stor_fstat_rsp_free);
        ptr->rfiles = NULL;
    }

    osapi_free(ptr);
}

/* constants */

