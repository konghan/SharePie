
#include "session.h"
#include "osapi.h"
#include "spie.h"

#include "fixbuffer.h"

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int session_init(session_t *ses){
    assert(ses != NULL);

    memset(ses, 0, sizeof(*ses));

    ses->sock = socket(AF_INET, SOCK_STREAM, 0);
    if(ses->sock < 0){
        return -EINVAL;
    }

    return 0;
}

int session_fini(session_t *ses){
    assert(ses != NULL);

    close(ses->sock);

    return 0;
}

int session_connect(session_t *ses, struct sockaddr *addr, int len){
    int rc;

    assert((ses != NULL) && (addr != NULL));

    if(ses->status > 0)
        return 0;

   rc = connect(ses->sock, addr, len);
   if(rc != 0){
       return rc;
   }

   ses->status = 1;

   return 0;
}

int session_close(session_t *ses){
    assert(ses);

    if(ses->status < 1)
        return 0;

    close(ses->sock);

    memset(ses, 0, sizeof(*ses));

    return 0;
}

int session_setup(session_t *ses, const char *user, const char *cred){
    return 0;
}

int session_cleanup(session_t *ses){
    return 0;
}

int session_send(session_t *ses, fixbuffer_t *fb){
    spie_header_t *header = fixbuffer_start(fb);
    uint16_t size;

    assert(ses != NULL);

    if((fixbuffer_length(fb) - sizeof(*header)) > 0xffff){
        return -EINVAL;
    }
 
    size = fixbuffer_length(fb) - sizeof(*header);
    header->size = htons(size);
    header->errcode = 0;
    header->sesid = htonl(ses->sesid);
    header->seqid = htonl(ses->seqid++);
    header->cred  = htonl(ses->cred);

    return send(ses->sock, header, fixbuffer_length(fb), 0);
}

int session_recv(session_t *ses, fixbuffer_t **fbout, int *ecode){
    fixbuffer_t *fb;
    spie_header_t *header;
    size_t  size;
    int  lenr;

    assert((ses != NULL) && (fbout != NULL));

    if(ses->status <= 0){
	printf("##sessin is closed\n");
        return -EINVAL;
    }

    fb = fixbuffer_new();
    if(fb == NULL){
	    printf("session without enought memory\n");
        return -ENOMEM;
    }

    size = fixbuffer_read_sock(fb, ses->sock, sizeof(*header));
    if(size != sizeof(*header)){
	    printf("fixbuffer read sock data fail:%d\n", (int)size);
        return -EINVAL;
    }

    header = (spie_header_t *)fixbuffer_start(fb);

    if(ntohl(header->seqid) != ses->seqid_recv){
	    printf("sequence id wrong\n");
        // drop unseqenced rsp
        return -EINVAL;
    }
    ses->seqid_recv++;

    lenr = ntohs(header->size);
    if(lenr > 0){
        size = fixbuffer_read_sock(fb, ses->sock, lenr);
        if(size != lenr){
	    printf("fixbuffer_read_sock return :%d\n", (int)size);
            return -EINVAL;
        }
    }

    // fixbuffer_read_sock may expand
    header = (spie_header_t *)fixbuffer_start(fb);
    
    header->size = ntohs(header->size);
    header->seqid = ntohl(header->seqid);
    header->sesid = ntohl(header->sesid);
    header->cred  = ntohl(header->cred);

    *ecode = header->errcode;
    *fbout = fb;

    return 0;
}

