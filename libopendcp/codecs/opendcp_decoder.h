/* generate decoder information */
#define FOREACH_OPENDCP_DECODER(OPENDCP_DECODER) \
            OPENDCP_DECODER(OPENDCP_DECODER_BMP,  bmp)  \
            OPENDCP_DECODER(OPENDCP_DECODER_DPX,  dpx)  \
            OPENDCP_DECODER(OPENDCP_DECODER_TIFF, tif)  \
            OPENDCP_DECODER(OPENDCP_DECODER_NONE, none)

#define GENERATE_DECODER_ENUM(DECODER, NAME) DECODER,
#define GENERATE_DECODER_STRING(DECODER, NAME) #NAME,
#define GENERATE_DECODER_NAME(DECODER, NAME) #DECODER,
#define GENERATE_DECODER_STRUCT(DECODER, NAME) { DECODER, #NAME, opendcp_decode_ ## NAME },
#define GENERATE_DECODER_EXTERN(DECODER, NAME) extern int opendcp_decode_ ## NAME(odcp_image_t **image_ptr, const char *infile);

enum OPENDCP_DECODERS {
     FOREACH_OPENDCP_DECODER(GENERATE_DECODER_ENUM)
};

//static const char *OPENDCP_DECODER_NAME[] = {
//    FOREACH_OPENDCP_DECODER(GENERATE_DECODER_STRING)
//};

FOREACH_OPENDCP_DECODER(GENERATE_DECODER_EXTERN)

typedef struct {
    int  id;
    char *name;
    int  (*decode)(odcp_image_t **image_ptr, const char *infile);
} opendcp_decoder_t;

opendcp_decoder_t *opendcp_find_decoder(char *name);
