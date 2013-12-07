/*
 *  imwtext.c
 *  Apple Imagewriter text output
 *  (c) 2013 Daniele Cattaneo
 */

#include <stdio.h>
#include <string.h>
#include <iconv.h>
#include <errno.h>
#include "imwtext.h"

#define N_A 0

#define CHARSET_CNT 8

const char iso_8859_to_imw[CHARSET_CNT][256] = {
  /* kAmerican (ASCII) */
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
  /* kBritish (ISO 646:GB) */
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
  /* kGerman (ISO 646:DE) */
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
  /* kFrench (Custom) */
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
  /* kSwedish (Custom) */
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
  /* kItalian (Olivetti ISO 646 encoding) */
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
  /* kSpanish (Custom) */
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
  /* kDanish (ISO 646:DK) */
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

const charSet charset_idx[CHARSET_CNT] = {
  kAmerican, kBritish, kGerman, kFrench, kSwedish, kItalian, kSpanish, kDanish
};


int prnISO8859TextPrint(printerRef prn, const char *s) {
  int i, j, res;
  char rc;
  charSet curCS;
  int csi;
  
  curCS = kAmerican;
  csi = 0;
  prnSelectCharacterSet(prn, 0, curCS);
  for (i=0; s[i] != '\0'; i++) {
    rc = iso_8859_to_imw[csi][(unsigned char)s[i]];
    
    if (rc == N_A) {
      res = -1;
      for (j=0; j<CHARSET_CNT && res<0; j++)
        if ((rc = iso_8859_to_imw[j][(unsigned char)s[i]]) != N_A) res = j;
      
      if (res >= 0) {
        csi = res;
        curCS = charset_idx[csi];
        prnSelectCharacterSet(prn, 0, curCS);
      } else
        rc = '?';
    }
    
    putc(rc, prn->s_out);
  }
  return 0;
}


int prnEncodedTextPrint(printerRef prn, const char *s, const char *fromcode) {
  iconv_t ic;
  char *outb;
  char *outb_wp;
  char *inb_wp;
  size_t inb_rem;
  size_t oub_rem;
  size_t res;
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
  if (!outb) return ERR_IMWAPI_OUTOFMEMORY;
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



