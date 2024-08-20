#include "dlt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void print_four_chars(uint8_t chars[4]) {
    printf("%c%c%c%c\t",*(chars),*(chars+1),*(chars+2),*(chars+3));
}

void print_byte_array(uint8_t * array, uint16_t len) {
    int i = 0;
    for (i = 0; i < len; i++)
        printf(" %02x",*(array+i));
    printf("\t");
}


void print_message(dlt_message_t * dlt_msg) {
    printf("%c%c%c%x\t",dlt_msg->storage_header->dlt_pattern[0],
                        dlt_msg->storage_header->dlt_pattern[1],
                        dlt_msg->storage_header->dlt_pattern[2],
                        dlt_msg->storage_header->dlt_pattern[3]);
    printf("%d.%d\t",be32toh(dlt_msg->storage_header->timestamp.seconds),
                     be32toh(dlt_msg->storage_header->timestamp.microseconds));
    printf("%s\t",dlt_msg->storage_header->ecu_id);
    printf("%d\t",dlt_msg->standard_header->mcnt);
    uint16_t message_len = be16toh(dlt_msg->standard_header->len);
    printf("%d\t",message_len);

    if (dlt_msg->standard_header->htyp.u.weid)
        printf("%s\t",dlt_msg->sh_ecu);
    if (dlt_msg->standard_header->htyp.u.wsid)
        printf("%d\t",be32toh(*dlt_msg->sh_seid));
    if (dlt_msg->standard_header->htyp.u.wtms)
        printf("%d\t",be32toh(*dlt_msg->sh_tmsp));

    if (dlt_msg->standard_header->htyp.u.ueh)
    {
        printf("%d\t",dlt_msg->extended_header->msin.data);
        printf("%d\t",dlt_msg->extended_header->noar);
        print_four_chars(dlt_msg->extended_header->apid);
        print_four_chars(dlt_msg->extended_header->ctid);
    }

    if (dlt_msg->standard_header->htyp.u.ueh && dlt_msg->extended_header->msin.u.verb) // verbose payload
    {
        printf("%d\t",dlt_msg->extended_header->noar);

    }
    else if (!dlt_msg->standard_header->htyp.u.ueh || !dlt_msg->extended_header->msin.u.verb) // non-verbose payload
    {
        printf("%d\t",dlt_msg->payload_non_verbose.message_id);
        print_byte_array(dlt_msg->payload_non_verbose.data, dlt_msg->payload_non_verbose.data_len);

    }

    printf("\n");
}

// must return the position of the first unparsed byte
int parse_read_buffer(dlt_context_t * ctx, void * ptr, size_t maxlen)
{
    int dlt_ret = 0;
    uint8_t * dlt_ptr = ptr;
    dlt_message_t dlt_message;
    int offset = 0;

    while ((dlt_ret = dlt_parser_read_message(ctx, dlt_ptr + offset, &dlt_message, maxlen-offset)) > 0)
    {
        print_message(&dlt_message);
        ctx->user++;
        // bytes parsed in this cycle printf("parsed %d so far...\n",dlt_ret);
        offset += dlt_ret;
        if (offset == maxlen) break;
    }

    if (dlt_ret < 0) {
        fprintf(stderr,"dlt_parser_read_message fail - errno %d: %s\n",dlt_errno(ctx), dlt_error(ctx));
    }
 
    return offset;
}

#define CHUNK_BUF_LEN 1024 // 1 Kb

int main(int argc, char* argv[]) {

    uint8_t* chunk_buffer = malloc(CHUNK_BUF_LEN);
    int fd;
    size_t bytes_read;
    int offset = 0;
    struct stat fd_stat;

    if ((fd = open(argv[1], O_RDONLY)) <= 0 )
    {
        perror (argv[1]);
        return EXIT_FAILURE;
    }

    fstat(fd, &fd_stat);
    //printf("file size: %ld\n",fd_stat.st_size);
    size_t total_bytes_read = 0;
    int previous_percent = 0;
    
    dlt_context_t * ctx = dlt_create_context();
    ctx->user = 0;

    while ((bytes_read = read(fd, chunk_buffer + offset, CHUNK_BUF_LEN - offset)) > 0)
    {
        total_bytes_read += bytes_read;
        int percent = (int)((total_bytes_read * 100.0f)/fd_stat.st_size);
        if (previous_percent < percent) 
        {
            //printf("%d%%\n",percent);
            previous_percent = percent;
        }

        // pos must be the position of the first unparsed byte
        int pos = parse_read_buffer(ctx, chunk_buffer, offset + bytes_read);
 
        // len is the lenght of the remaning unparsed bytes
        int len = offset + bytes_read - pos;
 
        // copy the remaining unparsed bytes to the beginning of the buffer and adjust the offset
        memmove(chunk_buffer, chunk_buffer+pos, len);
        offset = len;
    }
    //printf("parsed %d messages",(int)ctx->user);
    dlt_destroy_context(ctx);
    close(fd);
    free(chunk_buffer);
    return 0;
}