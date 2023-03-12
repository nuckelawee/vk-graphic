#pragma once

namespace vk {

class MeshStorage {
    std::vector<StaticMesh> meshes_;
    
public:

    void Create(MeshInfo *pMeshInfos, size_t meshInfoCount);

    MeshStorage() {}
    ~MeshStorage() {} 
};

} // vk
