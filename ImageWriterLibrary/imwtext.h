/*
 *  imwhgr.c
 *  Apple Imagewriter text output
 *  (c) 2013 Daniele Cattaneo
 */

#include "citohapi.h"


#ifndef IMAGEWRITERLIBRARY_IMWTEXT_H
#define IMAGEWRITERLIBRARY_IMWTEXT_H

int prnISO8859TextPrint(printerRef prn, const char *s);
int prnEncodedTextPrint(printerRef prn, const char *s, const char *fromcode);
int prnEncodedTextPrintF(printerRef prn, const char *fromcode, const char *format, ...);

#endif
