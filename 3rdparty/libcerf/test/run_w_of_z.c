/* Library libcerf:
 *   Compute complex error functions, based on a new implementation of
 *   Faddeeva's w_of_z. Also provide Dawson and Voigt functions.
 *
 * File runvoigt.c:
 *   Interactive evaluation of Voigt's function.
 * 
 * Copyright:
 *   (C) 2013 Forschungszentrum Jülich GmbH
 * 
 * Licence:
 *   Public domain.
 *
 * Author:
 *   Joachim Wuttke, Forschungszentrum Jülich, 2013
 *
 * Website:
 *   http://apps.jcns.fz-juelich.de/libcerf
 */

#include <stdio.h>
#include <stdlib.h>
#include "cerf.h"
#include "../lib/defs.h"

extern int faddeeva_algorithm, faddeeva_nofterms;

int main( int argc, char **argv )
{
    double x, y;

    if( argc!=3 ){
        fprintf( stderr,  "usage:\n" );
        fprintf( stderr,  "   run_w_of_z <x> <y>\n" );
        exit(-1);
    }

    x = atof( argv[1] );
    y = atof( argv[2] );

    double _Complex w = w_of_z( C(x,y) );
    
    double v[2][2];
    v[0][0] = creal(w);
    v[0][1] = cimag(w);

    printf( "%25.19g %25.19g %3i %3i\n", v[0][0], v[0][1],
            faddeeva_algorithm, faddeeva_nofterms );
/*
    // requires activation of lib/experimental.c

    // comparison with Fourier integration
    v[1][0] = cerf_experimental_rew(x,y);
    v[1][1] = cerf_experimental_imw(x,y);
    printf( "%25.19g %25.19g\n", v[1][0], v[1][1] );
    for( int i=0; i<2; ++i )
        printf( "%25.19g ", (v[0][i]-v[1][i])/v[1][i] );
    printf( "\n" );
*/
    return 0;
}
