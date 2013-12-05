//
//  AppleImagewriterIIPDE.m
//  DriverBundle
//
//  Created by Daniele Cattaneo on 05/12/13.
//
//

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




const int resolutions_x[] = {
  72, 80, 96, 107, 120, 136, 144, 160
};
#define CNT_RESOLUTIONS 8

const pdeOptions presets[] = {
  { 72,  72, YES},
  {144,  72,  NO},
  {160, 144,  NO}
};
#define CNT_PRESETS 3


@implementation AppleImagewriterIIPDE


- (id)initWithCallback:(PDEPluginCallback *)callback {
  self = [super init];
  if (!self) return nil;
  
  pdeBundle   = [[NSBundle bundleForClass:[self class]] retain];
  pdeCallback = [callback retain];
  ppd = [pdeCallback ppdFile];
  
  return self;
}


- (NSString *)panelName {
  return NSLocalizedStringFromTableInBundle(@"Print Quality", nil, pdeBundle, nil);
}


- (NSString *)panelKind {
  return @"com.danielecattaneo.printers.imagewriterii.pde";
}


- (NSView *)panelView {
  if (!view)
    [NSBundle loadNibNamed:@"PDEView" owner:self];
  return view;
}


- (NSArray *)supportedPPDOptionKeys {
  return [NSArray arrayWithObjects:@"Resolution", @"Bidirectional", nil];
}


- (IBAction)listQualityPresetsDidChange:(id)sender {
  preset = (int)[listQualityPresets indexOfSelectedItem];
  if (preset < 3)
    memcpy(&options, &presets[preset], sizeof(pdeOptions));
  [self updateControls];
}


- (IBAction)listResolutionYDidChange:(id)sender {
  NSInteger i;
  i = [listResolutionY indexOfSelectedItem];
  options.resY = (i==0) ? 72 : 144;
  [self updateControls];
}


- (IBAction)listResolutionXDidChange:(id)sender {
  NSInteger i;
  i = [listResolutionY indexOfSelectedItem];
  options.resY = resolutions_x[i];
  [self updateControls];
}


- (IBAction)buttonBidirectionalDidChange:(id)sender {
  options.bidir = ([buttonBidirectional state] == NSOnState);
  [self updateControls];
}


- (BOOL)saveValuesAndReturnError:(NSError **)error {
  [pdeCallback
    willChangePPDOptionKeyValue:@"Resolution"
    ppdChoice:[NSString stringWithFormat:@"%dx%d", options.resX, options.resY]];
  [pdeCallback
    willChangePPDOptionKeyValue:@"Bidirectional"
    ppdChoice:options.bidir ? @"True" : @"False"];
  
  return YES;
}


- (BOOL)restoreValuesAndReturnError:(NSError **)error {
  ppd_choice_t *temp;
  
  temp = ppdFindMarkedChoice(ppd, "Resolution");
  sscanf(temp->choice, "%dx%d", &options.resX, &options.resY);
  
  temp = ppdFindMarkedChoice(ppd, "Bidirectional");
  options.bidir = (temp->choice[0] == 'T');
  
  NSLog(@"Preset not set");
  preset = PRESET_NOTSET;
  
  return [self updateControls];
}


- (BOOL)updateControls {
  int i, stop;
  
  if (preset == PRESET_NOTSET) {
    stop = 0;
    for (i=0; i<CNT_PRESETS && !stop; i++) {
      if (memcmp(&presets[i], &options, sizeof(pdeOptions)) == 0)
        stop = 1;
    }
    if (stop)
      preset = i - 1;
    else
      preset = PRESET_CUSTOM;
  }
  
  [listQualityPresets selectItemAtIndex:preset];
  [viewAdvanced setHidden:(preset != PRESET_CUSTOM)];
  [pdeCallback panelViewDidResize];
  
  stop = 0;
  for (i=0; i<CNT_RESOLUTIONS && !stop; i++)
    if (options.resX == resolutions_x[i]) stop = 1;
  if (!stop) return NO; /* Ops! */
  [listResolutionX selectItemAtIndex:i-1];
  [listResolutionY selectItemAtIndex:(options.resY == 144)];
  [buttonBidirectional setState:(options.bidir?NSOnState:NSOffState)];
  
  return YES;
}


- (void)dealloc
{
  [pdeBundle release];
  [pdeCallback release];
  
  [super dealloc];
}


- (void)willShow {
  [self updateControls];
}


- (BOOL)shouldHide {
  return YES;
}


@end




