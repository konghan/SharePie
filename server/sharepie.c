
#include "sharepie.h"
#include "osapi.h"
#include "logger.h"

#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>

#ifndef __MACH__
#include <sys/sysinfo.h>
#endif

static const char *rdbs = "/stor.db";
static const char *rdbu = "/user.db";

#ifdef ANDROID_NDK
static int get_nprocs(){
	return 1;
}
#endif

#ifdef __MACH__
static int get_nprocs(){
	return 1;
}
#endif

int spie_init(sharepie_t *sp){
    memset(sp, 0, sizeof(*sp));
    
    sp->port = SHAREPIE_DEFAULT_PORT;
    
    return 0;
}

int spie_fini(sharepie_t *sp){
    
    if(sp->rdir != NULL){
        osapi_free(sp->rdir);
        sp->rdir = NULL;
    }

    sp->status = 0;

    return 0;
}

int spie_set_param(sharepie_t *sp, const char *rdir, short port){
    int len;

    sp->port = port;

    len = strlen(rdir) + 1;
    sp->rdir = osapi_malloc(len);
    if(sp->rdir == NULL){
    	log_warn("malloc memory for spie rdir fail\n");
        return -ENOMEM;
    }
    strcpy(sp->rdir, rdir);

    return 0;
}

static int spie_daemon(sharepie_t *sp, int nochdir, int  noclose)
{
   pid_t  pid;
   int sockets[2];

   if(!nochdir && chdir("/") != 0){
       log_warn("change dir fail\n");
       return -1;
   }

   if(!noclose)
   {
     int  fd = open("/dev/null", O_RDWR);

     if(fd < 0){
         log_warn("open /dev/null for redirect stdin fail\n");
         return -1;
     }

     if(dup2(fd, 0) < 0 || dup2( fd, 1 ) < 0 || dup2( fd, 2 ) < 0) 
     {
         log_warn("dup stdin stdout stderr fail\n");
         close(fd);
         return -1;
     }

     close(fd);
  }

   if(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) != 0){
       log_warn("socketpair fail\n");
       return -1;
   }

   sp->sock_daemon = sockets[0];
   sp->sock_spie   = sockets[1];

   pid = fork();
   if(pid < 0){
       log_warn("for daemon process fail\n");
       return -1;
   }

   if (pid > 0){
       // in parent process
       close(sp->sock_spie);
       return 0;
   }

   // in child process
   close(sp->sock_daemon);

   log_info("start sharepie daemon ...\n");

   if (setsid() < 0){
       log_warn("setsid in daemon fail!\n");
       return -1;
   }

   return 1;
}


int spie_start(sharepie_t *sp, int bg){
    int rc;

    if(sp->rdir == NULL){
        log_warn("before start sharepie, you need set rdir\n");
        return -1;
    }

    if(sp->status != 0){
        log_warn("init spie first\n");
        return -1;
    }
 
    if(bg){
        rc = spie_daemon(sp, 1, 1);
        switch(rc){
        case 0:
//            log_info("share pie is run in background\n");
            sp->status = 1;
            return 0;

        case 1:
//            log_info("share pie daemon is running...\n");
            break;

        default:
            log_warn("start share pie fail:%d\n", rc);
            return rc;
        }
    }

    // in daemon process if bg == 1

    rc = server_init(&sp->server, sp->sock_spie, 2*get_nprocs());
    if(rc != 0){
        log_warn("server init fail:%d\n", rc);
        return -1;
    }

    rc = dbrepo_init(&sp->server.repo, sp->rdir, rdbs, rdbu);
    if(rc != 0){
        log_warn("repo init fail:%s %s %s - %d\n", sp->rdir, rdbs, rdbu, rc);
        return -1;
    }

//    log_info("server is looping ...\n");

    server_start(&sp->server, sp->port);

    log_info("server is exit...\n");
    dbrepo_fini(&sp->server.repo);

    server_fini(&sp->server);

    return 0;
}

static int spie_request(sharepie_t *sp, int cmd){
    spie_cmdreq_t req;
    int rc;

    req.cmd = (unsigned char)cmd;
    req.pad = 0;
    req.param = 0;

    rc = send(sp->sock_daemon, &req, sizeof(req), 0);
    if(rc != sizeof(req))
        return -1;

    return 0;
}

static int spie_respone(sharepie_t *sp, spie_cmdreq_t *rsp){
    struct timeval tv;
    int rc;

    tv.tv_sec = 0;
    tv.tv_usec = 100000;    // 100ms
    setsockopt(sp->sock_daemon, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    rc = recv(sp->sock_daemon, &rsp, sizeof(*rsp), 0);
    if(rc < sizeof(*rsp)){
        return -1;
    }
    
    return 0;
}

int spie_status(sharepie_t *sp){
    spie_cmdreq_t rsp;
    int rc;

    rc = spie_request(sp, SPIE_STATUS);
    if(rc != 0)
        return -1;

    do{
        rc = spie_respone(sp, &rsp);
        if(rc != 0)
            break;

        if(rsp.cmd == SPIE_STATUS)
            return 0;

    }while(1);

    return -1;
}

int spie_stop(sharepie_t *sp){
    spie_cmdreq_t rsp;
    int rc;

    log_info("stop sharepie daemon ...\n");
    
    rc = spie_request(sp, SPIE_STOP);
    if(rc == 0){
        sp->status = 0;
    }

    rc = spie_respone(sp, &rsp);

    return rc;
}

int spie_restart(sharepie_t *sp, int bg){
    int rc;

    rc = spie_stop(sp);
    if(rc != 0)
        return rc;

    return spie_start(sp, bg);
}

int spie_clean(sharepie_t *sp){
    int rc;

    rc = spie_stop(sp);
    if(rc != 0)
        return rc;

    rc = dbrepo_clean(&sp->server.repo);

    return rc;
}

