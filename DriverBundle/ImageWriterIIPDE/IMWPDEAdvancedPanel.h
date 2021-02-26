//
//  IMWPDEAdvancedPanel.h
//  ImageWriterIIPDE
//
//  Created by Daniele Cattaneo on 25/02/2021.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@class PDEPluginCallback;

@interface IMWPDEAdvancedPanel : NSObject /*<PDEPanel>*/

- (instancetype)initWithCallback:(PDEPluginCallback *)callback;

- (NSString *)panelName;
- (NSString *)panelKind;
- (NSView *)panelView;
- (NSArray *)supportedPPDOptionKeys;
- (BOOL)saveValuesAndReturnError:(NSError **)error;
- (BOOL)restoreValuesAndReturnError:(NSError **)error;
- (void)willShow;
- (BOOL)shouldHide;

@end

NS_ASSUME_NONNULL_END
