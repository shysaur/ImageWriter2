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
  PDEPluginCallback *callback;
  NSMutableArray *pdes;
  AppleImagewriterIIPDE *pde;
  
  pdes = [NSMutableArray array];
  callback = (PDEPluginCallback *)host;
  
  if ([pdeType isEqual:(NSString*)kPrinterModuleTypeIDStr]) {
    pde = [[[AppleImagewriterIIPDE alloc] initWithCallback:callback] autorelease];
    if (pde)
      [pdes addObject:pde];
    else
      pdes = nil;
  }
  
  return pdes;
}


@end
