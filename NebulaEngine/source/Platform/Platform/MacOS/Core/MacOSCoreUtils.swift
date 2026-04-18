import AppKit

@MainActor
private var appDelegate: AppDelegate?

@MainActor
class AppDelegate: NSObject, NSApplicationDelegate {
    func applicationShouldHandleReopen(_ sender: NSApplication, hasVisibleWindows: Bool) -> Bool {
        if !hasVisibleWindows {
            for window in sender.windows {
                window.makeKeyAndOrderFront(nil)
            }
        }
        return true
    }
}

public func InitThread() {
    MainActor.assumeIsolated {
        NSApplication.shared.setActivationPolicy(.regular)
        
        if appDelegate == nil {
            appDelegate = AppDelegate()
        }
        NSApplication.shared.delegate = appDelegate
        
        NSApplication.shared.finishLaunching()
        NSApplication.shared.activate(ignoringOtherApps: true)
    }
}
