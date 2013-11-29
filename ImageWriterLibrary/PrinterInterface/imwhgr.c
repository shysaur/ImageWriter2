//
//  imwhgr.c
//  ImageWriterLibrary
//
//  Created by Daniele Cattaneo on 29/11/13.
//  Copyright (c) 2013 Daniele Cattaneo. All rights reserved.
//

#include <stdio.h>
#include "imwhgr.h"

int hgrPrintBitmapStripeH8Wk8(printerRef prn, const uint8_t bitmap[], int chrWidth) {
  uint8_t *stripes;
  int i, j;
  
  stripes = malloc(chrWidth * 8);
  if (stripes) {
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
    return prnGraphicStripePrint(prn, stripes, chrWidth*8);
  } else
    return ERR_IMWAPI_OUTOFMEMORY;
}
