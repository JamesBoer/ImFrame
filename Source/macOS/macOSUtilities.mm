/*
The MIT License (MIT)

Copyright (c) 2021 James Boer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "../ImfInternal.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

namespace ImFrame
{
    static NSInteger s_selectedTagId = -1;
}

@interface MenuItemHandler : NSObject
-(void) OnClick: (id) sender;
@end

@implementation MenuItemHandler

-(void) OnClick: (id) sender
{
    NSMenuItem * menuItem = sender;
    ImFrame::s_selectedTagId = menuItem.tag;
}

@end


namespace ImFrame
{
    
    struct MenuState
    {
        NSMenu * menu = nullptr;
        NSUInteger idx = 0;
    };

    static MenuItemHandler * s_menuHandler;
    static std::vector<MenuState> s_menus;
    static NSInteger s_currentTagId = 1;
    static bool s_buildMenus = true;
    static bool s_clearMenus = false;

	std::filesystem::path OsGetConfigFolder()
	{
        NSArray * paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
        NSString * path = [paths objectAtIndex:0];
        const char * str = [path UTF8String];
        return std::filesystem::path(str);
	}

	std::filesystem::path OsGetExecutableFolder()
	{		
        NSString * path = [[NSBundle mainBundle] executablePath];
        const char * str = [path UTF8String];
		auto p = std::filesystem::path(str);
        p.remove_filename();
        return p;
	}

	std::filesystem::path OsGetResourceFolder()
	{
		return OsGetExecutableFolder();
	}

    void * OsGetNativeWindow(GLFWwindow * window)
    {
        return static_cast<void *>(glfwGetCocoaWindow(window));
    }

    void OsInitialize()
    {
        s_menuHandler = [[MenuItemHandler new] autorelease];
        s_menus.push_back(MenuState());
        s_menus.back().menu = [NSApp menu];
    }

    void OsShutDown()
    {
        s_menuHandler = nullptr;
        s_menus.clear();
    }

    bool OsBeginMainMenuBar()
    {
        assert(s_menus.size() == 1);
        s_menus.front().idx = 1;
        s_currentTagId = 1;
        return true;
    }

    void OsEndMainMenuBar()
    {
        assert(s_menus.size() == 1);
        
        // By default, we'll want to clear the build flag at this point.
        s_buildMenus = false;
        
        // Check for both the clear menu flag being set, as well as
        // the number of menuitems in the topmost menu being changed.
        NSMenu * mainMenuBar = s_menus.front().menu;
        // Note idx + 1, because we started at one, and we never reached
        // the last "Window" menu.
        NSUInteger checkedCount = s_menus.back().idx + 1;
        if (s_clearMenus || mainMenuBar.itemArray.count != checkedCount)
        {
            // This is sort of weird, I know.  We want to preserve the first
            // and last menus ("AppName" and Window), which we didn't create.
            // But we need to clear everything between those two.  Thus, the
            // weird clearing code here.
            while (mainMenuBar.itemArray.count > 2)
                [mainMenuBar removeItemAtIndex:1];
            
            // All custom menus are cleared, so we're reset the clear flag and
            // trigger a rebuild pass on the next update.
            s_clearMenus = false;
            s_buildMenus = true;
        }
    }

    bool OsBeginMenu(const char * label, bool enabled)
    {
        if (s_clearMenus)
            return false;
        
        NSMenu * menu = s_menus.back().menu;
        NSInteger idx = s_menus.back().idx;
        s_menus.back().idx++;

        // Preserve only characters up to ## in label
        NSString * labelStr = [NSString stringWithUTF8String:label];
        NSUInteger loc = [labelStr rangeOfString:@"##"].location;
        if (loc < [labelStr length])
            labelStr = [labelStr substringToIndex:loc];

        // Create a new menuitem and add to menu
        if (s_buildMenus)
        {
            // New submenus consist of both a new menu and a new menuitem to trigger it
            NSMenu * newMenu = [[NSMenu new] autorelease];
            newMenu.autoenablesItems = false;
            newMenu.title = labelStr;
            NSMenuItem * menuItem = [[NSMenuItem new] autorelease];
            menuItem.title = labelStr;
            [menuItem setSubmenu:newMenu];
            [menu insertItem:menuItem atIndex:idx];
        }
        
        NSMenuItem * menuItem = [menu itemAtIndex:idx];
        
        // If the labels don't match, something has dynamically changed and we need to clear and rebuild the menus
        if (![labelStr isEqualToString:menuItem.title])
        {
            s_clearMenus = true;
            return false;
        }
        
        // Update enabled state
        menuItem.enabled = enabled;
        
        // Push this menu on the stack for tracking purposes
        s_menus.push_back(MenuState());
        s_menus.back().menu = menuItem.submenu;
        
        // Unlike Dear ImGui menus, we typically return true so clicks can be checked any any menuitem
        return true;
    }

    void OsEndMenu()
    {
        // Check to see if the number of menu items doesn't agree.  If so, something has
        // been dynamically added or removed, and we need to clear the menus and rebuild.
        NSMenu * menu = s_menus.back().menu;
        NSUInteger idx = s_menus.back().idx;
        if (menu.itemArray.count != idx)
            s_clearMenus = true;
        
        // We've finished this menu, so pop tracking data off the menu stack
        s_menus.pop_back();
    }

    bool OsMenuItem(const char * label, const char * shortcut, bool selected, bool enabled)
    {
        if (s_clearMenus)
            return false;
        
       // Save off current index value, and increment index on stack
        NSInteger idx = s_menus.back().idx;
        s_menus.back().idx++;
        
        // Preserve only characters up to ## in label
        NSString * labelStr = [NSString stringWithUTF8String:label];
        NSUInteger loc = [labelStr rangeOfString:@"##"].location;
        if (loc < [labelStr length])
            labelStr = [labelStr substringToIndex:loc];
            
        // If we're in a building pass, create new NSMenuItem and insert into menu
        if (s_buildMenus)
        {
            NSMenuItem * menuItem = [[NSMenuItem new] autorelease];
            menuItem.title = labelStr;
            [menuItem setTag:s_currentTagId];
            ++s_currentTagId;
            menuItem.action = @selector(OnClick:);
            menuItem.target = s_menuHandler;
            if (shortcut) // We'll assume the shortcut isn't going to dynamically change
            {
                NSString * shortcutStr = [[NSString stringWithUTF8String:shortcut] lowercaseString];
                NSUInteger flags = 0;
                if ([shortcutStr containsString:@"ctrl"])
                    flags |= NSEventModifierFlagCommand;
                if ([shortcutStr containsString:@"shift"])
                    flags |= NSEventModifierFlagShift;
                if ([shortcutStr containsString:@"alt"])
                    flags |= NSEventModifierFlagOption;
                [menuItem setKeyEquivalentModifierMask:flags];
                menuItem.keyEquivalent = [shortcutStr substringFromIndex:[shortcutStr length] - 1];
            }
            [s_menus.back().menu addItem:menuItem];
        }
        
        // Get the current menu item
        NSMenuItem * menuItem = [s_menus.back().menu itemAtIndex:idx];
        
        // If the labels don't match, something has dynamically changed and we need to clear and rebuild the menus
        if (![labelStr isEqualToString:menuItem.title])
        {
            s_clearMenus = true;
            return false;
        }
        
        // Set enabled and selected properties
        menuItem.enabled = enabled;
        menuItem.state = selected ? NSControlStateValueOn : NSControlStateValueOff;
        
        // Check for click only if we're enabled
        if (enabled)
        {
            // We use the tag integer to uniquely identify which menuitem was clicked, since
            // they all use the same handler target object.
            if ([menuItem tag] == s_selectedTagId)
            {
                // Reset selected tag and return true, indicating we've clicked on this menuitem
                s_selectedTagId = -1;
                return true;
            }
        }
        return false;
    }

    bool OsMenuItem(const char * label, const char * shortcut, bool * p_selected, bool enabled)
    {
        if (OsMenuItem(label, shortcut, p_selected ? *p_selected : false, enabled))
        {
            if (p_selected)
                *p_selected = !*p_selected;
            return true;
        }
        return false;
    }

    void OsSeparator()
    {
        if (s_clearMenus)
            return;
        
       // Increment index on stack
        s_menus.back().idx++;
        
        // If we're in a building pass, create new NSMenuItem as separator and insert into menu
        if (s_buildMenus)
        {
            NSMenuItem * menuItem = [NSMenuItem separatorItem];
            [s_menus.back().menu addItem:menuItem];
        }
    }
}
