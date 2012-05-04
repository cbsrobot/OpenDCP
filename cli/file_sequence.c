#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
static void prefix_of_all(const char *files[], int nfiles, char *prefix){
    int i;

    common_prefix(files[0], files[1], prefix);
    for(i = 2; i < nfiles; i++){
        common_prefix(files[i], prefix, prefix);
    }
}

/* A filename of the form <prefix>N*.tif paired with its index. */
typedef struct {
    const char *file;
    int         index;
} File_index;

static int file_cmp(const void *a, const void *b){
    File_index *fa, *fb;
    fa = a;
    fb = b;

    return (fa->index) - (fb->index);
}

/* Given an array of pointers to filenames of the form:
 *
 *   <prefix>N*.tif
 *
 * where:
 *  * <prefix> is the longest (though possibly empty) common prefix of all the
 *    files.
 *  * N is some decimal number which I'll refer to as it's "index".
 *  * N is unique among all the files.
 *
 * Sorts the files in order of increasing index.
 */
void order_tiff_files(const char *files[], int nfiles){
    int prefix_len, i;
    char prefix_buffer[100];

    prefix_of_all(files, nfiles, prefix_buffer);
    prefix_len = strlen(prefix_buffer);

    File_index *fns = malloc(sizeof(*fns) * nfiles);
    for(i = 0; i < nfiles; i++){
        fns[i].file  = files[i];
        fns[i].index = atoi(files[i] + prefix_len);
    }
    qsort(fns, nfiles, sizeof(*fns), file_cmp);

    for(i = 0; i < nfiles; i++){
        files[i] = fns[i].file;
    }

    free(fns);
}

/* For some quick command-line testing. */
int main(int argc, char *argv[]){
    int i;

    order_tiff_files(argv+1, argc-1);
    for(i = 1; i < argc; i++){
        puts(argv[i]);
    }

    return 0;
}
