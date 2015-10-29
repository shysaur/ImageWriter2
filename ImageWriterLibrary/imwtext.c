/*
 *  imwtext.c
 *  Apple Imagewriter text output
 *  (c) 2013 Daniele Cattaneo
 */

#include <stdio.h>
#include <string.h>
#include <iconv.h>
#include <errno.h>
#include <stdarg.h>
#include "imwtext.h"


#define N_A 0


const char iso_8859_to_imw[][256] = {
  /* kPrnCharSetAmerican (ASCII) */
  {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
   ' ','!','"','#','$','%','&', 27,'(',')','*','+',',','-','.','/',
   '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',
   '@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
   'P','Q','R','S','T','U','V','W','X','Y','Z','[', 92,']','^','_',
   '`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
   'p','q','r','s','t','u','v','w','x','y','z','{','|','}','~',127,
   N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
   N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
   N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
   N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
   N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
   N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
   N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
   N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A },
  /* kPrnCharSetBritish (ISO 646:GB) */
  {   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
     16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    ' ','!','"',N_A,'$','%','&', 27,'(',')','*','+',',','-','.','/',
    '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',
    '@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
    'P','Q','R','S','T','U','V','W','X','Y','Z','[', 92,']','^','_',
    '`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
    'p','q','r','s','t','u','v','w','x','y','z','{','|','}','~',127,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,'#',N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A },
  /* kPrnCharSetGerman (ISO 646:DE) */
  {   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
     16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    ' ','!','"','#','$','%','&', 27,'(',')','*','+',',','-','.','/',
    '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',
    N_A,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
    'P','Q','R','S','T','U','V','W','X','Y','Z',N_A,N_A,N_A,'^','_',
    '`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
    'p','q','r','s','t','u','v','w','x','y','z',N_A,N_A,N_A,N_A,127,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,'@',N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,'[',N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A, 92,N_A,N_A,N_A,N_A,N_A,']',N_A,N_A,'~',
    N_A,N_A,N_A,N_A,'{',N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,'|',N_A,N_A,N_A,N_A,N_A,'}',N_A,N_A,N_A },
  /* kPrnCharSetFrench (Custom) */
  {   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
     16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    ' ','!','"',N_A,'$','%','&', 27,'(',')','*','+',',','-','.','/',
    '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',
    N_A,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
    'P','Q','R','S','T','U','V','W','X','Y','Z',N_A,N_A,N_A,'^','_',
    '`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
    'p','q','r','s','t','u','v','w','x','y','z',N_A,N_A,N_A,N_A,127,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,'#',N_A,N_A,N_A,']','~',N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    '[',N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    '@',N_A,N_A,N_A,N_A,N_A,N_A, 92,'}','{',N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,'|',N_A,N_A,N_A,N_A,N_A,N_A },
  /* kPrnCharSetSwedish (Custom) */
  {   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
     16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    ' ','!','"','#','$','%','&', 27,'(',')','*','+',',','-','.','/',
    '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',
    '@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
    'P','Q','R','S','T','U','V','W','X','Y','Z',N_A,N_A,N_A,'^','_',
    '`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
    'p','q','r','s','t','u','v','w','x','y','z',N_A,N_A,N_A,'~',127,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,'[',']',N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A, 92,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,'{','}',N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,'|',N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A },
  /* kPrnCharSetItalian (Olivetti ISO 646 encoding) */
  {   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    ' ','!','"',N_A,'$','%','&', 27,'(',')','*','+',',','-','.','/',
    '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',
    N_A,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
    'P','Q','R','S','T','U','V','W','X','Y','Z',N_A,N_A,N_A,'^','_',
    N_A,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
    'p','q','r','s','t','u','v','w','x','y','z',N_A,N_A,N_A,N_A,127,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,'#',N_A,N_A,N_A,'@',N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    '[',N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    '{',N_A,N_A,N_A,N_A,N_A,N_A, 92,'}',']',N_A,N_A,'~',N_A,N_A,N_A,
    N_A,N_A,'|',N_A,N_A,N_A,N_A,N_A,N_A,'`',N_A,N_A,N_A,N_A,N_A,N_A },
  /* kPrnCharSetSpanish (Custom) */
  {   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
     16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    ' ','!','"',N_A,'$','%','&', 27,'(',')','*','+',',','-','.','/',
    '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',
    N_A,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
    'P','Q','R','S','T','U','V','W','X','Y','Z',N_A,N_A,N_A,'^','_',
    '`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
    'p','q','r','s','t','u','v','w','x','y','z',N_A,N_A,N_A,'~',127,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,'[',N_A,'#',N_A,N_A,N_A,'@',N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    '{',N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,']',
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A, 92,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,'}',N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,'|',N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A },
  /* kPrnCharSetDanish (ISO 646:DK) */
  {   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
     16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    ' ','!','"','#','$','%','&', 27,'(',')','*','+',',','-','.','/',
    '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',
    '@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
    'P','Q','R','S','T','U','V','W','X','Y','Z',N_A,N_A,N_A,'^','_',
    '`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
    'p','q','r','s','t','u','v','w','x','y','z',N_A,N_A,N_A,'~',127,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,']','[',N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A, 92,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,'}','{',N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,
    N_A,N_A,N_A,N_A,N_A,N_A,N_A,N_A,'|',N_A,N_A,N_A,N_A,N_A,N_A,N_A }
};


int prnISO8859TextPrint(printerRef prn, const char *s) {
  int i, j, res;
  char rc;
  prnCharSet curCS;
  
  curCS = kPrnCharSetAmerican;
  prnSelectCharacterSet(prn, 0, curCS);
  
  for (i=0; s[i] != '\0'; i++) {
    rc = iso_8859_to_imw[curCS][(unsigned char)s[i]];
    
    if (rc == N_A) {
      res = -1;
      for (j=0; j<kPrnCharSetCount && res<0; j++)
        if ((rc = iso_8859_to_imw[j][(unsigned char)s[i]]) != N_A)
          res = j;
      
      if (res >= 0) {
        curCS = res;
        prnSelectCharacterSet(prn, 0, curCS);
      } else
        rc = '?';
    }
    
    putc(rc, prn->s_out);
  }
  return 0;
}


int prnEncodedTextPrint(printerRef prn, const char *s, const char *fromcode) {
  char *outb, *outb_wp, *inb_wp;
  size_t inb_rem, oub_rem, res;
  iconv_t ic;
  int e;
  
  ic = iconv_open("ISO-8859-1", fromcode);
  if (ic == (iconv_t)(-1)) {
    if (errno == EINVAL)
      return ERR_IMWAPI_INVALIDPARAM;
    else
      return ERR_IMWAPI_UNKNOWN;
  }
  
  inb_rem = strlen(s);
  oub_rem = inb_rem + 32;
  outb = malloc(oub_rem + 1);
  if (!outb)
    return ERR_IMWAPI_OUTOFMEMORY;
  outb_wp = outb;
  inb_wp = (char*)s;
  
  res = iconv(ic, &inb_wp, &inb_rem, &outb_wp, &oub_rem);
  if (res == (size_t)(-1)) {
    free(outb);
    if (errno == EILSEQ || errno == EINVAL)
      return ERR_IMWAPI_INVALIDPARAM;
    else
      return ERR_IMWAPI_UNKNOWN;
  }
  if (iconv_close(ic)) {
    free(outb);
    return ERR_IMWAPI_UNKNOWN;
  }
  
  *outb_wp = '\0';
  e = prnISO8859TextPrint(prn, outb);
  free(outb);
  return e;
}


int prnEncodedTextPrintF(printerRef prn, const char *fromcode, const char *format, ...) {
  char *tempbuf;
  va_list args;
  int ret;
  
  va_start(args, format);
  ret = vasprintf(&tempbuf, format, args);
  va_end(args);
  if (ret < 0)
    return ERR_IMWAPI_OUTOFMEMORY;
  
  ret = prnEncodedTextPrint(prn, tempbuf, fromcode);
  
  free(tempbuf);
  return ret;
}



