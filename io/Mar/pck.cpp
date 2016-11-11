/***********************************************************************
 *
 * pck
 *
 *********************************************************************
 * Copyright 2015,   Claudio Klein, marXperts GmbH
 * Original code by Prof. Dr. Jan Pieter Abrahams, PSI Switzerland
 *
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
 * Version: 	2.1
 * Date:	01/08/1996
 *
 **********************************************************************/
#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#include "MarReader.h"

/*
 * Definitions
 */
#define BYTE char
#define WORD short int
#define LONG int

#define PACKIDENTIFIER "\nCCP4 packed image, X: %04d, Y: %04d\n"
#define PACKBUFSIZ BUFSIZ
#define DIFFBUFSIZ 16384L
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))
#define abs(x) (((x) < 0) ? (-(x)) : (x))
const LONG setbits[33] = {0x00000000L, 0x00000001L, 0x00000003L, 0x00000007L,
              0x0000000FL, 0x0000001FL, 0x0000003FL, 0x0000007FL,
              0x000000FFL, 0x000001FFL, 0x000003FFL, 0x000007FFL,
              0x00000FFFL, 0x00001FFFL, 0x00003FFFL, 0x00007FFFL,
              0x0000FFFFL, 0x0001FFFFL, 0x0003FFFFL, 0x0007FFFFL,
              0x000FFFFFL, 0x001FFFFFL, 0x003FFFFFL, 0x007FFFFFL,
              0x00FFFFFFL, 0x01FFFFFFL, 0x03FFFFFFL, 0x07FFFFFFL,
              0x0FFFFFFFL, 0x1FFFFFFFL, 0x3FFFFFFFL, 0x7FFFFFFFL,
                           LONG(0xFFFFFFFFL)};
#define shift_left(x, n)  (((x) & setbits[32 - (n)]) << (n))
#define shift_right(x, n) (((x) >> (n)) & setbits[32 - (n)])

/*
 * Local functions
 */
void 		get_pck		(FILE *,           WORD *);
static void unpack_word	(FILE *, int, int, WORD *);

/***************************************************************************
 * Function: get_pck
 ***************************************************************************/
void
get_pck(FILE *fp, WORD *img)
{
int x = 0, y = 0, i = 0, c = 0;
char header[BUFSIZ];

    if (fp == NULL) return;

    rewind( fp );
    header[0] = '\n';
    header[1] = 0;

    /*
     * Scan file until PCK header is found
     */
    while ((c != EOF) && ((x == 0) || (y == 0))) {
        c = i = x = y = 0;
        while ((++i < BUFSIZ) && (c != EOF) && (c != '\n') && (x==0) && (y==0))
            if ((header[i] = c = getc(fp)) == '\n')
                sscanf(header, PACKIDENTIFIER, &x, &y);
    }

    unpack_word(fp, x, y, img);
}


/*****************************************************************************
 * Function: unpack_word
 * Unpacks a packed image into the WORD-array 'img'.
 *****************************************************************************/
static void
unpack_word(FILE *packfile, int x, int y, WORD *img)
{
int 		valids = 0, spillbits = 0, usedbits, total = x * y;
LONG 		window = 0L, spill, pixel = 0, nextint, bitnum, pixnum;
static int 	bitdecode[8] = {0, 4, 5, 6, 7, 8, 16, 32};

    while (pixel < total) {
        if (valids < 6) {
            if (spillbits > 0) {
                window |= shift_left(spill, valids);
                valids += spillbits;
                spillbits = 0;
        }
                else {
                spill = (LONG) getc(packfile);
                spillbits = 8;
        }
        }
            else {
            pixnum = 1 << (window & setbits[3]);
            window = shift_right(window, 3);
            bitnum = bitdecode[window & setbits[3]];
            window = shift_right(window, 3);
            valids -= 6;
            while ((pixnum > 0) && (pixel < total)) {
                if (valids < bitnum) {
                if (spillbits > 0) {
                    window |= shift_left(spill, valids);
                        if ((32 - valids) > spillbits) {
                        valids += spillbits;
                        spillbits = 0;
                }
                        else {
                        usedbits = 32 - valids;
                    spill = shift_right(spill, usedbits);
                    spillbits -= usedbits;
                    valids = 32;
                }
            }
                else {
                    spill = (LONG) getc(packfile);
                        spillbits = 8;
            }
            }
                else {
                --pixnum;
            if (bitnum == 0)
                        nextint = 0;
                else {
                    nextint = window & setbits[bitnum];
                        valids -= bitnum;
                        window = shift_right(window, bitnum);
                    if ((nextint & (1 << (bitnum - 1))) != 0)
                        nextint |= ~setbits[bitnum];
            }
                if (pixel > x) {
                    img[pixel] = (WORD) (nextint +
                                      (img[pixel-1] + img[pixel-x+1] +
                                       img[pixel-x] + img[pixel-x-1] + 2) / 4);
                        ++pixel;
            }
                else if (pixel != 0) {
                    img[pixel] = (WORD) (img[pixel - 1] + nextint);
                        ++pixel;
            }
                else
                        img[pixel++] = (WORD) nextint;
            }
        }
        }
    }
}

