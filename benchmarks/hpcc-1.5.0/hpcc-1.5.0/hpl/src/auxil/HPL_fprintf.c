/* 
 * -- High Performance Computing Linpack Benchmark (HPL)                
 *    HPL - 2.0 - September 10, 2008                          
 *    Antoine P. Petitet                                                
 *    University of Tennessee, Knoxville                                
 *    Innovative Computing Laboratory                                 
 *    (C) Copyright 2000-2008 All Rights Reserved                       
 *                                                                      
 * -- Copyright notice and Licensing terms:                             
 *                                                                      
 * Redistribution  and  use in  source and binary forms, with or without
 * modification, are  permitted provided  that the following  conditions
 * are met:                                                             
 *                                                                      
 * 1. Redistributions  of  source  code  must retain the above copyright
 * notice, this list of conditions and the following disclaimer.        
 *                                                                      
 * 2. Redistributions in binary form must reproduce  the above copyright
 * notice, this list of conditions,  and the following disclaimer in the
 * documentation and/or other materials provided with the distribution. 
 *                                                                      
 * 3. All  advertising  materials  mentioning  features  or  use of this
 * software must display the following acknowledgement:                 
 * This  product  includes  software  developed  at  the  University  of
 * Tennessee, Knoxville, Innovative Computing Laboratory.             
 *                                                                      
 * 4. The name of the  University,  the name of the  Laboratory,  or the
 * names  of  its  contributors  may  not  be used to endorse or promote
 * products  derived   from   this  software  without  specific  written
 * permission.                                                          
 *                                                                      
 * -- Disclaimer:                                                       
 *                                                                      
 * THIS  SOFTWARE  IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY
 * OR  CONTRIBUTORS  BE  LIABLE FOR ANY  DIRECT,  INDIRECT,  INCIDENTAL,
 * SPECIAL,  EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES  (INCLUDING,  BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA OR PROFITS; OR BUSINESS INTERRUPTION)  HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT,  STRICT LIABILITY,  OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 * ---------------------------------------------------------------------
 */ 
/*
 * Include files
 */
#include "hpl.h"

#ifdef HPL_STDC_HEADERS
void HPL_fprintf
(
   FILE *                           STREAM,
   const char *                     FORM,
   ...                              
)
#else
void HPL_fprintf( va_alist )
va_dcl
#endif
{
/* 
 * Purpose
 * =======
 *
 * HPL_fprintf is a wrapper around fprintf flushing the output stream.
 * 
 *
 * Arguments
 * =========
 *
 * STREAM  (local input)                 FILE *
 *         On entry, STREAM specifies the output stream.
 *
 * FORM    (local input)                 const char *
 *         On entry, FORM specifies the format, i.e., how the subsequent
 *         arguments are converted for output.
 *
 *         (local input)                 ...
 *         On entry,  ...  is the list of arguments to be printed within
 *         the format string.
 *
 * ---------------------------------------------------------------------
 */ 
/*
 * .. Local Variables ..
 */
   va_list                    argptr;
   char                       cline[256];
#ifndef HPL_STDC_HEADERS
   FILE                       * STREAM;
   char                       * FORM;
#endif
/* ..
 * .. Executable Statements ..
 */
#ifdef HPL_STDC_HEADERS
   va_start( argptr, FORM );
#else
   va_start( argptr );
   STREAM = va_arg( argptr, FILE * );
   FORM   = va_arg( argptr, char * );
#endif
   (void) vsprintf( cline, FORM, argptr );
   va_end( argptr ); 

   (void) fprintf( STREAM, "%s", cline );
   (void) fflush( STREAM );
/*
 * End of HPL_fprintf
 */
} 
