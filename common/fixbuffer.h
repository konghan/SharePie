/*
 * Copyright (c) 2014, Konghan. All rights reserved.
 * Distributed under the GNU v3 license, see the LICENSE file.
 */

#ifndef __FIXBUFFER_H__
#define __FIXBUFFER_H__

#include <stdio.h>

#define FIXBUFFER_DEFAULT_BUFFER_LEN    4096

typedef struct _fixbuffer{
    void *buffer;
    size_t len;
    size_t start;
    size_t end;
}fixbuffer_t;

size_t fixbuffer_read(fixbuffer_t *fb, void *data, size_t len);
size_t fixbuffer_write(fixbuffer_t *fb, const void *data, size_t len);

fixbuffer_t *fixbuffer_new();
void fixbuffer_free(fixbuffer_t *fb);

// reserve continue space and return addree in ptr
int fixbuffer_reserved(fixbuffer_t *fb, size_t len, void **ptr);
size_t fixbuffer_length(fixbuffer_t *fb);

size_t fixbuffer_copy(fixbuffer_t *fb, void *buffer, size_t len);
int fixbuffer_drain(fixbuffer_t *fb, size_t len);

size_t fixbuffer_read_sock(fixbuffer_t *fb, int sock, size_t len);

static inline void *fixbuffer_start(fixbuffer_t *fb){
    return fb->buffer + fb->start;
}

#endif // __FIXBUFFER_H__

