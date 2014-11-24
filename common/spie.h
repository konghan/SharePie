
#ifndef __SPIE_H__
#define __SPIE_H__

#include <stdint.h>

#define SPIE_ALIGN(_ap,_as)   ((_ap)+(_as)-1)&(~((_as)-1))

typedef enum{
    //SPIE_CMD_RESET = 0,

    SPIE_CMD_TEMPLOGIN = 0,
    SPIE_CMD_TEMPADD,

    SPIE_CMD_LOGIN,

    SPIE_CMD_ADDUSER,
    SPIE_CMD_DELUSER,
    SPIE_CMD_LISTUSER,
    SPIE_CMD_UPDATEUSER,
    
    SPIE_CMD_SYSINFO,

    // lookup
    SPIE_CMD_LOOKUP_CREATE,
    SPIE_CMD_LOOKUP_DESTROY,
    SPIE_CMD_LOOKUP_NEXT,

    // dset operations
    SPIE_CMD_FSTAT,
    SPIE_CMD_FOPEN,
    SPIE_CMD_FCLOSE,
    SPIE_CMD_FREAD,
    SPIE_CMD_FWRITE,
    SPIE_CMD_FTRUNCATE,
    SPIE_CMD_FREADSNAP,
    SPIE_CMD_FWRITESNAP,

    // dset operations
    SPIE_CMD_FMOVETO,
    SPIE_CMD_FCREATE,
    SPIE_CMD_FDELETE,
    
}spie_cmd_t;

enum {
    SPIE_ECODE_SUCCESS = 0,
    SPIE_ECODE_INVAL,
    SPIE_ECODE_NOENT,
    SPIE_ECODE_EXIST,
    SPIE_ECODE_EOF,
    SPIE_ECODE_MEDIAERR,
    SPIE_ECODE_FOLDERNOTEXIST,
    SPIE_ECODE_NOMEM,
    SPIE_ECODE_DBERR,
    SPIE_ECODE_OPENED,
    SPIE_ECODE_NOTFILE,
    SPIE_ECODE_PRIORITY,
};

typedef enum{
    SPIE_FOLDER_VIDEO = 0,
    SPIE_FOLDER_MUSIC,
    SPIE_FOLDER_PHOTO,
    SPIE_FOLDER_DOCS,
}spie_rootfolder_t;

typedef enum{
    SPIE_TYPE_NATIVE = 0,   // files is in subdir
    SPIE_TYPE_ROOTFS,   // file is not in subdir, but in rootfs  
    SPIE_TYPE_FOLDER,   // not file, but folder
}spie_type_t;

typedef enum{
    SPIE_LUTYPE_NAME,
    SPIE_LUTYPE_SIZE,
    SPIE_LUTYPE_ATIME,
    SPIE_LUTYPE_CTIME,
}spie_lookup_type_t;

#define SPIE_PRIORITY_TEMP      1
#define SPIE_PRIORITY_NORMAL    10
#define SPIE_PRIORITY_MGMT      20

typedef struct _spie_header{
    uint16_t    size;
    uint8_t     cmdcode;    // command / response
    uint8_t     errcode;    // error code in response

    uint32_t    sesid;      // session id
    uint32_t    seqid;      // sequence id
    uint32_t    cred;       // cred for safety

}__attribute__((packed)) spie_header_t;

#define SPIE_HEADER_LEN sizeof(spie_header_t)

//
// misc functions
//
#if 0
static inline uint64_t ntoh64(const uint64_t input)
{
    uint64_t rval;
    uint8_t *data = (uint8_t *)&rval;

    data[0] = input >> 56;
    data[1] = input >> 48;
    data[2] = input >> 40;
    data[3] = input >> 32;
    data[4] = input >> 24;
    data[5] = input >> 16;
    data[6] = input >> 8;
    data[7] = input >> 0;

    return rval;
}

static inline uint64_t hton64(const uint64_t input)
{
    return (ntoh64(input));
}
#endif
int spie_header_dump(spie_header_t *header, int len);

#endif

