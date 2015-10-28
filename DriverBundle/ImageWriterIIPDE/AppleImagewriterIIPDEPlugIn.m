/*
 *  AppleImagewriterIIPDEPlugin.m
 *  Apple Imagewriter OS X PDE classes
 *  (c) 2015 Daniele Cattaneo
 */

#import "AppleImagewriterIIPDEPlugIn.h"
#import "AppleImagewriterIIPDE.h"


@implementation AppleImagewriterIIPDEPlugIn


- (BOOL)initWithBundle:(NSBundle *)bundle {
  return YES;
}


- (NSArray*)PDEPanelsForType:(NSString*)pdeType withHostInfo:(id)host {
  NSMutableArray *pdes;
  AppleImagewriterIIPDE *pde;
  
  pdes = [NSMutableArray array];
  
  if ([pdeType isEqual:(NSString *)kPrinterModuleTypeIDStr]) {
    pde = [[[AppleImagewriterIIPDE alloc] initWithCallback:host] autorelease];
    if (pde)
      [pdes addObject:pde];
    else
      pdes = nil;
  }
  
  return [pdes copy];
}


@end
