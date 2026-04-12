import NebulaPlatformWindow

import AppKit

public struct OpenGLContext {
    let m_Window: NSWindow
    let m_OGLContext: NSOpenGLContext
    nonisolated(unsafe) private static var s_MainOGLContext: NSOpenGLContext? = nil
    
    init(windowID: Int32) {
        m_Window = NSApplication.shared.window(withWindowNumber: Int(windowID))!
        
        var attribs: [NSOpenGLPixelFormatAttribute] = [
            NSOpenGLPixelFormatAttribute(NSOpenGLPFADoubleBuffer),
            NSOpenGLPixelFormatAttribute(NSOpenGLPFADepthSize),
            NSOpenGLPixelFormatAttribute(24),
            NSOpenGLPixelFormatAttribute(NSOpenGLPFAOpenGLProfile),
            NSOpenGLPixelFormatAttribute(NSOpenGLProfileVersion4_1Core),
            NSOpenGLPixelFormatAttribute(0)
        ]
        
        let pf: NSOpenGLPixelFormat? = NSOpenGLPixelFormat(attributes: attribs)
        m_OGLContext = NSOpenGLContext(format: pf!, share: OpenGLContext.s_MainOGLContext)!
        m_OGLContext.view = m_Window.contentView
        m_OGLContext.makeCurrentContext()
        
        if (OpenGLContext.s_MainOGLContext == nil) {
            OpenGLContext.s_MainOGLContext = m_OGLContext
        }
    }
    
    public func SetCurrent() {
        m_OGLContext.makeCurrentContext()
    }
    public func SetVSync(mode: NebulaPlatformWindow.Nebula.Platform.VSyncMode) {
        var newMode: [GLint] = [1]
        if (mode == .Off) { newMode[0] = 0 }
        m_OGLContext.setValues(newMode, for: .swapInterval)
    }
    public func IsVSync() -> NebulaPlatformWindow.Nebula.Platform.VSyncMode {
        var mode: [GLint] = [0]
        let p_mode: UnsafeMutablePointer = UnsafeMutablePointer<GLint>.allocate(capacity: 1)
        p_mode.initialize(from: &mode, count: 1)
        m_OGLContext.getValues(p_mode, for: .swapInterval)
        
        if (mode[0] == 0) { return .Off }
        else { return .On }
    }
    public func SwapBuffers() {
        MainActor.assumeIsolated {
            m_OGLContext.update()
        }
        m_OGLContext.flushBuffer()
    }
}

public func CreateOpenGLContext(windowID: Int32) -> OpenGLContext {
    return OpenGLContext(windowID: windowID)
}
