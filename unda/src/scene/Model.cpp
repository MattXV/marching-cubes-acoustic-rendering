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
            GLCALL(glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), (void*)mesh.vertices.data(), GL_STATIC_DRAW));
            GLCALL(glEnableVertexAttribArray(unda::shaders::vertexPositionLocation)); // vertexPosition
            GLCALL(glVertexAttribPointer(shaders::vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)));

            GLCALL(glEnableVertexAttribArray(unda::shaders::uvCoordinatesLocation)); // uvCoordinates
            GLCALL(glVertexAttribPointer(shaders::uvCoordinatesLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u)));

            GLCALL(glEnableVertexAttribArray(unda::shaders::vertexNormalLocation)); // vertexNormal
            GLCALL(glVertexAttribPointer(shaders::vertexNormalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx)));
            if (!mesh.indices.empty()) {
                indexCount = (unsigned int)mesh.indices.size();
                GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
                GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), (void*)mesh.indices.data(), GL_STATIC_DRAW));
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

    void Model::addMesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, Texture* texture, const std::string& name, Texture* normal, glm::mat4 transform)
    {
        meshes.push_back(Mesh(std::forward<std::vector<Vertex>>(vertices), std::forward<std::vector<unsigned int>>(indices), texture, name, normal, transform));
    }

    void Model::calculateAABB()
    {
        if (isBuffered) {
            std::cerr << "Too late to do vertex operation! Meshes already buffered!" << std::endl;
            return;
        }

        Vertex minVertex, maxVertex;
        float distance = 0.0f;
        bool first = true;

        Vertex nearBottomLeft, nearBottomRight, nearTopRight, nearTopLeft;
        Vertex farBottomLeft, farBottomRight, farTopRight, farTopLeft;
        glm::vec3 surfaceNormal = glm::vec3();

        std::function<void(Vertex&)> getMinimum = [&](Vertex& vertex) {
            bool changed = false;
            if (first) {
                minVertex = vertex;
                nearBottomLeft = nearBottomRight = nearTopRight = nearTopLeft = vertex;
                farBottomLeft = farBottomRight = farTopRight = farTopLeft = vertex;
                first = false;
                surfaceNormal = glm::vec3(vertex.nx, vertex.ny, vertex.nz);
            }
            
            if (vertex.x < minVertex.x) { minVertex.x = vertex.x; changed = true; } // Bounding points
            if (vertex.y < minVertex.y) { minVertex.y = vertex.y; changed = true; }
            if (vertex.z < minVertex.z) { minVertex.z = vertex.z; changed = true; }
            if (changed) {
                minVertex.u = vertex.u;
                minVertex.v = vertex.v;
                minVertex.nx = vertex.nx;
                minVertex.ny = vertex.ny;
                minVertex.nz = vertex.nz;
            }

            if (vertex.x < nearBottomLeft.x  && vertex.y < nearBottomLeft.y  && vertex.z < nearBottomLeft.z)  nearBottomLeft  = vertex;
            if (vertex.x < nearTopLeft.x     && vertex.y > nearTopLeft.y     && vertex.z < nearTopLeft.z)     nearTopLeft     = vertex;
            if (vertex.x > nearTopRight.x    && vertex.y > nearTopRight.y    && vertex.z < nearTopRight.z)    nearTopRight    = vertex;
            if (vertex.x > nearBottomRight.x && vertex.y < nearBottomRight.y && vertex.z < nearBottomRight.z) nearBottomRight = vertex;

            if (vertex.x < farBottomLeft.x   && vertex.y < farBottomLeft.y   && vertex.z > farBottomLeft.z)  farBottomLeft    = vertex;
            if (vertex.x < farTopLeft.x      && vertex.y > farTopLeft.y      && vertex.z > farTopLeft.z)     farTopLeft       = vertex;
            if (vertex.x > farTopRight.x     && vertex.y > farTopRight.y     && vertex.z > farTopRight.z)    farTopRight      = vertex;
            if (vertex.x > farBottomRight.x  && vertex.y < farBottomRight.y  && vertex.z > farBottomRight.z) farBottomRight   = vertex;
            surfaceNormal = glm::normalize(surfaceNormal + glm::vec3(vertex.nx, vertex.ny, vertex.nz));
        };

        std::function<void(Vertex&)> getMaximum = [&minVertex, &maxVertex, &distance](Vertex& vertex) {
            glm::vec3 currentPoint{ vertex.x, vertex.y, vertex.z };
            float newDistance = fabs(glm::distance(glm::vec3(minVertex.x, minVertex.y, minVertex.z), currentPoint));
            if (newDistance > distance) {
                distance = newDistance;
                maxVertex = vertex;
            }
        };

        for (Mesh& mesh : meshes) {
            distance = 0.0f;
            first = true;
            std::for_each(mesh.vertices.begin(), mesh.vertices.end(), getMinimum);
            std::for_each(mesh.vertices.begin(), mesh.vertices.end(), getMaximum);

            AABB aabb = AABB(minVertex, maxVertex);
            aabb.nearBottomLeft = nearBottomLeft;
            aabb.nearBottomRight = nearBottomRight;
            aabb.nearTopRight = nearTopRight;
            aabb.nearTopLeft = nearTopLeft;
            aabb.farBottomLeft = farBottomLeft;
            aabb.farBottomRight = farBottomRight;
            aabb.farTopRight = farTopRight;
            aabb.farTopLeft = farTopLeft;
            aabb.surfaceNormal = surfaceNormal;

            mesh.aabb = aabb;
        }
  
    }

    void Model::normaliseMeshes()
    {
        float* maxValue = new float{ 1.0f };
        glm::mat4 transform = glm::mat4(1.0f);

        std::function<void(Vertex&)> getAbsMax = [&maxValue, &transform](Vertex& vertex) { 

            glm::vec4 position = glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f);


            float x = fabs(position.x);
            float y = fabs(position.y);
            float z = fabs(position.z);
            if (x > *maxValue) memcpy(maxValue, &x, sizeof(float));
            if (y > *maxValue) memcpy(maxValue, &y, sizeof(float));
            if (z > *maxValue) memcpy(maxValue, &z, sizeof(float));

        };
        std::function<void(Vertex&)> normalise = [&maxValue, &transform](Vertex& vertex) { 
            glm::vec4 position = glm::vec4(vertex.x / float(*maxValue), vertex.y / float(*maxValue), vertex.z / float(*maxValue), 1.0f);

            //position = transform * position;
            //position = position * 2.0f;

            vertex.x = position.x;
            vertex.y = position.y;
            vertex.z = position.z;

        };
        for (Mesh& mesh : meshes) {
            transform = mesh.tranform;
            std::for_each(mesh.vertices.begin(), mesh.vertices.end(), getAbsMax);
        }
        for (Mesh& mesh : meshes) {
            float scale = float(*maxValue);
            transform = transform / scale;

            glm::vec4 test = glm::vec4(1.0f);
            test = transform * test;
            glm::vec4 test2 = glm::vec4(1.0f);


            std::for_each(mesh.vertices.begin(), mesh.vertices.end(), normalise);
            mesh.scale = *maxValue;
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
            aiNode* node = scene->mRootNode->FindNode(mesh->mName);
            if (node) {
                aiMatrix4x4 transform = node->mTransformation;
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

        auto modelParentPath = baseModelPath.parent_path().remove_filename();
        auto texturesFolder = modelParentPath.append("Textures");
        baseModelPath.append("*.png");
        modelParentPath.append("**.png");
        texturesFolder.append("*.png");

        std::vector<std::filesystem::path> imageTexturesFound = glob::rglob(
            { baseModelPath.string(), modelParentPath.string() });
        
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
            
            glm::mat4 transform = glm::mat4(1.0f);
            aiNode* node = scene->mRootNode->FindNode(mesh->mName);
            if (node) {
                aiMatrix4x4 aiTransform = node->mTransformation;
                transform = glm::make_mat4(&aiTransform.a1);
            }
            // Look for textures
            std::string texturePath, normalPath;
            //aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            //for (auto& textureCandidate : imageTexturesFound) {
            //    
            //    if (textureCandidate.stem().string().find(material->GetName().C_Str()) != std::string::npos) {
            //        if (verbose) {
            //            std::cout << "Found aiStandardSurface!" << std::endl;
            //        }
            //        
            //        if (textureCandidate.stem().string().find("BaseColor" + mesh->mMaterialIndex) != std::string::npos) {
            //            if (verbose) {
            //                std::cout << "Found colour texture!" << std::endl;
            //            }
            //            texturePath = textureCandidate.string();
            //            //textures.erase(iterator);
            //            break;
            //        }
            //    } else {
            //        if (textureCandidate.stem().string().find(modelFileName) != std::string::npos) {
            //            texturePath = textureCandidate.string();
            //        }
            //    }
            //}
            //// Normals
            //for (auto& textureCandidate : imageTexturesFound) {

            //    if (textureCandidate.stem().string().find(material->GetName().C_Str()) != std::string::npos) {
            //        if (verbose) {
            //            std::cout << "Found aiStandardSurface!" << std::endl;
            //        }

            //        if (textureCandidate.stem().string().find("Normal" + mesh->mMaterialIndex) != std::string::npos) {
            //            if (verbose) {
            //                std::cout << "Found colour texture!" << std::endl;
            //            }
            //            normalPath = textureCandidate.string();
            //            //textures.erase(iterator);
            //            break;
            //        }
            //    }
            //    else {
            //        //if (path.string().find(modelFileName) != std::string::npos) {
            //        //    texturePath = path.string();
            //        //}
            //    }
            //}
            for (unsigned int m = 0; m < scene->mNumMaterials; ++m)
            {
                aiMaterial* material = scene->mMaterials[m];//Get the current material
                aiString materialName;//The name of the material found in mesh file
                aiReturn ret;//Code which says whether loading something has been successful of not

                ret = material->Get(AI_MATKEY_NAME, materialName);//Get the material name (pass by reference)
                if (ret != AI_SUCCESS) materialName = "";//Failed to find material name so makes var empty

                //Diffuse maps
                int numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);//Amount of diffuse textures
                aiString textureName;//Filename of the texture using the aiString assimp structure

                if (numTextures > 0)
                {
                    //Get the file name of the texture by passing the variable by reference again
                    //Second param is 0, which is the first diffuse texture
                    //There can be more diffuse textures but for now we are only interested in the first one
                    ret = material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), textureName);
                    texturePath = textureName.data;//The actual name of the texture file
                }
            }
            Texture* texture = nullptr;
            Texture* normalMap = nullptr;
            if (!texturePath.empty()) {
                baseModelPath = baseModelPath.remove_filename();
                std::filesystem::path relativeTexturePath = baseModelPath.make_preferred() / std::filesystem::path(texturePath).make_preferred();
                std::string textureName = relativeTexturePath.filename().stem().string();

                for (std::pair<const std::string, std::unique_ptr<Texture>>& textureEntry : textures) {
                    if (textureEntry.first.find(textureName) != std::string::npos) {
                        texture = textureEntry.second.get();
                        break;
                    }
                }
                if (!texture) {
                    texture = new Texture(relativeTexturePath.string());
                    textures.insert(std::make_pair(textureName, std::unique_ptr<Texture>(std::move(texture))));
                }
            }
            else {
                std::string textureName = "Default";
                for (std::pair<const std::string, std::unique_ptr<Texture>>& textureEntry : textures) {
                    if (textureEntry.first.find(textureName) != std::string::npos) {
                        texture = textureEntry.second.get();
                        break;
                    }
                }
                if (!texture) {
                    texture = new Texture(1024, 1024,
                        unda::Colour<unsigned char>((unsigned int)(baseColour.r * 255.0f), 
                                                    (unsigned int)(baseColour.g * 255.0f),
                                                    (unsigned int)(baseColour.b * 255.0f),
                                                    (unsigned int)(baseColour.a * 255.0f)));
                    textures.insert(std::make_pair(textureName, std::unique_ptr<Texture>(std::move(texture))));
                    }
                }
            if (normalPath.size() > 0) {
                normalMap = new Texture(normalPath);
            }

            std::string name = std::string(mesh->mName.C_Str());
            model->addMesh(std::move(vertices), std::move(indices), texture, name, nullptr, transform);

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
                combined->addMesh(std::move(mesh.vertices), std::move(mesh.indices), mesh.texture, name);
                i++;
            }

            delete loaded;
        }

        return combined;
    }
}