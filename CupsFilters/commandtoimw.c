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

#include "filterlocalizer.h"
#include "citohapi.h"
#include "imwhgr.h"
#include "imwtext.h"
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
  char temp[400], temp2[64];
  int c, i, j;
  
  prnTextPrint(prn, " ");
  /* This dummy space is needed to signal the beginning of a print job in case a */
  /* sheet-feeder is installed. This can be triggered by a graphical command too. */
  /* In absence of this, if a sheet is not inserted, the printer will simply wait */
  /* silently offline until the user feeds a sheet manually. */
  prnSetHighBitMode(prn, IMWAPI_8BITMODE);
  prnSetLineHeight(prn, IMWAPI_6LPI);
  prnSetBidirectionalMode(prn, IMWAPI_LEFTTORIGHT);
  prnSelectCharacterSet(prn, 0, kPrnCharSetAmerican);
  prnSetHorizontalResolution(prn, 72);
  prnCarriageReturnLineFeed(prn);
  prnSelectFont(prn, kPrnFontNLQ);
  
  l10nGetString("ImageWriter Test Page", temp, sizeof(temp));
  prnEncodedTextPrintF(prn, "UTF-8", "\016%s\017\r\n\r\n", temp);
  prnSetHorizontalResolution(prn, 96);
  
  l10nGetString("Filter version", temp, sizeof(temp));
  l10nGetString("Library version", temp2, sizeof(temp2));
  prnEncodedTextPrintF(prn, "UTF-8", "%s %.2f - %s %.2f\r\n\r\n", temp, imwfiltersVersion, temp2, citohapiVersion);
  
  prnSelectFont(prn, kPrnFontDraft);
  prnSetBidirectionalMode(prn, IMWAPI_BIDIRECTIONAL);
  
  l10nGetString("Test Page Additional Information", temp, sizeof(temp));
  prnEncodedTextPrintF(prn, "UTF-8", "%s\r\n", temp);
  
  
  /* Nozzle Test */
  prnSetBidirectionalMode(prn, IMWAPI_LEFTTORIGHT);
  prnSelectFont(prn, kPrnFontStandard);
  prnSetHorizontalResolution(prn, 72);
  prnSelectCharacterSet(prn, 0, kPrnCharSetAmerican);
  prnCarriageReturnLineFeed(prn);
  for (i=0; i<4; i++) {
    prnGraphicStripePrint(prn, nozzle_test, sizeof(nozzle_test), 0);
    prnTextPrint(prn, "_ ");
  }
  prnSetLineHeight(prn, 1);
  prnCarriageReturnLineFeed(prn);
  prnGraphicGoToX(prn, 8);
  for (i=0; i<4; i++) {
    prnGraphicStripePrint(prn, nozzle_test, sizeof(nozzle_test), 0);
    prnTextPrint(prn, "_ ");
  }
  prnSetLineHeight(prn, IMWAPI_6LPI);
  prnCarriageReturnLineFeed(prn);
  
  
  /* Charset test */
  prnSelectFont(prn, kPrnFontStandard);
  prnSetHorizontalResolution(prn, 72);
  j = 0;
  temp[1] = '\0';
  for (c=kPrnCharSetAmerican; c<=kPrnCharSetDanish; c++) {
    prnSelectCharacterSet(prn, 0, c);
    for (i=32; i<127; i++) {
      if (j % 72 == 0) prnCarriageReturnLineFeed(prn);
      temp[0] = i;
      prnTextPrint(prn, temp);
      j++;
    }
  }
  for (i=192; i<223; i++) { /* MouseText (who ever used that?) */
    if (j % 72 == 0) prnCarriageReturnLineFeed(prn);
    temp[0] = i;
    prnTextPrint(prn, temp);
    j++;
  }
  prnCarriageReturnLineFeed(prn);
  prnCarriageReturnLineFeed(prn);
  
  
  /* Resolution Test */
  prnSelectFont(prn, kPrnFontDraft);
  prnSetHorizontalResolution(prn, 144);
  l10nGetString("dpi vertical", temp, sizeof(temp));
  prnGraphicGoToX(prn, TEST_COL_WIDTH*144/72);
  prnEncodedTextPrintF(prn, "UTF-8", "72 %s", temp);
  prnGraphicGoToX(prn, 2*TEST_COL_WIDTH*144/72);
  prnEncodedTextPrintF(prn, "UTF-8", "144 %s", temp);
  l10nGetString("dpi horizontal", temp, sizeof(temp));
  prnCarriageReturnLineFeed(prn);
  for (i=0; i<8; i++) {
    prnSetHorizontalResolution(prn, resolution_list[i]);
    prnEncodedTextPrintF(prn, "UTF-8", "%d %s", resolution_list[i], temp);
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
  
  prnSetLineHeight(prn, IMWAPI_6LPI);
  prnFormFeed(prn);
  return 0;
}


int main(int argc, const char *argv[]) {
  ppd_file_t *ppd = NULL;
  job_data_t job;
  printerRef prn;
  cups_file_t *cmdfile;
  char ls[128];
  char line[1024], *value;
  int ln;
  
  l10nInitialize();
  
  setbuf(stderr, NULL);
  if (argc < 6 || argc > 7) {
    fprintf(stderr, "Usage: %s job user title copies options [filename]\n", argv[0]);
    return 0;
  }
  
  if (initializeCupsOptions(&job, ppd, argv)) return 1;
  
  if (argc == 7) {
    if ((cmdfile = cupsFileOpen(argv[6], "r")) == NULL) {
      l10nGetString("Unable to open command file", ls, sizeof(ls));
      fprintf(stderr, "ERROR: %s - %s\n", ls, strerror(errno));
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
      l10nGetString("Unknown printer command", ls, sizeof(ls));
      fprintf(stderr, "ERROR: %s %s\n", ls, line);
    }
  }
  prnDealloc(prn);
  
  return 0;
}

