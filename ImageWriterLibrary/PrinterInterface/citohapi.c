//
//  citohapi.c
//  ImageWriterLibrary
//
//  Created by Daniele Cattaneo on 28/11/13.
//  Copyright (c) 2013 Daniele Cattaneo. All rights reserved.
//

#include "citohapi.h"

printerRef prnAlloc(FILE *output, FILE *input, int *err) {
  printerRef prn;
  if (output) {
    prn = malloc(sizeof(printer));
    if (prn) {
      prn->s_in = input;
      prn->s_out = output;
      return prn;
    } else
      if (err) *err = ERR_IMWAPI_OUTOFMEMORY;
  } else
    if (err) *err = ERR_IMWAPI_INVALIDPARAM;
  return NULL;
}

int prnDealloc(printerRef prn) {
  free(prn);
  return 0;
}

int prnHardwareQuery(printerRef prn, char *answer) {
  if (prn == NULL) return ERR_IMWAPI_UNKNOWN;
  if (answer == NULL) return ERR_IMWAPI_UNKNOWN;
  if (prn->s_in == NULL || prn->s_in == stdin) return ERR_IMWAPI_WRONGORDER;
  fflush(prn->s_out);
  fflush(prn->s_in);
  fputs("\0x1B?", prn->s_out);
  fread(answer, 3, 1, prn->s_in);
  return 0;
}

int prnTextPrint(printerRef prn, char *text) {
  if (prn == NULL) return ERR_IMWAPI_UNKNOWN;
  fputs(text, prn->s_out);
  prn->headPos = -1;
  return 0;
}

int prnSetHorizontalResolution(printerRef prn, int res) {
  char rc;
  
  if (prn == NULL) return ERR_IMWAPI_UNKNOWN;
  switch (res) {
    case  72: rc = 'n'; break;
    case  80: rc = 'N'; break;
    case  96: rc = 'E'; break;
    case 107: rc = 'e'; break;
    case 120: rc = 'q'; break;
    case 136: rc = 'Q'; break;
    case 144: rc = 'p'; break;
    case 160: rc = 'P'; break;
    default:
      return ERR_IMWAPI_WRONGORDER;
  }
  fprintf(prn->s_out, "\033%c", rc);
  return 0;
}

int prnFormFeed(printerRef prn) {
  if (prn == NULL) return ERR_IMWAPI_UNKNOWN;
  fprintf(prn->s_out, "\014");
  prn->headPos = 0;
  return 0;
}

int prnSetLineHeight(printerRef prn, int rows) {
  if (prn == NULL) return ERR_IMWAPI_UNKNOWN;
  if (rows < 1 || rows > 99) return ERR_IMWAPI_WRONGORDER;
  fprintf(prn->s_out, "\033T%02d", rows);
  return 0;
}

int prnSetFormHeight(printerRef prn, int rows) {
  if (prn == NULL) return ERR_IMWAPI_UNKNOWN;
  if (rows < 1 || rows > 9999) return ERR_IMWAPI_WRONGORDER;
  fprintf(prn->s_out, "\033H%04d", rows);
  return 0;
}

/* TODO: improve this with RLE compression */
int prnGraphicStripePrint(printerRef prn, const uint8_t stripe[], int swidth) {
  if (prn == NULL) return ERR_IMWAPI_UNKNOWN;
  if (swidth < 1 || swidth > 9999) return ERR_IMWAPI_WRONGORDER;
  fprintf(prn->s_out, "\033G%04d", swidth);
  fwrite(stripe, sizeof(uint8_t), swidth, prn->s_out);
  if (prn->headPos > 0) prn->headPos += swidth;
  return 0;
}

int prnGraphicGoToX(printerRef prn, int swidth) {
  if (prn == NULL) return ERR_IMWAPI_UNKNOWN;
  if (swidth < 1 || swidth > 9999) return ERR_IMWAPI_WRONGORDER;
  fprintf(prn->s_out, "\033F%04d", swidth);
  prn->headPos = swidth;
  return 0;
}

int prnCarriageReturnLineFeed(printerRef prn) {
  if (prn == NULL) return ERR_IMWAPI_UNKNOWN;
  fprintf(prn->s_out, "\r\n");
  prn->headPos = 0;
  return 0;
}

