/*
 *  filterlocalizer.h
 *  Apple Imagewriter CUPS command filter localization support
 *  (c) 2013 Daniele Cattaneo
 */

#ifndef CUPSFILTERS_FILTERLOCALIZER_H
#define CUPSFILTERS_FILTERLOCALIZER_H

int l10nInitialize();
int l10nGetString(const char *id, char *outbuf, size_t bufsize);

#endif
