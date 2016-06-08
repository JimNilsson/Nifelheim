#ifndef _MESH_MANAGER_H_
#define _MESH_MANAGER_H_

#include "Structs.h"
#include <string>
#include <unordered_map>
#include <vector>

class MeshManager
{
public:
	MeshManager();
	~MeshManager();
	int LoadMesh(const std::string& filename);
private:
	std::unordered_map<std::string, int> _filenameToIndex;
	std::vector<Mesh> _meshes;
};


#endif
