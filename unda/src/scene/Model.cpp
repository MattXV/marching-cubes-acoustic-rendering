#include "Model.h"

Model* loadModel(const std::string& modelPath, const std::string& texturePath)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        modelPath,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);
    if (!scene) {
        std::cerr << "[Assimp Error]: Unable to read file... " << modelPath << std::endl;
        return nullptr;
    }

    if (scene->mNumMeshes > 1) {
        std::cout << "Warning! More than one mesh in file: " << modelPath << std::endl;
    }
    aiMesh* mesh = scene->mMeshes[0];
    std::vector<unda::Vertex> vertices;
    std::vector<unsigned int> indices;
    float x, y, z, u, v;
    for (int j = 0; j < (int)mesh->mNumVertices; j++) {
        // Extract vertices from the mesh object.
        const aiVector3D* pos = &mesh->mVertices[j];
        x = pos->x;
        y = pos->y;
        z = pos->z;
        u = 0.0f;
        v = 0.0f;
        if (mesh->HasTextureCoords(0)) {
            aiVector3D textureCoords = mesh->mTextureCoords[0][j];
            u = textureCoords.x;
            v = textureCoords.y;
        }
        vertices.emplace_back(x, y, z, u, v);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];
        if (face.mNumIndices != 3)
            continue;
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }
    unsigned int vbo = unda::createVBO(vertices);
    unsigned int ibo = unda::createIBO(indices);
    Texture* texture = nullptr;
    if (!texturePath.empty())
        texture = new Texture(texturePath);

    return new Model(vbo, ibo, texture);
}
