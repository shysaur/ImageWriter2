/*
 *  imwhgr.h
 *  Apple Imagewriter bitmap output helper
 *  (c) 2013 Daniele Cattaneo
 */

#include "citohapi.h"


#ifndef IMAGEWRITERLIBRARY_IMWHGR_H
#define IMAGEWRITERLIBRARY_IMWHGR_H

int hgrPrintBitmapStripeH8Wk8(printerRef prn, const uint8_t bitmap[], int chrWidth);
int hgrPrintBitmapStripeHiresH16Wk8(printerRef prn, const uint8_t bitmap[], int chrWidth);

#endif
