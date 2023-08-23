#include "object.hpp"

Object::Object(std::string name) : log("Object", name), name(name) {

}

Object::~Object() {
    if (this->vertexBuffer != NULL)
	    this->vertexBuffer->Release();

    if (this->indexBuffer != NULL)
        this->indexBuffer->Release();

    if (this->vertexLayout != NULL)
        this->vertexLayout->Release();
}

void Object::setPosition(D3DXVECTOR3 vec) { 
    position = vec;
};

void Object::setRotation(D3DXVECTOR3 vec) { 
    rotation = vec;
};

void Object::addVertex(Vertex_t& vertex) {
    this->vertices.push_back(vertex);
}

void Object::addNormal(Normal_t& normal) {
    this->normals.push_back(normal);
}

void Object::addTexCoord(TexCoord_t& texCoord) {
    this->texCoords.push_back(texCoord);
}

void Object::addTrangle(Trangle_t& trangle) {
    this->trangles.push_back(trangle);
}

int Object::getVertexCount() {
    return (int)this->vertices.size();
}

int Object::getNormalCount() {
    return (int)this->normals.size();
}

int Object::getTexCoordCount() {
    return (int)this->texCoords.size();
}

int Object::getTrangleCount() {
    return (int)this->trangles.size();
}

D3DXMATRIX Object::getModelMatrix() {
    return this->modelMatrix;
}

void Object::pack() {
    log << Log::Level::INFO << "Packing object" << std::endl;

    for (const auto& trangle : this->trangles) {
        for (const auto& face : trangle.faces) {
            PackedVertex_t vertex;
            vertex.color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
            vertex.vertex = this->vertices[face.vertexIndex];
            vertex.normal = this->normals[face.normalIndex];

            // log << Log::Level::DEBUG << std::format("Packing vertex (v({}) {} {} {}, vn({}) {} {} {})", 
            //                                     face.vertexIndex, vertex.vertex.x, vertex.vertex.y, vertex.vertex.z, 
            //                                     face.normalIndex, vertex.normal.x, vertex.normal.y, vertex.normal.z) << std::endl;

            this->packedVertices.push_back(vertex);
        }
    }
}

void Object::prepareVertexLayout(ID3D10Device* device, ID3D10EffectTechnique* technique) {
    D3D10_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };

    UINT layoutNumElements = sizeof(layout)/sizeof(layout[0]);

    D3D10_PASS_DESC PassDesc;
    technique->GetPassByIndex(0)->GetDesc(&PassDesc);

    if (this->vertexLayout != NULL)
        this->vertexLayout->Release();

    HRESULT hr = device->CreateInputLayout(layout, layoutNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &this->vertexLayout);
    log << Log::Assert(FAILED(hr))  << "Failed to create Input Layount" << std::endl;;
}

void Object::prepareVertexBuffer(ID3D10Device* device) {
    D3D10_SUBRESOURCE_DATA initData;
    D3D10_BUFFER_DESC vertexBufferDesc;

    ZeroMemory(&initData, sizeof(initData));
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

    vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(PackedVertex_t) * (UINT)this->packedVertices.size();
    vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;

    initData.pSysMem = this->packedVertices.data();

    if (this->vertexBuffer != NULL)
        this->vertexBuffer->Release();

    HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &initData, &this->vertexBuffer);
    log << Log::Assert(FAILED(hr)) << "Failed to create Vertex Buffer " << hr << std::endl;
}

void Object::update() {
	D3DXMatrixIdentity(&this->modelMatrix);
	D3DXMatrixIdentity(&this->positionMatrix);
	D3DXMatrixIdentity(&this->rotationMatrix);
    D3DXMatrixIdentity(&this->scaleMatrix);

    D3DXMatrixTranslation(&this->positionMatrix, position.x, position.y, position.z);
    D3DXMatrixRotationYawPitchRoll(&this->rotationMatrix, rotation.x, rotation.y, rotation.z);
    D3DXMatrixScaling(&this->scaleMatrix, scale.x, scale.y, scale.z);

    D3DXMatrixMultiply(&this->modelMatrix, &this->modelMatrix, &this->scaleMatrix);
    D3DXMatrixMultiply(&this->modelMatrix, &this->modelMatrix, &this->rotationMatrix);
    D3DXMatrixMultiply(&this->modelMatrix, &this->modelMatrix, &this->positionMatrix);
}

void Object::draw(ID3D10Device* device) {
    UINT stride = sizeof(PackedVertex_t);
    UINT offset = 0;

    device->IASetInputLayout(this->vertexLayout);
    device->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);
    device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    device->Draw((UINT)this->packedVertices.size(), 0);
    // device->DrawIndexed(this->vertexIndices.size(), 0, 0);
}