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
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <stdint.h>

#include "opendcp.h"
#include "opendcp_cli.h"

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

int file_selector(const char *filename, const char *filter) {
    char *extension;

    extension = strrchr(filename,'.');

    if ( extension == NULL ) {
        return 0;
    }

    extension++;

    if (strlen(extension) < 3) {
        return 0;
    }

    if (strstr(filter, extension) != NULL) {
        return 1;
    } 

    return 0;
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
        return 0;
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

filelist_t *get_filelist(const char *path, const char *filter) {
    DIR *d;
    struct stat st_in;
    struct dirent *de, **names=0, **tmp;
    size_t cnt=0, len=0;
    filelist_t *filelist;

    if (stat(path, &st_in) != 0 ) {
        return NULL;
    }

    if (!S_ISDIR(st_in.st_mode)) {
        filelist = filelist_alloc(1);
        sprintf(filelist->files[0],"%s",path);
        return filelist;
    }

	if ((d = opendir(path)) == NULL) {
		return(NULL);
    }

    while ((de = readdir(d))) {
        if (!file_selector(de->d_name, filter)) {
            continue;
        }
        if (cnt >= len) {
            len = 2*len+1;
            if (len > SIZE_MAX/sizeof *names) {
                break;
            }
            tmp = realloc(names, len * sizeof *names);
            if (!tmp) {
                break;
            }
            names = tmp;
        }
        names[cnt] = malloc(de->d_reclen);
        if (!names[cnt]) {
            break;
        }
        memcpy(names[cnt++], de, de->d_reclen);
    }
    closedir(d);

    filelist = filelist_alloc(cnt);

    if (names) {
        while (cnt-->0) {
            sprintf(filelist->files[cnt],"%s/%s",path,names[cnt]->d_name);
            free(names[cnt]);
        }
       free(names);
    }

    return filelist;
}

int build_filelist(char *input, filelist_t *filelist) {
    struct dirent **files;
    int x = 0;
    struct stat st_in;

    if (stat(input, &st_in) != 0 ) {
        dcp_log(LOG_ERROR,"Could not open input file %s",input);
        return OPENDCP_ERROR;
    }

    filelist->nfiles = scandir(input, &files, (void *)file_filter, alphasort);
    if (filelist->nfiles) {
        for (x=0;x<filelist->nfiles;x++) {
            sprintf(filelist->files[x],"%s/%s",input,files[x]->d_name);
        }
     }
    for (x=0;x<filelist->nfiles;x++) {
        free(files[x]);
    }
    free(files);

    return OPENDCP_NO_ERROR;
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
