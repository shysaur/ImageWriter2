/*
 *  IMWPDEPlugIn.m
 *  Apple Imagewriter OS X PDE classes
 *  (c) 2015 Daniele Cattaneo
 */

#import "IMWPDEPlugIn.h"
#import "IMWPDEPrintQualityPanel.h"
#import "IMWPDEAdvancedPanel.h"


@implementation IMWPDEPlugIn


- (BOOL)initWithBundle:(NSBundle *)bundle
{
  return YES;
}


- (NSArray*)PDEPanelsForType:(NSString*)pdeType withHostInfo:(id)host
{
  if (![pdeType isEqual:(NSString *)kPrinterModuleTypeIDStr])
    return @[];
  
  IMWPDEPrintQualityPanel *panel1 = [[IMWPDEPrintQualityPanel alloc] initWithCallback:host];
  IMWPDEAdvancedPanel *panel2 = [[IMWPDEAdvancedPanel alloc] initWithCallback:host];
  if (!panel1 || !panel2)
    return nil;
  
  return @[panel1, panel2];
}


@end
