#include "objparser.hpp"

ObjParser::ObjParser(const std::string& filename) : log("OBJ Parser") {
    log << Log::Level::INFO << "Trying to open file " << filename << std::endl;

    std::ifstream file(filename);
    if (!file.is_open()) {
        log << Log::Level::ERR << "Could not open file " << filename << std::endl;
        return;
    }

    Object* newObject = NULL;

    int vertexOffset = 0;
    int normalOffset = 0;
    int texCoordOffset = 0;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "o") {
            if (newObject != NULL) {
                vertexOffset += newObject->getVertexCount();
                normalOffset += newObject->getNormalCount();
                texCoordOffset += newObject->getTexCoordCount();

                if (newObject->getTrangleCount() > 0) {
                    newObject->pack();
                    this->objects.push_back(*newObject);

                    log << Log::Level::INFO << std::format("Loaded object: {} ({} vertices, {} normals, {} texture coords, {} trangles)",
                                                            newObject->name,
                                                            newObject->getVertexCount(),
                                                            newObject->getNormalCount(),
                                                            newObject->getTexCoordCount(),
                                                            newObject->getTrangleCount()) << std::endl;
                }
                delete newObject;
            }

            std::string name;
            iss >> name;
            log << Log::Level::INFO << std::format("Loading object: {}", name) << std::endl;
            newObject = new Object(name);
        }

        if (newObject == NULL)
            continue;

        if (prefix == "v") {
            Object::Vertex_t vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            vertex.w = 1.0f;
            newObject->addVertex(vertex);
        } else if (prefix == "vn") {
            Object::Normal_t normal;
            iss >> normal.x >> normal.y >> normal.z;
            newObject->addNormal(normal);
        } else if (prefix == "vt") {
            Object::TexCoord_t texCoord;
            iss >> texCoord.x >> texCoord.y;
            newObject->addTexCoord(texCoord);
        } else if (prefix == "f") {
            Object::Trangle_t trangle;

            std::string faceStr;
            for (int i = 0; i < 3; ++i) {
                iss >> faceStr;

                std::istringstream faceStream(faceStr);
                DWORD vIndex, nIndex, tIndex;
                char slash;

                if (std::count(faceStr.begin(), faceStr.end(), '/') == 2) {
                    // Format: v/t/n or v//n
                    faceStream >> vIndex >> slash >> tIndex >> slash >> nIndex;
                } else if (std::count(faceStr.begin(), faceStr.end(), '/') == 1) {
                    // Format: v/t or v/n
                    faceStream >> vIndex >> slash >> tIndex;
                    nIndex = 0;
                } else {
                    // Format: v
                    faceStream >> vIndex;
                    tIndex = nIndex = 0;
                }

                trangle.faces[i].vertexIndex = vIndex ? vIndex - vertexOffset - 1 : 0;
                trangle.faces[i].texCoordIndex = tIndex ? tIndex - texCoordOffset - 1 : 0;
                trangle.faces[i].normalIndex = nIndex ? nIndex - normalOffset - 1 : 0;
            }

            newObject->addTrangle(trangle);
        }
    }

    if (newObject != NULL) {
        if (newObject->getTrangleCount() > 0) {
            newObject->pack();
            this->objects.push_back(*newObject);
            log << Log::Level::INFO << std::format("Loaded object: {} ({} vertices, {} normals, {} texture coords, {} trangles)",
                                                    newObject->name,
                                                    newObject->getVertexCount(),
                                                    newObject->getNormalCount(),
                                                    newObject->getTexCoordCount(),
                                                    newObject->getTrangleCount()) << std::endl;
        }                                        
        delete newObject;
    }

    file.close();

    log << Log::Level::INFO << std::format("Loaded {} objects from {}", this->objects.size(), filename) << std::endl;
}