#ifndef __DLT_PRIV_H__
#define __DLT_PRIV_H__

#include "dlt.h"

#define DLT_MAX_ERR_DESC 128

struct dlt_handle
{
    int last_error_code;
    char last_error_description[DLT_MAX_ERR_DESC];
};

#endif /* __DLT_PRIV_H__ */