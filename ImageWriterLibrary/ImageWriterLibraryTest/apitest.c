//
//  main.c
//  ImageWriterLibraryTest
//
//  Created by Daniele Cattaneo on 28/11/13.
//  Copyright (c) 2013 Daniele Cattaneo. All rights reserved.
//

#include <stdio.h>
#include <errno.h>
#include "citohapi.h"
#include "imwhgr.h"

const uint8_t test_gfx[] = {
  0b00111100,
  0b01000010,
  0b10101001,
  0b10000101,
  0b10000101,
  0b10101001,
  0b01000010,
  0b00111100
};

const uint8_t test_gfx_linear[] = {
  0b10000000,0b00111000,0b00111100,0b00000000,0b10000000,0b10000000,0b10000000,0b01010101,
  0b10000000,0b01101000,0b00011000,0b00000000,0b00000000,0b00000000,0b00000000,0b10101011,
  0b10000000,0b11001100,0b00011000,0b00000000,0b00000000,0b00000000,0b00000000,0b01010101,
  0b10000001,0b10000100,0b00110000,0b00000000,0b00000000,0b00000000,0b00000000,0b10101011,
  0b10000011,0b11111110,0b00110000,0b00000000,0b00000000,0b00000000,0b00000000,0b01010101,
  0b10000110,0b00000010,0b01100000,0b00010000,0b00000000,0b00000000,0b00000000,0b10101011,
  0b10001100,0b00000011,0b01100000,0b00110000,0b00000000,0b00000000,0b00000000,0b01010101,
  0b10111110,0b00000111,0b11111111,0b11100000,0b00000001,0b00000001,0b00000001,0b10101011
};

int main(int argc, const char *argv[]) {
  printerRef prn;
  FILE *output, *input;
  
  input = output = NULL;
  if (argc > 2)
    input = fopen(argv[2], "rb");
  if (argc > 1)
    output = fopen(argv[1], "wb");
  else {
    input = NULL;
    output = stdout;
  }
  
  prn = prnAlloc(output, input, NULL);
  prnSetHorizontalResolution(prn, 72);
  prnTextPrint(prn, "New Stuff Test! New Graphics!");
  prnCarriageReturnLineFeed(prn);
  hgrPrintBitmapStripeH8Wk8(prn, test_gfx_linear, 8);
  prnFormFeed(prn);
  prnDealloc(prn);
  
  return 0;
}

