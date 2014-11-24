/*
 * Copyright (c) 2013, Konghan. All rights reserved.
 * Distributed under the BSD license, see the LICENSE file.
 */

#include "osapi.h"

#include <string.h>
#include <strings.h>

char *osapi_strdup(const char *str, int withspace){
    int len = withspace + 1;
    char *dup;

    if(str != NULL)
        len += strlen(str);

    dup = osapi_malloc(len);
    if(dup != NULL){
        memset(dup, 0, len);

        if(str != NULL)
            strcpy(dup, str);
    }

    return dup;
}

int osapi_is_inpath(const char *rpath, const char *file){
    if(strstr(file, rpath) == NULL)
        return 1;

    return 0;
}

int osapi_is_filename(const char *file){
    if(strchr(file, '/') == NULL)
        return 1;

    return 0;
}


