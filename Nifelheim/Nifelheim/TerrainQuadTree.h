#ifndef _TERRAIN_QUAD_TREE_H_
#define _TERRAIN_QUAD_TREE_H_

#include "Structs.h"
#include <vector>
#include <DirectXMath.h>
#include <DirectXCollision.h>

class TerrainQuadTree
{
public:
	TerrainQuadTree();
	TerrainQuadTree(Vertex* vertices, uint32_t* indices, unsigned n);
	~TerrainQuadTree();
	void GetMeshes(std::vector<Mesh>& meshes, const DirectX::XMMATRIX& world) const;
private:
	
	struct Node
	{
		Node(float centerX, float centerZ, float minY, float maxY, float width)
		{
			//minPoint = DirectX::XMFLOAT3(centerX - width / 2.0f, minY, centerZ - width / 2.0f);
			//maxPoint = DirectX::XMFLOAT3(centerX + width / 2.0f, maxY, centerZ + width / 2.0f);
			center = DirectX::XMFLOAT3(centerX, maxY - minY, centerZ);
			extent = DirectX::XMFLOAT3(width / 2.0f, (maxY - minY) / 2.0f, width / 2.0f);
			memset(children, NULL, sizeof(Node*) * 4);
		}
		Mesh mesh;
		DirectX::XMFLOAT3 center;
		DirectX::XMFLOAT3 extent;
		Node* children[4];
	};
	Node* _root;
	Vertex* _vertices;
	uint32_t* _indices;
	unsigned _vertexCount;
	unsigned _indexCount;
	unsigned _triangleCount;

	float _centerX;
	float _centerZ;
	float _minY;
	float _maxY;
	float _width;

	const unsigned _maxTrianglesPerLeaf = 2000;

	void _GetMeshDimensions();
	void _CreateNode(Node* n, float centerX, float centerZ, float width);
	unsigned _GetTrianglesInNode(float centerX, float centerZ, float width);
	bool _IsVertexInBounds(unsigned i, float centerX, float centerZ, float width);
	bool _IsTriangleInBounds(unsigned i, float centerX, float centerZ, float width);
	void _GetNodeDimension(float centerX, float centerZ, float width, unsigned& outX, unsigned& outZ);
	unsigned _CopyVerticesInBounds(Vertex* vertices, float centerX, float centerZ, float width);

	void _GetMeshes(Node* n, std::vector<Mesh>& meshes,const DirectX::BoundingFrustum& frust, bool checkCollision, const DirectX::XMMATRIX& world) const;
};

#endif
