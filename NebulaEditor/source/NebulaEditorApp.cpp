#include <Nebula.h>
#include <Nebula/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Nebula {

    class NebulaEditor : public Application
    {
    public:
        NebulaEditor()
        {
            PushLayer(new EditorLayer());
        }
        ~NebulaEditor() override = default;
    };

    Application* CreateApplication()
    {
        return new NebulaEditor();
    }

}
