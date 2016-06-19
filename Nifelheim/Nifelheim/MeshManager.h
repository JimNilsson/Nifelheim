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
	int LoadMesh(const int gameObject, const std::string& filename);
	int LoadTerrain(const int gameObject, const std::string& filename, float scale, float offset, unsigned byteperpixel = 1);
	Mesh GetMesh(unsigned id) const;
private:
	std::unordered_map<std::string, int> _filenameToIndex;
	std::vector<Mesh> _meshes;

	void _Filter3x3(float** heightmap, const size_t n);
	float _Sample3x3(float* heightmap, unsigned x, unsigned z, const size_t n);
};


#endif

