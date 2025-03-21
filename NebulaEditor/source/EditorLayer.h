#pragma once
#include "Nebula/Core/Layer.h"
#include "Nebula/Events/KeyEvent.h"

namespace Nebula {

    class EditorLayer : public Layer {
    public:
        EditorLayer() = default;
        ~EditorLayer() override = default;

        void OnUpdate(Timestep ts) override;
        void OnEvent(Event &event) override;
        void OnImGuiRender() override;

    private:
        bool OnKeyPressedEvent(const KeyPressedEvent& event);
    };

}
