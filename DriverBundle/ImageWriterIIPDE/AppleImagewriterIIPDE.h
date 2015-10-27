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

#define PRESET_NOTSET -1



@interface AppleImagewriterIIPDE : NSObject
{
  IBOutlet id listQualityPresets;
  IBOutlet id listResolutionX;
  IBOutlet id listResolutionY;
  IBOutlet id buttonBidirectional;
  IBOutlet NSView *viewAdvanced;
  IBOutlet id view;
  
  pdeOptions options;
  int preset;
  NSBundle *pdeBundle;
  PDEPluginCallback *pdeCallback;
  ppd_file_t *ppd;
}

- (id)initWithCallback:(PDEPluginCallback *)callback;
- (void)dealloc;

- (int)loadPresetsFromArray:(NSArray *)source;
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




