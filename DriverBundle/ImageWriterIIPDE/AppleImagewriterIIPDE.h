//
//  AppleImagewriterIIPDE.h
//  DriverBundle
//
//  Created by Daniele Cattaneo on 05/12/13.
//
//

#import <Cocoa/Cocoa.h>

#import <PrintCore/PDEPluginInterface.h>
#import <PrintCore/PMPrintingDialogExtensions.h>


typedef struct {
  int resX, resY;
  BOOL bidir;
} pdeOptions;

#define PRESET_NOTSET -1
#define PRESET_CUSTOM 4


@interface AppleImagewriterIIPDEPlugIn : NSObject
{
}
@end


@interface PDEPluginCallback : NSObject
{
}
@end


@interface AppleImagewriterIIPDE : NSObject
{
  IBOutlet id listQualityPresets;
  IBOutlet id listResolutionX;
  IBOutlet id listResolutionY;
  IBOutlet id buttonBidirectional;
  IBOutlet id viewAdvanced;
  IBOutlet id view;
  
  pdeOptions options;
  int preset;
  NSBundle *pdeBundle;
  PDEPluginCallback *pdeCallback;
  ppd_file_t *ppd;
}

- (id)initWithCallback:(PDEPluginCallback *)callback;



@end
