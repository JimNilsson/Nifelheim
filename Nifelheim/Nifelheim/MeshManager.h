#ifndef _MESH_MANAGER_H_
#define _MESH_MANAGER_H_

#include "Structs.h"
#include <string>
#include <unordered_map>
#include <vector>
#include "TerrainQuadTree.h"

class MeshManager
{
public:
	MeshManager();
	~MeshManager();
	int LoadMesh(const int gameObject, const std::string& filename);
	int LoadTerrain(const int gameObject, const std::string& filename, float scale, float offset, unsigned byteperpixel = 1);
	Mesh GetMesh(unsigned id) const;
	void GetTerrain(unsigned id, std::vector<Mesh>& meshes, const DirectX::XMMATRIX& scale) const;
private:
	std::unordered_map<std::string, int> _filenameToIndex;
	std::vector<Mesh> _meshes;
	std::vector<TerrainQuadTree> _terrains; //Probably only one of these fuckers will exist

	void _Filter3x3(float** heightmap, const size_t n);
	float _Sample3x3(float* heightmap, unsigned x, unsigned z, const size_t n);
};


#endif

