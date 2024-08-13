#ifndef __DLT_VERSION_H__
#define __DLT_VERSION_H__

#include <stdint.h>

#define DLT_VERSION "1.0.0"
#define DLT_VERSION_MAJOR 1
#define DLT_VERSION_MINOR 0
#define DLT_VERSION_PATCH 0

const char * dlt_get_version(uint32_t * major, uint32_t * minor, uint32_t * patch);

#endif /* __DLT_VERSION_H__ */
