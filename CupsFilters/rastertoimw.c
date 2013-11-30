//
//  main.c
//  CupsFilters
//
//  Created by Daniele Cattaneo on 29/11/13.
//  Copyright (c) 2013 Daniele Cattaneo. All rights reserved.
//

#include <stdio.h>
#include <cups/raster.h>
#include <signal.h>
#include <errno.h>

#include "citohapi.h"
#include "imwhgr.h"

typedef struct {
  int job_id;
  const char *user,	*title;
  int num_options;
  cups_option_t	*options;
} job_data_t;


int stripeIsEmpty(const uint8_t stripe[], int charWidth, int height) {
  int i, j, k;
  
  k=0;
  for (i=0; i<height; i++)
    for (j=0; j<charWidth; j++)
      if (stripe[k++] != 0) return 0;
  
  return 1;
}


int main(int argc, const char *argv[]) {
  ppd_file_t *ppd;
  cups_raster_t *ras;
  cups_page_header2_t	pagehdr;
  ppd_choice_t *temp;
  job_data_t job;
  printerRef prn;
  int rasterfile;
  int page;
  
  setbuf(stderr, NULL);
  if (argc < 6 || argc > 7) {
    fprintf(stderr, "Usage: %s job user title copies options [filename]\n", argv[0]);
    return 0;
  }
  
  job.job_id      = atoi(argv[1]);
  job.user        = argv[2];
  job.title       = argv[3];
  job.num_options = cupsParseOptions(argv[5], 0, &(job.options));
  
  if ((ppd = ppdOpenFile(getenv("PPD"))) != NULL) {
    ppdMarkDefaults(ppd);
    cupsMarkOptions(ppd, job.num_options, job.options);
  } else
    fprintf(stderr, "ERROR: Unable to open PPD file: %s\n", strerror(errno));
  
  if (argc == 7) {
    if ((rasterfile = open(argv[6], O_RDONLY)) == -1) {
      fprintf(stderr, "ERROR: Unable to open raster file - %s\n", strerror(errno));
      return (1);
    }
  } else
    rasterfile = 0;
  
  
  page = 0;
  ras = cupsRasterOpen(rasterfile, CUPS_RASTER_READ);
  prn = prnAlloc(stdout, NULL, NULL);
  
  if ((temp = ppdFindMarkedChoice(ppd, "Bidirectional"))) {
    if (!strcmp(temp->choice, "True"))
      prnSetBidirectionalMode(prn, IMWAPI_BIDIRECTIONAL);
    else
      prnSetBidirectionalMode(prn, IMWAPI_LEFTTORIGHT);
  }
  
  while (cupsRasterReadHeader2(ras, &pagehdr)) {
    uint8_t *curStripe;
    int y, row;
    int hgr;
    int lineskip;
    
    if (pagehdr.cupsBitsPerPixel > 1) {
      fprintf(stderr, "ERROR: Color page not supported!\n");
      return 1;
    }
    hgr = (pagehdr.HWResolution[1] == 144);
    if (hgr)
      curStripe = malloc(pagehdr.cupsBytesPerLine*16);
    else
      curStripe = malloc(pagehdr.cupsBytesPerLine*8);
    prnSetHorizontalResolution(prn, pagehdr.HWResolution[0]);
    
    page++;
    fprintf(stderr, "PAGE: %d %d\n", page, pagehdr.NumCopies);
    prnFormFeed(prn);
    row = 0;
    lineskip = 0;
    
    for (y=0; y<pagehdr.cupsHeight; y++) {
      cupsRasterReadPixels(ras, &curStripe[row*pagehdr.cupsBytesPerLine], pagehdr.cupsBytesPerLine);
      row++;
      
      if (row >= 8+(8*hgr)) {
        if (!stripeIsEmpty(curStripe, pagehdr.cupsBytesPerLine, 8+(8*hgr))) {
          if (lineskip > 0) {
            prnSetLineHeight(prn, lineskip);
            prnCarriageReturnLineFeed(prn);
            lineskip = 0;
          }
          if (hgr)
            hgrPrintBitmapStripeHiresH16Wk8(prn, curStripe, pagehdr.cupsBytesPerLine);
          else {
            prnSetLineHeight(prn, 16);
            hgrPrintBitmapStripeH8Wk8(prn, curStripe, pagehdr.cupsBytesPerLine);
            prnCarriageReturnLineFeed(prn);
          }
        } else {
          lineskip += 16;
          if (lineskip > 99) {
            prnSetLineHeight(prn, 99);
            prnCarriageReturnLineFeed(prn);
            lineskip -= 99;
          }
        }
        row = 0;
      }
      
      if (y * 25 % pagehdr.cupsHeight < 25)
        fprintf(stderr, "INFO: %d%%\n", y*100/pagehdr.cupsHeight);
    }
    
    free(curStripe);
  }
  
  prnFormFeed(prn);
  prnDealloc(prn);
  if (page == 0) {
    fprintf(stderr, "ERROR: No pages found!\n");
    return 1;
  } else {
    fprintf(stderr, "INFO: Ready to print.\n");
  }
  return 0;
}


