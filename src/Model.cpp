#define DBG_MACRO_NO_WARNING
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <dbg.h>
#include <fmt/format.h>

#include "Model.h"

#include <unordered_map>

std::unordered_map<std::string, std::unique_ptr<Texture> > loadedTextures;

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices,
           const std::vector<Texture> &textures): m_vertices{vertices}, m_indices{indices}, m_textures{textures} {
    setupMesh();
}

void Mesh::draw(const Shader &shader) const {
    auto diffuseNr = 1;
    auto specularNr = 1;

    for (auto i = 0; i < m_textures.size(); ++i) {
        auto &texture = m_textures[i];
        texture.setUnit(i);
        std::string name;
        int number;
        switch (texture.getType()) {
            case Texture::Type::Diffuse:
                name = "diffuse";
                number = diffuseNr++;
                break;
            case Texture::Type::Specular:
                name = "specular";
                number = specularNr++;
                break;
        }
        name = fmt::format("material.texture_{}{}", name, number);
        shader.setInt(name, i);
        texture.bind();
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(offsetof(Vertex, texCoords)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

Model::Model(const std::string &path) {
    loadModel(path);
}

void Model::draw(const Shader &shader) const {
    for (const auto &mesh: m_meshes)
        mesh.draw(shader);
}

void Model::loadModel(const std::string &path) {
    Assimp::Importer importer;
    const auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp: error when loading model: " << importer.GetErrorString() << std::endl;
        return;
    }

    m_directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(const aiNode *node, const aiScene *scene) {
    for (auto i = 0; i < node->mNumMeshes; ++i) {
        const auto mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene));
    }
    for (auto i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // Process vertices.
    for (auto i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;

        // Positions.
        glm::vec3 vec;
        vec.x = mesh->mVertices[i].x;
        vec.y = mesh->mVertices[i].y;
        vec.z = mesh->mVertices[i].z;
        vertex.position = vec;

        // Normals.
        vec.x = mesh->mNormals[i].x;
        vec.y = mesh->mNormals[i].y;
        vec.z = mesh->mNormals[i].z;
        vertex.normal = vec;

        // Texture coordinates (we only support one set of texture coordinates).
        if (mesh->mTextureCoords[0]) {
            glm::vec2 tex;
            tex.x = mesh->mTextureCoords[0][i].x;
            tex.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = tex;
        }

        vertices.push_back(vertex);
    }

    // Process indices.
    for (auto i = 0; i < mesh->mNumFaces; ++i) {
        auto face = mesh->mFaces[i];
        for (auto j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Process material.
    auto material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    return Mesh{vertices, indices, textures};
}

std::vector<Texture> Model::loadMaterialTextures(const aiMaterial *const mat, const aiTextureType type) {
    std::vector<Texture> textures;
    for (auto i = 0; i < mat->GetTextureCount(type); ++i) {
        aiString str;
        mat->GetTexture(type, i, &str);
        auto path = fmt::format("{}/{}", m_directory, str.C_Str());
        if (const auto it = loadedTextures.find(path); it != loadedTextures.end()) {
            textures.push_back(*it->second);
        } else {
            auto texture = std::make_unique<Texture>(
                path, type == aiTextureType_DIFFUSE ? Texture::Type::Diffuse : Texture::Type::Specular
            );
            textures.push_back(*texture);
            loadedTextures[path] = std::move(texture);
        }
    }
    return textures;
}
