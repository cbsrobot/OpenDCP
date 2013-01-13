#include <string.h>
#include <stdlib.h>
#include "opendcp.h"
#include "opendcp_decoder.h"
#include "opendcp_decoder_tif.c"

int opendcp_decode_none(odcp_image_t **image_ptr, const char *infile) {
    UNUSED(image_ptr);
    UNUSED(infile);

    return OPENDCP_NO_ERROR;
}

opendcp_decoder_t opendcp_decoders[] = {
    FOREACH_OPENDCP_DECODER(GENERATE_DECODER_STRUCT)
};

opendcp_decoder_t *opendcp_find_decoder(char *name) {
    int x;

    for (x = 0; x < OPENDCP_DECODER_NONE; x++) {
        if (!strncasecmp(opendcp_decoders[x].name, name, 3)) {
            return &opendcp_decoders[x];
        }
    }
    return &opendcp_decoders[OPENDCP_DECODER_NONE];
}
