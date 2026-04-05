import NebulaPlatformWindow
import NebulaOpenGLContext
import NebulaEventHandler

import AppKit
import Carbon
import CxxStdlib

public struct MacOSWindow {
    let m_Properties: Nebula.Platform.WindowProperties
    let m_Window: NSWindow
    var m_WindowStyle: NSWindow.StyleMask
    var m_OGLContext: Nebula.Platform.OpenGlContext
    var m_EventHandler: Nebula.Platform.EventHandler
    var m_WindowTrackingArea: NSTrackingArea
    var m_WindowDelegate: MacOSWindowDelegate
    
    init(props: Nebula.Platform.WindowProperties, eventHandler: Nebula.Platform.EventHandler) {
        m_Properties = props
        m_EventHandler = eventHandler
        
        var x: Int32 = props.x
        var y: Int32 = props.y
        if x == -INT32_MAX { x = 0 }
        if y == -INT32_MAX { y = 0 }
        let windowRect: NSRect = NSMakeRect(Double(x), Double(y), Double(props.Width), Double(props.Height))
        
        let screen: NSScreen = NSScreen.screens[0]
        
        m_WindowStyle = NSWindow.StyleMask()
        if props.Style == .BorderlessFullscreen {
            m_WindowStyle.insert(.borderless)
        } else if props.Style == .BorderlessInteractive {
            m_WindowStyle.insert(.titled)
            m_WindowStyle.insert(.closable)
            m_WindowStyle.insert(.miniaturizable)
            m_WindowStyle.insert(.fullSizeContentView)
        } else if props.Style == .Default {
            m_WindowStyle.insert(.titled)
            m_WindowStyle.insert(.closable)
            m_WindowStyle.insert(.miniaturizable)
        } else {
            m_WindowStyle.insert(.borderless)
            m_WindowStyle.insert(.fullScreen)
        }
        
        if (props.isResizable) { m_WindowStyle.insert(.resizable) }
        
        m_Window = NSWindow(contentRect: windowRect, styleMask: m_WindowStyle, backing: .buffered, defer: false, screen: screen)
        
        m_Window.title = String(props.Title)
        
        if (props.Maximized) { m_Window.performZoom(nil) }
        if (props.Minimized) { m_Window.miniaturize(nil) }
        
        if props.Style == .BorderlessInteractive {
            m_Window.titlebarAppearsTransparent = true
            m_Window.titleVisibility = .hidden
            m_Window.titlebarSeparatorStyle = .none
            
            m_Window.contentView!.additionalSafeAreaInsets = NSEdgeInsets(top: 0, left: 0, bottom: 0, right: 0)
        }
        
        let options: NSTrackingArea.Options = [
            .mouseEnteredAndExited,
            .inVisibleRect,
            .activeAlways
        ]
        
        m_WindowDelegate = MacOSWindowDelegate(EventHandler: m_EventHandler)
        m_Window.delegate = m_WindowDelegate
        m_Window.collectionBehavior = .fullScreenNone
        m_Window.makeKeyAndOrderFront(nil)
        m_Window.setIsVisible(true)
        
        m_OGLContext = Nebula.Platform.OpenGlContext(Int32(m_Window.windowNumber))
        
        m_WindowTrackingArea = NSTrackingArea(rect: .zero, options: options, owner: nil, userInfo: nil)
        MainActor.assumeIsolated {
            m_Window.contentView!.addTrackingArea(m_WindowTrackingArea)
        }
    }
    
    public mutating func OnUpdate() {
        SwapBuffers()
        
        RunLoop.main.run(mode: .default, before: Date())
        var now = Date()
        MainActor.assumeIsolated {
            var event = NSApp.nextEvent(matching: .any, until: now, inMode: .default, dequeue: true)
            while (event != nil) {
                if (event?.window == m_Window) {
                    switch (event!.type) {
                    case .leftMouseDown:
                        var nebulaEvent = Nebula.Platform.MouseButtonPressedEvent(.Left)
                        m_EventHandler.OnMouseButtonPressed(&nebulaEvent)
                        
                    case .leftMouseUp:
                        var nebulaEvent = Nebula.Platform.MouseButtonReleasedEvent(.Left)
                        m_EventHandler.OnMouseButtonReleased(&nebulaEvent)
                        
                    case .rightMouseDown:
                        var nebulaEvent = Nebula.Platform.MouseButtonPressedEvent(.Right)
                        m_EventHandler.OnMouseButtonPressed(&nebulaEvent)
                        
                    case .rightMouseUp:
                        var nebulaEvent = Nebula.Platform.MouseButtonReleasedEvent(.Right)
                        m_EventHandler.OnMouseButtonReleased(&nebulaEvent)
                        
                    case .otherMouseDown:
                        let button = event?.buttonNumber
                        var nebulaButton: MouseButton
                        switch (button) {
                        case 2:
                            nebulaButton = .Middle
                        case 3:
                            nebulaButton = .X1
                        case 4:
                            nebulaButton = .X2
                        default:
                            nebulaButton = .Unknown
                        }
                        var nebulaEvent = Nebula.Platform.MouseButtonPressedEvent(nebulaButton)
                        m_EventHandler.OnMouseButtonPressed(&nebulaEvent)
                        
                    case .otherMouseUp:
                        let button = event?.buttonNumber
                        var nebulaButton: MouseButton
                        switch (button) {
                        case 2:
                            nebulaButton = .Middle
                        case 3:
                            nebulaButton = .X1
                        case 4:
                            nebulaButton = .X2
                        default:
                            nebulaButton = .Unknown
                        }
                        var nebulaEvent = Nebula.Platform.MouseButtonReleasedEvent(nebulaButton)
                        m_EventHandler.OnMouseButtonReleased(&nebulaEvent)
                        
                    case .mouseMoved:
                        if (event!.window == nil) {break}
                        let mouseY = Float(m_Window.contentView!.frame.height - event!.window!.mouseLocationOutsideOfEventStream.y)
                        var nebulaEvent = Nebula.Platform.MouseMovedEvent(Float(event!.window!.mouseLocationOutsideOfEventStream.x), mouseY)
                        m_EventHandler.OnMouseMoved(&nebulaEvent)
                        
                    case .scrollWheel:
                        var nebulaEvent = Nebula.Platform.MouseScrolledEvent(Float(event!.deltaX), Float(event!.deltaY))
                        m_EventHandler.OnMouseScrolled(&nebulaEvent)
                        
                    case .mouseEntered:
                        if (event!.trackingArea == m_WindowTrackingArea) {
                            var nebulaEvent = Nebula.Platform.MouseEnteredEvent()
                            m_EventHandler.OnMouseEntered(&nebulaEvent)
                        }
                        
                    case .mouseExited:
                        if (event!.trackingArea == m_WindowTrackingArea) {
                            var nebulaEvent = Nebula.Platform.MouseLeftEvent()
                            m_EventHandler.OnMouseLeft(&nebulaEvent)
                        }
                        
                    case .keyDown:
                        let key = GetKeyboardKeyFromKeyCode(keycode: event!.keyCode)
                        
                        var nebulaEvent = Nebula.Platform.KeyPressedEvent(key, event!.isARepeat ? 1 : 0)
                        m_EventHandler.OnKeyPressed(&nebulaEvent)
                        
                        var nebulaCharEvent = Nebula.Platform.KeyTypedEvent(event!.characters!.cString(using: String.Encoding(rawValue: NSUTF8StringEncoding))![0])
                        m_EventHandler.OnKeyTyped(&nebulaCharEvent)
                        
                    case .keyUp:
                        let key = GetKeyboardKeyFromKeyCode(keycode: event!.keyCode)
                        
                        var nebulaEvent = Nebula.Platform.KeyReleasedEvent(key)
                        m_EventHandler.OnKeyReleased(&nebulaEvent)
                        
                    default:
                        break
                    }
                }
                NSApp.sendEvent(event!)
                event = m_Window.nextEvent(matching: .any, until: now, inMode: .default, dequeue: true)
            }
        }
    }
    public mutating func SwapBuffers() {
        m_OGLContext.SwapBuffers()
    }
    
    public func GetWindowWidth() -> UInt32 {
        return UInt32(m_Window.frame.width)
    }
    public func GetWindowHeight() -> UInt32 {
        return UInt32(m_Window.frame.height)
    }
    public func SetWindowSize(width: UInt32, height: UInt32) {
        var windowRect: NSRect = NSRect(x: m_Window.frame.minX, y: m_Window.frame.minY, width: CGFloat(width), height: CGFloat(height))
        m_Window.setFrame(windowRect, display: true)
    }
    public func GetClientWidth() -> UInt32 {
        var contentRect: NSRect = m_Window.frame
        contentRect = NSWindow.contentRect(forFrameRect: contentRect, styleMask: m_WindowStyle)
        return UInt32(contentRect.width)
    }
    public func GetClientHeight() -> UInt32 {
        var contentRect: NSRect = m_Window.frame
        contentRect = NSWindow.contentRect(forFrameRect: contentRect, styleMask: m_WindowStyle)
        return UInt32(contentRect.height)
    }
    public func SetClientSize(width: UInt32, height: UInt32) {
        m_Window.setContentSize(NSSize(width: Double(width), height: Double(height)))
    }
    public func GetX() -> Int32 {
        return Int32(m_Window.frame.origin.x)
    }
    public func GetY() -> Int32 {
        let windowFrame = m_Window.frame
        let mainScreen = NSScreen.main!
        return Int32(mainScreen.frame.height - windowFrame.origin.y - windowFrame.height)
    }
    public func SetPos(x: Int32, y: Int32) {
        var windowRect: NSRect = NSRect(x: CGFloat(x), y: CGFloat(y), width: m_Window.frame.width, height: m_Window.frame.height)
        m_Window.setFrame(windowRect, display: true)
    }
    public func GetClientX() -> Int32 {
        var contentRect = m_Window.contentView!.bounds
        contentRect = m_Window.convertToScreen(contentRect)
        return Int32(contentRect.origin.x)
    }
    public func GetClientY() -> Int32 {
        var contentRect = m_Window.contentView!.bounds
        contentRect = m_Window.convertToScreen(contentRect)
        let mainScreen = NSScreen.main!
        return Int32(mainScreen.frame.height - contentRect.origin.y - contentRect.height)
    }
    public func GetTitlebarHeight() -> Int32 {
        var contentRect: NSRect = m_Window.frame
        contentRect = NSWindow.contentRect(forFrameRect: contentRect, styleMask: m_WindowStyle)
        return Int32(contentRect.minX - m_Window.frame.minX)
    }
    
    public func SetMousePos(x: Int32, y:Int32) {
        var loc: CGPoint = CGPoint(x: CGFloat(GetClientX()), y: CGFloat(GetClientY()))
        loc.x += CGFloat(x)
        loc.y += m_Window.contentView!.frame.height - CGFloat(y)
        CGAssociateMouseAndMouseCursorPosition(boolean_t(false))
        CGDisplayMoveCursorToPoint(0, loc)
        CGAssociateMouseAndMouseCursorPosition(boolean_t(true))
    }
    
    public mutating func SetVSync(mode: Nebula.Platform.VSyncMode) {
        m_OGLContext.SetVSync(mode)
    }
    public func IsVSync() -> Nebula.Platform.VSyncMode {
        return m_OGLContext.IsVsync()
    }
    public func SetFullscreen(enabled: Bool) {
        if (m_Window.contentView!.isInFullScreenMode == enabled) { return }
        m_Window.toggleFullScreen(nil)
    }
    public func IsFullscreen() -> Bool {
        return m_Window.contentView!.isInFullScreenMode
    }
    public func SetMaximized(enabled: Bool) {
        let screen = m_Window.screen!
        m_Window.setFrame(screen.visibleFrame, display: true)
    }
    public func IsMaximized() -> Bool {
        let screen = m_Window.screen!
        return m_Window.frame == screen.visibleFrame
    }
    public func Minimize() {
        m_Window.miniaturize(nil)
    }
    public func IsMinimized() -> Bool {
        return m_Window.isMiniaturized
    }
    public func IsFocused() -> Bool {
        return m_Window.isKeyWindow
    }
    public func GetTitle() -> std.string {
        return std.string(m_Window.title)
    }
    public func SetTitle(title: std.string) {
        m_Window.title = String(title)
    }
    
    public func Show() {
        m_Window.setIsVisible(true)
    }
    public func Hide() {
        m_Window.setIsVisible(false)
    }
    public func Raise() {
        m_Window.setIsVisible(true)
    }
    public func Flash() {
        NSApplication.shared.requestUserAttention(.informationalRequest)
    }
    
    public mutating func SetCurrentContext() {
        m_OGLContext.SetCurrent()
    }
    
    public func GetID() -> UInt32 {
        return UInt32(m_Window.windowNumber)
    }
}

public class MacOSWindowDelegate: NSObject, NSWindowDelegate {
    var m_EventHandler: Nebula.Platform.EventHandler
    
    init(EventHandler: Nebula.Platform.EventHandler) {
        m_EventHandler = EventHandler
    }
    
    public func windowDidResize(_ notification: Notification) {
        guard let window = notification.object as? NSWindow else { return }
        let width = UInt32(window.contentView!.frame.width)
        let height = UInt32(window.contentView!.frame.height)
        
        var nebulaEvent = Nebula.Platform.WindowResizedEvent(width, height)
        m_EventHandler.OnWindowResized(&nebulaEvent)
    }
    
    public func windowShouldClose(_ sender: NSWindow) -> Bool {
        var nebulaEvent = Nebula.Platform.WindowClosedEvent()
        m_EventHandler.OnWindowClosed(&nebulaEvent)
        
        return false
    }
    
    public func windowWillMiniaturize(_ notification: Notification) {
        var nebulaEvent = Nebula.Platform.WindowMinimizedEvent()
        m_EventHandler.OnWindowMinimized(&nebulaEvent)
    }
    
    public func windowDidDeminiaturize(_ notification: Notification) {
        var nebulaEvent = Nebula.Platform.WindowRestoredEvent()
        m_EventHandler.OnWindowRestored(&nebulaEvent)
    }
    
    public func windowDidMove(_ notification: Notification) {
        guard let window = notification.object as? NSWindow else { return }
        
        let x = Int32(window.frame.origin.x)
        let windowFrame = window.frame
        let mainScreen = NSScreen.main!
        let y = Int32(mainScreen.frame.height - windowFrame.origin.y - windowFrame.height)
        
        var nebulaEvent = Nebula.Platform.WindowMovedEvent(x, y)
        m_EventHandler.OnWindowMoved(&nebulaEvent)
    }
    
    public func windowDidBecomeKey(_ notification: Notification) {
        var nebulaEvent = Nebula.Platform.WindowFocusedEvent()
        m_EventHandler.OnWindowFocused(&nebulaEvent)
    }
    
    public func windowDidResignKey(_ notification: Notification) {
        var nebulaEvent = Nebula.Platform.WindowLostFocusEvent()
        m_EventHandler.OnWindowLostFocus(&nebulaEvent)
    }
}

public func GetKeyboardKeyFromKeyCode(keycode: UInt16) -> KeyboardKey {
    switch keycode {
    case 0x00: return .A
    case 0x0B: return .B
    case 0x08: return .C
    case 0x02: return .D
    case 0x0E: return .E
    case 0x03: return .F
    case 0x05: return .G
    case 0x04: return .H
    case 0x22: return .I
    case 0x26: return .J
    case 0x28: return .K
    case 0x25: return .L
    case 0x2E: return .M
    case 0x2D: return .N
    case 0x1F: return .O
    case 0x23: return .P
    case 0x0C: return .Q
    case 0x0F: return .R
    case 0x01: return .S
    case 0x11: return .T
    case 0x20: return .U
    case 0x09: return .V
    case 0x0D: return .W
    case 0x07: return .X
    case 0x10: return .Y
    case 0x06: return .Z

    case 0x1D: return .Num0
    case 0x12: return .Num1
    case 0x13: return .Num2
    case 0x14: return .Num3
    case 0x15: return .Num4
    case 0x17: return .Num5
    case 0x16: return .Num6
    case 0x1A: return .Num7
    case 0x1C: return .Num8
    case 0x19: return .Num9

    case 0x24: return .Return
    case 0x35: return .Escape
    case 0x33: return .Backspace
    case 0x30: return .Tab
    case 0x31: return .Space
    case 0x1B: return .Minus
    case 0x18: return .Equals
    case 0x21: return .LeftBracket
    case 0x1E: return .RightBracket
    case 0x2A: return .Backslash
    case 0x29: return .Semicolon
    case 0x27: return .Apostrophe
    case 0x32: return .GraveAccent
    case 0x2B: return .Comma
    case 0x2F: return .Period
    case 0x2C: return .Slash

    case 0x7E: return .UpArrow
    case 0x7D: return .DownArrow
    case 0x7B: return .LeftArrow
    case 0x7C: return .RightArrow

    case 0x7A: return .F1
    case 0x78: return .F2
    case 0x63: return .F3
    case 0x76: return .F4
    case 0x60: return .F5
    case 0x61: return .F6
    case 0x62: return .F7
    case 0x64: return .F8
    case 0x65: return .F9
    case 0x6D: return .F10
    case 0x67: return .F11
    case 0x6F: return .F12
    case 0x69: return .F13
    case 0x6B: return .F14
    case 0x71: return .F15
    case 0x6A: return .F16
    case 0x40: return .F17
    case 0x4F: return .F18
    case 0x50: return .F19
    case 0x5A: return .F20

    case 0x72: return .Insert
    case 0x73: return .Home
    case 0x74: return .PageUp
    case 0x75: return .Delete
    case 0x77: return .End
    case 0x79: return .PageDown

    case 0x47: return .NumLock
    case 0x4B: return .KeypadDivide
    case 0x43: return .KeypadMultiply
    case 0x4E: return .KeypadMinus
    case 0x45: return .KeypadPlus
    case 0x4C: return .KeypadEnter
    case 0x52: return .Keypad0
    case 0x53: return .Keypad1
    case 0x54: return .Keypad2
    case 0x55: return .Keypad3
    case 0x56: return .Keypad4
    case 0x57: return .Keypad5
    case 0x58: return .Keypad6
    case 0x59: return .Keypad7
    case 0x5B: return .Keypad8
    case 0x5C: return .Keypad9
    case 0x41: return .KeypadPeriod
    case 0x51: return .KeypadEquals

    case 0x3B: return .LeftControl
    case 0x38: return .LeftShift
    case 0x3A: return .LeftAlt
    case 0x37: return .LeftGUI
    case 0x3E: return .RightControl
    case 0x3C: return .RightShift
    case 0x3D: return .RightAlt
    case 0x36: return .RightGUI

    case 0x39: return .CapsLock

    default: return .Unknown
    }
}

public func CreateMacOSWindow(props: Nebula.Platform.WindowProperties, eventHandler: Nebula.Platform.EventHandler) -> MacOSWindow {
    return MacOSWindow(props: props, eventHandler: eventHandler)
}
