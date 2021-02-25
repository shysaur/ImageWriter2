/*
 *  AppleImagewriterIIPDE.m
 *  Apple Imagewriter OS X PDE classes
 *  (c) 2013 Daniele Cattaneo
 */

#import "AppleImagewriterIIPDEPlugIn.h"
#import "AppleImagewriterIIPDE.h"


const int resolutions_x[] = {
  72, 80, 96, 107, 120, 136, 144, 160
};
#define CNT_RESOLUTIONS 8

#define PRESET_CUSTOM ((presets_cnt + 1))
#define PRESET_NOTSET -1


@implementation AppleImagewriterIIPDE


- (id)initWithCallback:(PDEPluginCallback *)callback {
  self = [super init];
  if (!self) return nil;
  
  pdeBundle   = [NSBundle bundleForClass:[self class]];
  pdeCallback = callback;
  ppd = [pdeCallback ppdFile];
  presets_cnt = 0;
  
  return self;
}


- (void)loadPresetsFromArray:(NSArray *)source {
  NSInteger c, i;
  NSString *title, *label;
  NSDictionary *pset;
  NSArray *psetdpi;
  
  if (source == nil) {
    c = 0;
    goto fail;
  }
  
  free(presets);
  c = [source count];
  presets = calloc(c, sizeof(pdeOptions));
  
  i=0;
  for (pset in source) {
    presets[i].bidir = [[pset objectForKey:@"Bidirectional"] boolValue];
    
    psetdpi = [pset objectForKey:@"DPI"];
    presets[i].resX = [[psetdpi objectAtIndex:0] intValue];
    presets[i].resY = [[psetdpi objectAtIndex:1] intValue];
    
    label = [pset objectForKey:@"Label"];
    title = NSLocalizedStringFromTableInBundle(label, nil, pdeBundle, nil);
    [listQualityPresets insertItemWithTitle:title atIndex:i];
    
    i++;
  }
  
fail:
  preset = PRESET_NOTSET;
  presets_cnt = c;
  return;
}


- (NSString *)panelName {
  return NSLocalizedStringFromTableInBundle(@"Print Quality", nil, pdeBundle, nil);
}


- (NSString *)panelKind {
  return @"com.danielecattaneo.printers.imagewriterii.pde";
}


- (NSView *)panelView {
  if (!view)
    [[NSBundle bundleForClass:self.class] loadNibNamed:@"PDEView" owner:self topLevelObjects:nil];
  return view;
}


- (NSArray *)supportedPPDOptionKeys {
  return @[@"Bidirectional", @"Resolution"];
}


- (IBAction)listQualityPresetsDidChange:(id)sender {
  preset = [listQualityPresets indexOfSelectedItem];
  if (preset < presets_cnt)
    options = presets[preset];
  [self updateControls];
}


- (IBAction)listResolutionYDidChange:(id)sender {
  NSInteger i;
  
  i = [listResolutionY indexOfSelectedItem];
  options.resY = !i ? 72 : 144;
  [self updateControls];
}


- (IBAction)listResolutionXDidChange:(id)sender {
  NSInteger i;
  
  i = [listResolutionX indexOfSelectedItem];
  options.resX = resolutions_x[i];
  [self updateControls];
}


- (IBAction)buttonBidirectionalDidChange:(id)sender {
  options.bidir = ([buttonBidirectional state] == NSOnState);
  [self updateControls];
}


- (BOOL)saveValuesAndReturnError:(NSError **)error {
  NSString *res, *bidir;
  
  res = [NSString stringWithFormat:@"%dx%ddpi", options.resX, options.resY];
  bidir = options.bidir ? @"True" : @"False";
  
  [pdeCallback willChangePPDOptionKeyValue:@"Resolution" ppdChoice:res];
  [pdeCallback willChangePPDOptionKeyValue:@"Bidirectional" ppdChoice:bidir];
  
  return YES;
}


- (BOOL)restoreValuesAndReturnError:(NSError **)error {
  ppd_choice_t *temp;
  
  temp = ppdFindMarkedChoice(ppd, "Resolution");
  sscanf(temp->choice, "%dx%d", &options.resX, &options.resY);
  
  temp = ppdFindMarkedChoice(ppd, "Bidirectional");
  options.bidir = (temp->choice[0] == 'T');
  
  preset = PRESET_NOTSET;
  
  return [self updateControls];
}


- (BOOL)updateControls {
  int i, stop;
  
  if (preset == PRESET_NOTSET) {
    stop = 0;
    for (i=0; i<presets_cnt && !stop; i++) {
      if (memcmp(&presets[i], &options, sizeof(pdeOptions)) == 0)
        stop = 1;
    }
    preset = stop ? i - 1 : PRESET_CUSTOM;
  }
  
  [listQualityPresets selectItemAtIndex:preset];
  [viewAdvanced setHidden:(preset != PRESET_CUSTOM)];
  if ([pdeCallback respondsToSelector:@selector(panelViewDidResize)])
    [pdeCallback panelViewDidResize];
  
  stop = 0;
  for (i=0; i<CNT_RESOLUTIONS && !stop; i++)
    if (options.resX == resolutions_x[i])
      stop = 1;
  if (!stop)
    return NO; /* Ops! */
  
  [listResolutionX selectItemAtIndex:i-1];
  [listResolutionY selectItemAtIndex:(options.resY == 144)];
  [buttonBidirectional setState:(options.bidir ? NSOnState : NSOffState)];
  
  return YES;
}


- (void)dealloc
{
  free(presets);
}


- (void)willShow {
  NSURL *plist;
  NSArray *defPresets;
  
  plist = [pdeBundle URLForResource:@"PDEPresets" withExtension:@"plist"];
  defPresets = [[NSArray alloc] initWithContentsOfURL:plist];
  [self loadPresetsFromArray:defPresets];

  [self updateControls];
}


- (BOOL)shouldHide {
  return YES;
}


@end




