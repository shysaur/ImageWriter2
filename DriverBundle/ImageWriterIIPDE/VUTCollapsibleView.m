/*
 *  VUTCollapsibleView.m
 *  View with additional methods to collapse and expand it
 *  (c) 2013 Daniele Cattaneo
 */


#import "VUTCollapsibleView.h"




@implementation VUTCollapsibleView


- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
      [self setHidden:NO];
      collapsed = NO;
    }
    return self;
}


- (IBAction)collapse:(id)sender {
  if (!collapsed) {
    NSRect myBounds;
    NSView *sv;
    
    myBounds = [self frame];
    [self setHidden:YES];
    if ((sv = [self superview])) {
      NSRect temp;
      NSArray *subviews;
      NSInteger i, c;
      
      subviews = [sv subviews];
      c = [subviews count];
      for (i=0; i<c; i++) {
        NSView *thisview;
        
        thisview = [subviews objectAtIndex:i];
        temp = [thisview frame];
        if (temp.origin.y > myBounds.origin.y + myBounds.size.height) {
          temp.origin.y -= myBounds.size.height;
          [thisview setFrame:temp];
        }
      }
      temp = [sv bounds];
      temp.origin.y -= myBounds.size.height;
      [sv setBounds:temp];
      temp = [sv frame];
      temp.size.height -= myBounds.size.height;
      [sv setFrame:temp];
      
    }
    collapsed = YES;
  }
}


- (IBAction)expand:(id)sender {
  if (collapsed) {
    NSRect myBounds;
    NSView *sv;
    
    myBounds = [self frame];
    if ((sv = [self superview])) {
      NSRect temp;
      NSArray *subviews;
      NSInteger i, c;
      
      subviews = [sv subviews];
      c = [subviews count];
      for (i=0; i<c; i++) {
        NSView *thisview;
        
        thisview = [subviews objectAtIndex:i];
        temp = [thisview frame];
        if (temp.origin.y > myBounds.origin.y) {
          temp.origin.y += myBounds.size.height;
          [thisview setFrame:temp];
        }
      }
      temp = [sv bounds];
      temp.origin.y += myBounds.size.height;
      [sv setBounds:temp];
      temp = [sv frame];
      temp.size.height += myBounds.size.height;
      [sv setFrame:temp];
    }
    collapsed = NO;
    [self setHidden:NO];
  }

}


@end
