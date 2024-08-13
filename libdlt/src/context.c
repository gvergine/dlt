#include "dlt_priv.h"

#include <stdlib.h>
#include <string.h>

dlt_context_t * dlt_create_context()
{
    dlt_context_t * ctx = malloc(sizeof(dlt_context_t));
    ctx->handle = malloc(sizeof(struct dlt_handle));
    ctx->handle->last_error_code = 0;
    memset(ctx->handle->last_error_description,0,DLT_MAX_ERR_DESC);
    return ctx;
}

void dlt_destroy_context(dlt_context_t * ctx)
{
    free(ctx->handle);
    free(ctx);
}
