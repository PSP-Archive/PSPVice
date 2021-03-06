/*
 * viceapplication.h - VICEApplication - main application class
 *
 * Written by
 *  Christian Vogelgsang <chris@vogelgsang.org>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#import <Cocoa/Cocoa.h>
#import "viceapplicationprotocol.h"
#import "vicemachineprotocol.h"
#import "consolewindow.h"

@interface VICEApplication : NSApplication <VICEApplicationProtocol>
{
    NSMutableArray *argsArray;
    BOOL postponeAutostart;
    
    id<VICEMachineProtocol> machine;
    BOOL canTerminate;
    
    ConsoleWindow *consoleWindow;
    int canvasCount;
    
    ConsoleWindow *monitorWindow;
    NSWindow *oldKeyWindow;
}

// start application with command line arguments
- (void)runWithArgC:(int)argc argV:(char**)argv;
// application is ready with startup
- (void)applicationDidFinishLaunching:(NSNotification*)aNotification;
// open a file on start up or drag file while running
- (BOOL)application:(NSApplication*)app openFile:(NSString*)file;

// ask for quit
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)app;
// actually quit and stop machine
- (void)applicationWillTerminate:(NSNotification*)notification;

// access the VICEMachine proxy object in the main thread
- (id<VICEMachineProtocol>)machine;
// convenience class message for querying the proxy machine object
+ (id<VICEMachineProtocol>)theMachine;

// access the VICEMachineController proxy object in the main thread
- (VICEMachineController *)machineController;
//! convenience class message
+ (VICEMachineController *)theMachineController;

// show/hide the console window
- (void)toggleConsoleWindow:(id)sender;
// show/hide the monitor window
- (void)toggleMonitorWindow:(id)sender;

// show error message
+ (void)runErrorMessage:(NSString *)message;
// show warning message
+ (void)runWarningMessage:(NSString *)message;

@end

