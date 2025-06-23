#include <Nebula.h>
#include <Nebula/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Nebula {

    class NebulaEditor : public Application
    {
    public:
        NebulaEditor()
            : Application({ .Title = "NebulaEditor", .Border = true })
        {
            NB_INFO("Launching Nebula Editor");

            PushLayer(new EditorLayer());
        }
        ~NebulaEditor() override
        {
            NB_INFO("Closing Nebula Editor");
        };
    };

    Application* CreateApplication()
    {
        return new NebulaEditor();
    }

}
