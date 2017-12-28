/**
 @author Contributions from the community; see CONTRIBUTORS.md
 @date 2005-2016
 @copyright MPL2; see LICENSE.txt
*/

#import <Cocoa/Cocoa.h>
#import "DKDrawingToolProtocol.h"
#import "DKCommonTypes.h"

NS_ASSUME_NONNULL_BEGIN

@class DKToolController;

/** @brief DKDrawingTool is the semi-abstract base class for all types of drawing tool.

DKDrawingTool is the semi-abstract base class for all types of drawing tool. The point of a tool is to act as a translator for basic mouse events and
convert those events into meaningful operations on the target layer or object(s). One tool can be set at a time (see DKToolController) and
establishes a "mode" of operation for handling mouse events.

The tool also supplies a cursor for the view when that tool is selected.

A tool typically targets a layer or the objects within it. The calling sequence to a tool is coordinated by the DKToolController, targeting
the current active layer. Tools can change the data content of the layer or not - for example a zoom zool would only change the scale of
a view, not change any data.

Tools should be considered to be controllers, and sit between the view and the drawing data model.

Note: do not confuse "tools" as DK defines them with a palette of buttons or other UI - an application might implement an interface to
select a tool in such a way, but the buttons are not tools. A button could store a tool as its representedObject however. These UI
considerations are outside the scope of DK itself.
*/
@interface DKDrawingTool : NSObject <DKDrawingTool> {
@private
	NSString* mKeyboardEquivalent;
	NSUInteger mKeyboardModifiers;
}

/** @brief Does the tool ever implement undoable actions?

 Classes must override this and say \c YES if the tool does indeed perform an undoable action
 (i.e. it does something to an object)
 */
@property (class, readonly) BOOL toolPerformsUndoableAction;

/** @brief Load tool defaults from the user defaults

 If used, this sets up the state of the tools and the styles they are set to to whatever was saved
 by the saveDefaults method in an earlier session. Someone (such as the app delegate) needs to call this
 on app launch after the tools have all been set up and registered.
 */
+ (void)loadDefaults;

/** @brief Save tool defaults to the user defaults

 Saves the persistent data, if any, of each registered tool. The main use for this is to
 restore the styles associated with each tool when the app is next launched.
 */
+ (void)saveDefaults;
+ (nullable id)firstResponderAbleToSetTool;

/** @brief Return the registry name for this tool

 If the tool isn't registered, returns nil
a string, the name this tool is registerd under, if any.
 */
@property (readonly, copy, nullable) DKToolName registeredName;

- (void)drawRect:(NSRect)aRect inView:(NSView*)aView;
- (void)flagsChanged:(NSEvent*)event inLayer:(DKLayer*)layer;
- (BOOL)isValidTargetLayer:(DKLayer*)aLayer;

/** @brief Return whether the tool is some sort of object selection tool

 This method is used to assist the tool controller in making sensible decisions about certain
 automatic operations. Subclasses that implement a selection tool should override this to return YES.
 @return \c YES if the tool selects objects, \c NO otherwise
 */
@property (readonly, getter=isSelectionTool) BOOL selectionTool;

/** @brief Sets the tool as the current tool for the key view in the main window, if possible

 This follows the \c -set approach that cocoa uses for many objects. It looks for the key view in the
 main window. If it's a DKDrawingView that has a tool controller, it sets itself as the controller's
 current tool. This might be more convenient than other ways of setting a tool.
 */
- (void)set;

/** @brief Called when this tool is set by a tool controller

 Subclasses can make use of this message to prepare themselves when they are set if necessary
 @param aController the controller that set this tool
 */
- (void)toolControllerDidSetTool:(DKToolController*)aController;

/** @brief Called when this tool is about to be unset by a tool controller

 Subclasses can make use of this message to prepare themselves when they are unset if necessary, for
 example by finishing the work they were doing and cleaning up.
 @param aController the controller that set this tool
 */
- (void)toolControllerWillUnsetTool:(DKToolController*)aController;

/** @brief Called when this tool is unset by a tool controller

 Subclasses can make use of this message to prepare themselves when they are unset if necessary
 @param aController the controller that set this tool
 */
- (void)toolControllerDidUnsetTool:(DKToolController*)aController;
- (void)setCursorForPoint:(NSPoint)mp targetObject:(DKDrawableObject*)obj inLayer:(DKLayer*)aLayer event:(NSEvent*)event;

// if a keyboard equivalent is set, the tool controller will set the tool if the keyboard equivalent is received in keyDown:
// the tool must be registered for this to function.

- (void)setKeyboardEquivalent:(NSString*)str modifierFlags:(NSUInteger)flags;
- (nullable NSString*)keyboardEquivalent;
/** @brief Return the keyboard modifier flags that need to be down to select this tool using the keyboard modifier
 
 A *registered* tool can be looked up by keyboard equivalent. This is implemented by DKToolController
 in conjunction with this class.
 @return the modifier flags - may be 0 if no flags are needed
 */
- (NSUInteger)keyboardModifierFlags;
@property (readonly, copy, nullable) NSString *keyboardEquivalent;
@property (readonly) NSUInteger keyboardModifierFlags;

// drawing tools can optionally return arbitrary persistent data that DK will store in the prefs for it

- (nullable NSData*)persistentData;
- (void)shouldLoadPersistentData:(NSData*)data;

@end

@interface DKDrawingTool (OptionalMethods)

- (void)mouseMoved:(NSEvent*)event inView:(NSView*)view;

@end

#pragma mark -

//! most of these are now implemented by \c DKToolRegistry - these methods call it for compatibility
@interface DKDrawingTool (Deprecated)

/** @brief Return the shared instance of the tool registry

 Creates a new empty registry if it doesn't yet exist
 @return a dictionary - contains drawing tool objects keyed by name
 @deprecated +[DKDrawingTool sharedToolRegistry] is deprecated and is a no-op
 */
+ (null_unspecified NSDictionary*)sharedToolRegistry DEPRECATED_MSG_ATTRIBUTE("Use DKToolRegistry instead");

/** @brief Retrieve a tool from the registry with the given name

 Registered tools may be conveniently set by name - see DKToolController
 @param name the registry name of the tool required.
 @return the tool if it exists, or nil
 */
+ (nullable DKDrawingTool*)drawingToolWithName:(null_unspecified DKToolName)name DEPRECATED_MSG_ATTRIBUTE("Use DKToolRegistry instead");

/** @brief Register a tool in th eregistry with the given name

 Registered tools may be conveniently set by name - see \c DKToolController
 @param tool a tool object to register
 @param name a name to register it against.
 */
+ (void)registerDrawingTool:(null_unspecified DKDrawingTool*)tool withName:(null_unspecified DKToolName)name DEPRECATED_MSG_ATTRIBUTE("Use DKToolRegistry instead");

/** @brief Retrieve a tool from the registry matching the key equivalent indicated by the key event passed

 See DKToolController
 @param keyEvent a keyDown event.
 @return the tool if it can be matched, or nil
 */
+ (nullable DKDrawingTool*)drawingToolWithKeyboardEquivalent:(null_unspecified NSEvent*)keyEvent DEPRECATED_MSG_ATTRIBUTE("Use DKToolRegistry instead");

/** @brief Set a "standard" set of tools in the registry

 "Standard" tools are creation tools for various basic shapes, the selection tool, zoom tool and
 launch time, may be safely called more than once - subsequent calls are no-ops.
 If the conversion table has been set up prior to this, the tools will automatically pick up
 the class from the table, so that apps don't need to swap out all the tools for subclasses, but
 can simply set up the table.
 */
+ (void)registerStandardTools DEPRECATED_MSG_ATTRIBUTE("Use DKToolRegistry instead");

/** @brief Return a list of registered tools' names, sorted alphabetically

 May be useful for supporting a UI
 @return an array, a list of NSStrings
 */
+ (null_unspecified NSArray<DKToolName>*)toolNames DEPRECATED_MSG_ATTRIBUTE("Use DKToolRegistry instead");

@end

NS_ASSUME_NONNULL_END
