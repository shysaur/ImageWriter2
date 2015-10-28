/*
 *  AppleImagewriterIIPDE.m
 *  Apple Imagewriter OS X PDE classes
 *  (c) 2013 Daniele Cattaneo
 */


#import <Cocoa/Cocoa.h>


typedef struct {
  int resX, resY;
  BOOL bidir;
} pdeOptions;


@interface AppleImagewriterIIPDE : NSObject
{
  IBOutlet NSPopUpButton *listQualityPresets;
  IBOutlet NSPopUpButton *listResolutionX;
  IBOutlet NSPopUpButton *listResolutionY;
  IBOutlet NSButton *buttonBidirectional;
  IBOutlet NSView *viewAdvanced;
  IBOutlet NSView *view;
  
  pdeOptions *presets;
  int presets_cnt;
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




