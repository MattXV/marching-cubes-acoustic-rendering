#include "Model.h"

namespace unda {
    Model::Model(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, Texture* t)
    {
        addMesh(std::forward<std::vector<Vertex>>(vertices), std::forward<std::vector<unsigned int>>(indices), t);
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

    void Model::toVertexArray()
    {
        for (Mesh& mesh : meshes) {
            unsigned int vao, vbo, ibo = 0;
            long unsigned int indexCount = 0, vertexCount;
            GLCALL(glGenVertexArrays(1, &vao));
            GLCALL(glGenBuffers(1, &vbo));
            if (!mesh.indices.empty()) {
                GLCALL(glGenBuffers(1, &ibo));
            }
            GLCALL(glBindVertexArray(vao));

            GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
            GLCALL(glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW));
            GLCALL(glEnableVertexAttribArray(unda::shaders::vertexPositionLocation)); // vertexPosition
            GLCALL(glVertexAttribPointer(shaders::vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)));

            GLCALL(glEnableVertexAttribArray(unda::shaders::uvCoordinatesLocation)); // uvCoordinates
            GLCALL(glVertexAttribPointer(shaders::uvCoordinatesLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u)));

            GLCALL(glEnableVertexAttribArray(unda::shaders::vertexNormalLocation)); // vertexNormal
            GLCALL(glVertexAttribPointer(shaders::vertexNormalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx)));
            if (!mesh.indices.empty()) {
                indexCount = (unsigned int)mesh.indices.size();
                GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
                GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW));
            }

            GLCALL(glBindVertexArray(NULL));
            GLCALL(glBindBuffer(GL_ARRAY_BUFFER, NULL));
            if (!mesh.indices.empty())
                GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL));
            vertexCount = (unsigned int)mesh.vertices.size();

            mesh.vao = vao;
            mesh.vbo = vbo;
            mesh.ibo = ibo;
            mesh.vertexCount = (unsigned int)mesh.vertices.size();
            mesh.indexCount = indexCount;
            mesh.vertices.clear();
            mesh.indices.clear();
        }
        isBuffered = true;
    }

    void Model::addMesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, Texture* texture, const std::string& name, Texture* normal)
    {
        meshes.push_back(Mesh(std::forward<std::vector<Vertex>>(vertices), std::forward<std::vector<unsigned int>>(indices), texture, name, normal));
    }

    void Model::calculateAABB()
    {
        if (isBuffered) {
            std::cerr << "Too late to do vertex operation! Meshes already buffered!" << std::endl;
            return;
        }

        glm::vec3 min, max;
        float distance = 0.0f;
        bool first = true;

        std::function<void(Vertex&)> getMinimum = [&min, &first](Vertex& vertex) {
            if (first) {
                min.x = vertex.x;
                min.y = vertex.y;
                min.z = vertex.z;
                first = false;
            }

            if (vertex.x < min.x) min.x = vertex.x;
            if (vertex.y < min.y) min.y = vertex.y;
            if (vertex.z < min.z) min.z = vertex.z;
        };

        std::function<void(Vertex&)> getMaximum = [&min, &max, &distance](Vertex& vertex) {
            glm::vec3 point{ vertex.x, vertex.y, vertex.z };
            float newDistance = fabs(glm::distance(min, point));
            if (newDistance > distance) {
                distance = newDistance;
                max = point;
            }
        };

        for (Mesh& mesh : meshes) {
            distance = 0.0f;
            first = true;
            std::for_each(mesh.vertices.begin(), mesh.vertices.end(), getMinimum);
            std::for_each(mesh.vertices.begin(), mesh.vertices.end(), getMaximum);

            mesh.aabb = AABB(min, max, Transform::getPosition());
        }


        //for (Mesh& mesh : meshes) {
        //    memcpy(&min, &mesh.vertices[0], sizeof(Vertex));
        //    memcpy(&max, &mesh.vertices[0], sizeof(Vertex));
        //    for (Vertex& vertex : mesh.vertices) {
        //        if (vertex.x < min.x && vertex.y < min.y && vertex.z < min.z) {
        //            memcpy(&min, &vertex, sizeof(Vertex));
        //            //min = vertex;
        //        }
        //        if (vertex.x > max.x && vertex.y > max.y && vertex.z > max.z) {
        //            memcpy(&max, &vertex, sizeof(Vertex));
        //            //max = vertex;
        //        }
        //    }
        //    glm::vec3 cubeSize{ 0.05f, 0.05f, 0.05f };

            //mesh.aabb = AABB(glm::vec3(min.x, min.y, min.z) - cubeSize, glm::vec3(max.x, max.y, max.z) + cubeSize, getPosition());

            //ModifyVertices(mesh.vertices, getAABB);


        
    }

    void Model::normaliseMeshes()
    {
        float* maxValue = new float{ 1.0f };
        std::function<void(Vertex&)> getAbsMax = [&maxValue](Vertex& vertex) { 
            float x = fabs(vertex.x);
            float y = fabs(vertex.y);
            float z = fabs(vertex.z);
            if (x > *maxValue) memcpy(maxValue, &x, sizeof(float));
            if (y > *maxValue) memcpy(maxValue, &y, sizeof(float));
            if (z > *maxValue) memcpy(maxValue, &z, sizeof(float));

        };
        std::function<void(Vertex&)> normalise = [&maxValue](Vertex& vertex) { 
            vertex.x /= *maxValue;
            vertex.y /= *maxValue;
            vertex.z /= *maxValue;
        };
        for (Mesh& mesh : meshes) {
            ModifyVertices(mesh.vertices, getAbsMax); // maxValue = max(|vertices|) 
        }
        for (Mesh& mesh : meshes) {
            ModifyVertices(mesh.vertices, normalise); // vertices = vertices / maxValue
        }
        delete maxValue;
    }



    // -------------------------------------------------------------------------


    Model* loadSingleMesh(const std::string& modelPath, const std::string texturePath, const std::string normalPath)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            modelPath,
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType |
            aiProcess_FlipUVs);
        if (!scene) {
            std::cerr << "[Assimp Error]: Unable to read file... " << modelPath << std::endl;
            return nullptr;
        }


        Model* model = new Model();

        unsigned int sceneCount = 0;
        std::vector<unda::Vertex> vertices;
        std::vector<unsigned int> indices;

        aiMesh* mesh = scene->mMeshes[sceneCount];
        float x, y, z, u = 0.0f, v = 0.0f, nx = 0.0f, ny = 0.0f, nz = 0.0f;
        for (int j = 0; j < (int)mesh->mNumVertices; j++) {
            // Extract vertices from the mesh object.
            const aiVector3D& pos = mesh->mVertices[j];
            x = pos.x;
            y = pos.y;
            z = pos.z;

            if (mesh->HasTextureCoords(0)) {
                const aiVector3D& textureCoords = mesh->mTextureCoords[0][j];
                u = textureCoords.x;
                v = textureCoords.y;
            }
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
            indices.push_back((unsigned int)face.mIndices[0]);
            indices.push_back((unsigned int)face.mIndices[1]);
            indices.push_back((unsigned int)face.mIndices[2]);
        }

        Texture* texture = nullptr;
        Texture* normalMap = nullptr;
        if (!texturePath.empty()) {
            texture = new Texture(texturePath);
        }
        else {
            texture = new Texture(1024, 1024,
                unda::Colour<unsigned char>(255, 255, 255, 255));
        }
        if (normalPath.size() > 0) {
            normalMap = new Texture(normalPath);
        }

        model->addMesh(std::move(vertices), std::move(indices), texture, "singleMesh", normalMap);

        return model;

    }

    Model* loadModel(const std::string& modelPath, Colour<float> baseColour, bool verbose)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            modelPath,
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType |
            aiProcess_FlipUVs);
        if (!scene) {
            std::cerr << "[Assimp Error]: Unable to read file... " << modelPath << std::endl;
            return nullptr;
        }

        std::filesystem::path baseModelPath(modelPath);
        std::string modelFileName = baseModelPath.stem().string();
        baseModelPath.remove_filename();
        baseModelPath.append("*.png");
        std::vector<std::filesystem::path> textures = glob::glob(baseModelPath.string());

        
        std::vector<std::vector<unda::Vertex>> verticesArray; 
        std::vector<std::vector<unsigned int>> indicesArray; 
        std::vector<Texture*> texturesArrays;

        Model* model = new Model();
        
        float zero = 0.0f;
        float* maxValue = &zero;
        std::function<float(float&)> getMax = [&maxValue](float& vertexValue) {
            float x = fabs(vertexValue);
            if (x > fabs(*maxValue)) {
                *maxValue = x;
            }
            return vertexValue;
        };

        verticesArray.resize(scene->mNumMeshes);
        indicesArray.resize(scene->mNumMeshes);
        texturesArrays.resize(scene->mNumMeshes);
        for (unsigned int sceneCount = 0; sceneCount < scene->mNumMeshes; sceneCount++) {
            std::vector<unda::Vertex> vertices;
            std::vector<unsigned int> indices;
            aiMesh* mesh = scene->mMeshes[sceneCount];
            float x, y, z, u = 0.0f, v = 0.0f, nx = 0.0f, ny = 0.0f, nz = 0.0f;
            for (int j = 0; j < (int)mesh->mNumVertices; j++) {
                // Extract vertices from the mesh object.
                const aiVector3D& pos = mesh->mVertices[j];
                x = pos.x;
                y = pos.y;
                z = pos.z;

                if (mesh->HasTextureCoords(0)) {
                    const aiVector3D& textureCoords = mesh->mTextureCoords[0][j];
                    u = textureCoords.x;
                    v = textureCoords.y;
                }
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
                indices.push_back((unsigned int)face.mIndices[0]);
                indices.push_back((unsigned int)face.mIndices[1]);
                indices.push_back((unsigned int)face.mIndices[2]);
            }
            
            std::string texturePath, normalPath;
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            for (auto iterator = textures.begin(); iterator < textures.end(); iterator++) {
                std::filesystem::path& path = *iterator;
                
                if (path.string().find(material->GetName().C_Str()) != std::string::npos) {
                    if (verbose) {
                        std::cout << "Found aiStandardSurface!" << std::endl;
                    }
                    
                    if (path.string().find("BaseColor" + mesh->mMaterialIndex) != std::string::npos) {
                        if (verbose) {
                            std::cout << "Found colour texture!" << std::endl;
                            texturePath = path.string();
                        }
                        //textures.erase(iterator);
                        break;
                    }
                } else {
                    //if (path.string().find(modelFileName) != std::string::npos) {
                    //    texturePath = path.string();
                    //}
                }
            }
            // Normals
            for (auto iterator = textures.begin(); iterator < textures.end(); iterator++) {
                std::filesystem::path& path = *iterator;

                if (path.string().find(material->GetName().C_Str()) != std::string::npos) {
                    if (verbose) {
                        std::cout << "Found aiStandardSurface!" << std::endl;
                    }

                    if (path.string().find("Normal" + mesh->mMaterialIndex) != std::string::npos) {
                        if (verbose) {
                            std::cout << "Found colour texture!" << std::endl;
                            normalPath = path.string();
                        }
                        //textures.erase(iterator);
                        break;
                    }
                }
                else {
                    //if (path.string().find(modelFileName) != std::string::npos) {
                    //    texturePath = path.string();
                    //}
                }
            }


            Texture* texture = nullptr;
            Texture* normalMap = nullptr;
            if (!texturePath.empty()) {
                texture = new Texture(texturePath);
            }
            else {
                texture = new Texture(1024, 1024,
                    unda::Colour<unsigned char>((unsigned int)(baseColour.r * 255.0f), 
                                                (unsigned int)(baseColour.g * 255.0f),
                                                (unsigned int)(baseColour.b * 255.0f),
                                                (unsigned int)(baseColour.a * 255.0f)));
            }
            if (normalPath.size() > 0) {
                normalMap = new Texture(normalPath);
            }

            model->addMesh(std::move(vertices), std::move(indices), texture);

        }

        return model;

    }




    Model* loadMeshDirectory(const std::string& directoryPath, const std::string& extension, const Colour<float>& baseColour, bool verbose)
    {

        std::filesystem::path dirPath(directoryPath);
        dirPath.append("*." + extension);
        std::vector<std::filesystem::path> modelPaths = glob::rglob(dirPath.string());


        Model* combined = new Model();
        for (std::filesystem::path& path : modelPaths) {
            if (verbose) {
                std::cout << "Loading: " << path.string() << std::endl;
            }

            Model* loaded = loadModel(path.string(), baseColour, false);
            int i = 0;
            for (Mesh& mesh : loaded->getMeshes()) {
                std::string name = path.filename().stem().string() + std::to_string(i);
                combined->addMesh(std::move(mesh.vertices), std::move(mesh.indices), mesh.texture.release(), name);
                i++;
            }

            delete loaded;
        }

        return combined;
    }


}
