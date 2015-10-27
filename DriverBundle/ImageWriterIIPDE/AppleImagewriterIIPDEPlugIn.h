/*
 *  AppleImagewriterIIPDEPlugin.h
 *  Apple Imagewriter OS X PDE classes
 *  (c) 2015 Daniele Cattaneo
 */

#import <Cocoa/Cocoa.h>
#import <PrintCore/PDEPluginInterface.h>
#import <PrintCore/PMPrintingDialogExtensions.h>


@interface PDEPluginCallback : NSObject /* <PDEPlugInCallbackProtocol> */

- (void)panelViewDidResize;

@end


@interface AppleImagewriterIIPDEPlugIn : NSObject /* <PMPlugIn> */

- (BOOL)initWithBundle:(NSBundle *)bundle;
- (NSArray*)PDEPanelsForType:(NSString*)pdeType withHostInfo:(id)host;

@end
