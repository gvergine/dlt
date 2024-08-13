#include "dlt.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

int main()
{
    uint32_t major, minor, patch = 0;
    const char * strversion = dlt_get_version(0,0,0);
    assert(strncmp(DLT_VERSION,strversion,strlen(DLT_VERSION))==0 && "dlt_get_version(0,0,0) not returning " DLT_VERSION);
    dlt_get_version(&major, &minor, &patch);
    printf("%d %d %d - %d %d %d", major, minor, patch, DLT_VERSION_MAJOR, DLT_VERSION_MINOR, DLT_VERSION_PATCH);
    assert(major ==  DLT_VERSION_MAJOR && "dlt_get_version major != " "#DLT_VERSION_MAJOR");
    assert(minor ==  DLT_VERSION_MINOR && "dlt_get_version minor != " "#DLT_VERSION_MINOR");
    assert(patch ==  DLT_VERSION_PATCH && "dlt_get_version patch != " "#DLT_VERSION_PATCH");
    
    dlt_context_t * ctx = dlt_create_context();
    assert (ctx != 0 && "ctx is null");
    assert (ctx->handle != 0 && "ctx->handle is null");
    dlt_destroy_context(ctx);
    return 0;
}