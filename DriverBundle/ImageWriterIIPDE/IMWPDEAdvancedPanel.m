//
//  IMWPDEAdvancedPanel.m
//  ImageWriterIIPDE
//
//  Created by Daniele Cattaneo on 25/02/2021.
//

#import "IMWPDEPlugIn.h"
#import "IMWPDEAdvancedPanel.h"


@implementation IMWPDEAdvancedPanel {
  IBOutlet NSView *_view;
  IBOutlet NSPopUpButton *_btnMemorySize;
  IBOutlet NSButton *_btnFlushBuffer;
  
  NSBundle *_pdeBundle;
  PDEPluginCallback *_pdeCallback;
  ppd_file_t *_ppd;
  
  BOOL _memoryExpansion;
  BOOL _flushBuffer;
}


- (instancetype)initWithCallback:(PDEPluginCallback *)callback
{
  self = [super init];
  _pdeBundle = [NSBundle bundleForClass:self.class];
  _pdeCallback = callback;
  _ppd = [callback ppdFile];
  return self;
}


- (NSString *)panelName
{
  return NSLocalizedStringFromTableInBundle(@"Advanced", nil, _pdeBundle, nil);
}


- (NSString *)panelKind
{
  return @"com.danielecattaneo.printers.imagewriterii.pde.advanced";
}


- (NSView *)panelView
{
  if (!_view)
    [_pdeBundle loadNibNamed:@"IMWPDEAdvancedPanel" owner:self topLevelObjects:nil];
  return _view;
}


- (NSArray *)supportedPPDOptionKeys
{
  return @[@"MemorySize", @"FlushBufferAfterPrint"];
}


- (BOOL)saveValuesAndReturnError:(NSError **)error
{
  return YES;
}


- (BOOL)restoreValuesAndReturnError:(NSError **)error
{
  ppd_choice_t *resChoice = ppdFindMarkedChoice(_ppd, "MemorySize");
  _memoryExpansion = strcmp(resChoice->choice, "32K") == 0;
  
  ppd_choice_t *flushChoice = ppdFindMarkedChoice(_ppd, "FlushBufferAfterPrint");
  _flushBuffer = flushChoice->choice[0] == 'T';
  
  [self updateControls];
  return YES;
}


- (void)willShow
{
  [self updateControls];
}


- (BOOL)shouldHide
{
  return YES;
}


- (void)updateControls
{
  [_btnMemorySize selectItemAtIndex:(_memoryExpansion ? 0 : 1)];
  _btnFlushBuffer.state = _flushBuffer ? NSOnState : NSOffState;
}


- (IBAction)changeMemorySize:(id)sender
{
  _memoryExpansion = _btnMemorySize.indexOfSelectedItem == 1;
  
  NSString *mem = _memoryExpansion ? @"32K" : @"2K";
  [_pdeCallback willChangePPDOptionKeyValue:@"MemorySize" ppdChoice:mem];
}


- (IBAction)changeFlushBuffer:(id)sender
{
  _flushBuffer = _btnFlushBuffer.state == NSOnState;
  
  NSString *flush = _flushBuffer ? @"True" : @"False";
  [_pdeCallback willChangePPDOptionKeyValue:@"FlushBufferAfterPrint" ppdChoice:flush];
}


@end
