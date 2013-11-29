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

const uint8_t test_gfx_linear_hires[] = {
  0xE0,0x01,0x00,0x00,0x00,0xAA,0xAA,0xAF,
  0x80,0x02,0x40,0x00,0x00,0x55,0x55,0x55,
  0x80,0x04,0x40,0x00,0x00,0xAA,0xAA,0xAB,
  0x83,0x04,0x40,0x00,0x08,0x55,0x55,0x55,
  0x84,0x8C,0x00,0x00,0x08,0xAA,0xAA,0xAB,
  0x88,0x88,0x00,0x00,0x08,0x55,0x55,0x55,
  0x88,0x88,0x8B,0x73,0x88,0xAA,0xAA,0xAB,
  0x89,0x09,0x9C,0x94,0x90,0x55,0x55,0x55,
  0x81,0x0A,0xA9,0x14,0x10,0xAA,0xAA,0xAB,
  0x83,0xF8,0x89,0x24,0x10,0x55,0x55,0x55,
  0x81,0x11,0x11,0x26,0x10,0xAA,0xAA,0xAB,
  0x81,0x11,0x11,0x43,0x10,0x55,0x55,0x55,
  0x82,0x11,0x11,0x81,0x00,0xAA,0xAA,0xAB,
  0x84,0x11,0x51,0x11,0x00,0x55,0x55,0x55,
  0x84,0x11,0x51,0x19,0x20,0xAA,0xAA,0xAB,
  0xD8,0x1D,0x90,0xEE,0x20,0x55,0x55,0x5F
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
  prnSetBidirectionalMode(prn, IMWAPI_LEFTTORIGHT);
  prnTextPrint(prn, "New Stuff Test! New Graphics! This is LORES:");
  prnCarriageReturnLineFeed(prn);
  hgrPrintBitmapStripeH8Wk8(prn, test_gfx_linear, 8);
  prnCarriageReturnLineFeed(prn);
  prnTextPrint(prn, "This is the newest HIRES:");
  prnCarriageReturnLineFeed(prn);
  hgrPrintBitmapStripeHiresH16Wk8(prn, test_gfx_linear_hires, 8);
  prnFormFeed(prn);
  
  prnDealloc(prn);
  
  return 0;
}

