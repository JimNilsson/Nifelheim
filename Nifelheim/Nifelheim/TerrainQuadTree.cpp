#include "TerrainQuadTree.h"
#include "Core.h"
#include <math.h>
#include <DirectXCollision.h>

using namespace DirectX;

TerrainQuadTree::TerrainQuadTree(Vertex * vertices, uint32_t * indices, unsigned n)
{
	_vertices = vertices;
	_indices = indices;
	_vertexCount = n * n;
	_indexCount = (n - 1)*(n - 1) * 6;
	_triangleCount = (n - 1)*(n - 1) * 2;

	
	_GetMeshDimensions();

	_root = new Node(_centerX, _centerZ, _minY, _maxY, _width);
	_CreateNode(_root, _centerX, _centerZ, _width);
	_vertices = nullptr;
	_indices = nullptr;

}

TerrainQuadTree::~TerrainQuadTree()
{
}

void TerrainQuadTree::GetMeshes(std::vector<Mesh>& meshes, const XMMATRIX& world) const
{
	BoundingFrustum bf;
	bf.CreateFromMatrix(bf, Core::GetInstance()->GetCameraManager()->GetProj());
	XMMATRIX invView = Core::GetInstance()->GetCameraManager()->GetView();
	invView = XMMatrixInverse(nullptr, invView);
	bf.Transform(bf, invView);
	
	_GetMeshes(_root, meshes, bf, true, world);
}

void TerrainQuadTree::_GetMeshes(Node * n, std::vector<Mesh>& meshes, const DirectX::BoundingFrustum& frust, bool checkCollision, const XMMATRIX& world) const
{
	
	XMVECTOR c, e;
	c = XMLoadFloat3(&n->center);
	e = XMLoadFloat3(&n->extent);
	c = XMVector3Transform(c, world);
	e = XMVector3Transform(e, world);
	XMFLOAT3 center, extent;
	XMStoreFloat3(&center, c);
	XMStoreFloat3(&extent, e);
	BoundingBox b = BoundingBox(center, extent);
	
	
	if (n->children[0] != nullptr)
	{
		if (b.Intersects(frust))
		{
			for (const auto& i : n->children)
			{
				_GetMeshes(i, meshes,frust, true, world);
			}
		}
	}
	else
	{
		if(b.Intersects(frust))
			meshes.push_back(n->mesh);
	}
}


void TerrainQuadTree::_GetMeshDimensions()
{
	_centerX = _centerZ = 0;
	_minY = FLT_MAX;
	_maxY = -FLT_MAX;
	for (unsigned i = 0; i < _vertexCount; ++i)
	{
		_centerX += _vertices[i].position.x;
		_centerZ += _vertices[i].position.z;
		if (_vertices[i].position.y > _maxY) _maxY = _vertices[i].position.y;
		if (_vertices[i].position.y < _minY) _minY = _vertices[i].position.y;
	}

	_centerX /= static_cast<float>(_vertexCount);
	_centerZ /= static_cast<float>(_vertexCount);

	_width = 0;
	for (unsigned i = 0; i < _vertexCount; ++i)
	{
		float zw = fabsf(_vertices[i].position.z - _centerZ);
		float xw = fabsf(_vertices[i].position.x - _centerX);
		if (zw > _width) _width = zw;
		if (xw > _width) _width = xw;
	}
	_width *= 2.0f;
}

void TerrainQuadTree::_CreateNode(Node * n, float centerX, float centerZ, float width)
{
	unsigned triangleCount = _GetTrianglesInNode(centerX, centerZ, width);
	if (triangleCount == 0)
		return;

	if (triangleCount > _maxTrianglesPerLeaf)
	{
		for (unsigned i = 0; i < 4; ++i)
		{
			float offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f); /* -1, 1, -1, 1 */
			float offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f); /* -1, -1, 1, 1 */

			n->children[i] = new Node(centerX + offsetX, centerZ + offsetZ, _minY, _maxY, width / 2.0f);
			_CreateNode(n->children[i], centerX + offsetX, centerZ + offsetZ, width / 2.0f);
		}
		return;
	}
	else
	{
		unsigned x, z;
		_GetNodeDimension(centerX, centerZ, width, x, z);
		unsigned vertexCount = x * z;
		unsigned indexCount = (x - 1)*(z - 1) * 6;

		Vertex* vertices = new Vertex[vertexCount];
		uint32_t* indices = new uint32_t[indexCount];

		_CopyVerticesInBounds(vertices, centerX, centerZ, width);

		unsigned k = 0;
		for (unsigned i = 0; i < x - 1; ++i)
		{
			for (unsigned j = 0; j < z - 1; ++j)
			{
				indices[k] = i*z + j;
				indices[k + 1] = i*z + j + 1;
				indices[k + 2] = (i + 1)*z + j;
				indices[k + 3] = (i + 1)*z + j;
				indices[k + 4] = i*z + j + 1;
				indices[k + 5] = (i + 1)*z + j + 1;
				k += 6;
			}
		}
		const Core* c = Core::GetInstance();
		n->mesh.vertexBuffer = c->GetDirect3D11()->CreateVertexBuffer(vertices, vertexCount);
		n->mesh.vertexCount = vertexCount;
		n->mesh.indexBuffer = c->GetDirect3D11()->CreateIndexBuffer(indices, indexCount);
		n->mesh.indexCount = indexCount;

		delete[] vertices;
		delete[] indices;

	}
}

unsigned TerrainQuadTree::_GetTrianglesInNode(float centerX, float centerZ, float width)
{
	unsigned count = 0;
	for (unsigned i = 0; i < _indexCount; i += 3)
	{
		if (_IsTriangleInBounds(i, centerX, centerZ, width))
			++count;
	}
	return count;
}


bool TerrainQuadTree::_IsVertexInBounds(unsigned i, float centerX, float centerZ, float width)
{
	float halfWidth = width / 2.0f;
	if (_vertices[i].position.x < centerX - halfWidth - 0.001f)
		return false;
	if (_vertices[i].position.x > centerX + halfWidth + 0.001f)
		return false;
	if (_vertices[i].position.z < centerZ - halfWidth - 0.001f)
		return false;
	if (_vertices[i].position.z > centerZ + halfWidth + 0.001f)
		return false;

	return true;
}

bool TerrainQuadTree::_IsTriangleInBounds(unsigned i, float centerX, float centerZ, float width)
{
	float halfWidth = width / 2.0f;

	float x1 = _vertices[_indices[i    ]].position.x;
	float x2 = _vertices[_indices[i + 1]].position.x;
	float x3 = _vertices[_indices[i + 2]].position.x;

	float z1 = _vertices[_indices[i    ]].position.z;
	float z2 = _vertices[_indices[i + 1]].position.z;
	float z3 = _vertices[_indices[i + 2]].position.z;

	if (XMMin(x1, XMMin(x2, x3)) < centerX - halfWidth)
		return false;
	if (XMMax(x1, XMMax(x2, x3)) > centerX + halfWidth)
		return false;
	if (XMMin(z1, XMMin(z2, z3)) < centerZ - halfWidth)
		return false;
	if (XMMax(z1, XMMax(z2, z3)) > centerZ + halfWidth)
		return false;
	return true;
}

void TerrainQuadTree::_GetNodeDimension(float centerX, float centerZ, float width, unsigned & outX, unsigned & outZ)
{
	outX = 0;
	outZ = 0;
	float lowX = -FLT_MAX;
	float lowZ = -FLT_MAX;
	unsigned longestRow = 0;
	unsigned currentRow = 0;
	unsigned vertexCount = 0;
	for (unsigned i = 0; i < _vertexCount; ++i)
	{
		if (_IsVertexInBounds(i, centerX, centerZ, width))
		{
			++currentRow;
			++vertexCount;
		}
		else
		{
			if (currentRow > longestRow) 
				longestRow = currentRow;
			currentRow = 0;
		}
	}
	outX = longestRow;
	outZ = vertexCount / longestRow;
}

unsigned TerrainQuadTree::_CopyVerticesInBounds(Vertex * vertices, float centerX, float centerZ, float width)
{
	unsigned count = 0;
	for (unsigned i = 0; i < _vertexCount; ++i)
	{
		if (_IsVertexInBounds(i, centerX, centerZ, width))
		{
			vertices[count] = _vertices[i];
			++count;
		}
	}
	return count;
}

