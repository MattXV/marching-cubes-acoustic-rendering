#include "Model.h"

namespace unda {
    std::unordered_map<std::string, std::vector<LoadedMesh>> loadedMeshes = std::unordered_map<std::string, std::vector<LoadedMesh>>();
    std::unordered_map<std::string, std::unique_ptr<Texture>> loadedTextures = std::unordered_map<std::string, std::unique_ptr<Texture>>();
    unsigned int Model::uniqueId = 0;
    Model::~Model()
    {
        for (Mesh& mesh : meshes) {
            GLCALL(glDeleteBuffers(1, &mesh.vbo));
            if (mesh.indexCount > 0) { GLCALL(glDeleteBuffers(1, &mesh.ibo)); }
            GLCALL(glDeleteVertexArrays(1, &mesh.vao));
        }
        meshes.clear();
    }


    LoadedMesh* unda::loadMesh(const std::vector<Vertex>&& _vertices, const std::vector<unsigned int>&& _indices, Texture* texture)
    {
        LoadedMesh* mesh = new LoadedMesh();
        mesh->vertices = std::make_unique<std::vector<Vertex>>(_vertices);
        mesh->indices = std::make_unique<std::vector<unsigned int>>(_indices);
        mesh->texture = texture;

        unsigned int vao = 0, vbo = 0, ibo = 0;
        long unsigned int indexCount = 0, vertexCount;
        GLCALL(glGenVertexArrays(1, &vao));
        GLCALL(glGenBuffers(1, &vbo));
        if (!mesh->indices->empty()) {
            GLCALL(glGenBuffers(1, &ibo));
        }
        GLCALL(glBindVertexArray(vao));
            GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
                GLCALL(glBufferData(GL_ARRAY_BUFFER, mesh->vertices->size() * sizeof(Vertex), (void*)mesh->vertices->data(), GL_STATIC_DRAW));
                GLCALL(glEnableVertexAttribArray(unda::shaders::vertexPositionLocation)); // vertexPosition
                GLCALL(glVertexAttribPointer(shaders::vertexPositionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)));
                GLCALL(glEnableVertexAttribArray(unda::shaders::uvCoordinatesLocation)); // uvCoordinates
                GLCALL(glVertexAttribPointer(shaders::uvCoordinatesLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u)));
                GLCALL(glEnableVertexAttribArray(unda::shaders::vertexNormalLocation)); // vertexNormal
                GLCALL(glVertexAttribPointer(shaders::vertexNormalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx)));
                if (!mesh->indices->empty()) {
                    indexCount = (unsigned int)mesh->indices->size();
                    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
                    GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), (void*)mesh->indices->data(), GL_STATIC_DRAW));
                }
            GLCALL(glBindVertexArray(NULL));
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, NULL));
        if (!mesh->indices->empty())
            GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL));
        vertexCount = (unsigned int)mesh->vertices->size();
        mesh->vao = vao;
        mesh->vbo = vbo;
        mesh->ibo = ibo;
        return mesh;
    }


    void Model::calculateAABB()
    {
        if (isBuffered) {
            UNDA_ERROR("Too late to do vertex operation! Meshes already buffered!");
            return;
        }
        Vertex minVertex, maxVertex;
        float distance = 0.0f;
        bool first = true, absFirst = true;

        Vertex nearBottomLeft, nearBottomRight, nearTopRight, nearTopLeft;
        Vertex farBottomLeft, farBottomRight, farTopRight, farTopLeft;
        glm::vec3 surfaceNormal = glm::vec3();
        glm::mat4 worldTransform;
        glm::vec3 absMin, absMax;
        glm::vec2 UVmin, UVmax;

        std::function<void(Vertex&)> getMinimum = [&](Vertex& vertex) {
            glm::vec4 worldPosition = worldTransform * glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f);
            glm::vec4 worldNormal = worldTransform * glm::vec4(vertex.nx, vertex.ny, vertex.nz, 1.0f);
            //vertex.x = worldPosition.x;
            //vertex.y = worldPosition.y;
            //vertex.z = worldPosition.z;
            //vertex.nx = worldNormal.x;
            //vertex.ny = worldNormal.y;
            //vertex.nz = worldNormal.z;

            bool changed = false;
            bool minChanged = false;
            bool maxChanged = false;
            if (first) {
                minVertex = Vertex(worldPosition.x, worldPosition.y, worldPosition.z, vertex.u, vertex.v, worldNormal.x, worldNormal.y, worldNormal.z);
                maxVertex = Vertex(worldPosition.x, worldPosition.y, worldPosition.z, vertex.u, vertex.v, worldNormal.x, worldNormal.y, worldNormal.z);
                nearBottomLeft = nearBottomRight = nearTopRight = nearTopLeft = vertex;
                farBottomLeft = farBottomRight = farTopRight = farTopLeft = vertex;
                first = false;
                surfaceNormal = glm::vec3(vertex.nx, vertex.ny, vertex.nz);
            }
            if (absFirst) {
                absMin = absMax = worldPosition;
                UVmin = UVmax = glm::vec2(vertex.u, vertex.v);
                absFirst = false;
            }

            if (worldPosition.x < minVertex.x) { minVertex.x = worldPosition.x; minChanged = true; } // Bounding points
            if (worldPosition.y < minVertex.y) { minVertex.y = worldPosition.y; minChanged = true; }
            if (worldPosition.z < minVertex.z) { minVertex.z = worldPosition.z; minChanged = true; }

            if (worldPosition.x > maxVertex.x) { maxVertex.x = worldPosition.x; maxChanged = true; } // Bounding points
            if (worldPosition.y > maxVertex.y) { maxVertex.y = worldPosition.y; maxChanged = true; }
            if (worldPosition.z > maxVertex.z) { maxVertex.z = worldPosition.z; maxChanged = true; }

            if (worldPosition.x > absMax.x) { absMax.x = worldPosition.x; } 
            if (worldPosition.y > absMax.y) { absMax.y = worldPosition.y; }
            if (worldPosition.z > absMax.z) { absMax.z = worldPosition.z; }

            if (worldPosition.x < absMin.x) { absMin.x = worldPosition.x; } 
            if (worldPosition.y < absMin.y) { absMin.y = worldPosition.y; }
            if (worldPosition.z < absMin.z) { absMin.z = worldPosition.z; }

            if (vertex.u < UVmin.x) UVmin.x = vertex.u;
            if (vertex.v < UVmin.y) UVmin.y = vertex.v;
            if (vertex.u > UVmax.x) UVmax.x = vertex.u;
            if (vertex.v > UVmax.y) UVmax.y = vertex.v;

            if (minChanged) {
                minVertex.u = vertex.u;
                minVertex.v = vertex.v;
                //minVertex.nx = worldNormal.x;
                //minVertex.ny = worldNormal.y;
                //minVertex.nz = worldNormal.z;
            }
            if (maxChanged) {
                maxVertex.u = vertex.u;
                maxVertex.v = vertex.v;
                //minVertex.nx = worldNormal.x;
                //minVertex.ny = worldNormal.y;
                //minVertex.nz = worldNormal.z;
            }

            

            // 
            if (worldPosition.x < nearBottomLeft.x  && worldPosition.y < nearBottomLeft.y  && worldPosition.z < nearBottomLeft.z)  nearBottomLeft  = vertex;
            if (worldPosition.x < nearTopLeft.x     && worldPosition.y > nearTopLeft.y     && worldPosition.z < nearTopLeft.z)     nearTopLeft     = vertex;
            if (worldPosition.x > nearTopRight.x    && worldPosition.y > nearTopRight.y    && worldPosition.z < nearTopRight.z)    nearTopRight    = vertex;
            if (worldPosition.x > nearBottomRight.x && worldPosition.y < nearBottomRight.y && worldPosition.z < nearBottomRight.z) nearBottomRight = vertex;

            if (worldPosition.x < farBottomLeft.x   && worldPosition.y < farBottomLeft.y   && worldPosition.z > farBottomLeft.z)  farBottomLeft    = vertex;
            if (worldPosition.x < farTopLeft.x      && worldPosition.y > farTopLeft.y      && worldPosition.z > farTopLeft.z)     farTopLeft       = vertex;
            if (worldPosition.x > farTopRight.x     && worldPosition.y > farTopRight.y     && worldPosition.z > farTopRight.z)    farTopRight      = vertex;
            if (worldPosition.x > farBottomRight.x  && worldPosition.y < farBottomRight.y  && worldPosition.z > farBottomRight.z) farBottomRight   = vertex;
            //surfaceNormal = glm::normalize(surfaceNormal + glm::vec3(vertex.nx, vertex.ny, vertex.nz));
        };

        std::function<void(Vertex&)> getMaximum = [&](Vertex& vertex) {
            glm::vec4 worldPosition = worldTransform * glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f);
            glm::vec4 worldNormal = worldTransform * glm::vec4(vertex.nx, vertex.ny, vertex.nz, 1.0f);

            //glm::vec3 currentPoint{ worldPosition.x, worldPosition.y, worldPosition.z };
            float newDistance = fabs(glm::distance(glm::vec3(minVertex.x, minVertex.y, minVertex.z), glm::vec3(worldPosition.x, worldPosition.y, worldPosition.z)));
            if (newDistance > distance) {
                distance = newDistance;
                maxVertex = Vertex(worldPosition.x, worldPosition.y, worldPosition.z, vertex.u, vertex.v, worldNormal.x, worldNormal.y, worldNormal.z);
            }
        };

        float maxAABB = 0.0f;
        for (Mesh& mesh : meshes) {
            distance = 0.0f;
            first = true;
            //worldTransform = createModelMatrix(mesh.rotation, mesh.position / (float)normalisationScale, mesh.scale);
            worldTransform = mesh.transform;
            std::for_each(mesh.vertices->begin(), mesh.vertices->end(), getMinimum);
            //std::for_each(mesh.vertices->begin(), mesh.vertices->end(), getMaximum);

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
            aabb.UVmin = UVmin;
            aabb.UVmax = UVmax;

            mesh.aabb = aabb;
            if (fabs(mesh.aabb.min.x) > maxAABB) maxAABB = fabs(mesh.aabb.min.x);
            if (fabs(mesh.aabb.min.y) > maxAABB) maxAABB = fabs(mesh.aabb.min.y);
            if (fabs(mesh.aabb.min.z) > maxAABB) maxAABB = fabs(mesh.aabb.min.z);
            if (fabs(mesh.aabb.max.x) > maxAABB) maxAABB = fabs(mesh.aabb.max.x);
            if (fabs(mesh.aabb.max.y) > maxAABB) maxAABB = fabs(mesh.aabb.max.y);
            if (fabs(mesh.aabb.max.z) > maxAABB) maxAABB = fabs(mesh.aabb.max.z);
            
        }
        for (Mesh& mesh : meshes) {
            mesh.aabb.min.x /= maxAABB;
            mesh.aabb.min.y /= maxAABB;
            mesh.aabb.min.z /= maxAABB;
            mesh.aabb.max.x /= maxAABB;
            mesh.aabb.max.y /= maxAABB;
            mesh.aabb.max.z /= maxAABB;
        }

        volume = absMax - absMin;
        normaliseMeshes();
    }

    double Model::normaliseMeshes()
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
            glm::vec3 position = glm::vec3(
                vertex.x / float(*maxValue) * 2.0f - 1.0f,
                vertex.y / float(*maxValue) * 2.0f - 1.0f,
                vertex.z / float(*maxValue) * 2.0f - 1.0f);
            vertex.x = position.x;
            vertex.y = position.y;
            vertex.z = position.z;
        };

        for (std::pair<const std::string, std::vector<LoadedMesh>>& loaded : loadedMeshes) {
            for (LoadedMesh& submesh : loaded.second) {
                std::for_each(submesh.vertices->begin(), submesh.vertices->end(), getAbsMax);
            }
        }
        for (std::pair<const std::string, std::vector<LoadedMesh>>& loaded : loadedMeshes) {
            for (LoadedMesh& submesh : loaded.second) {
                std::for_each(submesh.vertices->begin(), submesh.vertices->end(), normalise);
            }
        }

        double out = double(*maxValue);
        normalisationScale = out;
        delete maxValue;
        return out;
    }

    glm::vec3 Model::calculateBoundingVolume()
    {
        calculateAABB();
        bool first = true;
        glm::vec3 dimensions;
        glm::vec3 min, max;

        assert(!meshes.empty());
        if (meshes.empty()) return glm::vec3();
        for (auto& mesh : meshes) {
            if (first) {
                min = glm::vec3(mesh.aabb.min.x, mesh.aabb.min.y, mesh.aabb.min.z);
                max = glm::vec3(mesh.aabb.max.x, mesh.aabb.max.y, mesh.aabb.max.z);
                first = false;
            }
            if (mesh.aabb.min.x < min.x) min.x = mesh.aabb.min.x;
            if (mesh.aabb.min.y < min.y) min.y = mesh.aabb.min.y;
            if (mesh.aabb.min.z < min.z) min.z = mesh.aabb.min.z;

            if (mesh.aabb.max.x > max.x) min.x = mesh.aabb.max.x;
            if (mesh.aabb.max.y > max.y) min.y = mesh.aabb.max.y;
            if (mesh.aabb.max.z > max.z) min.z = mesh.aabb.max.z;
        }

        return max - min;
        //return volume;
    }

    bool Model::exportModel(const std::string& filename, bool overwrite)
    {
        if (isBuffered) return false;
        auto path = std::filesystem::path(filename);
        if (std::filesystem::exists(path)) {
            if (std::filesystem::is_directory(path)) return false;
            if (overwrite) std::filesystem::remove(path); else return false;
        }

        std::ofstream fs;
        fs.open(filename.c_str());
        if (!fs.is_open()) return false;

        fs << "# Marching Cubes Volume" << std::endl;
        fs << std::endl;
        const Mesh& mesh = getMeshes()[0];
        for (const Vertex& vertex : *mesh.vertices) {
            fs << "v " << std::to_string(vertex.x) +  " " + std::to_string(vertex.y) + " " + std::to_string(vertex.z) << std::endl;
        }
        for (const Vertex& vertex : *mesh.vertices) {

            std::string nx = std::to_string(vertex.nx);
            std::string ny = std::to_string(vertex.ny);
            std::string nz = std::to_string(vertex.nz);
            if (nx.find("nan") != std::string::npos) nx = "0.0";
            if (ny.find("nan") != std::string::npos) ny = "0.0";
            if (nz.find("nan") != std::string::npos) nz = "0.0";
            fs << "vn " << nx + " " + ny + " " + nz << std::endl;
        }
        size_t nTriangles = mesh.vertices->size() / 3;
        for (size_t f = 0; f < nTriangles - 1; f++) {
            
            unsigned int index = ((unsigned int)f * 3);
            //std::string n = std::to_string(index);
            //fs << "f " + std::to_string(index + 0) + "//" + n + " " + std::to_string(index + 1) + "//" + n + " " + std::to_string(index + 2) + "//" + n + " " << std::endl;
            fs << "f " + std::to_string(index + 1) + " " + std::to_string(index + 2) + " " + std::to_string(index + 3) << std::endl;
        }
        fs.close();
        return true;
    }

    // -------------------------------------------------------------------------


    Model* fromVertexData(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, const std::string& name, Texture* texture)
    {
        Model* model = new Model();
        LoadedMesh* loadedMesh = loadMesh(std::forward<std::vector<Vertex>>(vertices), std::forward<std::vector<unsigned int>>(indices), texture);

        loadedMeshes[name] = std::vector<LoadedMesh>();

        Mesh mesh = Mesh();
        mesh.vertices = loadedMesh->vertices.get();
        mesh.indices = loadedMesh->indices.get();

        mesh.vao = loadedMesh->vao;
        mesh.vbo = loadedMesh->vbo;
        mesh.ibo = loadedMesh->ibo;
        if (!texture) {
            if (loadedTextures.find("Default") == loadedTextures.end())
                loadedTextures["Default"] = std::make_unique<Texture>(1, 1, Colour<unsigned char>(255, 255, 255, 255));
            texture = loadedTextures["Default"].get();
        }
        mesh.texture = texture;
        mesh.vertexCount = (unsigned long)loadedMesh->vertices->size();
        mesh.vertexCount = (unsigned long)loadedMesh->indices->size();

        mesh.meshFileName = name;
        model->getMeshes().push_back(std::move(mesh));
        loadedMeshes[name].push_back(std::move(*loadedMesh));
        return model;
    }

    std::vector<Mesh> loadMeshes(const std::string& objFileName)
    {
        std::vector<Mesh> outMeshes;

        std::string meshName = std::filesystem::path(objFileName).filename().stem().string();
        if (loadedMeshes.find(meshName) != loadedMeshes.end()) {
            // Point this mesh to existing vertices and indices
            
            for (const LoadedMesh& loaded : loadedMeshes[meshName]) {
                Mesh mesh = Mesh();
                mesh.vertices = loaded.vertices.get();
                mesh.indices = loaded.indices.get();
                mesh.texture = loaded.texture;
                mesh.vao = loaded.vao;
                mesh.vbo = loaded.vbo;
                mesh.ibo = loaded.ibo;
                mesh.vertexCount = (unsigned long)loaded.vertices->size();
                mesh.vertexCount = (unsigned long)loaded.indices->size();
                outMeshes.push_back(mesh);
            }
        } else {
            // Load from OBJ file
            loadedMeshes[meshName] = std::vector<LoadedMesh>();

            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(
                objFileName,
                aiProcess_CalcTangentSpace      |
                aiProcess_Triangulate           |
                aiProcess_JoinIdenticalVertices |
                aiProcess_SortByPType           |
                aiProcess_FlipUVs);
            if (!scene) {
                UNDA_ERROR("[Assimp Error]: Unable to read file... " << objFileName);
            }
            std::filesystem::path baseModelPath(objFileName);
            std::string modelFileName = baseModelPath.stem().string();
            baseModelPath.remove_filename();
            loadedMeshes[meshName].resize(scene->mNumMeshes);
            for (unsigned int n_mesh = 0; n_mesh < scene->mNumMeshes; n_mesh++) {
                std::vector<unda::Vertex> vertices;
                std::vector<unsigned int> indices;

                aiMesh* ai_mesh = scene->mMeshes[n_mesh];
                float x, y, z, u = 0.0f, v = 0.0f, nx = 0.0f, ny = 0.0f, nz = 0.0f;
                for (int j = 0; j < (int)ai_mesh->mNumVertices; j++) {
                    const aiVector3D& pos = ai_mesh->mVertices[j];
                    x = pos.x;
                    y = pos.y;
                    z = pos.z;
                    if (ai_mesh->HasTextureCoords(0)) {
                        const aiVector3D& textureCoords = ai_mesh->mTextureCoords[0][j];
                        u = textureCoords.x;
                        v = textureCoords.y;
                    }
                    if (ai_mesh->HasNormals()) {
                        const aiVector3D& normal = ai_mesh->mNormals[j];
                        nx = normal.x;
                        ny = normal.y;
                        nz = normal.z;
                    }
                    vertices.emplace_back(x, y, z, u, v, nx, ny, nz);
                }
                for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++) {
                    const aiFace& face = ai_mesh->mFaces[i];
                    if (face.mNumIndices != 3)
                        continue;
                    indices.push_back((unsigned int)face.mIndices[0]);
                    indices.push_back((unsigned int)face.mIndices[1]);
                    indices.push_back((unsigned int)face.mIndices[2]);
                }
                glm::mat4 transform = glm::mat4(1.0f);
                aiNode* node = scene->mRootNode->FindNode(ai_mesh->mName);
                if (node) {
                    aiMatrix4x4 aiTransform = node->mTransformation;
                    transform = glm::make_mat4(&aiTransform.a1);
                }
                std::string texturePath;
                for (unsigned int m = 0; m < scene->mNumMaterials; ++m)
                {
                    aiMaterial* material = scene->mMaterials[m];//Get the current material
                    aiString materialName;//The name of the material found in mesh file
                    aiReturn ret;//Code which says whether loading something has been successful of not
                    ret = material->Get(AI_MATKEY_NAME, materialName);//Get the material name (pass by reference)
                    if (ret != AI_SUCCESS) materialName = "";//Failed to find material name so makes var empt
                    //Diffuse maps
                    int numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);//Amount of diffuse textures
                    aiString textureName;//Filename of the texture using the aiString assimp structure
                    if (numTextures > 0)
                    {
                        ret = material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), textureName);
                        texturePath = textureName.data;//The actual name of the texture file
                        break;
                    }
                }
                Texture* texture = nullptr;
                if (!texturePath.empty()) {
                    std::filesystem::path relativeTexturePath = baseModelPath.make_preferred() / std::filesystem::path(texturePath).make_preferred();
                    std::string textureName = relativeTexturePath.filename().stem().string();
                    if (loadedTextures.find(textureName) == loadedTextures.end())
                        loadedTextures[textureName] = std::make_unique<Texture>(relativeTexturePath.string());
                    texture = loadedTextures[textureName].get();
                    
                }
                else {
                    texture = new Texture(1, 1, Colour<unsigned char>(255, 255, 255, 255));
                }
                LoadedMesh* newLoaded = loadMesh(std::move(vertices), std::move(indices), texture);
                Mesh mesh = Mesh();
                mesh.vertices = newLoaded->vertices.get();
                mesh.indices = newLoaded->indices.get();
                mesh.texture = newLoaded->texture;
                mesh.vao = newLoaded->vao;
                mesh.vbo = newLoaded->vbo;
                mesh.ibo = newLoaded->ibo;
                mesh.vertexCount = (unsigned long)newLoaded->vertices->size();
                mesh.vertexCount = (unsigned long)newLoaded->indices->size();

                loadedMeshes[meshName][n_mesh] = *newLoaded;
                outMeshes.push_back(mesh);
            }
            UNDA_LOG_MESSAGE("Loaded: " + meshName);
        }
        return outMeshes;
    }


    Model* loadSceneGraph(const std::string& sceneGraphJson)
    {
        auto basePath = std::filesystem::path(sceneGraphJson).remove_filename();
        Model* scene = new Model();

        using json = nlohmann::json;
        std::string raw = utils::ReadTextFile(sceneGraphJson);
        json sceneNodes = json::parse(raw);
        
        std::string meshFileName, nodeName;
        std::array<double, 3> transform, rotation, scale;
        for (const auto& entry : sceneNodes) {
            meshFileName = entry["meshFileName"].get<std::string>();
            transform = entry["transform"].get<std::array<double, 3>>();
            rotation = entry["rotation"].get<std::array<double, 3>>();
            scale = entry["scale"].get<std::array<double, 3>>();
            nodeName = entry["nodeName"].get<std::string>();

            glm::vec3 vPos(transform[0], transform[1], transform[2]), vRot(rotation[0], rotation[1], rotation[2]), vScl(scale[0], scale[1], scale[2]);
            glm::mat4 myTranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(transform[0], transform[1], transform[2]));
            glm::mat4 myRotationMatrix = glm::mat4(1.0f);
            myRotationMatrix = glm::rotate(myRotationMatrix, vRot.x * pi / 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            myRotationMatrix = glm::rotate(myRotationMatrix, vRot.y * pi / 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
            myRotationMatrix = glm::rotate(myRotationMatrix, vRot.z * pi / 180.0f, glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 myScaleMatrix = glm::scale(glm::mat4(1.0f), vScl);
            glm::mat4 myModelMatrix = myTranslationMatrix * myRotationMatrix * myScaleMatrix;

            std::vector<Mesh> meshes = loadMeshes(std::filesystem::path(basePath / meshFileName).string());
            for (Mesh& mesh : meshes) {
                mesh.position = vPos;
                mesh.rotation = vRot;
                mesh.scale = vScl;
                mesh.transform = myModelMatrix;
                mesh.meshFileName = meshFileName;
                mesh.name = nodeName + std::to_string(Model::uniqueId++);

                scene->getMeshes().push_back(std::move(mesh));
            }
        }
        return scene;
    }
}