#include <Nebula/CoreSystems/Core/EntryPoint.h>

#include "RendererLayer.h"

class ExampleApp : public Nebula::Application
{
public:
    ExampleApp()
    {
        m_Layer = Nebula::CreateRef<RendererLayer>();
        PushLayer(m_Layer.get());
    }
    ~ExampleApp() override
    {
        PopLayer(m_Layer.get());
        m_Layer.reset();
    }

private:
    Nebula::Ref<RendererLayer> m_Layer;
};

Nebula::Application* Nebula::CreateApplication()
{
    return new ExampleApp();
}