/*
 *  VUTCollapsibleView.h
 *  View with additional methods to collapse and expand it
 *  (c) 2013 Daniele Cattaneo
 */


#import <Cocoa/Cocoa.h>




@interface VUTCollapsibleView : NSView
{
  BOOL collapsed;
}

- (IBAction)collapse:(id)sender;
- (IBAction)expand:(id)sender;

@end



