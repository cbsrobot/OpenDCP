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

#include <getopt.h>
#include <signal.h>
#ifdef OPENMP
#include <omp.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <opendcp.h>
#include "opendcp_cli.h"

#ifndef _WIN32
sig_atomic_t SIGINT_received = 0;

void sig_handler(int signum) {
    SIGINT_received = 1;
    #pragma omp flush(SIGINT_received)
}
#else
int SIGINT_received = 0;
#endif

/* prototypes */
char *basename_noext(const char *str);
int   is_dir(char *path);
void  build_j2k_filename(char *in, char *path, char *out);
int   get_filelist(char *path, filelist_t *filelist);
void  progress_bar(int val, int total);
void  version();
void  dcp_usage();

void version() {
    FILE *fp;

    fp = stdout;
    fprintf(fp,"\n%s version %s %s\n\n",OPENDCP_NAME,OPENDCP_VERSION,OPENDCP_COPYRIGHT);

    exit(0);
}

void dcp_usage() {
    FILE *fp;
    fp = stdout;

    fprintf(fp,"\n%s version %s %s\n\n",OPENDCP_NAME,OPENDCP_VERSION,OPENDCP_COPYRIGHT);
    fprintf(fp,"Usage:\n");
    fprintf(fp,"       opendcp_j2k -i <file> -o <file> [options ...]\n\n");
    fprintf(fp,"Required:\n");
    fprintf(fp,"       -i | --input <file>            - input file or directory)\n");
    fprintf(fp,"       -o | --output <file>           - output file or directory\n");
    fprintf(fp,"\n");
    fprintf(fp,"Options:\n");
    fprintf(fp,"       -r | --rate <rate>                 - frame rate (default 24)\n");
    fprintf(fp,"       -p | --profile <profile>           - profile cinema2k | cinema4k (default cinema2k)\n");
    fprintf(fp,"       -3 | --3d                          - adjust frame rate for 3D\n");
    fprintf(fp,"       -t | --threads <threads>           - set number of threads (default 4)\n");
    fprintf(fp,"       -x | --no_xyz                      - do not perform rgb->xyz color conversion\n");
    fprintf(fp,"       -e | --encoder <openjpeg | kakadu> - jpeg2000 encoder (default openjpeg)\n");
    fprintf(fp,"       -g | --dpx <linear | film | video> - process dpx image as linear, log film, or log video (default linear)\n");
    fprintf(fp,"       -b | --bw                          - max Mbps bandwitdh (default: 250)\n");
    fprintf(fp,"       -n | --no_overwrite                - do not overwrite existing jpeg2000 files\n");
    fprintf(fp,"       -l | --log_level <level>           - sets the log level 0:Quiet, 1:Error, 2:Warn (default),  3:Info, 4:Debug\n");
    fprintf(fp,"       -h | --help                        - show help\n");
    fprintf(fp,"       -c | --lut                         - select color conversion LUT, 0:srgb, 1:rec709, 2:P3\n");
    fprintf(fp,"       -z | --resize                      - resize image to DCI compliant resolution\n");
    fprintf(fp,"       -s | --start                       - start frame\n");
    fprintf(fp,"       -d | --end                         - end frame\n");
    fprintf(fp,"       -v | --version                     - show version\n");
    fprintf(fp,"       -m | --tmp_dir                     - sets temporary directory (usually tmpfs one) to save there temporary tiffs for Kakadu");
    fprintf(fp,"\n\n");
    fprintf(fp,"^ Kakadu requires you to download and have the kdu_compress utility in your path.\n");
    fprintf(fp,"  You must agree to the Kakadu non-commerical licensing terms and assume all respsonsibility of its use.\n");
    fprintf(fp,"\n\n");
    
    fclose(fp);
    exit(0);
}

char *substring(const char *str, size_t begin, size_t len) {
  if (str == 0 || strlen(str) == 0 || strlen(str) < begin || strlen(str) < (begin+len)) {
      return 0;
  }

  return strndup(str + begin, len);
}

char *basename_noext(const char *str) {
  int start, end;

  if (str == 0 || strlen(str) == 0) {
    return 0;
  }

  char *base = strrchr(str,'/') + 1;
  char *ext  = strrchr(str,'.');

  start = strlen(str) - strlen(base);
  end   = strlen(base) - strlen(ext);

  char *substr = substring(str, start, end); 

  return(substr);
}

void build_j2k_filename(char *in, char *path, char *out) {
    if (!is_dir(path)) {
        sprintf(out,"%s",path);
    } else {
        char *base = basename_noext(in);
        sprintf(out, "%s/%s.j2c", path, base);
        free(base);
    }
}

int is_dir(char *path) {
    struct stat st_in;

    if (stat(path, &st_in) != 0 ) {
        dcp_log(LOG_ERROR,"Could not open input file %s",path);
        return 0;
    }

    if (S_ISDIR(st_in.st_mode)) {
        return 1;
    }

    return 0;
}

int get_filelist(char *path, filelist_t *filelist) {
    char *extension;

    if (is_dir(path)) {
        build_filelist(path, filelist);
    } else {
        dcp_log(LOG_DEBUG,"%-15.15s: input is a single file %s","get_filelist", path);
        extension = strrchr(path,'.');
        ++extension;
        filelist->file_count = 1;
        sprintf(filelist->in[0],"%s",path);
    }

   return OPENDCP_NO_ERROR;
}

void progress_bar(int val, int total) {
    int x;
    int step = 20;
    float c = (float)step/total * (float)val;
#ifdef OPENMP
    int nthreads = omp_get_num_threads(); 
#else
    int nthreads = 1;
#endif
    printf("  JPEG2000 Conversion (%d thread",nthreads);
    if (nthreads > 1) {
        printf("s");
    }
    printf(") [");
    for (x=0;x<step;x++) {
        if (c>x) {
            printf("=");
        } else {
            printf(" ");
        }
    }
    printf("] 100%% [%d/%d]\r",val,total);
    fflush(stdout);
}

int main (int argc, char **argv) {
    int rc, c, result, count = 0;
    int openmp_flag = 0;
    opendcp_t *opendcp;
    char *in_path = NULL;
    char *out_path = NULL;
    char *tmp_path = NULL;
    char *log_file = NULL;
    filelist_t *filelist;

#ifndef _WIN32
    struct sigaction sig_action;
    sig_action.sa_handler = sig_handler;
    sig_action.sa_flags = 0;
    sigemptyset(&sig_action.sa_mask);

    sigaction(SIGINT,  &sig_action, NULL);
#endif

    if ( argc <= 1 ) {
        dcp_usage();
    }

    opendcp = opendcp_create();

    /* set initial values */
    opendcp->j2k.xyz         = 1;
    opendcp->log_level       = LOG_WARN;
    opendcp->cinema_profile  = DCP_CINEMA2K;
    opendcp->j2k.encoder     = J2K_OPENJPEG;
    opendcp->frame_rate      = 24;
    opendcp->j2k.start_frame = 1;
    opendcp->j2k.bw          = 250;
#ifdef OPENMP
    openmp_flag              = 1;
    opendcp->threads = omp_get_num_procs();
#endif
 
    /* parse options */
    while (1)
    {
        static struct option long_options[] =
        {
            {"help",           required_argument, 0, 'h'},
            {"input",          required_argument, 0, 'i'},
            {"output",         required_argument, 0, 'o'},
            {"bw",             required_argument, 0, 'b'},
            {"dpx ",           required_argument, 0, 'g'},
            {"rate",           required_argument, 0, 'r'},
            {"profile",        required_argument, 0, 'p'},
            {"log_level",      required_argument, 0, 'l'},
            {"log_file",       required_argument, 0, 'w'},
            {"threads",        required_argument, 0, 't'},
            {"encoder",        required_argument, 0, 'e'},
            {"start",          required_argument, 0, 's'},
            {"end",            required_argument, 0, 'd'},
            {"no_xyz",         no_argument,       0, 'x'},
            {"no_overwrite",   no_argument,       0, 'n'},
            {"3d",             no_argument,       0, '3'},
            {"version",        no_argument,       0, 'v'},
            {"resize",         no_argument,       0, 'z'},
            {"tmp_dir",        required_argument, 0, 'm'},
            {"lut",            required_argument, 0, 'c'},
            {0, 0, 0, 0}
        };

        /* getopt_long stores the option index here. */
        int option_index = 0;
     
        c = getopt_long (argc, argv, "b:c:d:e:g:i:l:m:o:p:r:s:t:w:3hnvxz",
                         long_options, &option_index);
     
        /* Detect the end of the options. */
        if (c == -1)
            break;
     
        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0) {
                    break;
                }
                break;
            case '3': 
                opendcp->stereoscopic = 1;
                break;
            case 'd':
                opendcp->j2k.end_frame = strtol(optarg, NULL, 10);
                break;
            case 's':
                opendcp->j2k.start_frame = atoi(optarg);
                break;
            case 'g':
                if (!strcmp(optarg,"linear")) {
                    opendcp->j2k.dpx = DPX_LINEAR;
                } else if (!strcmp(optarg,"film")) {
                    opendcp->j2k.dpx = DPX_FILM;
                } else if (!strcmp(optarg,"video")) {
                    opendcp->j2k.dpx = DPX_VIDEO;
                } else {
                    dcp_fatal(opendcp,"Invalid DPX argument");
                }
                break;
            case 'p':
                if (!strcmp(optarg,"cinema2k")) {
                    opendcp->cinema_profile = DCP_CINEMA2K;
                } else if (!strcmp(optarg,"cinema4k")) {
                    opendcp->cinema_profile = DCP_CINEMA4K;
                } else {
                    dcp_fatal(opendcp,"Invalid profile argument");
                }
                break;
            case 'i':
                in_path = optarg;
                break;
            case 'l':
                opendcp->log_level = atoi(optarg);
                break;
            case 'o':
                out_path = optarg;
                break;
            case 'h':
                 dcp_usage();
                 break;
            case 'r':
                opendcp->frame_rate = atoi(optarg);
                break;
            case 'e':
                if (!strcmp(optarg,"openjpeg")) {
                    opendcp->j2k.encoder = J2K_OPENJPEG;
                } else if (!strcmp(optarg,"kakadu")) {
                    opendcp->j2k.encoder = J2K_KAKADU;
                } else {
                    dcp_fatal(opendcp,"Invalid encoder argument");
                }
                break;
            case 'b':
                opendcp->j2k.bw = atoi(optarg);
                break;
            case 't':
                opendcp->threads = atoi(optarg);
                break;
            case 'x':
                opendcp->j2k.xyz = 0;
                break;
            case 'n':
                opendcp->j2k.no_overwrite = 1;
                break;
            case 'v':
                version();
                break;
            case 'w':
                log_file = optarg;
                break;
            case 'm':
                tmp_path = optarg;
                break;
            case 'c':
                opendcp->j2k.lut = atoi(optarg);
                break;
            case 'z':
                opendcp->j2k.resize = 1;
                break;
        }
    }

    /* set log level */
    dcp_log_init(opendcp->log_level, log_file);

    if (opendcp->log_level > 0) {
        printf("\nOpenDCP J2K %s %s\n",OPENDCP_VERSION,OPENDCP_COPYRIGHT);
        if (opendcp->j2k.encoder == J2K_KAKADU) {
            printf("  Encoder: Kakadu\n");
        } else {
            printf("  Encoder: OpenJPEG\n");
        }
    }

    /* cinema profile check */
    if (opendcp->cinema_profile != DCP_CINEMA4K && opendcp->cinema_profile != DCP_CINEMA2K) {
        dcp_fatal(opendcp,"Invalid profile argument, must be cinema2k or cinema4k");
    }

    /* end frame check */
    if (opendcp->j2k.end_frame < 0) {
        dcp_fatal(opendcp,"End frame  must be greater than 0");
    }

    /* start frame check */
    if (opendcp->j2k.start_frame < 1) {
        dcp_fatal(opendcp,"Start frame must be greater than 0");
    }

    /* frame rate check */
    if (opendcp->frame_rate > 60 || opendcp->frame_rate < 1 ) {
        dcp_fatal(opendcp,"Invalid frame rate. Must be between 1 and 60.");
    }

    /* encoder check */
    if (opendcp->j2k.encoder == J2K_KAKADU) {
        result = system("kdu_compress -u >/dev/null 2>&1");
        if (result>>8 != 0) {
            dcp_fatal(opendcp,"kdu_compress was not found. Either add to path or remove -e 1 flag");
        }
    }

    /* bandwidth check */
    if (opendcp->j2k.bw < 10 || opendcp->j2k.bw > 250) {
        dcp_fatal(opendcp,"Bandwidth must be between 10 and 250");
    } else {
        opendcp->j2k.bw *= 1000000;
    }

    /* input path check */
    if (in_path == NULL) {
        dcp_fatal(opendcp,"Missing input file");
    }

    /* output path check */
    if (out_path == NULL) {
        dcp_fatal(opendcp,"Missing output file");
    }

    /* make sure path modes are ok */
    if (is_dir(in_path) && !is_dir(out_path)) {
        dcp_fatal(opendcp,"Input is a directory, so output must also be a directory");
    }


    /* get file list */
    dcp_log(LOG_DEBUG,"%-15.15s: getting files in %s","opendcp_j2k_cmd",in_path);
    count = get_file_count(in_path, J2K_INPUT);
    filelist = (filelist_t *)filelist_alloc(count);
    get_filelist(in_path, filelist);

    if (filelist->file_count < 1) {
        dcp_fatal(opendcp,"No input files located");
    }

    /* end frame check */
    if (opendcp->j2k.end_frame) {
        if (opendcp->j2k.end_frame > filelist->file_count) {
            dcp_fatal(opendcp,"End frame is greater than the actual frame count");
        }
    } else {
        opendcp->j2k.end_frame = filelist->file_count;
    }
   
    /* start frame check */
    if (opendcp->j2k.start_frame > opendcp->j2k.end_frame) {
        dcp_fatal(opendcp,"Start frame must be less than end frame");
    }

    dcp_log(LOG_DEBUG,"%-15.15s: checking file sequence","opendcp_j2k_cmd",in_path);

    /* Sort files by index, and make sure they're sequential. */
    if (order_indexed_files(filelist->in, filelist->file_count) != OPENDCP_NO_ERROR) {
        dcp_fatal(opendcp,"Could not order image files"); 
    }

    rc = ensure_sequential(filelist->in, filelist->file_count); 
    if (rc != OPENDCP_NO_ERROR) {
        dcp_log(LOG_WARN, "Filenames not sequential between %s and %s.",filelist->in[rc],filelist->in[rc+1]);
    }

    if (opendcp->log_level>0 && opendcp->log_level<3) { progress_bar(0,0); }

#ifdef OPENMP
    omp_set_num_threads(opendcp->threads);
    dcp_log(LOG_DEBUG,"OpenMP Enable");
#endif

    count = opendcp->j2k.start_frame;

    #pragma omp parallel for private(c)
    for (c=opendcp->j2k.start_frame-1;c<opendcp->j2k.end_frame;c++) {    
        #pragma omp flush(SIGINT_received)
        char out[MAX_FILENAME_LENGTH];
        build_j2k_filename(filelist->in[c], out_path, out);
        if (!SIGINT_received) {
            dcp_log(LOG_INFO,"JPEG2000 conversion %s started OPENMP: %d",filelist->in[c],openmp_flag);
            if(access(out, F_OK) != 0 || opendcp->j2k.no_overwrite == 0) {
                result = convert_to_j2k(opendcp,filelist->in[c],out, tmp_path);
            } else {
                result = OPENDCP_NO_ERROR;
            }

            if (count) {
               if (opendcp->log_level>0 && opendcp->log_level<3) {progress_bar(count,opendcp->j2k.end_frame);}
            }

            if (result == OPENDCP_ERROR) {
                dcp_log(LOG_ERROR,"JPEG2000 conversion %s failed",filelist->in[c]);
                dcp_fatal(opendcp,"Exiting...");
            } else {
                dcp_log(LOG_INFO,"JPEG2000 conversion %s complete",filelist->in[c]);
            }
            count++;
        }
    }

    if (opendcp->log_level>0 && opendcp->log_level<3) {progress_bar(count-1,opendcp->j2k.end_frame);}

    filelist_free(filelist);

    if (opendcp->log_level > 0) {
        printf("\n");
    }

    opendcp_delete(opendcp);

    exit(0);
}
