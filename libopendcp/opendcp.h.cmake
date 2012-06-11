/*
    OpenDCP: Builds Digital Cinema Packages
    Copyright (c) 2010-2011 Terrence Meiczinger, All Rights Reserved

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _OPENDCP_H_
#define _OPENDCP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <opendcp_image.h>

#define MAX_ASSETS          10   /* Soft limit */
#define MAX_REELS           30   /* Soft limit */
#define MAX_PKL             1    /* Soft limit */
#define MAX_CPL             5    /* Soft limit */
#define MAX_PATH_LENGTH     4096 
#define MAX_FILENAME_LENGTH 256 
#define MAX_BASENAME_LENGTH 256 

#define MAX_DCP_JPEG_BITRATE 250000000  /* Maximum DCI compliant bit rate for JPEG2000 */
#define MAX_DCP_MPEG_BITRATE  80000000  /* Maximum DCI compliant bit rate for MPEG */

#define MAX_WIDTH_2K        2048 
#define MAX_HEIGHT_2K       1080 

/* library information */
#define OPENDCP_NAME       "${OPENDCP_NAME}"
#define OPENDCP_VERSION    "${OPENDCP_VERSION}"
#define OPENDCP_COPYRIGHT  "${OPENDCP_COPYRIGHT}"
#define OPENDCP_LICENSE    "${OPENDCP_LICENSE}"
#define OPENDCP_URL        "${OPENDCP_URL}"

/* defaults */
#define DCP_ANNOTATION  "OPENDCP-FILM"
#define DCP_TITLE       "OPENDCP-FILM-TITLE"
#define DCP_KIND        "feature"

/* generate error message */
#define FOREACH_OPENDCP_ERROR_MSG(OPENDCP_ERROR_MSG) \
        OPENDCP_ERROR_MSG(OPENDCP_NO_ERROR,                "No error")  \
        OPENDCP_ERROR_MSG(OPENDCP_ERROR,                   "OpenDCP error")  \
        OPENDCP_ERROR_MSG(OPENDCP_FATAL,                   "OpenDCP fatal error")  \
        OPENDCP_ERROR_MSG(OPENDCP_FILEOPEN,                "Could not open file") \
        OPENDCP_ERROR_MSG(OPENDCP_INVALID_PICTURE_TRACK,   "Invalid picture track type") \
        OPENDCP_ERROR_MSG(OPENDCP_NO_PICTURE_TRACK,        "Reel does not contain a picture track") \
        OPENDCP_ERROR_MSG(OPENDCP_MULTIPLE_PICTURE_TRACK,  "Reel contains multiple picture tracks") \
        OPENDCP_ERROR_MSG(OPENDCP_INVALID_SOUND_TRACK,     "Invalid sound track type") \
        OPENDCP_ERROR_MSG(OPENDCP_NO_SOUND_TRACK,          "Reel does not contain a sound track") \
        OPENDCP_ERROR_MSG(OPENDCP_MULTIPLE_SOUND_TRACK,    "Reel contains multiple sound tracks") \
        OPENDCP_ERROR_MSG(OPENDCP_SPECIFICATION_MISMATCH,  "DCP contains MXF and SMPTE track") \
        OPENDCP_ERROR_MSG(OPENDCP_TRACK_NO_DURATION,       "Track has no duration") \
        OPENDCP_ERROR_MSG(OPENDCP_J2K_ERROR,               "JPEG2000 error") \
        OPENDCP_ERROR_MSG(OPENDCP_CALC_DIGEST,            "Could not caclulate MXF digest") \
        OPENDCP_ERROR_MSG(OPENDCP_DETECT_TRACK_TYPE,      "Could not determine MXF track type") \
        OPENDCP_ERROR_MSG(OPENDCP_INVALID_TRACK_TYPE,     "Invalid MXF track type") \
        OPENDCP_ERROR_MSG(OPENDCP_UNKOWN_TRACK_TYPE,      "Unknown MXF track type") \
        OPENDCP_ERROR_MSG(OPENDCP_FILEOPEN_MPEG2,         "Could not open MPEG2 file") \
        OPENDCP_ERROR_MSG(OPENDCP_FILEOPEN_J2K,           "Could not open JPEG200 file") \
        OPENDCP_ERROR_MSG(OPENDCP_FILEOPEN_WAV,           "Could not open wav file") \
        OPENDCP_ERROR_MSG(OPENDCP_FILEOPEN_TT,            "Could not open subtitle file") \
        OPENDCP_ERROR_MSG(OPENDCP_FILEWRITE_MXF,          "Could not write MXF file") \
        OPENDCP_ERROR_MSG(OPENDCP_FINALIZE_MXF,           "Could not finalize MXF file") \
        OPENDCP_ERROR_MSG(OPENDCP_PARSER_RESET,           "Could not reset MXF parser") \
        OPENDCP_ERROR_MSG(OPENDCP_STRING_LENGTH,          "Input files have differing file lengths") \
        OPENDCP_ERROR_MSG(OPENDCP_STRING_NOTSEQUENTIAL,   "Input files are not sequential") \
        OPENDCP_ERROR_MSG(OPENDCP_MAX_ERROR,               "Maximum error string")
 
#define GENERATE_ENUM(ERROR, STRING) ERROR,
#define GENERATE_STRING(ERROR, STRING) STRING,
#define GENERATE_NAME(ERROR, STRING) #ERROR,
#define GENERATE_STRUCT(ERROR, STRING) { ERROR, #ERROR, STRING },

enum OPENDCP_ERROR_MESSAGES {
    FOREACH_OPENDCP_ERROR_MSG(GENERATE_ENUM)
};

/* XML Namespaces */
extern const char *XML_HEADER;
extern const char *NS_CPL[];
extern const char *NS_PKL[];
extern const char *NS_AM[];
extern const char *NS_CPL_3D[];

/* digital signatures */
extern const char *DS_DSIG;  /* digital signature */
extern const char *DS_CMA;   /* canonicalization method */
extern const char *DS_DMA;   /* digest method */
extern const char *DS_TMA;   /* transport method */
extern const char *DS_SMA[]; /* signature method */

extern const char *RATING_AGENCY[]; 
extern const char *DCP_LOG[];

/* error messages */
extern const char *OPENDCP_ERROR_STRING[];
extern const char *OPENDCP_ERROR_NAME[];

typedef struct {
    int error;
    char *name;
    char *string;
} opendcp_error_t;

extern const opendcp_error_t OPENDCP_ERROR_T[];

typedef unsigned char byte_t; 

enum LOG_LEVEL {
    LOG_NONE = 0,
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG
};

enum ASSET_CLASS_TYPE {
    ACT_UNKNOWN,
    ACT_PICTURE,
    ACT_SOUND,
    ACT_TIMED_TEXT
};

enum CINEMA_PROFILE {
    DCP_CINEMA2K = 3,
    DCP_CINEMA4K = 4,
};

enum ASSET_ESSENCE_TYPE {
    AET_UNKNOWN,
    AET_MPEG2_VES,
    AET_JPEG_2000,
    AET_PCM_24b_48k,
    AET_PCM_24b_96k,
    AET_TIMED_TEXT,
    AET_JPEG_2000_S
};

enum XML_NAMESPACE {
    XML_NS_UNKNOWN,
    XML_NS_INTEROP,
    XML_NS_SMPTE
};

enum J2K_ENCODER {
    J2K_OPENJPEG,
    J2K_KAKADU
};

enum DPX_MODE {
    DPX_LINEAR = 0,
    DPX_FILM,
    DPX_VIDEO
};

typedef struct {
    char           **in;
    char           **out;
    int            file_count;
} filelist_t;

typedef struct {
    struct dirent  **entry;
    int            file_count;
} filelist2_t;

typedef struct {
    filelist_t *filelist;
    char       *output;
} mxf_writer_t;

typedef struct {
    char           filename[MAX_FILENAME_LENGTH];
} assetmap_t;

typedef struct {
    char           filename[MAX_FILENAME_LENGTH];
} volindex_t;

typedef struct {
    char           uuid[40];
    int            essence_class;
    int            essence_type;
    int            duration;
    int            intrinsic_duration;
    int            entry_point;
    int            xml_ns;
    int            stereoscopic;
    char           size[18];
    char           name[128];
    char           annotation[128];
    char           edit_rate[20];
    char           frame_rate[20];
    char           sample_rate[20];
    char           aspect_ratio[20];
    char           digest[40];
    char           filename[MAX_FILENAME_LENGTH];
} asset_t;

typedef struct {
    asset_t        asset_list[3];
    int            asset_count;
} asset_list_t;

typedef struct {
    char           uuid[40];
    char           annotation[128];
    int            asset_count; 
    asset_t        asset[MAX_ASSETS];
} reel_t;

typedef struct {
    char           uuid[40];
    char           annotation[128];
    char           size[18];
    char           digest[40];
    int            duration;
    int            entry_point;
    char           issuer[80];
    char           creator[80];
    char           title[80];
    char           timestamp[30];
    char           kind[15];
    char           rating[5];
    char           filename[MAX_FILENAME_LENGTH];
    int            reel_count;
    reel_t         reel[MAX_REELS]; 
} cpl_t;

typedef struct {
    char           uuid[40];
    char           annotation[128];
    char           size[18];
    char           issuer[80];
    char           creator[80];
    char           timestamp[30];
    char           filename[MAX_FILENAME_LENGTH];
    int            cpl_count;
    cpl_t          cpl[MAX_CPL]; 
} pkl_t;

typedef struct {
    int            start_frame;
    int            end_frame;
    int            encoder;
    int            no_overwrite;
    int            bw;
    int            duration;
    int            dpx;
    int            lut;
    int            xyz;
    int            xyz_method;
    int            resize;
} j2k_t;

typedef struct {
    int            start_frame;
    int            end_frame;
    int            duration;
    int            slide;
    int            encrypt_header_flag;
    int            key_flag;
    int            delete_intermediate;
    byte_t         key_id[16];
    byte_t         key_value[16];
    int            write_hmac;
    void          (*frame_done)(void *);
    void          (*write_done)(void *);
} mxf_t;

typedef struct {
    char           basename[40];
    char           issuer[80];
    char           creator[80];
    char           timestamp[30];
    char           annotation[128];
    char           title[80];
    char           kind[15];
    char           rating[5];
    char           aspect_ratio[20];
    int            digest_flag;
} xml_t;

typedef struct {
    int  sign;
    int  use_external;
    char *root;
    char *ca;
    char *signer;
    char *private_key;
} xml_signature_t;

typedef struct {
    int             cinema_profile;
    int             frame_rate;
    int             duration;
    int             entry_point;
    int             stereoscopic;
    int             log_level;
    int             ns;
    int             threads;
    char            dcp_path[MAX_BASENAME_LENGTH];
    j2k_t           j2k;
    mxf_t           mxf;
    xml_t           xml;
    assetmap_t      assetmap;
    volindex_t      volindex;
    int             pkl_count;
    pkl_t           pkl[MAX_PKL];
    xml_signature_t xml_signature;
} opendcp_t;

/* common functions */
void  dcp_log(int level, const char *fmt, ...);
void  dcp_fatal(opendcp_t *opendcp, char *error);
void  get_timestamp(char *timestamp);
int   get_asset_type(asset_t asset);
int   get_file_essence_class(char *filename);
int   validate_reel(opendcp_t *opendcp, cpl_t *cpl, int reel);
int   add_reel(opendcp_t *opendcp, cpl_t *cpl, asset_list_t reel);
int   add_cpl(opendcp_t *opendcp, pkl_t *pkl);
int   add_pkl(opendcp_t *opendcp);
void  dcp_set_log_level(int log_level);
void  dcp_log_init(int level, const char *file);

/* opendcp context */
opendcp_t *opendcp_create();
int       opendcp_delete(opendcp_t *opendcp);

/* image functions */
int check_image_compliance(int profile, odcp_image_t *image, char *file);

/* ASDCPLIB functions */
int read_asset_info(asset_t *asset);
void uuid_random(char *uuid);
int calculate_digest(const char *filename, char *digest);
int get_wav_duration(const char *filename, int frame_rate);
int get_file_essence_type(char *in_path);

/* MXF functions */
int write_mxf(opendcp_t *opendcp, filelist_t *filelist, char *output_file);

/* XML functions */
int write_cpl(opendcp_t *opendcp, cpl_t *cpl);
int write_pkl(opendcp_t *opendcp, pkl_t *pkl);
int write_assetmap(opendcp_t *opendcp);
int write_volumeindex(opendcp_t *opendcp);
int xml_verify(char *filename);
int xml_sign(opendcp_t *opendcp, char *filename);

/* J2K functions */
int convert_to_j2k(opendcp_t *opendcp, char *in_file, char *out_file, char *tmp_path);

/* retrieve error string */
char *error_string(int error_code);

#ifdef __cplusplus
}
#endif

#endif // _OPENDCP_H_
