/* Library libcerf:
 *   Compute complex error functions, based on a new implementation of
 *   Faddeeva's w_of_z. Also provide Dawson and Voigt functions.
 *
 * File runfwhm.c:
 *   Interactive evaluation of voigt_fwhm.
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

int main( int argc, char **argv )
{
    double s, g;

    if( argc!=3 ){
        fprintf( stderr,  "usage:\n" );
        fprintf( stderr,  "   runfwhm <sigma> <gamma>\n" );
        exit(-1);
    }

    s = atof( argv[1] );
    g = atof( argv[2] );

    double y = voigt_hwhm(s,g);
    printf( "hwhm=%25.19g fwhm=%25.19g\n", y, 2*y );
    return 0;
}
