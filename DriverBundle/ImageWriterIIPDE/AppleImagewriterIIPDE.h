/*
 *  AppleImagewriterIIPDE.m
 *  Apple Imagewriter OS X PDE classes
 *  (c) 2013 Daniele Cattaneo
 */


#import <Cocoa/Cocoa.h>

#import <PrintCore/PDEPluginInterface.h>
#import <PrintCore/PMPrintingDialogExtensions.h>
#import "VUTCollapsibleView.h"


typedef struct {
  int resX, resY;
  BOOL bidir;
} pdeOptions;

#define PRESET_NOTSET -1
#define PRESET_CUSTOM 4




@interface AppleImagewriterIIPDEPlugIn : NSObject /* <PMPlugIn> */
{
}

- (BOOL)initWithBundle:(NSBundle *)bundle;
- (NSArray*)PDEPanelsForType:(NSString*)pdeType withHostInfo:(id)host;

@end




@interface PDEPluginCallback : NSObject /* <PDEPlugInCallbackProtocol> */
{
}
@end




@interface AppleImagewriterIIPDE : NSObject
{
  IBOutlet id listQualityPresets;
  IBOutlet id listResolutionX;
  IBOutlet id listResolutionY;
  IBOutlet id buttonBidirectional;
  IBOutlet VUTCollapsibleView *viewAdvanced;
  IBOutlet id view;
  
  pdeOptions options;
  int preset;
  NSBundle *pdeBundle;
  PDEPluginCallback *pdeCallback;
  ppd_file_t *ppd;
}

- (id)initWithCallback:(PDEPluginCallback *)callback;
- (void)dealloc;

- (NSString *)panelName;
- (NSString *)panelKind;
- (NSView *)panelView;
- (NSArray *)supportedPPDOptionKeys;
- (BOOL)saveValuesAndReturnError:(NSError **)error;
- (BOOL)restoreValuesAndReturnError:(NSError **)error;
- (BOOL)updateControls;
- (void)willShow;
- (BOOL)shouldHide;

- (IBAction)listQualityPresetsDidChange:(id)sender;
- (IBAction)listResolutionYDidChange:(id)sender;
- (IBAction)listResolutionXDidChange:(id)sender;
- (IBAction)buttonBidirectionalDidChange:(id)sender;

@end




