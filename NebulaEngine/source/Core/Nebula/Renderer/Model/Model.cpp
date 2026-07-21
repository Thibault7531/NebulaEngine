#include "Model.h"

#include <glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace Nebula
{

    Model::Model(const std::string& path)
    {
        Assimp::Importer importer;
        const auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_PreTransformVertices);

        NB_CORE_ASSERT(scene, "Failed to load model: {}", path);

        ProcessNode(scene->mRootNode, scene);
    }

    void Model::Draw()
    {
        for (int32_t i = 0; i < m_Meshes.size(); i++) m_Meshes[i]->Draw();
    }

    void Model::ProcessNode(aiNode* node, const aiScene* scene)
    {
        for (int32_t i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_Meshes.push_back(ProcessMesh(mesh, scene));
        }
        for (int32_t i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    Ref<Mesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        /*
        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        for (int32_t i = 0; i < mesh->mNumVertices; i++)
        {
            glm::vec3 position;
            position.x = mesh->mVertices[i].x;
            position.y = mesh->mVertices[i].y;
            position.z = mesh->mVertices[i].z;
            vertices.push_back(position.x);
            vertices.push_back(position.y);
            vertices.push_back(position.z);
            glm::vec3 normal;
            normal.x = mesh->mNormals[i].x;
            normal.y = mesh->mNormals[i].y;
            normal.z = mesh->mNormals[i].z;
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
        }
        for (int32_t i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (int32_t j = 0; j < face.mNumIndices; j++) indices.push_back(face.mIndices[j]);
        }

        Ref<Graphics::VertexArray> vertexArray = VertexArray::Create();
        Ref<Graphics::VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices.size() * sizeof(float));
        vertexBuffer->SetData(vertices.data(), vertices.size() * sizeof(float));
        vertexBuffer->SetLayout({
            {Graphics::ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal" }
        });
        vertexArray->AddVertexBuffer(vertexBuffer);
        Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices.data(), indices.size());
        vertexArray->SetIndexBuffer(indexBuffer);

        return CreateRef<Mesh>(vertexArray);
        */
    }

    Ref<Model> Model::Create(const std::string& path)
    {
        return CreateRef<Model>(path);
    }
}
