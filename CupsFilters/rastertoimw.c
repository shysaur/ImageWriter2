/*
 *  rastertoimw.c
 *  Apple Imagewriter CUPS raster filter
 *  (c) 2013 Daniele Cattaneo
 */

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <cups/cups.h>
#include <cups/ppd.h>
#include <cups/raster.h>

#include "filterlocalizer.h"
#include "citohapi.h"
#include "imwhgr.h"
#include "filtersver.h"


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


int initializeCupsOptions(job_data_t *job, ppd_file_t **ppd, const char *argv[]) {
  char ls[64];
  
  job->job_id      = atoi(argv[1]);
  job->user        = argv[2];
  job->title       = argv[3];
  job->num_options = cupsParseOptions(argv[5], 0, &(job->options));
  
  if ((*ppd = ppdOpenFile(getenv("PPD"))) != NULL) {
    ppdMarkDefaults(*ppd);
    cupsMarkOptions(*ppd, job->num_options, job->options);
  } else {
    l10nGetString("Unable to open PPD file", ls, sizeof(ls));
    fprintf(stderr, "ERROR: %s: %s\n", ls, strerror(errno));
    return 1;
  }
  return 0;
}


int initializeBidirectional(ppd_file_t *ppd, printerRef prn) {
  char ls[64];
  ppd_choice_t *temp;
  
  if ((temp = ppdFindMarkedChoice(ppd, "Bidirectional"))) {
    if (!strcmp(temp->choice, "True"))
      return prnSetBidirectionalMode(prn, IMWAPI_BIDIRECTIONAL);
    else
      return prnSetBidirectionalMode(prn, IMWAPI_LEFTTORIGHT);
  } else {
    prnSetBidirectionalMode(prn, IMWAPI_LEFTTORIGHT);
    l10nGetString("Bidirectional option not found!", ls, sizeof(ls));
    fprintf(stderr, "%s\n", ls);
    return -1;
  }
  return 0;
}


int skipLines(printerRef prn, int *lineskip) {
  int ls;
  
  ls = *lineskip;
  while (ls > 99) {
    prnSetLineHeight(prn, 99);
    prnCarriageReturnLineFeed(prn);
    ls -= 99;
  }
  if (ls > 0) {
    prnSetLineHeight(prn, ls);
    prnCarriageReturnLineFeed(prn);
  }
  *lineskip = 0;
  
  return 0;
}


int outputStripeWithRes(printerRef prn, uint8_t curStripe[], int width, int hgr) {
  if (hgr)
    hgrPrintBitmapStripeHiresH16Wk8(prn, curStripe, width);
  else {
    prnSetLineHeight(prn, 16);
    hgrPrintBitmapStripeH8Wk8(prn, curStripe, width);
    prnCarriageReturnLineFeed(prn);
  }
  return 0;
}


int main(int argc, const char *argv[]) {
  ppd_file_t *ppd;
  cups_raster_t *ras;
  cups_page_header2_t	pagehdr;
  job_data_t job;
  printerRef prn;
  int rasterfile;
  int page;
  char ls[64];
  
  l10nInitialize();
  
  setbuf(stderr, NULL);
  if (argc < 6 || argc > 7) {
    fprintf(stderr, "Usage: %s job user title copies options [filename]\n", argv[0]);
    return 0;
  }
  
  if (initializeCupsOptions(&job, &ppd, argv)) return 1;
  
  if (argc == 7) {
    if ((rasterfile = open(argv[6], O_RDONLY)) == -1) {
      l10nGetString("Unable to open raster file", ls, sizeof(ls));
      fprintf(stderr, "ERROR: %s - %s\n", ls, strerror(errno));
      return 1;
    }
  } else
    rasterfile = 0;
  
  ras = cupsRasterOpen(rasterfile, CUPS_RASTER_READ);
  prn = prnAlloc(stdout, NULL, NULL);
  initializeBidirectional(ppd, prn);
  
  page = 0;
  while (cupsRasterReadHeader2(ras, &pagehdr)) {
    uint8_t *curStripe;
    int y, row;
    int hgr;
    int lineskip;
    
    if (pagehdr.cupsBitsPerPixel > 1) {
      l10nGetString("Color page not supported!", ls, sizeof(ls));
      fprintf(stderr, "ERROR: %s\n", ls);
      return 1;
    }
    hgr = (pagehdr.HWResolution[1] == 144);
    if (hgr)
      curStripe = malloc(pagehdr.cupsBytesPerLine*16);
    else
      curStripe = malloc(pagehdr.cupsBytesPerLine*8);
    if (prnSetHorizontalResolution(prn, pagehdr.HWResolution[0])) {
      l10nGetString("Horizontal resolution not supported", ls, sizeof(ls));
      fprintf(stderr, "ERROR: %s\n", ls);
      free(curStripe);
      return 1;
    };
    
    page++;
    fprintf(stderr, "PAGE: %d %d\n", page, pagehdr.NumCopies);
    
    row = 0;
    lineskip = 0;
    for (y=0; y<pagehdr.cupsHeight; y++) {
      cupsRasterReadPixels(ras, &curStripe[row*pagehdr.cupsBytesPerLine], pagehdr.cupsBytesPerLine);
      row++;
      
      if ((row >= 8+(8*hgr)) || (y+1 == pagehdr.cupsHeight)) {
        if (!stripeIsEmpty(curStripe, pagehdr.cupsBytesPerLine, 8+(8*hgr))) {
          skipLines(prn, &lineskip);
          outputStripeWithRes(prn, curStripe, pagehdr.cupsBytesPerLine, hgr);
          memset(curStripe, 0, pagehdr.cupsBytesPerLine*(8+(8*hgr)));
        } else
          lineskip += 16;
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
    l10nGetString("No pages found!", ls, sizeof(ls));
    fprintf(stderr, "ERROR: %s\n", ls);
    return 1;
  } else {
    l10nGetString("Ready to print.", ls, sizeof(ls));
    fprintf(stderr, "INFO: %s\n", ls);
  }
  return 0;
}


