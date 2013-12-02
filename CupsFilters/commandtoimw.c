/*
 *  commandtoimw.c
 *  Apple Imagewriter CUPS command filter
 *  (c) 2013 Daniele Cattaneo
 */

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <cups/cups.h>
#include <cups/ppd.h>

#include "citohapi.h"
#include "imwhgr.h"
#include "filtersver.h"


typedef struct {
  int job_id;
  const char *user,	*title;
  int num_options;
  cups_option_t	*options;
} job_data_t;


int initializeCupsOptions(job_data_t *job, ppd_file_t *ppd, const char *argv[]) {
  job->job_id      = atoi(argv[1]);
  job->user        = argv[2];
  job->title       = argv[3];
  job->num_options = cupsParseOptions(argv[5], 0, &(job->options));
  
  if ((ppd = ppdOpenFile(getenv("PPD"))) != NULL) {
    ppdMarkDefaults(ppd);
    cupsMarkOptions(ppd, job->num_options, job->options);
  } else {
    fprintf(stderr, "ERROR: Unable to open PPD file: %s\n", strerror(errno));
    return 1;
  }
  return 0;
}


#define TEST_COL_WIDTH 180
int prnOutputSelfTestPage(printerRef prn) {
  int resolution_list[] = {72,80,96,107,120,136,144,160};
  uint8_t resolution_test[] = {
    0xFB,0xAA,0x80,0x00,
    0xFF,0xFD,0x55,0x00,
    0xFE,0xAA,0xA0,0x00,
    0xFF,0xFF,0x55,0x44,
    0xFB,0xAA,0x80,0x00,
    0xFF,0xFD,0x55,0x00,
    0xFE,0xAA,0xA0,0x00,
    0xFF,0xFF,0x55,0x44,
  };
  uint8_t hires_test1[] = {
    0xFB,0xAA,0x80,0x00,
    0xFE,0xEA,0xA0,0x00,
    0xFB,0xAA,0x80,0x00,
    0xFE,0xEA,0xA0,0x00,
    0xFB,0xAA,0x80,0x00,
    0xFE,0xEA,0xA0,0x00,
    0xFB,0xAA,0x80,0x00,
    0xFE,0xEA,0xA0,0x00,
  };
  uint8_t hires_test2[] = {
    0xFF,0xFD,0x55,0x00,
    0xFF,0xFF,0x55,0x44,
    0xFF,0xFD,0x55,0x00,
    0xFF,0xFF,0x55,0x44,
    0xFF,0xFD,0x55,0x00,
    0xFF,0xFF,0x55,0x44,
    0xFF,0xFD,0x55,0x00,
    0xFF,0xFF,0x55,0x44
  };
  uint8_t nozzle_test[] = {
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
  };
  char temp[256];
  int i, j;
  
  prnTextPrint(prn, " ");
  /* This dummy space is needed to signal the beginning of a print job in case a */
  /* sheet-feeder is installed. This can be triggered by a graphical command too. */
  /* In absence of this, if a sheet is not inserted, the printer will simply wait */
  /* silently offline until the user feeds a sheet manually. */
  prnSetHighBitMode(prn, IMWAPI_8BITMODE);
  prnSetLineHeight(prn, IMWAPI_6LPI);
  prnSetBidirectionalMode(prn, 0);
  prnSelectCharacterSet(prn, 0, kAmerican);
  prnSetHorizontalResolution(prn, 72);
  prnCarriageReturnLineFeed(prn);
  prnTextPrint(prn, "\016ImageWriter Test Page\017\r\n\r\n");
  prnSetHorizontalResolution(prn, 96);
  sprintf(temp, "Filter version %.2f - Library version %.2f\r\n\r\n", imwfiltersVersion, citohapiVersion);
  prnTextPrint(prn, temp);
  prnTextPrint(prn,
               "To get the printer's ROM version and to run the printer's self test, hold the Form Feed but-\r\n"
               "ton while pressing the On/Off button, then release both buttons. To stop the print, turn off\r\n"
               "the printer.\r\n"
               "Disclaimer: This driver is not provided and is not endorsed by Apple Inc.\r\n\r\n");
  
  /* Nozzle Test */
  prnSetHorizontalResolution(prn, 72);
  prnSelectCharacterSet(prn, 0, kAmerican);
  prnCarriageReturnLineFeed(prn);
  prnGraphicStripePrint(prn, nozzle_test, sizeof(nozzle_test), 0);
  prnTextPrint(prn, "_");
  prnSetLineHeight(prn, 1);
  prnCarriageReturnLineFeed(prn);
  prnGraphicGoToX(prn, 8);
  prnGraphicStripePrint(prn, nozzle_test, sizeof(nozzle_test), 0);
  prnTextPrint(prn, "_");
  prnSetLineHeight(prn, IMWAPI_6LPI);
  prnCarriageReturnLineFeed(prn);
  prnCarriageReturnLineFeed(prn);
  
  /* Charset test */
  prnSetHorizontalResolution(prn, 72);
  prnSelectCharacterSet(prn, 0, kAmerican);
  temp[1] = '\0';
  for (i=32, j=0; i<127; i++) {
    if (i != 127) {
      if (j % 70 == 0) prnCarriageReturnLineFeed(prn);
      temp[0] = i;
      prnTextPrint(prn, temp);
      j++;
    }
  }
  prnCarriageReturnLineFeed(prn);
  prnCarriageReturnLineFeed(prn);
  
  /* Resolution Test */
  prnSetHorizontalResolution(prn, 72);
  prnSelectCharacterSet(prn, 0, kAmerican);
  prnGraphicGoToX(prn, TEST_COL_WIDTH);
  prnTextPrint(prn, "72 dpi vertical");
  prnGraphicGoToX(prn, 2*TEST_COL_WIDTH);
  prnTextPrint(prn, "144 dpi vertical");
  prnCarriageReturnLineFeed(prn);
  for (i=0; i<8; i++) {
    prnSetHorizontalResolution(prn, resolution_list[i]);
    sprintf(temp, "%d dpi horizontal", resolution_list[i]);
    prnTextPrint(prn, temp);
    prnGraphicGoToX(prn, TEST_COL_WIDTH*resolution_list[i]/72);
    hgrPrintBitmapStripeH8Wk8(prn, resolution_test, 4);
    prnGraphicGoToX(prn, TEST_COL_WIDTH*2*resolution_list[i]/72);
    hgrPrintBitmapStripeH8Wk8(prn, hires_test1, 4);
    prnSetLineHeight(prn, 1);
    prnCarriageReturnLineFeed(prn);
    prnGraphicGoToX(prn, TEST_COL_WIDTH*2*resolution_list[i]/72);
    hgrPrintBitmapStripeH8Wk8(prn, hires_test2, 4);
    prnSetLineHeight(prn, 19);
    prnCarriageReturnLineFeed(prn);
  }
  
  prnFormFeed(prn);
  return 0;
}


int main(int argc, const char *argv[]) {
  ppd_file_t *ppd = NULL;
  job_data_t job;
  printerRef prn;
  cups_file_t *cmdfile;
  
  char line[1024], *value;
  int ln;
  
  setbuf(stderr, NULL);
  if (argc < 6 || argc > 7) {
    fprintf(stderr, "Usage: %s job user title copies options [filename]\n", argv[0]);
    return 0;
  }
  
  if (initializeCupsOptions(&job, ppd, argv)) return 1;
  
  if (argc == 7) {
    if ((cmdfile = cupsFileOpen(argv[6], "r")) == NULL) {
      fprintf(stderr, "ERROR: Unable to open command file - %s\n", strerror(errno));
      return 1;
    }
  } else
    cmdfile = cupsFileStdin();
  
  ln = 0;
  
  prn = prnAlloc(stdout, NULL, NULL);
  while (cupsFileGetConf(cmdfile, line, sizeof(line), &value, &ln)) {
    if (!strcasecmp(line, "PrintSelfTestPage")) {
      prnOutputSelfTestPage(prn);
    } else {
      fprintf(stderr, "ERROR: Unknown printer command %s\n", line);
    }
  }
  prnDealloc(prn);
  
  return 0;
}

