#include "dlt.h"
#include <assert.h>

int main()
{
    dlt_context_t * ctx = dlt_create_context();
    assert (ctx != 0 && "ctx is null");
    assert (ctx->handle != 0 && "ctx->handle is null");
    dlt_destroy_context(ctx);
    return 0;
}