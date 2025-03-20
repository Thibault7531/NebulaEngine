#pragma once
#include "Nebula/Core/Layer.h"

namespace Nebula {

    class EditorLayer : public Layer {
    public:
        EditorLayer() = default;
        ~EditorLayer() override = default;

        void OnUpdate(Timestep ts) override;
        void OnImGuiRender() override;
    };

}
