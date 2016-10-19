/*********************************************************************
 *
 * mar300_header.c
 *
 *********************************************************************
 * Copyright 2015,   Claudio Klein, marXperts GmbH
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *********************************************************************
 *
 * History:
 * Version    Date    	Changes
 * ______________________________________________________________________
 * 1.10.0     10/08/15	Made public under Apache License 2.0
 * 1.9.0      27/03/06	Writemar300Header added, Putmar300Header->PutHeader
 * 1.8.0      30/11/05	Replace swapshort by swapint16, swaplong by swapint32
 *			and swapvms by swapfloat
 * 1.7.0      26/05/03	unistd.h added for ppc
 * 1.6	      08/01/02	Truncate h.date at byte 24 to \0
 *
 *********************************************************************/

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

/*
 * mar software include files
 */
#include "mar300_header.h"
#include "MarReader.h"

/*
 * External functions
 */
extern void 	swapint64		(unsigned char *, int);
extern void 	swapint32		(unsigned char *, int);
extern void 	swapint16		(unsigned char *, int);
extern void 	swapfloat		(float *, int);

/*
 * Local functions
 */
MAR300_HEADER	Getmar300Header		(FILE *);
MAR300_HEADER	Setmar300Header		(void);
//int 			Writemar300Header	(FILE *, int, MAR300_HEADER);
//int 			Putmar300Header		(int, int, MAR300_HEADER );
//static int		PutHeader			(int, FILE *, int, MAR300_HEADER);

/******************************************************************
 * Function: Getmar300Header
 ******************************************************************/
MAR300_HEADER
Getmar300Header(FILE *fp)
{
MAR300_HEADER	h;
int 		i,j, io,ntok=0;
char 		str[64], buf[64], *key, *token[20], q[64];
float		ftmp,fh[15];


    if ( fp == NULL ) return( h );

    /*
     * Read 10*int and 15*float and 24*char
     */

    fseek( fp, 0, SEEK_SET );
    io  = fread(  &h, sizeof( MAR300_HEADER ), 1, fp );

    if ( io==1 ) io = sizeof( MAR300_HEADER );

    io += fread( str, sizeof( char ), 16, fp );
    if ( !strstr( str, "MAR" ) ) {
        if ( io < 140 ) {
            fprintf( stderr, "ERROR: only %d out of 140 bytes read from image header!!!\n",io );
        }
        io = 588;
        goto SWAP;
    }
    else {

        /* New style header: take all information from strings */
        fseek( fp, sizeof(MAR300_HEADER)+16, SEEK_SET );
        j = ftell( fp );

        for ( i = 0; i< 28; i++ ) {
            io += fread( str, sizeof( char ), 16, fp );
            j += 16;
            fseek( fp, j, SEEK_SET );
            str[16]='\0';
            if      ( i ==  3 ) sscanf( str, "%d", &h.pixels_x);
            else if ( i ==  4 ) sscanf( str, "%d", &h.pixels_y);
            else if ( i ==  5 ) sscanf( str, "%d", &h.lrecl);
            else if ( i ==  6 ) sscanf( str, "%d", &h.max_rec);
            else if ( i ==  7 ) sscanf( str, "%d", &h.high_pixels);
            else if ( i ==  8 ) sscanf( str, "%d", &h.high_records);
            else if ( i ==  9 ) sscanf( str, "%d", &h.counts_start);
            else if ( i == 10 ) sscanf( str, "%d", &h.counts_end);
            else if ( i == 11 ) sscanf( str, "%d", &h.exptime_sec);
            else if ( i == 12 ) sscanf( str, "%d", &h.exptime_units);
            else if ( i == 13 ) sscanf( str, "%f", &h.prog_time);
            else if ( i == 14 ) sscanf( str, "%f", &h.r_max);
            else if ( i == 15 ) sscanf( str, "%f", &h.r_min);
            else if ( i == 16 ) sscanf( str, "%f", &h.p_r);
            else if ( i == 17 ) sscanf( str, "%f", &h.p_l);
            else if ( i == 18 ) sscanf( str, "%f", &h.p_x);
            else if ( i == 19 ) sscanf( str, "%f", &h.p_y);
            else if ( i == 20 ) sscanf( str, "%f", &h.centre_x);
            else if ( i == 21 ) sscanf( str, "%f", &h.centre_y);
            else if ( i == 22 ) sscanf( str, "%f", &h.lambda);
            else if ( i == 23 ) sscanf( str, "%f", &h.distance);
            else if ( i == 24 ) sscanf( str, "%f", &h.phi_start);
            else if ( i == 25 ) sscanf( str, "%f", &h.phi_end);
            else if ( i == 26 ) sscanf( str, "%f", &h.omega);
            else if ( i == 27 ) sscanf( str, "%f", &h.multiplier);
        }
    }

SWAP:
    if ( h.pixels_x < 100 || h.pixels_x > 5000  ) {
        swapint32( (unsigned char *)&h, 10*sizeof(int) );
        if ( h.pixels_x < 100 || h.pixels_x > 5000  ) {
            fprintf( stderr, "ERROR: something wrong with image header!!!\n");
            h.pixels_x = h.pixels_y = 0;
            return( h );
        }
        fseek( fp, 40, SEEK_SET );
        fread( fh, sizeof(float), 15, fp );

        swapint32( (unsigned char *)fh, 15*sizeof(float) );
/*
        for ( i=0;i<32; i++ ) {
            q[0] = '\0';
            if ( fh[10] & (int)( pow(2,i) )  )
                strcat( q, "0" );
            else
                strcat( q, "1" );
             printf("%s (0x%x)\n",q,fh[10]);
        }
*/
            sprintf( str, "%1.3f\0",fh[10] );
            ftmp  = (float)atof( str );
#if ( LINUX || linux || VMS || __linux__ || __cygwin__ || __i386__ )
            ftmp = fh[10];
#endif
#if ( __sgi || LINUX || linux || __linux__ || __cygwin__ || __i386__ )
            if ( ftmp <= 1.0 || ftmp  >  2000. ) {
            swapint32( (unsigned char *)fh, 15*sizeof(float) );
            swapfloat(  fh, 15 );
        }
#endif
#ifdef VMS
        if ( fh[ 9] < 0.001 || fh[9] > 100. ) {
            for ( i=0; i<15; i++ ) fh[i] = 0.0;
            fh[3] = fh[4] = fh[5] = fh[6] = 0.15;
        }
#endif

        h.prog_time 	= fh[0];
        h.r_max		= fh[1];
        h.r_min		= fh[2];
        h.p_r  		= fh[3];
        h.p_l  		= fh[4];
        h.p_x  		= fh[5];
        h.p_y  		= fh[6];
        h.centre_x	= fh[7];
        h.centre_y	= fh[8];
        h.lambda	= fh[9];
        h.distance	= fh[10];
        h.phi_start	= fh[11];
        h.phi_end  	= fh[12];
        h.omega		= fh[13];
        h.multiplier	= fh[14];
    }

    if ( io <  588 )
        fprintf( stderr, "ERROR: only %d out of 588 bytes read from image header!!!\n",io );

    fseek( fp, 2*h.pixels_x, SEEK_SET );

    return( h );
}

/******************************************************************
 * Function: Setmar300Header
 ******************************************************************/
MAR300_HEADER
Setmar300Header()
{
MAR300_HEADER	h;
time_t		now;

    h.pixels_x 		= 1200;
    h.pixels_x 		= 1200;
    h.lrecl    		= 1200;
    h.max_rec  		= 1200;
    h.high_pixels  		= 0;
    h.high_records 		= 0;
    h.counts_start		= 0;
    h.counts_end  		= 0;
    h.exptime_sec		= 0;
    h.exptime_units		= 0;

    h.prog_time		= 0.0;
    h.r_max			= 90.0;
    h.r_min			= 0.0;
    h.p_x       		= 0.15;
    h.p_y       		= 0.15;
    h.p_l       		= 0.15;
    h.p_r       		= 0.15;
    h.centre_x		= 90.;
    h.centre_y		= 90.;
    h.lambda    		= 1.541789;
    h.distance		= 70.0;
    h.phi_start		= 0.0;
    h.phi_end		= 0.0;
    h.omega 		= 0.0;
    h.multiplier		= 4.0;

    time( &now );
    sprintf( h.date, "%s", (char *)ctime( &now ) );

    return( h );
}

/******************************************************************
 * Function: Putmar300Header
 ******************************************************************/
/*
int Putmar300Header(int fd, int serial, MAR300_HEADER h)
{
        if ( fd < 0 )return(0);
    return( PutHeader(fd, NULL, serial, h) );
}
*/

/******************************************************************
 * Function: Writemar300Header
 ******************************************************************/
/*
int Writemar300Header(FILE *fp, int serial, MAR300_HEADER h)
{
        if ( fp == NULL )return(0);
    return( PutHeader(-1 , fp, serial, h) );
}
*/

/******************************************************************
 * Function: PutHeader
 ******************************************************************/
/*
static int
PutHeader(int fd, FILE *fp, int serial, MAR300_HEADER h)
{
int		i,k;
int		io=0;
char		c=' ',str[32];
time_t		now;

    if ( fp == NULL && fd < 0 ) return 0;

    if ( fp != NULL )
        fseek( fp, 0, SEEK_SET );
    else
        lseek( fd, 0, SEEK_SET );


    if ( strlen( h.date ) < 5 ) {
        time( &now );

        sprintf( str,"%s", (char *)ctime( &now ) );
        for ( i=24; i<32; i++ )
            str[i]='\0';
        strncpy( h.date, str, 24 );
        h.date[24] = '\0';
    }

    if ( fp != NULL ) {
        i = fwrite( &h, sizeof(h), 1, fp );
        io = ftell( fp );
    }
    else
        io = write( fd, &h, sizeof(h) );

    str[15] = '\0';
        sprintf( str, "MARCONTROL 1.0  ");
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }

        sprintf( str, "%16d", serial);
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
#if ( LINUX || linux || __linux__ || __cygwin__ )
        sprintf( str, "LINUX          " );
#elif ( __sgi )
        sprintf( str, "SGI            " );
#elif ( __ppc__ )
        sprintf( str, "PPC            " );
#elif ( __APPLE__ )
        sprintf( str, "OSX86          " );
#elif ( __osf__ )
        sprintf( str, "DEC UNIX       " );
#elif ( __hpux  )
        sprintf( str, "HPUX           " );
#elif ( __ALPHA )
        sprintf( str, "ALPHA          " );
#else
        sprintf( str, "UNKNOWN        " );
#endif
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16.3f", (float)h.exptime_units );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }

    str[16] = '\0';
        / * 10 x integer as string * /
        sprintf( str, "%16d",   h.pixels_x                     );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16d",   h.pixels_y                     );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16d",   h.lrecl                      );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16d",   h.max_rec                    );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16d",   h.high_pixels                );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16d",   h.high_records               );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16d",   h.counts_start               );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16d",   h.counts_end                 );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16d",   h.exptime_sec                );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16d",   h.exptime_units              );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }

        / * 15 x integer as string * /
        sprintf( str, "%16.3f", h.prog_time                  );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16.3f", h.r_max                      );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16.3f", h.r_min                      );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16.3f", h.p_r                        );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16.3f", h.p_l                        );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16.3f", h.p_x                        );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16.3f", h.p_y                        );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16.3f", h.centre_x                   );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16.3f", h.centre_y                   );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16.3f", h.lambda                     );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16.3f", h.distance                   );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16.3f", h.phi_start                  );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16.3f", h.phi_end                    );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16.3f", h.omega                      );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }
        sprintf( str, "%16.3f", h.multiplier                 );
    if ( fp != NULL ) {
        io += fwrite( str, sizeof(char), 16, fp);
    }
    else {
        io += write( fd, str, 16);
    }

FILL:
    k =2*h.pixels_x;
    if ( fp != NULL )
        i = ftell( fp );
    else
        i = lseek( fd, 0, SEEK_CUR );

    while ( i<k ) {
        if ( fp != NULL )
            i = fwrite( &c, sizeof(char), 1, fp);
        else
            i = write( fd, &c, 1 );
        if ( i < 1 ) break;
        io += i;
        if ( fp != NULL )
            i = ftell( fp );
        else
            i = lseek( fd, 0, SEEK_CUR );
    }
    if ( fp != NULL )
        i = fseek( fp, k, SEEK_SET );
    else
        i = lseek( fd, k, SEEK_SET );

    if ( io < k ) {
        fprintf( stderr, "ERROR: only %d out of %d bytes written into image header!!!\n",io,k);
        return 0;
    }

    return 1;

}
*/
