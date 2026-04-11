import AppKit
import UniformTypeIdentifiers

import NebulaFileDialogs

struct CallbackWrapper: @unchecked Sendable {
    let Callback: Nebula.Platform.CallbackHandler
}

public func OpenFile(callback: Nebula.Platform.CallbackHandler) {
    let CallBackWrapper = CallbackWrapper(Callback: callback)
    
    Task { @MainActor in
        var OpenPanel = NSOpenPanel()
        OpenPanel.canChooseFiles = true
        OpenPanel.canChooseDirectories = false
        OpenPanel.allowsMultipleSelection = false
        
        OpenPanel.begin { response in
            if response == .OK {
                if let url = OpenPanel.url {
                    CallBackWrapper.Callback.OnFileSelected(std.string(url.relativePath))
                }
            }
        }
    }
}

public func SaveFile(fileType: Nebula.Platform.FileType, callback: Nebula.Platform.CallbackHandler) {
    let CallBackWrapper = CallbackWrapper(Callback: callback)
    
    Task { @MainActor in
        var SavePanel = NSSavePanel()
        let FileInfo = SaveFileInfo(fileType: fileType)
        
        SavePanel.title = FileInfo.title
        SavePanel.prompt = FileInfo.prompt
        SavePanel.isExtensionHidden = false
        FileInfo.extensions.forEach({ type in
            let uttype = UTType(filenameExtension: type)
            if let uttypereal = uttype { SavePanel.allowedContentTypes.append(uttypereal) }
        })
        SavePanel.allowsOtherFileTypes = false
        
        SavePanel.begin { response in
            if response == .OK {
                if let url = SavePanel.url {
                    CallBackWrapper.Callback.OnFileSelected(std.string(url.absoluteString))
                }
            }
        }
    }
}

struct SaveFileInfo {
    let title: String
    let prompt: String
    let extensions: [String]
    
    init(fileType: Nebula.Platform.FileType) {
        switch (fileType) {
        case .NebulaScene:
            title = "Save Scene"
            prompt = "Save"
            extensions = ["nbscene"]
            
        default:
            title = "Save"
            prompt = "Save"
            extensions = []
        }
    }
}
