#pragma once

#include "Mesh.h"

#include <vector>
#include <assimp/scene.h>

namespace Nebula
{

    class Model
    {
    public:
        Model(const std::string& path);
        ~Model() = default;

        std::vector<Ref<Mesh>>& GetMeshes() { return m_Meshes; }

    private:
        void ProcessNode(aiNode* node, const aiScene* scene);
        Ref<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);

    public:
        static Ref<Model> Create(const std::string& path);

    private:
        std::vector<Ref<Mesh>> m_Meshes;
        std::string m_Path;
    };

}
