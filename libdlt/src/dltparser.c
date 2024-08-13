#include "dlt.h"

#include <stdlib.h>
#include <string.h>
#include <endian.h>

#include <stdio.h>

static const uint8_t DLT_PATTERN[] = {0x44, 0x4C, 0x54, 0x01};
static const uint8_t DLT_PATTERN_LEN = 4;

#define CHECK(x,y,z,e)     if (x + y > z) return e;
#define DLT_MAX_ERR_DESC 128
#define REPORT_ERROR(x,y)  ctx->handle->last_error_code = (x); \
                           strncpy(ctx->handle->last_error_description,#y"\0",DLT_MAX_ERR_DESC)

struct dlt_handle
{
    int last_error_code;
    char last_error_description[DLT_MAX_ERR_DESC];
};

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

int dlt_parser_errno(dlt_context_t * ctx)
{
    return ctx->handle->last_error_code;
}

char* dlt_parser_error(dlt_context_t * ctx)
{
    return ctx->handle->last_error_description;
}


int dlt_parser_read_message(dlt_context_t * ctx, void * ptr, dlt_message_t * dlt_msg, size_t len)
{
    if (sizeof(dlt_storage_header_t) + sizeof(dlt_standard_header_t) > len) {
        //printf("will not be able to read even the headers - i have only %d left\n",len);
        return -1;
    }

    dlt_msg->storage_header = ptr;
    dlt_msg->standard_header = (void*)dlt_msg->storage_header + sizeof(dlt_storage_header_t);
    uint16_t message_len = be16toh(dlt_msg->standard_header->len);

    //REPORT_ERROR(1,Not implemented);

    int lenght_needed = sizeof(dlt_storage_header_t) + message_len;
    if (lenght_needed > len) {
       // printf("will not be able to read it all - i have only %d left, i need %d\n",len, lenght_needed);
        //printf("first unread byte: %x\n",*((uint8_t*)ptr+lenght_needed));
        return -1;
    }


    // advance to the optional fields
    size_t optional_std_header_fields_offset =  sizeof(dlt_msg->standard_header->htyp) + sizeof(dlt_msg->standard_header->mcnt) + sizeof(dlt_msg->standard_header->len);
    uint8_t * next_ptr = (uint8_t*)dlt_msg->standard_header + optional_std_header_fields_offset;
    //printf("%p %c\n", next_ptr, *((uint8_t*)next_ptr));

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

    if (dlt_msg->extended_header->msin.u.verb) // verbose payload
    {
        uint8_t i = 0;
        for (i = 0; i < dlt_msg->extended_header->noar; i++)
        {
            dlt_msg->payload_verbose.arguments[i].type_info.data = (uint32_t*)next_ptr;
            next_ptr += sizeof(uint32_t);
            dlt_msg->payload_verbose.arguments[i].data = next_ptr;
            

        }
    }
    else // non-verbose payload
    {
        //printf("/// %d ///\n",sizeof(dlt_extended_header_t));
        dlt_msg->payload_non_verbose.message_id = dlt_msg->standard_header->htyp.u.msbf ? be32toh(*(uint32_t*)next_ptr) : le32toh(*(uint32_t*)next_ptr);
        next_ptr += sizeof(uint32_t);
        dlt_msg->payload_non_verbose.data = next_ptr;
        dlt_msg->payload_non_verbose.data_len = message_len 
                        - sizeof(dlt_standard_header_t) \
                        - (dlt_msg->standard_header->htyp.u.ueh ? sizeof(dlt_extended_header_t) : 0)
                        - (dlt_msg->standard_header->htyp.u.weid ? sizeof(uint8_t[4]) : 0) 
                        - (dlt_msg->standard_header->htyp.u.wsid ? sizeof(uint32_t) : 0)
                        - (dlt_msg->standard_header->htyp.u.wtms ? sizeof(uint32_t) : 0)
                        - sizeof(uint32_t); // the message_id

    }

    return lenght_needed;
}