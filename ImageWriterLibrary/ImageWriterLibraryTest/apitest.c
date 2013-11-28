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
  prnTextPrint(prn, "Advancement test");
  prnGraphicGoToX(prn, 100);
  prnGraphicStripePrint(prn, test_gfx, sizeof(test_gfx));
  prnTextPrint(prn, "\n\r");
  prnTextPrint(prn, "short!");
  prnGraphicGoToX(prn, 100);
  prnGraphicStripePrint(prn, test_gfx, sizeof(test_gfx));
  prnFormFeed(prn);
  prnDealloc(prn);
  
  return 0;
}

