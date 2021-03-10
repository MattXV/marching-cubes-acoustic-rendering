#include "Model.h"

namespace unda {
    Model::Model(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Texture* t)
    {
        addMesh(vertices, indices, t);
    }

    Model::~Model()
    {
        for (Mesh& mesh : meshes) {
            glDeleteBuffers(1, &mesh.vbo);
            if (mesh.indexCount > 0) glDeleteBuffers(1, &mesh.ibo);
            glDeleteVertexArrays(1, &mesh.vao);
        }
        meshes.clear();
    }

    void Model::addMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, Texture* texture)
    {
        unsigned int vao, vbo, ibo = 0;
        long unsigned int indexCount = 0, vertexCount;
        GLCALL(glGenVertexArrays(1, &vao));
        GLCALL(glGenBuffers(1, &vbo));
        if (!indices.empty()) {
            GLCALL(glGenBuffers(1, &ibo));
        }
        GLCALL(glBindVertexArray(vao));

        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW));
        
        GLCALL(glEnableVertexAttribArray(unda::shaders::vertexPositionLocation)); // vertexPosition
        GLCALL(glVertexAttribPointer(shaders::vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)));
        
        GLCALL(glEnableVertexAttribArray(unda::shaders::uvCoordinatesLocation)); // uvCoordinates
        GLCALL(glVertexAttribPointer(shaders::uvCoordinatesLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u)));
        
        GLCALL(glEnableVertexAttribArray(unda::shaders::vertexNormalLocation)); // vertexNormal
        GLCALL(glVertexAttribPointer(shaders::vertexNormalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx)));
        if (!indices.empty()) {
            indexCount = (unsigned int)indices.size();
            GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
            GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));
        }

        GLCALL(glBindVertexArray(NULL));
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, NULL));
        if (!indices.empty())
            GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL));
        vertexCount = (unsigned int)vertices.size();

        meshes.push_back(Mesh(vao, vbo, ibo, vertexCount, indexCount, texture));
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
        float x, y, z, u = 0.0f, v = 0.0f, nx = 0.0f, ny = 0.0f, nz = 0.0f;
        for (int j = 0; j < (int)mesh->mNumVertices; j++) {
            // Extract vertices from the mesh object.
            const aiVector3D* pos = &mesh->mVertices[j];
            x = pos->x;
            y = pos->y;
            z = pos->z;

            if (mesh->HasTextureCoords(0)) {
                aiVector3D textureCoords = mesh->mTextureCoords[0][j];
                u = textureCoords.x;
                v = textureCoords.y;
            }
            if (mesh->HasNormals()) {
                const aiVector3D& normal = mesh->mNormals[j];
                nx = normal.x;
                ny = normal.y;
                nz = normal.z;
            }

            vertices.push_back(Vertex(x, y, z, u, v, nx, ny, nz));
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            const aiFace& face = mesh->mFaces[i];
            if (face.mNumIndices != 3)
                continue;
            indices.push_back((unsigned int)face.mIndices[0]);
            indices.push_back((unsigned int)face.mIndices[1]);
            indices.push_back((unsigned int)face.mIndices[2]);
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


    Model* unda::createSphereModel(int resolution, float radius)
    {
        auto [vertices, indices] = unda::primitives::createSphere(resolution, radius);

        //unsigned int vbo = unda::createVBO(vertices);
        //unsigned int ibo = unda::createIBO(indices);
        //unsigned int indexCount = indices.size();
        Texture* texture = new Texture(1024, 1024, unda::Colour<unsigned char>(70, 70, 70, 255));

        return new unda::Model(vertices, indices, texture);
    }

}
