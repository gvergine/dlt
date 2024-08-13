#include "dlt_version.h"

const char * dlt_get_version(uint32_t * major, uint32_t * minor, uint32_t * patch)
{
    if (major != 0) *major = DLT_VERSION_MAJOR;
    if (minor != 0) *minor = DLT_VERSION_MINOR;
    if (patch != 0) *patch = DLT_VERSION_PATCH;
    return DLT_VERSION;
}
