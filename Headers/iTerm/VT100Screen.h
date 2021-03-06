// -*- mode:objc -*-
// $Id: VT100Screen.h,v 1.38 2008-09-30 06:21:12 yfabian Exp $
/*
 **  VT100Screen.h
 **
 **  Copyright (c) 2002, 2003
 **
 **  Author: Fabian, Ujwal S. Setlur
 **         Initial code by Kiichi Kusama
 **
 **  Project: iTerm
 **
 **  Description: Implements the VT100 screen.
 **
 **  This program is free software; you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation; either version 2 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program; if not, write to the Free Software
 **  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#import <Cocoa/Cocoa.h>
#import <iTerm/VT100Terminal.h>
#import <LineBuffer.h>

@class PTYTask;
@class PTYSession;
@class PTYTextView;
@class iTermGrowlDelegate;

// This is used in the rightmost column when a double-width character would
// have been split in half and was wrapped to the next line. It is nonprintable
// and not selectable. It is not copied into the clipboard. A line ending in this
// character should always have EOL_DWC. These are stripped when adding a line
// to the scrollback buffer.
#define DWC_SKIP 0xf000

#define TABWINDOW    300

@interface VT100Screen : NSObject
{
    int WIDTH; // width of screen
    int HEIGHT; // height of screen
    int CURSOR_X;
    int CURSOR_Y;
    int SAVE_CURSOR_X;
    int SAVE_CURSOR_Y;
    int ALT_SAVE_CURSOR_X;
    int ALT_SAVE_CURSOR_Y;
    int SCROLL_TOP;
    int SCROLL_BOTTOM;
    BOOL tabStop[TABWINDOW];
    
    VT100Terminal *TERMINAL;
    PTYTask *SHELL;
    PTYSession *SESSION;
    int charset[4], saveCharset[4];
    BOOL blinkShow;
    BOOL PLAYBELL;
    BOOL SHOWBELL;
    BOOL GROWL;

    
    BOOL blinkingCursor;
    PTYTextView *display;
    
    // A circular buffer exactly (WIDTH+1) * HEIGHT elements in size. This contains
    // only the contents of the screen. The scrollback buffer is stored in linebuffer.
    screen_char_t *buffer_lines;
    
    // The position in buffer_lines of the first line in the screen. The logical lines
    // wrap around the circular buffer.
    screen_char_t *screen_top;

    // buffer holding flags for each char on whether it needs to be redrawn
    char *dirty;
    
    // a single default line
    screen_char_t *default_line;
    screen_char_t *result_line;
    
    // temporary buffer to store main buffer in SAVE_BUFFER/RESET_BUFFER mode
    screen_char_t *temp_buffer;
        
    // default line stuff
    int default_bg_code;
    int default_fg_code;
    int default_line_width;

    // max size of scrollback buffer
    unsigned int  max_scrollback_lines;
    // current number of lines in scrollback buffer (not including what is on screen)
    unsigned int current_scrollback_lines;
    // how many scrollback lines have been lost due to overflow
    int scrollback_overflow;
    long long cumulative_scrollback_overflow;
    
    // print to ansi...
    BOOL printToAnsi;        // YES=ON, NO=OFF, default=NO;
    NSMutableString *printToAnsiString;
    
    // Growl stuff
    iTermGrowlDelegate* gd;
    
    // Scrollback buffer
    LineBuffer* linebuffer;
    FindContext findContext;
}


- (id)init;
- (void)dealloc;

- (NSString *)description;

- (screen_char_t*)initScreenWithWidth:(int)width Height:(int)height;
- (void)resizeWidth:(int)new_width height:(int)height;
- (void)reset;
- (void)setWidth:(int)width height:(int)height;
- (int)width;
- (int)height;
- (unsigned int)scrollbackLines;
- (void)setScrollback:(unsigned int)lines;
- (void)setTerminal:(VT100Terminal *)terminal;
- (VT100Terminal *)terminal;
- (void)setShellTask:(PTYTask *)shell;
- (PTYTask *)shellTask;
- (PTYSession *) session;
- (void)setSession:(PTYSession *)session;

- (PTYTextView *) display;
- (void) setDisplay: (PTYTextView *) aDisplay;

- (BOOL) blinkingCursor;
- (void) setBlinkingCursor: (BOOL) flag;
- (void)showCursor:(BOOL)show;
- (void)setPlayBellFlag:(BOOL)flag;
- (void)setShowBellFlag:(BOOL)flag;
- (void)setGrowlFlag:(BOOL)flag;

// line access
- (screen_char_t *) getLineAtIndex: (int) theIndex;
- (screen_char_t *) getLineAtScreenIndex: (int) theIndex;
- (char *) dirty;
- (NSString *) getLineString: (screen_char_t *) theLine;

// edit screen buffer
- (void)putToken:(VT100TCC)token;
- (void)clearBuffer;
- (void)clearScrollbackBuffer;
- (void)saveBuffer;
- (void)restoreBuffer;

// internal
- (void)setString:(NSString *)s ascii:(BOOL)ascii;
- (void)setStringToX:(int)x
                   Y:(int)y
              string:(NSString *)string
               ascii:(BOOL)ascii;
- (void)setNewLine;
- (void)deleteCharacters:(int)n;
- (void)backSpace;
- (void)backTab;
- (void)setTab;
- (void)clearTabStop;
- (void)clearScreen;
- (void)eraseInDisplay:(VT100TCC)token;
- (void)eraseInLine:(VT100TCC)token;
- (void)selectGraphicRendition:(VT100TCC)token;
- (void)cursorLeft:(int)n;
- (void)cursorRight:(int)n;
- (void)cursorUp:(int)n;
- (void)cursorDown:(int)n;
- (void)cursorToX: (int) x;
- (void)cursorToX:(int)x Y:(int)y; 
- (void)saveCursorPosition;
- (void)restoreCursorPosition;
- (void)setTopBottom:(VT100TCC)token;
- (void)scrollUp;
- (void)scrollDown;
- (void)activateBell;
- (void)deviceReport:(VT100TCC)token;
- (void)deviceAttribute:(VT100TCC)token;
- (void)insertBlank: (int)n;
- (void)insertLines: (int)n;
- (void)deleteLines: (int)n;
- (void)blink;
- (int)cursorX;
- (int)cursorY;

- (int)numberOfLines;

- (int)scrollbackOverflow;
- (long long)totalScrollbackOverflow;
- (void)resetScrollbackOverflow;
- (void)scrollScreenIntoScrollbackBuffer:(int)leaving;

- (void)resetDirty;
- (void)setDirty;

// print to ansi...
- (BOOL) printToAnsi;
- (void) setPrintToAnsi: (BOOL) aFlag;
- (void) printStringToAnsi: (NSString *) aString;

// UI stuff
- (void) doPrint;

// Is this character double width on this screen?
- (BOOL)isDoubleWidthCharacter:(unichar)c;

// Initialize the find context.
- (void)initFindString:(NSString*)aString forwardDirection:(BOOL)direction ignoringCase:(BOOL)ignoreCase startingAtX:(int)x startingAtY:(int)y withOffset:(int)offsetof;
- (BOOL)continueFindResultAtStartX:(int*)startX atStartY:(int*)startY atEndX:(int*)endX atEndY:(int*)endY found:(BOOL*)found;
- (void)cancelFind;

- (void) dumpDebugLog;

// Return a human-readable dump of the screen contents.
- (NSString*)debugString;
@end
