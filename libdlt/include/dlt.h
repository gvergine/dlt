#ifndef __LIBDLT_H__
#define __LIBDLT_H__

#include <stddef.h>
#include <stdint.h>
#include <endian.h>

#define DLT_VERSION "1.0.0"
#define DLT_VERSION_MAJOR 1
#define DLT_VERSION_MINOR 0
#define DLT_VERSION_PATCH 0

#pragma pack(push, 1)
typedef struct dlt_timestamp {
    uint32_t seconds;
    int32_t microseconds;
} dlt_timestamp_t;

typedef struct dlt_storage_header {
    uint8_t dlt_pattern[4];
    dlt_timestamp_t timestamp;
    uint8_t ecu_id[4];
} dlt_storage_header_t;

typedef union {
    struct {
        uint8_t ueh : 1;
        uint8_t msbf : 1;
        uint8_t weid : 1;
        uint8_t wsid : 1;
        uint8_t wtms : 1;
        uint8_t vers : 3;
    } u;
    uint8_t data;
} htyp_t;


typedef struct dlt_standard_header {
    htyp_t htyp;
    uint8_t mcnt;
    uint16_t len;
} dlt_standard_header_t;

typedef union {
    struct {
        uint8_t verb : 1;
        uint8_t mstp : 3; // based on this
        uint8_t mtin : 4; // this has different meanings
    } u;
    uint8_t data;
} msin_t;

typedef struct dlt_extended_header {
    msin_t msin;
    uint8_t noar; // shall be 0 if msin.u.verb is 0
    uint8_t apid[4];
    uint8_t ctid[4];
} dlt_extended_header_t;

typedef union {
    struct {
        uint8_t tyle : 4;
        uint8_t bool : 1;
        uint8_t sint : 1;
        uint8_t uint : 1;
        uint8_t floa : 1;
        uint8_t aray : 1;
        uint8_t strg : 1;
        uint8_t rawd : 1;
        uint8_t vari : 1;
        uint8_t fixp : 1;
        uint8_t trai : 1;
        uint8_t stru : 1;
        uint8_t scod : 3;
        uint16_t reserved : 14;
     } u;
    uint32_t data;
} type_info_t;

typedef struct dlt_payload_non_verbose {
    uint32_t message_id;
    uint8_t * data;
    uint16_t data_len;
} dlt_payload_non_verbose_t;

typedef struct dlt_payload_argument {
    type_info_t type_info;
    uint16_t data_len;
    uint8_t * data;
} dlt_payload_argument_t;

typedef struct dlt_payload_verbose {
    dlt_payload_argument_t arguments[255]; // because noar is  uint8_t
} dlt_payload_verbose_t;
#pragma pack(pop)

typedef struct dlt_message {
    dlt_storage_header_t* storage_header;
    dlt_standard_header_t* standard_header;
    uint8_t * sh_ecu; //lenght is 4 // optional depending on htyp.u.weid
    uint32_t * sh_seid; // optional depending on htyp.u.wsid
    uint32_t * sh_tmsp; // optional depending on htyp.u.wtms
    dlt_extended_header_t * extended_header; // optional depending on standard_header.htyp.u.ueh;
    dlt_payload_non_verbose_t payload_non_verbose;
    dlt_payload_verbose_t payload_verbose;
} dlt_message_t;

typedef struct dlt_handle dlt_handle_t;

typedef struct dlt_context {
    dlt_handle_t * handle;
    void * user;
} dlt_context_t;

dlt_context_t * dlt_create_context();
void dlt_destroy_context(dlt_context_t * ctx);

int dlt_errno(dlt_context_t * ctx);
char* dlt_error(dlt_context_t * ctx);
const char * dlt_get_version(uint32_t * major, uint32_t * minor, uint32_t * patch);

int dlt_parser_read_message(dlt_context_t * ctx, void * ptr, dlt_message_t * dlt_msg, size_t len);

#endif /* __LIBDLT_H__ */
