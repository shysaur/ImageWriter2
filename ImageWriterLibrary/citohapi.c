/* 
 *  citohapi.c
 *  C-ITOH 8510, Apple DMP, Apple ImageWriter, Apple Imagewriter II abstraction layer
 *  (c) 2013 Daniele Cattaneo
 */

#include "citohapi.h"


printerRef prnAlloc(FILE *output, FILE *input, int *err) {
  printerRef prn;
  
  if (output) {
    prn = calloc(1, sizeof(printer));
    if (prn) {
      prn->s_in = input;
      prn->s_out = output;
      return prn;
    } else
      if (err)
        *err = ERR_IMWAPI_OUTOFMEMORY;
  } else
    if (err)
      *err = ERR_IMWAPI_INVALIDPARAM;
  return NULL;
}


int prnDealloc(printerRef prn) {
  free(prn);
  return 0;
}


int prnHardwareQuery(printerRef prn, char *answer) {
  if (prn == NULL)
    return ERR_IMWAPI_UNKNOWN;
  if (answer == NULL)
    return ERR_IMWAPI_UNKNOWN;
  if (prn->s_in == NULL || prn->s_in == stdin)
    return ERR_IMWAPI_WRONGORDER;
  
  fflush(prn->s_out);
  fflush(prn->s_in);
  fputs("\0x1B?", prn->s_out);
  fread(answer, 3, 1, prn->s_in);
  return 0;
}


int prnTextPrint(printerRef prn, char *text) {
  if (prn == NULL)
    return ERR_IMWAPI_UNKNOWN;
  
  fputs(text, prn->s_out);
  prn->headPos = -1;
  return 0;
}


int prnSetHorizontalResolution(printerRef prn, int res) {
  char rc;
  
  if (prn == NULL)
    return ERR_IMWAPI_UNKNOWN;
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
  if (prn == NULL)
    return ERR_IMWAPI_UNKNOWN;
  
  fprintf(prn->s_out, "\014");
  prn->headPos = 0;
  return 0;
}


int prnSetLineHeight(printerRef prn, int rows) {
  if (prn == NULL)
    return ERR_IMWAPI_UNKNOWN;
  if ((rows < 1 || rows > 99) && rows != IMWAPI_6LPI && rows != IMWAPI_8LPI)
    return ERR_IMWAPI_WRONGORDER;
  
  switch (rows) {
    case IMWAPI_6LPI:
      fprintf(prn->s_out, "\033A"); break;
    case IMWAPI_8LPI:
      fprintf(prn->s_out, "\033B"); break;
    default:
      fprintf(prn->s_out, "\033T%02d", rows);
  }
  return 0;
}


int prnSetFormHeight(printerRef prn, int rows) {
  if (prn == NULL)
    return ERR_IMWAPI_UNKNOWN;
  if (rows < 1 || rows > 9999)
    return ERR_IMWAPI_WRONGORDER;
  
  fprintf(prn->s_out, "\033H%04d", rows);
  return 0;
}


#define GSP_MINRLE -7
int prnGraphicStripePrint(printerRef prn, const uint8_t stripe[], int swidth, int optimizeWidth) {
  int partstart, partlen, partrle, nextpart, minrle;
  
  if (prn == NULL)
    return ERR_IMWAPI_UNKNOWN;
  if (swidth < 1 || swidth > 9999)
    return ERR_IMWAPI_WRONGORDER;
  
  partrle = -1;
  partlen = 1;
  partstart = 0;
  nextpart = -1;
  minrle = GSP_MINRLE;
  while (partstart+partlen < swidth) {
    if (partrle < 0) {
      /* Not in a run */
      if (nextpart <= minrle) {
        partlen += nextpart;
        if (partlen > 0) {
          fprintf(prn->s_out, "\033G%04d", partlen);
          fwrite(&stripe[partstart], sizeof(uint8_t), partlen, prn->s_out);
        }
        nextpart = -1;
        partstart += partlen;
        partlen = 1;
        partrle = stripe[partstart];
      } else {
        if (stripe[partstart+partlen] == stripe[partstart+partlen-1])
          nextpart--;
        else {
          nextpart = -1;
          minrle = GSP_MINRLE;
        }
        partlen++;
      }
    } else {
      /* In a run */
      if (stripe[partstart+partlen] != stripe[partstart+partlen-1]) {
        if (partlen > 0)
          fprintf(prn->s_out, "\033V%04d%c", partlen, (uint8_t)partrle);
        partstart += partlen;
        partlen = 1;
        partrle = -1;
        minrle = GSP_MINRLE;
      } else
        partlen++;
    }
  }
  
  if (partlen > 0) {
    if (partrle < 0) {
      fprintf(prn->s_out, "\033G%04d", partlen);
      fwrite(&stripe[partstart], sizeof(uint8_t), partlen, prn->s_out);
    } else
      if (!(optimizeWidth && partrle == 0))
        fprintf(prn->s_out, "\033V%04d%c", partlen, (uint8_t)partrle);
  }
  
  if (prn->headPos > 0) prn->headPos += swidth;
  return 0;
}


int prnGraphicGoToX(printerRef prn, int swidth) {
  if (prn == NULL)
    return ERR_IMWAPI_UNKNOWN;
  if (swidth < 0 || swidth > 9999)
    return ERR_IMWAPI_WRONGORDER;
  
  fprintf(prn->s_out, "\033F%04d", swidth);
  prn->headPos = swidth;
  return 0;
}


int prnCarriageReturnLineFeed(printerRef prn) {
  if (prn == NULL)
    return ERR_IMWAPI_UNKNOWN;
  
  fprintf(prn->s_out, "\r\n");
  prn->headPos = 0;
  return 0;
}


int prnSetBidirectionalMode(printerRef prn, int bidi) {
  if (prn == NULL)
    return ERR_IMWAPI_UNKNOWN;
  
  switch (bidi) {
    case IMWAPI_BIDIRECTIONAL:
      fprintf(prn->s_out, "\033<"); break;
    case IMWAPI_LEFTTORIGHT:
      fprintf(prn->s_out, "\033>"); break;
    default:
      return ERR_IMWAPI_INVALIDPARAM;
  }
  return 0;
}


int prnSetHighBitMode(printerRef prn, int ascii) {
  if (prn == NULL)
    return ERR_IMWAPI_UNKNOWN;
  
  switch (ascii) {
    case IMWAPI_ASCIIMODE:
      fwrite("\033D\0 ", sizeof(char), 4, prn->s_out); break;
    case IMWAPI_8BITMODE:
      fwrite("\033Z\0 ", sizeof(char), 4, prn->s_out); break;
    default:
      return ERR_IMWAPI_INVALIDPARAM;
  }
  return 0;
}


int prnResetPrinterStatus(printerRef prn) {
  if (prn == NULL)
    return ERR_IMWAPI_UNKNOWN;
  
  fprintf(prn->s_out, "\033c");
  return 0;
}


int prnSelectCharacterSet(printerRef prn, int mouseText, prnCharSet lang) {
  if (prn == NULL)
    return ERR_IMWAPI_UNKNOWN;
  if (mouseText < 0 || mouseText > 1)
    return ERR_IMWAPI_INVALIDPARAM;
  
  if (mouseText) {
    fprintf(stderr, "DEBUG: mouseText is on. This is extremely dangerous"
      " because it can lead to garbage prints with no apparent reason if it's"
      " not disabled. You can also access mouseText stuff by printing a"
      " character > 128.\n");
    fprintf(prn->s_out, "\033&");
  } else
    fprintf(prn->s_out, "\033$");
  
  switch (lang) {
    case kPrnCharSetAmerican:
      fwrite("\033Z\07\0", sizeof(char), 4, prn->s_out); break;
    case kPrnCharSetBritish:
      fwrite("\033Z\04\0\033D\03\0", sizeof(char), 8, prn->s_out); break;
    case kPrnCharSetGerman:
      fwrite("\033Z\03\0\033D\04\0", sizeof(char), 8, prn->s_out); break;
    case kPrnCharSetFrench:
      fwrite("\033Z\01\0\033D\06\0", sizeof(char), 8, prn->s_out); break;
    case kPrnCharSetSwedish:
      fwrite("\033Z\02\0\033D\05\0", sizeof(char), 8, prn->s_out); break;
    case kPrnCharSetItalian:
      fwrite("\033Z\06\0\033D\01\0", sizeof(char), 8, prn->s_out); break;
    case kPrnCharSetSpanish:
      fwrite("\033D\07\0", sizeof(char), 4, prn->s_out); break;
    case kPrnCharSetDanish:
      fwrite("\033Z\05\0\033D\02\0", sizeof(char), 8, prn->s_out); break;
    default:
      return ERR_IMWAPI_INVALIDPARAM;
  }
  return 0;
}


int prnSelectFont(printerRef prn, prnFont font) {
  if (prn == NULL)
    return ERR_IMWAPI_UNKNOWN;
  
  switch (font) {
    case kPrnFontDraft:
      fprintf(prn->s_out, "\033a1"); break;
    case kPrnFontStandard:
      fprintf(prn->s_out, "\033a0"); break;
    case kPrnFontNLQ:
      fprintf(prn->s_out, "\033a2"); break;
    default:
      return ERR_IMWAPI_INVALIDPARAM;
  }
  return 0;
}




