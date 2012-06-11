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
#ifdef WIN32
#include "win32/opendcp_win32_dirent.h"
#else
#include <dirent.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

#include "opendcp.h"
#include "opendcp_cli.h"

extern int build_filelist(char *input, char *output, filelist_t *filelist, int file_type);
extern int get_file_count(char *path, int file_type);

int check_extension(char *filename, char *pattern) {
    char *extension;

    extension = strrchr(filename,'.');

    if ( extension == NULL ) {
        return 0;
    }

    extension++;

   if (strncasecmp(extension,pattern,3) !=0) {
       return 0;
   }

   return 1;
}

char *get_basename(const char *filename) {
    char *extension;
    char *base = 0;

    base = (char *)filename;
    extension = strrchr(filename,'.');
    base[(strlen(filename) - strlen(extension))] = '\0';

    return(base);
}

static int filter;
int file_filter(struct dirent *filename) {
    char *extension;

    extension = strrchr(filename->d_name,'.');

    if ( extension == NULL ) {
        return 0;
    }

    extension++;

    /* return only known asset types */
    if (filter == MXF_INPUT) {
        if (strncasecmp(extension,"j2c",3) != 0 &&
            strncasecmp(extension,"j2k",3) != 0 &&
            strncasecmp(extension,"wav",3) != 0)
        return 0;
    } else if (filter == J2K_INPUT) {
        if (strncasecmp(extension,"tif",3) != 0 &&
            strncasecmp(extension,"dpx",3) != 0)
        return 0;
    }

    return 1;
}

int get_file_count(char *path, int file_type) {
    struct dirent **files;
    struct stat st_in;

    int x,count;

    filter = file_type;

    if (stat(path, &st_in) != 0 ) {
        dcp_log(LOG_ERROR,"Could not open input file %s",path);
        return DCP_FATAL;
    }

    if (S_ISDIR(st_in.st_mode)) {
        count = scandir(path,&files,(void *)file_filter,alphasort);
        for (x=0;x<count;x++) {
            free(files[x]);
        }
        free(files);

    } else {
        count = 1;
    }

    return count;
}

int build_filelist(char *input, char *output, filelist_t *filelist, int file_type) {
    struct dirent **files;
    int x = 0;
    struct stat st_in;

    if (stat(input, &st_in) != 0 ) {
        dcp_log(LOG_ERROR,"Could not open input file %s",input);
        return DCP_FATAL;
    }

    filelist->file_count = scandir(input,&files,(void *)file_filter,alphasort);
    if (filelist->file_count) {
        for (x=0;x<filelist->file_count;x++) {
            sprintf(filelist->in[x],"%s/%s",input,files[x]->d_name);
            if (file_type == J2K_INPUT) {
                sprintf(filelist->out[x],"%s/%s.j2c",output,get_basename(files[x]->d_name));
            }
        }
     }
    for (x=0;x<filelist->file_count;x++) {
        free(files[x]);
    }
    free(files);

    return DCP_SUCCESS;
}

filelist_t *filelist_alloc(int count) {
    int x;
    filelist_t *filelist;

    filelist = malloc(sizeof(filelist_t));

    filelist->file_count = count;
    filelist->in  = malloc(filelist->file_count*sizeof(char*));
    filelist->out = malloc(filelist->file_count*sizeof(char*));

    if (filelist->file_count) {
        for (x=0;x<filelist->file_count;x++) {
                filelist->in[x]  = malloc(MAX_FILENAME_LENGTH*sizeof(char *));
                filelist->out[x] = malloc(MAX_FILENAME_LENGTH*sizeof(char *));
        }
    }

    return filelist;
}

void filelist_free(filelist_t *filelist) {
    int x;

    for (x=0;x<filelist->file_count;x++) {
        if (filelist->in[x]) {
            free(filelist->in[x]);
        }
        if (filelist->out[x]) {
            free(filelist->out[x]);
        }
    }

    if (filelist->in) {
        free(filelist->in);
    }

    if (filelist->out) {
        free(filelist->out);
    }

    if (filelist) {
        free(filelist);
    }

    return;
}

int find_seq_offset(char str1[], char str2[]) {
    unsigned int i;
    unsigned int offset = 0;

    for (i = 0; (i < strlen(str1)) && (offset == 0); i++) {
        if(str1[i] != str2[i])
            offset = i;
    }

    return offset;
}

int find_ext_offset(char str[]) {
    int i = strlen(str);
    while(i) {
        if(str[i] == '.')
            return i;
        i--;
    }

    return 0;
}

/* Populates prefix with the longest common prefix of s1 and s2. */
static void common_prefix(const char *s1, const char *s2, char *prefix){
    int i;

    for(i = 0; s1[i] && s2[i] && s1[i] == s2[i]; i++){
        prefix[i] = s1[i];
    }
    prefix[i] = '\0';
}

/* Populates prefix with the longest common prefix of all the files.
 * Assumes nfiles >= 2. */
static void prefix_of_all(char *files[], int nfiles, char *prefix){
    int i;

    common_prefix(files[0], files[1], prefix);
    for(i = 2; i < nfiles; i++){
        common_prefix(files[i], prefix, prefix);
    }
}

/* A filename of the form <prefix>N*.tif paired with its index.
 * (used only for sorting) */
typedef struct {
    char *file;
    int   index;
} File_and_index;

/* Compare 2 File_and_index structs by their index. */
static int file_cmp(const void *a, const void *b){
    const File_and_index *fa, *fb;
    fa = a;
    fb = b;

    return (fa->index) - (fb->index);
}

/* Ensure fis[i].index == i+1 for all i. */
int ensure_sequential(File_and_index fis[], int nfiles){
    int i;

    for(i = 0; i < nfiles-1; i++){
        if(fis[i].index+1 != fis[i+1].index){
            return STRING_NOTSEQUENTIAL;
        }
    }

    return DCP_SUCCESS;
}

/* Given an array of pointers to filenames of the form:
 *
 *   <prefix>N*
 *
 * where:
 *  * <prefix> is the longest (though possibly empty) common prefix of all the
 *    files.
 *  * N is some decimal number which I call its "index".
 *  * N is unique for each file.
 *  * 1 <= N <= nfiles  OR  0 <= N < nfiles
 *
 * Sorts the files in order of increasing index.
 *
 * Returns: DCP error code.
 */
int order_indexed_files(char *files[], int nfiles){
    int  prefix_len, i, rc;
    char prefix_buffer[MAX_FILENAME_LENGTH];
    File_and_index *fis;

    /* A single file is trivially sorted. */
    if(nfiles < 2){
      return DCP_SUCCESS;
    }

    prefix_of_all(files, nfiles, prefix_buffer);
    prefix_len = strlen(prefix_buffer);

    /* Create an array of files and their indices to sort. */
    fis = malloc(sizeof(*fis) * nfiles);
    for(i = 0; i < nfiles; i++){
        char *index_string = files[i] + prefix_len;
        if(!isdigit(index_string[0])){
            return DCP_FATAL;
        }
        fis[i].file  = files[i];
        fis[i].index = atoi(index_string);
    }
    qsort(fis, nfiles, sizeof(*fis), file_cmp);

    /* Reorder the original file array. */
    for(i = 0; i < nfiles; i++){
        files[i] = fis[i].file;
    }

    rc = ensure_sequential(fis, nfiles);
    free(fis);

    return rc;
}
