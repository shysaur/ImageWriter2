/*
 *  filterlocalizer.c
 *  Apple Imagewriter CUPS command filter localization support
 *  (c) 2013 Daniele Cattaneo
 */

#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

int initialized = 0;

int l10nInitialize() {
#ifdef __APPLE__
  
  const char *lang;
  CFStringRef cflang;
  CFArrayRef arlang;
  
  if (!(lang = getenv("APPLE_LANGUAGE")))
    lang = getenv("LANG");
  
  if (lang) {
    cflang = CFStringCreateWithCString(kCFAllocatorDefault, lang, kCFStringEncodingUTF8);
    arlang = CFArrayCreate(kCFAllocatorDefault, (const void**)&cflang, 1, &kCFTypeArrayCallBacks);
    CFPreferencesSetValue(CFSTR("AppleLanguages"), arlang,
                          kCFPreferencesCurrentApplication,
                          kCFPreferencesAnyUser,
                          kCFPreferencesAnyHost);
    setlocale(LC_MESSAGES, "");
    
    CFRelease(arlang);
    CFRelease(cflang);
    initialized = 1;
  }
  return 0;
  
#else
  initialized = 1;
  return 0;
#endif
}


int l10nGetString(const char *id, char *outbuf, size_t bufsize) {
#ifdef __APPLE__
  
  CFStringRef locs;
  CFStringRef key;
  if (!initialized) return -1;
  
  key = CFStringCreateWithCString(kCFAllocatorDefault, id, kCFStringEncodingUTF8);
  locs = CFCopyLocalizedString(key, NULL);
  CFStringGetCString(locs, outbuf, bufsize, kCFStringEncodingUTF8);
  CFRelease(locs);
  CFRelease(key);
  
  return 0;
  
#else
  if (!initialized) return -1;
  strcpy(outbuf, id);
  return 0;
#endif
}

