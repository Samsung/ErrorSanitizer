/*
    Copyright (c) 2018 - 2019, Samsung Electronics Co., Ltd. All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

    Author: Jakub Botwicz <j.botwicz@samsung.com>
    Author: Mateusz Nosek <m.nosek@samsung.com>
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "esan_wrapper.h"

int perform_testing(uint8_t* buffer_ptr, size_t buffer_size)
{

    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;
    int int_result;

    pFile = fopen ("Makefile" , "rb");
    if (pFile==NULL) {printf ("File error"); exit (1);}
    printf("fopen - SUCCESS!\n");

    /* obtain file size: */
    int_result = fseeko(pFile , 0 , SEEK_END);
    if (int_result) {printf ("fseeko error"); exit (1);}
    printf("fseek - SUCCESS!\n");
    lSize = ftello(pFile);
    if (lSize == -1) {printf ("ftello error"); exit (1);}
    printf("ftell - SUCCESS!\n");
    if (fseeko(pFile, 0L, SEEK_SET) != 0) {
        fprintf (stderr, "fseeko error"); exit (1);
    }
    rewind (pFile);
    printf("rewind - SUCCESS!\n");

    /* allocate memory to contain the whole file: */
    buffer = (char*) malloc (sizeof(char)*lSize);
    printf("malloc - SUCCESS!\n");
    if (buffer == NULL) {printf ("Memory error"); exit (2);}

    /* copy the file into the buffer: */
    result = fread (buffer,1,lSize,pFile);
    printf("fread - SUCCESS!\n");
    if (result != (size_t)lSize) {printf ("Reading error"); exit (3);}

    /* the whole file is now loaded in the memory buffer. */

    /* terminate */
    int_result = fclose (pFile);
    if (int_result) {printf ("fclose error"); exit (1);}
    printf("fclose - SUCCESS!\n");
    free (buffer);
    printf("free - SUCCESS!\n");

    pFile = fopen ( "Makefile" , "rb" );
    printf("fopen - SUCCESS!\n");

    /* obtain file size: */
    fseek (pFile , 0 , SEEK_END);
    printf("fseek - SUCCESS!\n");
    lSize = ftell (pFile);
    printf("ftell - SUCCESS!\n");
    rewind (pFile);
    printf("rewind - SUCCESS!\n");

    /* allocate memory to contain the whole file: */
    buffer = (char*) malloc (sizeof(char)*lSize);
    printf("malloc - SUCCESS!\n");

    /* copy the file into the buffer: */
    result = fread (buffer,1,lSize,pFile);
    printf("fread - SUCCESS!\n");

    /* the whole file is now loaded in the memory buffer. */

    /* terminate */
    fclose (pFile);
    printf("fclose - SUCCESS!\n");
    free (buffer);
    printf("free - SUCCESS!\n");

    return 0;

}

int main(int argc, char **argv) {return main0(argc, argv);}

