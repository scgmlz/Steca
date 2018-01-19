/***********************************************************************
 * 
 * swap.c
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
 * 2.2          10/08/15	Made public under Apache License 2.0
 * 2.1		07/11/05	if ( SGI || LINUX ) replaced by __sgi || __linux__
 **********************************************************************/
#ifdef __unix__
      #include <unistd.h>
#elif __MSDOS__ || __WIN32__ || _MSC_VER
      #include <io.h>
#endif

#include "MarReader.h"

/*
 * Local functions
 */
void swapint64	(unsigned char *, int);
void swapint32	(unsigned char *, int);
void swapint16	(unsigned char *, int);
void swapfloat	(float *, int);

/******************************************************************
 * Function: swapint64 = swaps bytes of 64 bit integers
 ******************************************************************/
void swapint64(unsigned char *data, int nbytes)
{
int 		i,j;
unsigned char 	t[8];

        for(i=nbytes/8;i--;) {
		for ( j=0; j<8; j++ )
			t[j] = data[ i*8+7-j];
		for ( j=0; j<8; j++ )
                	data[i*8+j] = t[j];
	}
}

/******************************************************************
 * Function: swapint32 = swaps bytes of 32 bit integers
 ******************************************************************/
void swapint32(unsigned char *data, int nbytes)
{
int 		i;
unsigned char 	t1, t2, t3, t4;

        for(i=nbytes/4;i--;) {
                t1 = data[i*4+3];
                t2 = data[i*4+2];
                t3 = data[i*4+1];
                t4 = data[i*4+0];
                data[i*4+0] = t1;
                data[i*4+1] = t2;
                data[i*4+2] = t3;
                data[i*4+3] = t4;
        }
}

/******************************************************************
 * Function: swapint16 = swaps bytes of 32 bit integers
 ******************************************************************/
void swapint16(unsigned char *data, int nbytes)
{
int i;
unsigned char t1, t2;

        for(i=nbytes/2;i--;) {
                t1 = data[i*2+1];
                t2 = data[i*2+0];
                data[i*2+0] = t1;
                data[i*2+1] = t2;
        }
}

#if ( _sgi || __linux__ || __i386__ )

/******************************************************************
 * Function: swapvms = swaps bytes of floats in image header (SGI only)
 ******************************************************************/
void swapfloat(float *data, int n)
{
	int i;
	float d;

	for (i=0; i<n; i++ ) {
                swab( &data[i], &data[i], 4 );
#if ( _sgi || __linux__ || __i386__ )
                data[i]/=4.;
#else
                sprintf( str, "%1.6f\0",data[i] );
                d = atof( str );
                data[i]=d/4.;
#endif
        }
}
#endif

#ifdef SWAP
/******************************************************************
 * Function: swaplong = swaps bytes of 32 bit integers
 ******************************************************************/
void
swaplong(data, nbytes)
register char *data;
int nbytes;
{
        register int i, t1, t2, t3, t4;

        for(i=nbytes/4;i--;) {
                t1 = data[i*4+3];
                t2 = data[i*4+2];
                t3 = data[i*4+1];
                t4 = data[i*4+0];
                data[i*4+0] = t1;
                data[i*4+1] = t2;
                data[i*4+2] = t3;
                data[i*4+3] = t4;
        }
}

/******************************************************************
 * Function: swapshort = swaps the two 8-bit halves of a 16-bit word
 ******************************************************************/
void
swapshort(data, n)
register unsigned short *data;
int n;
{
	register int i;
	register unsigned short t;

	for(i=(n>>1)+1;--i;) {
		/*t = (( (*data)&0xFF) << 8 ) | (( (*data)&0xFF00) >> 8);*/
		t = (((*data) << 8) | ((*data) >> 8));
		*data++ = t;
	}
}
#endif
