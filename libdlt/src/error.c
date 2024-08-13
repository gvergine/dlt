#include "dlt_priv.h"


int dlt_errno(dlt_context_t * ctx)
{
    return ctx->handle->last_error_code;
}

char* dlt_error(dlt_context_t * ctx)
{
    return ctx->handle->last_error_description;
}