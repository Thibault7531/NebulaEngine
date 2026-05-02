import NebulaPlatformWindow

import UIKit

public struct iOSWindow {
    let m_Window: UIWindow
    
    public init(props: Nebula.Platform.WindowProperties) {
        m_Window = UIWindow()
    }
}

public func CreateiOSWindow(props: Nebula.Platform.WindowProperties) -> iOSWindow {
    return iOSWindow(props: props)
}
