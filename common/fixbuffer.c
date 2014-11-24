/*
 * Copyright (c) 2014, Konghan. All rights reserved.
 * Distributed under the GNU v3 license, see the LICENSE file.
 */


#include "fixbuffer.h"
#include "osapi.h"

#include <string.h>
#include <unistd.h>

#define FIXBUFFER_ALIGN(_ap,_as)   ((_ap)+(_as)-1)&(~((_as)-1))

size_t fixbuffer_read(fixbuffer_t *fb, void *data, size_t len){
    size_t copy;
    size_t data_len;

    data_len = fb->end - fb->start;

    copy = (data_len > len) ? len : data_len;

    memcpy(data, fb->buffer + fb->start, copy);
    fb->start += copy;
    
    // no data, reset
    if(fb->start == fb->end){
        fb->start = 0;
        fb->end = 0;
    }

    return copy;
}

int fixbuffer_expand(fixbuffer_t *fb, size_t len){
    void *buffer, *temp;
    size_t new_len;

    new_len = fb->len + len;
    new_len = FIXBUFFER_ALIGN(new_len, FIXBUFFER_DEFAULT_BUFFER_LEN);
    
    buffer = osapi_malloc(new_len);
    if(buffer == NULL){
        return -1;
    }
            
    memcpy(buffer, fb->buffer + fb->start, fb->end - fb->start);
    temp = fb->buffer;
    fb->buffer = buffer;
    fb->end = fb->end - fb->start;
    fb->start = 0;
    fb->len = new_len;

    osapi_free(temp);

    return 0;
}

size_t fixbuffer_write(fixbuffer_t *fb, const void *data, size_t len){
    void *buffer;
    size_t space;

    space = fb->len - fb->end;
    if(space < len){
        fixbuffer_expand(fb, len - space);
    }

    space = fb->len - fb->end;
    buffer = fb->buffer + fb->end;
    if(space > len)
        space = len;
    
    memcpy(buffer, data, space);
    fb->end += space;

    return space;
}

fixbuffer_t *fixbuffer_new(){
    fixbuffer_t *fb;

    fb = osapi_malloc(sizeof(*fb));
    if(fb == NULL){
        return NULL;
    }
    fb->len = FIXBUFFER_DEFAULT_BUFFER_LEN;
    fb->start = 0;
    fb->end = 0;
    
    fb->buffer = osapi_malloc(fb->len);
    if(fb->buffer == NULL){
        osapi_free(fb);
        return NULL;
    }

    return fb;
}

void fixbuffer_free(fixbuffer_t *fb){
    if(fb == NULL)
        return ;

    if(fb->buffer != NULL){
        osapi_free(fb->buffer);
        fb->buffer = NULL;
    }

    osapi_free(fb);
}

int fixbuffer_reserved(fixbuffer_t *fb, size_t len, void **ptr){
    void *header;
    size_t move;

    if(fb->start >= len){
        fb->start -= len;
        header = fb->buffer + fb->start;
        *ptr = header;
        return 0;
    }

    if((fb->start == 0) && (fb->end == 0)){
        header = fb->buffer;
        fb->end += len;
        *ptr = header;
        return 0;
    }

    move = len - fb->start;
    if((move + fb->end) > fb->len)
        return -1;

    memmove(fb->buffer + fb->start + move, fb->buffer + fb->start, fb->end - fb->start);
    header = fb->buffer;
    fb->start = 0;
    fb->end += move;

    *ptr = header;

    return 0;
}

size_t fixbuffer_length(fixbuffer_t *fb){
    return fb->end - fb->start;
}

size_t fixbuffer_copy(fixbuffer_t *fb, void *buffer, size_t len){
    size_t copy;

    copy = ((fb->end - fb->start) > len) ? len : fb->end - fb->start;

    memcpy(buffer, fb->buffer + fb->start, copy);

    return copy;
}

int fixbuffer_drain(fixbuffer_t *fb, size_t len){
    fb->start += len;

    if(fb->start >= fb->end){
        fb->start = 0;
        fb->end = 0;
    }

    return 0;
}

size_t fixbuffer_read_sock(fixbuffer_t *fb, int sock, size_t len){
    void *buffer;
    size_t space;

    space = fb->len - fb->end;
    if(space < len){
        fixbuffer_expand(fb, len - space);
    }

    buffer = fb->buffer + fb->end;
    space = fb->len - fb->end;

    if(space >= len)
        space = len;

    space = read(sock, buffer, space);
    if(space == (size_t)-1){
	return space;
    }

    fb->end += space;
    return space;
}

