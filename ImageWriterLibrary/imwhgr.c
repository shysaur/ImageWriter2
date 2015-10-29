/*
 *  imwhgr.c
 *  Apple Imagewriter bitmap output helper
 *  (c) 2013 Daniele Cattaneo
 */

#include <stdio.h>
#include <string.h>
#include "imwhgr.h"


int hgrPrintBitmapStripeH8Wk8(printerRef prn, const uint8_t bitmap[], int chrWidth) {
  uint8_t *stripes;
  int i, j;
  int err;
  
  stripes = malloc(chrWidth * 8);
  if (!stripes)
    return ERR_IMWAPI_OUTOFMEMORY;
  
  for (i=0; i<chrWidth; i++)
    for (j=0; j<8; j++)
      stripes[i*8+j] =
        ((bitmap[i+0*chrWidth] >> (7-j) & 1) << 0) |
        ((bitmap[i+1*chrWidth] >> (7-j) & 1) << 1) |
        ((bitmap[i+2*chrWidth] >> (7-j) & 1) << 2) |
        ((bitmap[i+3*chrWidth] >> (7-j) & 1) << 3) |
        ((bitmap[i+4*chrWidth] >> (7-j) & 1) << 4) |
        ((bitmap[i+5*chrWidth] >> (7-j) & 1) << 5) |
        ((bitmap[i+6*chrWidth] >> (7-j) & 1) << 6) |
        ((bitmap[i+7*chrWidth] >> (7-j) & 1) << 7);
  err = prnGraphicStripePrint(prn, stripes, chrWidth*8, 1);
  free(stripes);
  return err;
}


int hgrInterleaveBitmapH16Wk8(const uint8_t inbmp[], uint8_t outbmp[], int chrWidth) {
  int i;
  
  for (i=0; i<8; i++)
    memcpy(&outbmp[i*chrWidth], &inbmp[2*i*chrWidth], chrWidth);
  for (i=0; i<8; i++)
    memcpy(&outbmp[(i+8)*chrWidth], &inbmp[(2*i+1)*chrWidth], chrWidth);
  return 0;
}


int hgrPrintBitmapStripeHiresH16Wk8(printerRef prn, const uint8_t bitmap[], int chrWidth) {
  uint8_t *intbmp;
  int err;
  
  intbmp = malloc(chrWidth * 16);
  if (!intbmp)
    return ERR_IMWAPI_OUTOFMEMORY;
  
  err = hgrInterleaveBitmapH16Wk8(bitmap, intbmp, chrWidth);
  if (err) goto error_catch;
  err = prnSetLineHeight(prn, 1);
  if (err) goto error_catch;
  err = hgrPrintBitmapStripeH8Wk8(prn, intbmp, chrWidth);
  if (err) goto error_catch;
  err = prnCarriageReturnLineFeed(prn);
  if (err) goto error_catch;
  err = hgrPrintBitmapStripeH8Wk8(prn, intbmp+8*chrWidth, chrWidth);
  if (err) goto error_catch;
  err = prnSetLineHeight(prn, 15);
  if (err) goto error_catch;
  err = prnCarriageReturnLineFeed(prn);
  
error_catch:
  free(intbmp);
  return err;
}






