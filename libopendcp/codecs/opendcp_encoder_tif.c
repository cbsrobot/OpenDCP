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

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <tiffio.h>
#include "opendcp.h"
#include "opendcp_image.h"

int opendcp_encode_tif(odcp_image_t *image, const char *outfile) {
    int y;
    TIFF *tif;
    tdata_t data;

    /* open tiff using filename or file descriptor */
    tif = TIFFOpen(outfile, "wb");

    if (tif == NULL) {
        dcp_log(LOG_ERROR, "%-15.15s: failed to open file %s for writing","write_tif", outfile);
        return OPENDCP_ERROR;
    }

    /* Set tags */
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, image->w);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, image->h);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, image->precision);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 1);

    /* allocate memory for read line */
    data = _TIFFmalloc(TIFFScanlineSize(tif));

    if (data == NULL) {
        dcp_log(LOG_ERROR, "%-15.15s: tiff memory allocation error: %s", "write_tif", outfile);
        return OPENDCP_ERROR;
    }

    /* write each row */
    for (y = 0; y<image->h; y++) {
        odcp_image_readline(image, y, data);
        TIFFWriteScanline(tif, data, y, 0);
    }

    _TIFFfree(data);
    TIFFClose(tif);

    return OPENDCP_NO_ERROR;
}
