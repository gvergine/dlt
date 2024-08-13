#include "dlt_priv.h"

#include <stdlib.h>
#include <string.h>
#include <endian.h>

static const uint8_t DLT_PATTERN[] = {0x44, 0x4C, 0x54, 0x01};
static const uint8_t DLT_PATTERN_LEN = 4;

#define CHECK(x,y,z,e)     if (x + y > z) return e;
#define REPORT_ERROR(x,y)  ctx->handle->last_error_code = (x); \
                           strncpy(ctx->handle->last_error_description,#y"\0",DLT_MAX_ERR_DESC)




int dlt_parser_read_message(dlt_context_t * ctx, void * ptr, dlt_message_t * dlt_msg, size_t len)
{
    if (sizeof(dlt_storage_header_t) + sizeof(dlt_standard_header_t) > len) {
        REPORT_ERROR(1,Not enough buffer to read the headers);
        return -1;
    }

    dlt_msg->storage_header = ptr;
    dlt_msg->standard_header = (void*)dlt_msg->storage_header + sizeof(dlt_storage_header_t);
    uint16_t message_len = be16toh(dlt_msg->standard_header->len);


    int lenght_needed = sizeof(dlt_storage_header_t) + message_len;
    if (lenght_needed > len) {
        REPORT_ERROR(2,Not enough buffer to read it completely);
        return -1;
    }

    size_t optional_std_header_fields_offset =  sizeof(dlt_msg->standard_header->htyp) + sizeof(dlt_msg->standard_header->mcnt) + sizeof(dlt_msg->standard_header->len);
    uint8_t * next_ptr = (uint8_t*)dlt_msg->standard_header + optional_std_header_fields_offset;

    if (dlt_msg->standard_header->htyp.u.weid)
    {
        dlt_msg->sh_ecu = next_ptr;
        next_ptr += sizeof(uint8_t[4]);
    }

    if (dlt_msg->standard_header->htyp.u.wsid)
    {
        dlt_msg->sh_seid = (uint32_t *)next_ptr;
        next_ptr += sizeof(uint32_t);
    }

    if (dlt_msg->standard_header->htyp.u.wtms)
    {
        dlt_msg->sh_tmsp = (uint32_t *)next_ptr;
        next_ptr += sizeof(uint32_t);
    }

    if (dlt_msg->standard_header->htyp.u.ueh)
    {
        dlt_msg->extended_header = (dlt_extended_header_t *)next_ptr;
        next_ptr += sizeof(dlt_extended_header_t);
    }


    if (dlt_msg->standard_header->htyp.u.ueh && dlt_msg->extended_header->msin.u.verb) // verbose payload
    {
        uint8_t i = 0;
        for (i = 0; i < dlt_msg->extended_header->noar; i++)
        {
            dlt_msg->payload_verbose.arguments[i].type_info.data = (uint32_t*)next_ptr;
            next_ptr += sizeof(uint32_t);
            dlt_msg->payload_verbose.arguments[i].data = next_ptr;
            

        }
    }
    else if (!dlt_msg->standard_header->htyp.u.ueh || !dlt_msg->extended_header->msin.u.verb) // non-verbose payload
    {
        dlt_msg->payload_non_verbose.message_id = dlt_msg->standard_header->htyp.u.msbf ? be32toh(*(uint32_t*)next_ptr) : le32toh(*(uint32_t*)next_ptr);
        next_ptr += sizeof(uint32_t);
        dlt_msg->payload_non_verbose.data = next_ptr;
        dlt_msg->payload_non_verbose.data_len = message_len 
                        - sizeof(dlt_standard_header_t) \
                        - (dlt_msg->standard_header->htyp.u.ueh ? sizeof(dlt_extended_header_t) : 0)
                        - (dlt_msg->standard_header->htyp.u.weid ? sizeof(uint8_t[4]) : 0) 
                        - (dlt_msg->standard_header->htyp.u.wsid ? sizeof(uint32_t) : 0)
                        - (dlt_msg->standard_header->htyp.u.wtms ? sizeof(uint32_t) : 0)
                        - sizeof(uint32_t);

    }

    return lenght_needed;
}