import AppKit
import Metal
import MetalKit

import NebulaPlatformWindow

public struct Metal3Surface {
    let m_Window: NSWindow
    let m_View: NSView
    let m_MetalLayer: CAMetalLayer
    
    init(window: UnsafeMutableRawPointer) {
        m_Window = Unmanaged<NSWindow>.fromOpaque(window).takeUnretainedValue()
        m_View = m_Window.contentView!
        
        m_MetalLayer = CAMetalLayer();
        m_MetalLayer.pixelFormat = MTLPixelFormat.rgba8Unorm
        m_MetalLayer.isOpaque = true
        m_MetalLayer.framebufferOnly = false;
        
        m_View.layer = m_MetalLayer
        m_View.wantsLayer = true
    }
    
    public func SetDevice(device: UnsafeMutableRawPointer) {
        let metalDevice = Unmanaged<MTLDevice>.fromOpaque(device).takeUnretainedValue()
        m_MetalLayer.device = metalDevice
    }
    
    public func GetMetalLayer() -> UnsafeMutableRawPointer {
        let ptr = Unmanaged.passUnretained(m_MetalLayer).toOpaque()
        return ptr
    }
}

public func CreateMetal3Surface(window: UnsafeMutableRawPointer) -> Metal3Surface {
    return Metal3Surface(window: window);
}
