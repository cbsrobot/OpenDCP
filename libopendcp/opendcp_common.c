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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <sys/stat.h>
#include <dirent.h>
#include <inttypes.h>
#include <ctype.h>
#include <time.h>
#include "opendcp.h"

const char *XML_HEADER  = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>";

const char *NS_CPL[]    = { "none", 
                            "http://www.digicine.com/PROTO-ASDCP-CPL-20040511#", /* MXF Interop */
                            "http://www.smpte-ra.org/schemas/429-7/2006/CPL"     /* SMPTE */
                          };

const char *NS_CPL_3D[] = { "none", 
                            "http://www.digicine.com/schemas/437-Y/2007/Main-Stereo-Picture-CPL",   /* MXF Interop */
                            "http://www.smpte-ra.org/schemas/429-10/2008/Main-Stereo-Picture-CPL"   /* SMPTE */
                          };

const char *NS_PKL[]    = { "none", 
                            "http://www.digicine.com/PROTO-ASDCP-PKL-20040311#", /* MXF Interop */ 
                            "http://www.smpte-ra.org/schemas/429-8/2007/PKL"     /* SMPTE */
                          };

const char *NS_AM[]     = { "none",
                            "http://www.digicine.com/PROTO-ASDCP-AM-20040311#",  /* MXF Interop */
                            "http://www.smpte-ra.org/schemas/429-9/2007/AM"      /* SMPTE */
                          };

const char *DS_DSIG = "http://www.w3.org/2000/09/xmldsig#";                      /* digial signature */
const char *DS_CMA  = "http://www.w3.org/TR/2001/REC-xml-c14n-20010315";         /* canonicalization method */
const char *DS_DMA  = "http://www.w3.org/2000/09/xmldsig#sha1";                  /* digest method */
const char *DS_TMA  = "http://www.w3.org/2000/09/xmldsig#enveloped-signature";   /* transport method */

const char *DS_SMA[] = { "none", 
                         "http://www.w3.org/2000/09/xmldsig#rsa-sha1",           /* MXF Interop */
                         "http://www.w3.org/2001/04/xmldsig-more#rsa-sha256"     /* SMPTE */
                       };

const char *RATING_AGENCY[] = { "none",
                                "http://www.mpaa.org/2003-ratings",
                                "http://rcq.qc.ca/2003-ratings"
                              };

const char *DCP_LOG[] = { "NONE",
                          "ERROR",
                          "WARN",
                          "INFO",
                          "DEBUG"
                        };

void dcp_fatal(opendcp_t *opendcp, char *error) {
    dcp_log(LOG_ERROR, "%s",error);
    exit(OPENDCP_ERROR);
}

/*
char *base64(const unsigned char *data, int length) {
    int len; 
    char *b_ptr;

    BIO *b64 = BIO_new(BIO_s_mem());
    BIO *cmd = BIO_new(BIO_f_base64());
    b64 = BIO_push(cmd, b64);

    BIO_write(b64, data, length);
    BIO_flush(b64);

    len = BIO_get_mem_data(b64, &b_ptr);
    b_ptr[len-1] = '\0';

    return b_ptr;
}
*/

/* populates prefix with the longest common prefix of s1 and s2. */
static void common_prefix(const char *s1, const char *s2, char *prefix) {
    int i;

    for(i = 0; s1[i] && s2[i] && s1[i] == s2[i]; i++){
        prefix[i] = s1[i];
    }
    prefix[i] = '\0';
}

/* populates prefix with the longest common prefix of all the files. */
static void prefix_of_all(char *files[], int nfiles, char *prefix) {
    int i;

    common_prefix(files[0], files[1], prefix);
    for(i = 2; i < nfiles; i++){
        common_prefix(files[i], prefix, prefix);
    }
}

/* a filename of the form <prefix>N*.<ext> paired with its index. */
typedef struct {
    char *file;
    int   index;
} opendcp_sort_t;

/* compare 2 File_and_index structs by their index. */
static int file_cmp(const void *a, const void *b) {
    const opendcp_sort_t *fa, *fb;
    fa = a;
    fb = b;

    return (fa->index) - (fb->index);
}

/* return the index of a filename */
static int get_index(char *file, int prefix_len) {
    int index;

    char *index_string = file + prefix_len;

    if (!isdigit(index_string[0])) {
        return OPENDCP_ERROR;
    }

    index = atoi(index_string);

    return index;
}

/**
Ensure a list of ordered filenames are sequential 

@param  files is an array of file names
@param  nfiles is the number of files
@return returns 0 if files are sequential, otherwise it returns the index
        the first out of order file.
*/
int ensure_sequential(char *files[], int nfiles) {
    int  prefix_len, i;
    char prefix_buffer[MAX_FILENAME_LENGTH];

    if (nfiles <= 1) {
        return OPENDCP_NO_ERROR;
    }

    prefix_of_all(files, nfiles, prefix_buffer);
    prefix_len = strlen(prefix_buffer);

    for(i = 0; i < nfiles-1; i++) {
        if (get_index(files[i], prefix_len)+1 != get_index(files[i+1], prefix_len)) {
            return i;
        }
    }

    return OPENDCP_NO_ERROR;
}


/**
Order a list of filenames using a version type sort.

This function will order a list of filenames of the form:

  <index>N* or N*<index>

where:
  <index> is the longest (though possibly empty) common prefix/suffic of all the
  files.
  N is some decimal number which I call its "index".
  N is unique for each file.
  1 <= N <= nfiles  OR  0 <= N < nfiles

@param  files is an array of file names
@param  nfiles is the number of files
@return OPENDCP_ERROR_CODE
*/
int order_indexed_files(char *files[], int nfiles) {
    int  prefix_len, i;
    char prefix_buffer[MAX_FILENAME_LENGTH];
    opendcp_sort_t *fis;

    /* A single file is trivially sorted. */
    if(nfiles < 2) {
      return OPENDCP_NO_ERROR;
    }

    prefix_of_all(files, nfiles, prefix_buffer);
    prefix_len = strlen(prefix_buffer);

    /* Create an array of files and their indices to sort. */
    fis = malloc(sizeof(*fis) * nfiles);
    for(i = 0; i < nfiles; i++) {
        fis[i].file  = files[i];
        fis[i].index = get_index(files[i], prefix_len);
    }
    qsort(fis, nfiles, sizeof(*fis), file_cmp);

    /* Reorder the original file array. */
    for(i = 0; i < nfiles; i++){
        files[i] = fis[i].file;
    }

    free(fis);

    return OPENDCP_NO_ERROR;
}

/**
Allocate a list of filenames

This function allocates memory for a list of filenames. 

@param  nfiles is the number of files to allocate 
@return filelist_t  
*/
filelist_t *filelist_alloc(int nfiles) {
    int x;
    filelist_t *filelist;

    filelist = malloc(sizeof(filelist_t));

    filelist->nfiles = nfiles;
    filelist->files  = malloc(filelist->nfiles*sizeof(char*));

    if (filelist->nfiles) {
        for (x=0;x<filelist->nfiles;x++) {
                filelist->files[x]  = malloc(MAX_FILENAME_LENGTH*sizeof(char));
        }
    }

    return filelist;
}

/**
free a filelist_t structure 

This function frees memory used by filelist_t. 

@param  filelist_t 
@return NONE  
*/
void filelist_free(filelist_t *filelist) {
    int x;

    if (filelist == NULL) {
        return;
    }

    if (filelist->files) {
        for (x=0; x<filelist->nfiles; x++) {
            free(filelist->files[x]);
        }
        free(filelist->files);
    }

    free(filelist);
}

void get_timestamp(char *timestamp) { 
    time_t time_ptr;
    struct tm *time_struct;
    char buffer[30];

    time(&time_ptr);
    time_struct = localtime(&time_ptr);
    strftime(buffer,30,"%Y-%m-%dT%I:%M:%S+00:00",time_struct);
    sprintf(timestamp,"%.30s",buffer);
}  

int get_asset_type(asset_t asset) {
    switch (asset.essence_type) {
       case AET_MPEG2_VES:
       case AET_JPEG_2000:
       case AET_JPEG_2000_S:
           return ACT_PICTURE;
           break;
       case AET_PCM_24b_48k:
       case AET_PCM_24b_96k:
           return ACT_SOUND;
           break;
       case AET_TIMED_TEXT:
           return ACT_TIMED_TEXT;
           break;
       default:
           return ACT_UNKNOWN;
    }
}

opendcp_t *opendcp_create() {
    opendcp_t *opendcp;

    /* allocation opendcp memory */
    opendcp = malloc(sizeof(opendcp_t));
    memset(opendcp,0,sizeof (opendcp_t));

    /* initialize opendcp */
    opendcp->log_level = LOG_WARN;
    sprintf(opendcp->xml.issuer,"%.80s %.80s",OPENDCP_NAME,OPENDCP_VERSION);
    sprintf(opendcp->xml.creator,"%.80s %.80s",OPENDCP_NAME, OPENDCP_VERSION);
    sprintf(opendcp->xml.annotation,"%.128s",DCP_ANNOTATION);
    sprintf(opendcp->xml.title,"%.80s",DCP_TITLE);
    sprintf(opendcp->xml.kind,"%.15s",DCP_KIND);
    get_timestamp(opendcp->xml.timestamp);

    /* initialize callbacks */
    opendcp->mxf.frame_done.callback  = NULL;
    opendcp->mxf.frame_done.argument  = NULL;
    opendcp->mxf.file_done.callback   = NULL;
    opendcp->mxf.file_done.argument   = NULL;

    return opendcp;
}

int opendcp_delete(opendcp_t *opendcp) {
    if ( opendcp != NULL) {
        free(opendcp);
    }
    return OPENDCP_NO_ERROR;
}

int add_pkl(opendcp_t *opendcp) {
    char uuid_s[40];
    int i = opendcp->pkl_count++;

    strcpy(opendcp->pkl[i].issuer,     opendcp->xml.issuer);
    strcpy(opendcp->pkl[i].creator,    opendcp->xml.creator);
    strcpy(opendcp->pkl[i].annotation, opendcp->xml.annotation);
    strcpy(opendcp->pkl[i].timestamp,  opendcp->xml.timestamp);

    /* Generate UUIDs */
    uuid_random(uuid_s);
    sprintf(opendcp->pkl[i].uuid,"%.36s",uuid_s);

    /* Generate XML filename */
    if ( !strcmp(opendcp->xml.basename,"") ) {
        sprintf(opendcp->pkl[i].filename,"%.40s_pkl.xml",opendcp->pkl[i].uuid);
    } else {
        sprintf(opendcp->pkl[i].filename,"%.40s_pkl.xml",opendcp->xml.basename);
    }

    opendcp->pkl_count++;

    return OPENDCP_NO_ERROR;
}

int add_cpl(opendcp_t *opendcp, pkl_t *pkl) {
    char uuid_s[40];
    int i = pkl->cpl_count;

    strcpy(pkl->cpl[i].annotation, opendcp->xml.annotation);
    strcpy(pkl->cpl[i].issuer,     opendcp->xml.issuer);
    strcpy(pkl->cpl[i].creator,    opendcp->xml.creator);
    strcpy(pkl->cpl[i].title,      opendcp->xml.title);
    strcpy(pkl->cpl[i].kind,       opendcp->xml.kind);
    strcpy(pkl->cpl[i].rating,     opendcp->xml.rating);
    strcpy(pkl->cpl[i].timestamp,  opendcp->xml.timestamp);

    uuid_random(uuid_s);
    sprintf(pkl->cpl[i].uuid,"%.36s",uuid_s);

    /* Generate XML filename */
    if ( !strcmp(opendcp->xml.basename,"") ) {
        sprintf(pkl->cpl[i].filename,"%.40s_cpl.xml",pkl->cpl[i].uuid);
    } else {
        sprintf(pkl->cpl[i].filename,"%.40s_cpl.xml",opendcp->xml.basename);
    }

    pkl->cpl_count++;

    return OPENDCP_NO_ERROR;
}

int init_asset(asset_t *asset) {
    memset(asset,0,sizeof(asset_t));

    return OPENDCP_NO_ERROR;
}

int validate_reel(opendcp_t *opendcp, cpl_t *cpl, int reel) {
    int d = 0;
    int x,a;
    int picture = 0;
    int duration_mismatch = 0;

    dcp_log(LOG_INFO,"Validating Reel %d\n",reel+1);

    a = cpl->reel[reel].asset_count; 

    /* check if reel has a picture track */ 
    for (x=0;x<a;x++) {
        if (cpl->reel[reel].asset[x].essence_class == ACT_PICTURE) {
            picture++;
        }
    }

    if (picture < 1) {
        dcp_log(LOG_ERROR,"Reel %d has no picture track",reel);
        return OPENDCP_NO_PICTURE_TRACK;
    } else if (picture > 1) {
        dcp_log(LOG_ERROR,"Reel %d has multiple picture tracks",reel);
        return OPENDCP_MULTIPLE_PICTURE_TRACK;
    }

    d = cpl->reel[reel].asset[0].duration;

    /* check durations */
    for (x=0;x<a;x++) {
        if (cpl->reel[reel].asset[x].duration) {
            if (cpl->reel[reel].asset[x].duration != d) {
                duration_mismatch = 1;
                if (cpl->reel[reel].asset[x].duration < d) {
                   d = cpl->reel[reel].asset[x].duration;
                }
            }
        } else {
            dcp_log(LOG_ERROR,"Asset %s has no duration",cpl->reel[reel].asset[x].filename);
           return OPENDCP_TRACK_NO_DURATION;
        }
    }

    if (duration_mismatch) {
       dcp_log(LOG_WARN,"Asset duration mismatch, adjusting all durations to shortest asset duration of %d frames", d);
        for (x=0;x<a;x++) {
            cpl->reel[reel].asset[x].duration = d;
        }
    }
          
    return OPENDCP_NO_ERROR;
}

int add_reel(opendcp_t *opendcp, cpl_t *cpl, asset_list_t reel) {
    int result;
    int x,r;
    FILE *fp;
    char *filename;
    asset_t asset;
    struct stat st;
    char uuid_s[40];

    dcp_log(LOG_INFO,"Adding Reel");

    r = cpl->reel_count; 

    /* add reel uuid */
    uuid_random(uuid_s);
    sprintf(cpl->reel[r].uuid,"%.36s",uuid_s);

    /* parse argument and read asset information */
    for (x=0;x<reel.asset_count;x++) {
        filename=reel.asset_list[x].filename;
        init_asset(&asset);
      
        sprintf(asset.filename,"%s",filename);
        sprintf(asset.annotation,"%s",basename(filename));

        /* check if file exists */
        if ((fp = fopen(filename, "r")) == NULL) {
            dcp_log(LOG_ERROR,"add_reel: Could not open file: %s",filename);
            return OPENDCP_FILEOPEN;
        } else {
            fclose (fp);
        }

        /* get file size */
        stat(filename, &st);
        sprintf(asset.size,"%"PRIu64, st.st_size);

        /* read asset information */
        dcp_log(LOG_INFO,"add_reel: Reading %s asset information",filename);

        result = read_asset_info(&asset);

        if (result == OPENDCP_ERROR) {
            dcp_log(LOG_ERROR,"%s is not a proper essence file",filename);
            return OPENDCP_INVALID_TRACK_TYPE;
        }

        if (x == 0) {
            opendcp->ns = asset.xml_ns;
            dcp_log(LOG_DEBUG,"add_reel: Label type detected: %d",opendcp->ns);
        } else {
            if (opendcp->ns != asset.xml_ns) {
                dcp_log(LOG_ERROR,"Warning DCP specification mismatch in assets. Please make sure all assets are MXF Interop or SMPTE");
                return OPENDCP_SPECIFICATION_MISMATCH;
            }
        }

        /* force aspect ratio, if specified */
        if (strcmp(opendcp->xml.aspect_ratio,"") ) {
            sprintf(asset.aspect_ratio,"%s",opendcp->xml.aspect_ratio);
        }

        /* Set duration, if specified */
        if (opendcp->duration) {
            if  (opendcp->duration<asset.duration) {
                asset.duration = opendcp->duration;
            } else {
                dcp_log(LOG_WARN,"Desired duration %d cannot be greater than assset duration %d, ignoring value",opendcp->duration,asset.duration);
            }
        }

        /* Set entry point, if specified */
        if (opendcp->entry_point) {
            if (opendcp->entry_point<asset.duration) {
                asset.entry_point = opendcp->entry_point;
            } else {
                dcp_log(LOG_WARN,"Desired entry point %d cannot be greater than assset duration %d, ignoring value",opendcp->entry_point,asset.duration);
            }
        }

        /* calculate digest */
        calculate_digest(filename,asset.digest);
   
        /* get asset type */
        result = get_asset_type(asset);

        /* add asset to cpl */
        cpl->reel[r].asset[x] = asset;
        cpl->reel[r].asset_count++;
    }

    cpl->reel_count++;

    return OPENDCP_NO_ERROR;
}
