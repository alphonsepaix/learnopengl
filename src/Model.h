#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/scene.h>

#include "Shader.h"
#include "Texture.h"

#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices,
         const std::vector<Texture> &textures);

    void draw(const Shader *shader) const;

private:
    GLuint m_vao, m_vbo, m_ebo;
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture> m_textures;

    void setupMesh();
};

class Model {
public:
    explicit Model(const std::string &path);

    void draw(const Shader *shader) const;

private:
    std::vector<Mesh> m_meshes;
    std::string m_directory;

    void loadModel(const std::string &path);

    void processNode(const aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    std::vector<Texture> loadMaterialTextures(const aiMaterial *mat, aiTextureType type);
};

struct ModelMatrix {
    glm::vec3 translation = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f); // in degrees
    float scale = 1.0f;

    [[nodiscard]] std::pair<glm::mat4, glm::mat3> compute() const;
};

class ModelManager {
public:
    ModelManager();

    void widgets();

    void draw(const Shader *shader) const;

private:
    std::vector<Model> m_objects;
    std::vector<bool> m_activeObjects;
    std::vector<ModelMatrix> m_models;

    void loadObject(const std::string &path);
};

#endif
