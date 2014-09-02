require('Common');
process.bridge.objc.import('Foundation');
process.bridge.objc.import('Quartz');
process.bridge.objc.import('Cocoa');
process.bridge.objc.import('AppKit');
process.bridge.objc.import('CoreGraphics');

var $ = process.bridge.objc;


// Click into the dead center of the screen.
var s = Screens.active.bounds;

var point = $.CGPointMake(s.width/2, s.height/2);
$.CGEventPost($.kCGHIDEventTap, $.CGEventCreateMouseEvent(null, $.kCGEventMouseMoved, point, 0));
$.CGEventPost($.kCGHIDEventTap, $.CGEventCreateMouseEvent(null, $.kCGEventLeftMouseDown, point, 0));
$.CGEventPost($.kCGHIDEventTap, $.CGEventCreateMouseEvent(null, $.kCGEventLeftMouseUp, point, 0));

// Send an enter command to void out any modals.
$.CGEventPost($.kCGHIDEventTap, $.CGEventCreateKeyboardEvent(null, 52, true));
$.CGEventPost($.kCGHIDEventTap, $.CGEventCreateKeyboardEvent(null, 52, false));


$.CGEventPost($.kCGHIDEventTap, $.CGEventCreateKeyboardEvent(null, 36, true));
$.CGEventPost($.kCGHIDEventTap, $.CGEventCreateKeyboardEvent(null, 36, false));

// Give a second before we begin.
setTimeout(function() { process.exit(0); }, 1000);