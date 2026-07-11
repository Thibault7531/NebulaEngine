import AppKit
import Metal

import NebulaPlatformWindow

public struct Metal3Context {
    let m_Window: NSWindow
    let m_View: NSView
    let m_MetalLayer: CAMetalLayer
    
    @MainActor static var s_MetalDevice: MTLDevice = MTLCreateSystemDefaultDevice()!
    
    init(window: UnsafeMutableRawPointer) {
        m_Window = Unmanaged<NSWindow>.fromOpaque(window).takeUnretainedValue()
        m_View = m_Window.contentView!
        
        m_MetalLayer = CAMetalLayer();
        MainActor.assumeIsolated {
            m_MetalLayer.device = Metal3Context.s_MetalDevice
        }
        m_MetalLayer.pixelFormat = MTLPixelFormat.bgra8Unorm
        m_MetalLayer.isOpaque = true
        
        m_View.layer = m_MetalLayer
        m_View.wantsLayer = true
    }
    
    public func GetMetalLayer() -> UnsafeMutableRawPointer {
        let ptr = Unmanaged.passUnretained(m_MetalLayer).toOpaque()
        return ptr
    }
    
    public func GetMetalDevice() -> UnsafeMutableRawPointer {
        let ptr = Unmanaged.passUnretained(m_MetalLayer.device!).toOpaque()
        return ptr
    }
}

public func CreateMetal3Context(window: UnsafeMutableRawPointer) -> Metal3Context {
    return Metal3Context(window: window);
}
