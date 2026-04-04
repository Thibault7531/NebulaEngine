import AppKit
import Carbon.HIToolbox

import NebulaInput

public func IsKeyPressed(key: KeyboardKey) -> Bool {
    return CGEventSource.keyState(.combinedSessionState, key: CGKeyCode(GetNativeKeycode(key: key)))
}

public func IsMouseButtonPressed(button: MouseButton) -> Bool {
    let nativeButton: CGMouseButton
    switch (button) {
    case .Left:
        nativeButton = CGMouseButton(rawValue: 0)!
        
    case .Right:
        nativeButton = CGMouseButton(rawValue: 1)!
        
    case .Middle:
        nativeButton = CGMouseButton(rawValue: 2)!
        
    case .X1:
        nativeButton = CGMouseButton(rawValue: 3)!
        
    case .X2:
        nativeButton = CGMouseButton(rawValue: 4)!
        
    default:
        nativeButton = CGMouseButton(rawValue: 0)!
    }
    return CGEventSource.buttonState(.combinedSessionState, button: nativeButton)
}

public func GetMouseX() -> Float {
    return Float(NSEvent.mouseLocation.x)
}

public func GetMouseY() -> Float {
    return Float(NSScreen.main!.frame.height - NSEvent.mouseLocation.y)
}

public func GetNativeKeycode(key: KeyboardKey) -> Int {
    switch (key) {
    case .A: return 0x00
    case .S: return 0x01
    case .D: return 0x02
    case .F: return 0x03
    case .H: return 0x04
    case .G: return 0x05
    case .Z: return 0x06
    case .X: return 0x07
    case .C: return 0x08
    case .V: return 0x09
    case .B: return 0x0B
    case .Q: return 0x0C
    case .W: return 0x0D
    case .E: return 0x0E
    case .R: return 0x0F
    case .Y: return 0x10
    case .T: return 0x11
    case .Num1: return 0x12
    case .Num2: return 0x13
    case .Num3: return 0x14
    case .Num4: return 0x15
    case .Num6: return 0x16
    case .Num5: return 0x17
    case .Equals: return 0x18
    case .Num9: return 0x19
    case .Num7: return 0x1A
    case .Minus: return 0x1B
    case .Num8: return 0x1C
    case .Num0: return 0x1D
    case .RightBracket: return 0x1E
    case .O: return 0x1F
    case .U: return 0x20
    case .LeftBracket: return 0x21
    case .I: return 0x22
    case .P: return 0x23
    case .Return: return 0x24
    case .L: return 0x25
    case .J: return 0x26
    case .Apostrophe: return 0x27
    case .K: return 0x28
    case .Semicolon: return 0x29
    case .Backslash: return 0x2A
    case .Comma: return 0x2B
    case .Slash: return 0x2C
    case .N: return 0x2D
    case .M: return 0x2E
    case .Period: return 0x2F
    case .Tab: return 0x30
    case .Space: return 0x31
    case .GraveAccent: return 0x32
    case .Backspace: return 0x33
    case .Escape: return 0x35
    case .LeftGUI: return 0x37
    case .LeftShift: return 0x38
    case .CapsLock: return 0x39
    case .LeftAlt: return 0x3A
    case .LeftControl: return 0x3B
    case .RightShift: return 0x3C
    case .RightAlt: return 0x3D
    case .RightControl: return 0x3E
    case .F17: return 0x40
    case .KeypadPeriod: return 0x41
    case .KeypadMultiply: return 0x43
    case .KeypadPlus: return 0x45
    case .KeypadClear: return 0x47
    case .VolumeUp: return 0x48
    case .VolumeDown: return 0x49
    case .Mute: return 0x4A
    case .KeypadDivide: return 0x4B
    case .KeypadEnter: return 0x4C
    case .KeypadMinus: return 0x4E
    case .F18: return 0x4F
    case .F19: return 0x50
    case .KeypadEquals: return 0x51
    case .Keypad0: return 0x52
    case .Keypad1: return 0x53
    case .Keypad2: return 0x54
    case .Keypad3: return 0x55
    case .Keypad4: return 0x56
    case .Keypad5: return 0x57
    case .Keypad6: return 0x58
    case .Keypad7: return 0x59
    case .F20: return 0x5A
    case .Keypad8: return 0x5B
    case .Keypad9: return 0x5C
    case .F5: return 0x60
    case .F6: return 0x61
    case .F7: return 0x62
    case .F3: return 0x63
    case .F8: return 0x64
    case .F9: return 0x65
    case .F11: return 0x67
    case .F13: return 0x69
    case .F16: return 0x6A
    case .F14: return 0x6B
    case .F10: return 0x6D
    case .F12: return 0x6F
    case .F15: return 0x71
    case .Help: return 0x72
    case .Home: return 0x73
    case .PageUp: return 0x74
    case .Delete: return 0x75
    case .F4: return 0x76
    case .End: return 0x77
    case .F2: return 0x78
    case .PageDown: return 0x79
    case .F1: return 0x7A
    case .LeftArrow: return 0x7B
    case .RightArrow: return 0x7C
    case .DownArrow: return 0x7D
    case .UpArrow: return 0x7E
    default: return -1
    }
}
