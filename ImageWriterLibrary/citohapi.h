/*
 *  citohapi.h
 *  C-ITOH 8510, Apple DMP, Apple ImageWriter, Apple Imagewriter II abstraction layer
 *  (c) 2013 Daniele Cattaneo
 */

#include <stdio.h>
#include <stdlib.h>
#include "imwlibver.h"

#ifndef IMAGEWRITERLIBRARY_CITOHAPI_H
#define IMAGEWRITERLIBRARY_CITOHAPI_H

#define ERR_IMWAPI_NOERR              0
#define ERR_IMWAPI_UNKNOWN           -1
#define ERR_IMWAPI_OUTOFMEMORY   -0x100
#define ERR_IMWAPI_WRONGORDER    -0x101
#define ERR_IMWAPI_INVALIDPARAM  -0x102

#define IMWAPI_BIDIRECTIONAL          0
#define IMWAPI_LEFTTORIGHT            1

#define IMWAPI_ASCIIMODE              0
#define IMWAPI_8BITMODE               1

#define IMWAPI_6LPI                  -1
#define IMWAPI_8LPI                  -2

typedef enum {
  kAmerican = 0,
  kBritish,
  kGerman,
  kFrench,
  kSwedish,
  kItalian,
  kSpanish,
  kDanish
} charSet;

typedef enum {
  kDraft = 0,
  kStandard,
  kNLQ
} printFont;

typedef struct {
  FILE *s_out;
  FILE *s_in;
  int headPos;
} printer, *printerRef;

printerRef prnAlloc(FILE *output, FILE *input, int *err);
int prnDealloc(printerRef prn);

int prnTextPrint(printerRef prn, char *text);
int prnHardwareQuery(printerRef prn, char *answer);
int prnSetHorizontalResolution(printerRef prn, int res);
int prnFormFeed(printerRef prn);
int prnSetLineHeight(printerRef prn, int rows);
int prnSetFormHeight(printerRef prn, int rows);
int prnGraphicStripePrint(printerRef prn, const uint8_t stripe[], int swidth, int optimizeWidth);
int prnGraphicGoToX(printerRef prn, int swidth);
int prnCarriageReturnLineFeed(printerRef prn);
int prnSetBidirectionalMode(printerRef prn, int bidi);
int prnSetHighBitMode(printerRef prn, int ascii);
int prnResetPrinterStatus(printerRef prn);
int prnSelectCharacterSet(printerRef prn, int mouseText, charSet lang);
int prnSelectFont(printerRef prn, printFont font);

#endif
