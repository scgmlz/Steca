/***********************************************************************
 *
 * mar300_header.h
 *
 * Copyright by:        Dr. Claudio Klein
 *                      X-ray Research GmbH, Hamburg
 *
 * Version:     1.1
 * Date:        08/01/2002
 *
 **********************************************************************/

#ifndef _MAR300_HEADER_H
#define _MAR300_HEADER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>
#ifndef __sgi
#include <stdlib.h>
#endif
#ifdef __unix__
#include <unistd.h>
#elif __MSDOS__ || __WIN32__ || _MSC_VER
#include <io.h>
#endif

typedef struct {
    int pixels_x;
    int pixels_y;
    int lrecl;
    int max_rec;
    int high_pixels;
    int high_records;
    int counts_start;
    int counts_end;
    int exptime_sec;
    int exptime_units;

    float prog_time;
    float r_max;
    float r_min;
    float p_r;
    float p_l;
    float p_x;
    float p_y;
    float centre_x;
    float centre_y;
    float lambda;
    float distance;
    float phi_start;
    float phi_end;
    float omega;
    float multiplier;

    char date[26];

} MAR300_HEADER;

#endif //_MAR300_HEADER_H