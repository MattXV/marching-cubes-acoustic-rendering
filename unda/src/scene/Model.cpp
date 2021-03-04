#include "Model.h"

namespace unda {
    Model::Model(const std::vector<Vertex>& vertices, const std::vector<unsigned int> indices, Texture* t)
        : texture(t)
    {
        GLCALL(glGenVertexArrays(1, &vao));
        GLCALL(glGenBuffers(1, &vbo));
        if (!indices.empty()) {
            hasIndices = true;
            GLCALL(glGenBuffers(1, &ibo));
        }
        GLCALL(glBindVertexArray(vao));

        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW));
        GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), nullptr));
        GLCALL(glEnableVertexAttribArray(unda::shaders::vertexPositionLocation)); // vertexPosition
        GLCALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u)));
        GLCALL(glEnableVertexAttribArray(unda::shaders::uvCoordinatesLocation)); // uvCoordinates
        GLCALL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx)));
        GLCALL(glEnableVertexAttribArray(unda::shaders::vertexNormalLocation)); // vertexNormal
        if (hasIndices) {
            indexCount = (unsigned int)indices.size();
            GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
            GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));
        }

        glBindVertexArray(NULL);
        glBindBuffer(GL_ARRAY_BUFFER, NULL);
        if (hasIndices)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
    }

    Model::~Model()
    {
        glDeleteBuffers(1, &vbo);
        if (hasIndices) glDeleteBuffers(1, &ibo);
        glDeleteVertexArrays(1, &vao);
    }


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
        float x, y, z, u, v, nx, ny, nz;
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
            nx = 0.0f;
            ny = 0.0f;
            nz = 0.0f;
            if (mesh->HasNormals()) {
                const aiVector3D& normal = mesh->mNormals[j];
                nx = normal.x;
                ny = normal.y;
                nz = normal.z;
            }

            vertices.emplace_back(x, y, z, u, v, nx, ny, nz);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            const aiFace& face = mesh->mFaces[i];
            if (face.mNumIndices != 3)
                continue;
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }
        //unsigned int vbo = unda::createVBO(vertices);
        //unsigned int ibo = unda::createIBO(indices);
        Texture* texture = nullptr;
        if (!texturePath.empty())
            texture = new Texture(texturePath);
        else
            texture = new Texture(1024, 1024, unda::Colour<unsigned char>(70, 70, 70, 255));

        return new Model(vertices, indices, texture);
        //return new unda::Model(vbo, ibo, (unsigned int)indices.size(), texture);
    }
}
