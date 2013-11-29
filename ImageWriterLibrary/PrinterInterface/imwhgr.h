//
//  imwhgr.h
//  ImageWriterLibrary
//
//  Created by Daniele Cattaneo on 29/11/13.
//  Copyright (c) 2013 Daniele Cattaneo. All rights reserved.
//

#include "citohapi.h"

#ifndef IMAGEWRITERLIBRARY_IMWHGR_H
#define IMAGEWRITERLIBRARY_IMWHGR_H

int hgrPrintBitmapStripeH8Wk8(printerRef prn, const uint8_t bitmap[], int chrWidth);
int hgrPrintBitmapStripeHiresH16Wk8(printerRef prn, const uint8_t bitmap[], int chrWidth);

#endif
