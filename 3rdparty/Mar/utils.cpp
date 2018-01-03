/*********************************************************************
 *
 * io: 		utils.c
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
 * Ver      Date	Description
 * 1.3  10/08/2015	Made public under Apache License 2.0
 * 1.2	09/02/2007	Resol2stol, Resol2twotheta added
 * 1.1	09/12/2005	SplitmarName added
 * 1.0	05/07/1996	Original version
 *********************************************************************/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <math.h>
#ifdef __unix__
      #include <unistd.h>
#elif __MSDOS__ || __WIN32__ || _MSC_VER
      #include <io.h>
#endif

#include "MarReader.h"

/*
 * Definitions
 */
#define STRING		0
#define INTEGER		1
#define FLOAT		2

/*
 * Local functions
 */
int		InputType		(char *);
void	WrongType		(int, char *, char *);
float	GetResol		(float, float, float);
float 	Resol2twotheta	(float,float );
float 	Resol2stol		(float, float);
void	RemoveBlanks	(char *);
//char 	*isdir			(int createdir, char *dir);
int 	SplitmarName	(char *, char *, char *,int *);

/*****************************************************************
 * Function: isdir 
 *****************************************************************/
/*char *isdir( int createdir, char *dir)
{
int		i;
char		*p = dir;
struct stat	sbuf;

	i = lstat( dir, &sbuf );

	if ( !createdir ) {
		if ( i ==(-1) ) return ( NULL );
		return ( p );
	}

	// So we want to create this directory ...
	if ( i == (-1) ) {
		i=mkdir( dir, S_IRWXU | S_IRGRP | S_IROTH | S_IXGRP | S_IXOTH );
	}
	if ( i!=0 ) {
		return( NULL );
	}
	return ( p );
}*/

/******************************************************************
 * Function: Resol2stol
 ******************************************************************/
float Resol2stol(float resol, float wave)
{
float	theta;
float	stol;
double	d;

	if ( resol < 0.0001 ) return 0.0;
	if ( wave <=  0.f ) wave = 1.541789;
	
	theta = Resol2twotheta( resol, wave ) / 2.0;

	/* stol = 4 * ( sin(theta)/ lambda ) ** 2 */

	d = sin(theta)/wave;

	stol = 4.0 * d * d;

	return( stol );
}


/******************************************************************
 * Function: Resol2twotheta from resolution, wavelength and distance
 ******************************************************************/
float Resol2twotheta(float reso, float wave)
{
float result=0.0;

        /* Return 2-theta [rad] from resolution & distance */
        if (reso != 0.0 )
                        result  = 2.0 * asin( wave/(reso*2.0));
        return result;
}                  
	
/******************************************************************
 * Function: GetResol: get resolution from radius (mm), distance and wavelength
 ******************************************************************/
float GetResol(float radius, float distance, float wave)
{
float	res;

	if ( distance < 1.0 ) return 0.0;

	res = atan( radius/distance); 	/* Two theta !!! */

	if ( res < 0.0001 ) return 0.0;

	res = wave/(2.0*sin(res/2.0)); 
	
	return( res );
}
	
/******************************************************************
 * Function: InputType 
 ******************************************************************/
int InputType(char *string)
{
int		i,j=0,k=0,l=0;

   	for ( i=0 ; i<strlen(string)-1; i++ ) {
		if ( isspace( string[i] ) ) {
			string[i] = ' ';
			j++;
		}
		else if ( string[i] == '.' ) {
			k++;
			continue;
		}

		else if ( string[i] == '-' || string[i] == '+' ) {
			l++;
			continue;
		}
		else if ( isdigit( string[i] ) ) {
			j++;
			continue;
		}
	}

	if ( k == 1 && l == 0 && j == i-1 )
		return( FLOAT );
	else if ( k == 1 && l == 1 && j == i-2 )
		return( FLOAT );
	else if ( k == 0 && l == 1 && j == i-1 )
		return( INTEGER );
	else if ( k == 0 && l == 0 && j == i )
		return( INTEGER );
	else
		return( STRING  );

}

/******************************************************************
 * Function: WrongType 
 ******************************************************************/
void WrongType(int type, char *key, char *string)
{

	fprintf(stderr,"INPUT ERROR at key '%s' and argument '%s'!\n",key,string);

	if ( type == FLOAT )
		fprintf(stderr,"            Expected data type is: FLOAT...\n");
	else if ( type == INTEGER )
		fprintf(stderr,"            Expected data type is: INTEGER...\n");

}

/******************************************************************
 * Function: RemoveBlanks = remove white space from string
 ******************************************************************/
void RemoveBlanks(char *str)
{
int i, j=0, len=strlen(str);

        for(i=0;i<len;i++) {
                if (!isspace(str[i]))
                        str[j++] = str[i];
        }
        if ( j>0)
                str[j] = 0;
}

/******************************************************************
 * Function: 	SplitmarName
 * Description:	A string xtal_000.mar2300 is separated into components
 * Input:	name
 * Output:	prefix    = prefix 
 *		suffix    = file name extension after last dot
 *		ires	  = 4*int32 with results
 *			a) no        = current image number
 *			b) ndigits   = length of image number component
 *			c) suffix_nx = size of image derived from extension
 *			d) sep       = separator preceding image number
 ******************************************************************/
int SplitmarName(char *name, char *prefix, char *suffix, int *ires)
{
int	no;
int	ndigits;
int	suffix_nx;
char 	sep;
char	str[128];
int 	i, n;
char	*c;

	/* The last dot is the filename suffix extension */
	strcpy( str, name);
	c = strrchr ( str, '.' );
	if ( c != NULL ) {
		if ( suffix != NULL ) {
			/* Extract a digit with the size of the image from suffix */
			strcpy( suffix, c+1);
			i = strlen( suffix );
			n = 0;
			while( i ) {
				if ( !isdigit( suffix[i-1] ) ) break;
				n++;
				i--;
			}
			if ( n ) 
				suffix_nx = atoi( suffix + i);
			else
				suffix_nx = -1;
		}

		/* Chop off the dot */
		*c = '\0';
	}

	/* The last part of remaining string should contain some digits */	
	n = 0;
	i = strlen( str );
	while( i ) {
		if ( !isdigit( str[i-1] ) ) break;
		n++;
		i--;
	}

	/* So here we go with the image no */
	if ( n ) {
		ndigits = n;
		no = atoi( str + i);
		sep = str[i-1];
		if ( sep == '.' || sep == '_' || sep == '-' ) str[i-1] = '\0';
	}
	else {
		ndigits = 0;
		no = -1;
		sep = '0';	
	}

	/* Copy remaining part of the string into prefix. Separator has been chopped off */
	if ( prefix != NULL ) {
		strcpy( prefix, str);
	}

	ires[0] = no;
	ires[1] = ndigits;
	ires[2] = suffix_nx;
	ires[3] = sep;

	return no;  
}
