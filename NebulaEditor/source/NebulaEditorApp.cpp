#include <Nebula.h>
#include <Nebula/CoreSystems/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Nebula {

    class NebulaEditor : public Application
    {
    public:
        NebulaEditor()
            : Application({ .Title = "NebulaEditor", .Border = false})
        {
            NB_INFO("Launching Nebula Editor");

            m_EditorLayer = new EditorLayer();
            PushLayer(m_EditorLayer);
        }
        ~NebulaEditor() override
        {
            NB_INFO("Closing Nebula Editor");

            PopLayer(m_EditorLayer);
            delete m_EditorLayer;
        };

    private:
        Layer* m_EditorLayer;
    };

    Application* CreateApplication()
    {
        return new NebulaEditor();
    }

}
