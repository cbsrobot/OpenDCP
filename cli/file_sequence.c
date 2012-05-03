#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void common_prefix(const char *s1, const char *s2, char *prefix){
  int i;

  for(i = 0; s1[i] && s2[i] && s1[i] == s2[i]; i++)
    prefix[i] = s1[i];
  prefix[i] = '\0';
}

/* Assumes nfiles is >= 2. */
void prefix_of_all(const char *files[], int nfiles, char *prefix){
  int i;

  common_prefix(files[0], files[1], prefix);
  for(i = 2; i < nfiles; i++)
    common_prefix(files[i], prefix, prefix);
}

typedef struct {
  const char *file;
  int original_index;
  int num;
}*foo;

int file_cmp(const void *a, const void *b){
  foo *fa, *fb;
  fa = a;
  fb = b;

  fa = a;
  fb = b;

  return ((*fa)->num) - ((*fb)->num);
}

void order_tiff_files(const char *files[], int nfiles){
  int prefix_len, i;
  char prefix_buffer[100];

  prefix_of_all(files, nfiles, prefix_buffer);
  prefix_len = strlen(prefix_buffer);

  foo *fs = malloc(sizeof(*fs) * nfiles);
  for(i = 0; i < nfiles; i++){
    fs[i] = malloc(sizeof(**fs));
    fs[i]->file = files[i];
    fs[i]->original_index = i;
    fs[i]->num = atoi(files[i] + prefix_len);
  }
  qsort(fs, nfiles, sizeof(*fs), file_cmp);

  for(i = 0; i < nfiles; i++){
    files[i] = fs[i]->file;
    free(fs[i]);
  }

  free(fs);
}

int main(int argc, char *argv[]){
  int i;

  order_tiff_files(argv+1, argc-1);
  for(i = 1; i < argc; i++)
    puts(argv[i]);

  return 0;
}
