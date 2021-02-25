/*
 *  IMWPDEPlugIn.m
 *  Apple Imagewriter OS X PDE classes
 *  (c) 2015 Daniele Cattaneo
 */

#import "IMWPDEPlugIn.h"
#import "IMWPDEPrintQualityPanel.h"


@implementation IMWPDEPlugIn


- (BOOL)initWithBundle:(NSBundle *)bundle {
  return YES;
}


- (NSArray*)PDEPanelsForType:(NSString*)pdeType withHostInfo:(id)host {
  NSMutableArray *pdes;
  IMWPDEPrintQualityPanel *pde;
  
  pdes = [NSMutableArray array];
  
  if ([pdeType isEqual:(NSString *)kPrinterModuleTypeIDStr]) {
    pde = [[IMWPDEPrintQualityPanel alloc] initWithCallback:host];
    if (pde)
      [pdes addObject:pde];
    else
      pdes = nil;
  }
  
  return [pdes copy];
}


@end
