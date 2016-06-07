#ifndef _MESH_MANAGER_H_
#define _MESH_MANAGER_H_

#include "Structs.h"
#include <string>
#include <unordered_map>

class MeshManager
{
public:
	MeshManager();
	~MeshManager();
	Mesh LoadMesh(const std::string& filename);
private:
	std::unordered_map<std::string, Mesh> _filenameToMesh;
};


#endif

