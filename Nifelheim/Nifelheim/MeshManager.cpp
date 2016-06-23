#include "MeshManager.h"
#include <fstream>
#include <sstream>
#include "Core.h"

using namespace DirectX;

bool IsPowerOf2(size_t n);

MeshManager::MeshManager()
{
	
}

MeshManager::~MeshManager()
{
}

int MeshManager::LoadMesh(const int gameObject, const std::string & filename)
{
	auto exists = _filenameToIndex.find(filename);
	if (exists != _filenameToIndex.end())
	{
		const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
		const_cast<GameObject&>(go).components[Components::MESH] = exists->second;
		return exists->second;
	}

	std::ifstream fin(filename);
	if (!fin.is_open())
	{
		DebugLogger::AddMsg("Could not find file: " + filename);
		return -1;
	}

	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> texcoords;

	std::vector<unsigned> positionIndices;
	std::vector<unsigned> normalIndices;
	std::vector<unsigned> texcoordIndices;

	std::vector<Vertex> finishedVertices;
	std::vector<unsigned> finishedIndices;

	if (filename.substr(filename.size() - 3) == "obj")
	{


		for (std::string line; std::getline(fin, line);)
		{
			std::istringstream input(line);
			std::string type;
			input >> type;

			if (type == "v")
			{
				XMFLOAT3 pos;
				input >> pos.x >> pos.y >> pos.z;
				positions.push_back(pos);
			}
			else if (type == "vt")
			{
				XMFLOAT2 tex;
				input >> tex.x >> tex.y;
				texcoords.push_back(tex);
			}
			else if(type == "vn")
			{
				XMFLOAT3 normal;
				input >> normal.x >> normal.y >> normal.z;
				normals.push_back(normal);
			}
			else if (type == "f")
			{
				int pos, tex, nor;
				char garbage;
				for (int i = 0; i < 3; ++i)
				{
					input >> pos >> garbage >> tex >> garbage >> nor;
					positionIndices.push_back(pos);
					texcoordIndices.push_back(tex);
					normalIndices.push_back(nor);

				}
			}
		}
	}
	else
	{
		DebugLogger::AddMsg("Unknown file format: " + filename);
		return -1;
	}

	std::vector<XMFLOAT3> realPos;
	realPos.reserve(positionIndices.size());
	std::vector<XMFLOAT2> realTex;
	realTex.reserve(texcoordIndices.size());
	std::vector<XMFLOAT3> realNor;
	realNor.reserve(normalIndices.size());
	


	for (auto& pos : positionIndices)
	{
		realPos.push_back(positions[pos - 1]);
	}
	for (auto& tex : texcoordIndices)
	{
		realTex.push_back(texcoords[tex - 1]);
	}
	for (auto& nor : normalIndices)
	{
		realNor.push_back(normals[nor - 1]);
	}
	std::vector<XMFLOAT4> tan1;
	std::vector<XMFLOAT4> tan2;
	tan1.resize(realNor.size(), XMFLOAT4(0.0f, 0.0f, 0.0f,0.0f));
	tan2.resize(realNor.size(), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));

	std::vector<XMFLOAT4> realTan;
	realTan.resize(realNor.size(), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
	for (unsigned i = 0; i < positionIndices.size(); i += 3)
	{
		const XMFLOAT3& v1 = realPos[i];
		const XMFLOAT3& v2 = realPos[i + 1];
		const XMFLOAT3& v3 = realPos[i + 2];

		const XMFLOAT2& u1 = realTex[i];
		const XMFLOAT2& u2 = realTex[i + 1];
		const XMFLOAT2& u3 = realTex[i + 2];

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = u2.x - u1.x;
		float s2 = u3.x - u1.x;
		float t1 = u2.y - u1.y;
		float t2 = u3.y - u1.y;

		float r = 1.0f / (s1 * t2 - s2 * t1);

		XMFLOAT3 sdir = XMFLOAT3((t2*x1 - t1*x2) * r, (t2*y1 - t1*y2)*r, (t2*z1 - t1*z2)*r);
		XMFLOAT3 tdir = XMFLOAT3((s1*x2 - s2*x1)*r, (s1*y2 - s2*y1)*r, (s1*z2 - s2*z1)*r);

		tan1[i].x += sdir.x;
		tan1[i].y += sdir.y;
		tan1[i].z += sdir.z;

		tan2[i].x += tdir.x;
		tan2[i].y += tdir.y;
		tan2[i].z += tdir.z;

		tan1[i + 1].x += sdir.x;
		tan1[i + 1].y += sdir.y;
		tan1[i + 1].z += sdir.z;
				   
		tan2[i + 1].x += tdir.x;
		tan2[i + 1].y += tdir.y;
		tan2[i + 1].z += tdir.z;

		tan1[i + 2].x += sdir.x;
		tan1[i + 2].y += sdir.y;
		tan1[i + 2].z += sdir.z;

		tan2[i + 2].x += tdir.x;
		tan2[i + 2].y += tdir.y;
		tan2[i + 2].z += tdir.z;
		

	}

	for (unsigned i = 0; i < realPos.size(); ++i)
	{
		XMVECTOR n = XMLoadFloat3(&realNor[i]);
		XMVECTOR t = XMLoadFloat4(&tan1[i]);

		XMVECTOR tangent = XMVector3Normalize(t - n * XMVector3Dot(n, t));

		XMStoreFloat4(&realTan[i], tangent);
		realTan[i].w = XMVectorGetX(XMVector3Dot(XMVector3Cross(n, t), XMLoadFloat4(&tan2[i]))) < 0.0f ? -1.0f : 1.0f;
	}

	finishedVertices.resize(realPos.size());

	for (unsigned i = 0; i < realPos.size(); ++i)
	{
		finishedVertices[i].position = realPos[i];
		finishedVertices[i].normal = realNor[i];
		finishedVertices[i].tangent = realTan[i];
		finishedVertices[i].texcoord = realTex[i];
	}
	Mesh mesh;
	mesh.vertexBuffer = Core::GetInstance()->GetDirect3D11()->CreateVertexBuffer(&finishedVertices[0], finishedVertices.size());
	mesh.vertexCount = finishedVertices.size();
	_meshes.push_back(mesh);
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	const_cast<GameObject&>(go).components[Components::MESH] = _meshes.size() - 1;
	_filenameToIndex[filename] = _meshes.size() - 1;
	return _meshes.size() - 1;
}

int MeshManager::LoadTerrain(const int gameObject, const std::string & filename, float scale, float offset, unsigned byteperpixel)
{
	auto exists = _filenameToIndex.find(filename);
	if (exists != _filenameToIndex.end())
	{
		const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
		const_cast<GameObject&>(go).components[Components::MESH] = exists->second;
		return exists->second;
	}
	if (!(byteperpixel == 1 || byteperpixel == 2 || byteperpixel == 4))
	{
		DebugLogger::AddMsg("LoadTerrain: byteperpixel must be 1, 2 or 4.");
		return -1;
	}
	//Open with fileptr at end of file to figure the size of the file
	std::ifstream fin(filename, std::ifstream::ate | std::ifstream::binary);
	if (!fin.is_open())
	{
		DebugLogger::AddMsg("Could not find file: " + filename);
		return -1;
	}

	size_t filesize = static_cast<size_t>(fin.tellg());
	size_t datapoints = filesize / byteperpixel;
	fin.seekg(std::ios_base::beg);

	//Valid dimensions for heightmaps are (2^m + 1)x(2^m + 1)
	size_t n = static_cast<int>(sqrt(datapoints));
	if (n * n != datapoints || !IsPowerOf2(n - 1))
	{
		DebugLogger::AddMsg("Unsupported dimension for heightmap. Must be of dimension (2^m + 1)x(2^m + 1)");
		return -1;
	}

	char* rawData = new char[filesize];
	fin.read((char*)&rawData[0], filesize);
	fin.close();
	float* heightmap = new float[datapoints];
	for (unsigned i = 0; i < datapoints; ++i)
	{
		
		unsigned height = 0;
		memcpy(&height, &rawData[i * byteperpixel], byteperpixel);
		float heightf = static_cast<float>(height);
		heightmap[i] = heightf * scale + offset;
	}
	delete[] rawData;

	Vertex* vertices = new Vertex[datapoints];
	float halfWidth = 0.5f * static_cast<float>(n);
	float dd = static_cast<float>(n) / static_cast<float>(n - 1);
	float duv = 1.0f / static_cast<float>(n - 1);
	for (unsigned i = 0; i < datapoints; ++i)
	{
		float x = -halfWidth + (i % n)*dd;
		float z = -halfWidth + (i / n)*dd;
		vertices[i].position = XMFLOAT3(x, heightmap[i], z);
		vertices[i].normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertices[i].tangent = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		vertices[i].texcoord = XMFLOAT2((i % n) * duv * n, (i / n) * duv * n);
	}

	uint32_t* indices = new uint32_t[n * n * 6];
	unsigned k = 0;
	for (unsigned i = 0; i < n - 1; ++i)
	{
		for (unsigned j = 0; j < n - 1; ++j)
		{
			indices[k]     = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1)*n + j;
			indices[k + 3] = (i + 1)*n + j;
			indices[k + 4] = i*n + j + 1;
			indices[k + 5] = (i + 1)*n + j + 1;
			k += 6;
		}
	}

	for (unsigned i = 0; i < (n - 1)*(n - 1) * 6; i += 3)
	{
		XMVECTOR v0 = XMLoadFloat3(&vertices[indices[i]].position);
		XMVECTOR v1 = XMLoadFloat3(&vertices[indices[i + 1]].position);
		XMVECTOR v2 = XMLoadFloat3(&vertices[indices[i + 2]].position);

		XMVECTOR e0 = XMVectorSubtract(v2, v0);
		XMVECTOR e1 = XMVectorSubtract(v1, v0);
		XMVECTOR normal = XMVector3Normalize(XMVector3Cross(e0, e1));

		XMStoreFloat3(&vertices[indices[i]].normal, normal + XMLoadFloat3(&vertices[indices[i]].normal));
		XMStoreFloat3(&vertices[indices[i + 1]].normal, normal + XMLoadFloat3(&vertices[indices[i + 1]].normal));
		XMStoreFloat3(&vertices[indices[i + 2]].normal, normal + XMLoadFloat3(&vertices[indices[i + 2]].normal));

		//vertices[indices[i + 1]].texcoord.y *= XMVectorGetX(XMVector3Length(e0));
		//vertices[indices[i + 2]].texcoord.y *= XMVectorGetX(XMVector3Length(e1));
	}

	for (unsigned i = 0; i < datapoints; ++i)
	{
		XMStoreFloat3(&vertices[i].normal, XMVector3Normalize(XMLoadFloat3(&vertices[i].normal)));
		XMVECTOR v = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		XMVECTOR tan = XMVector3Cross(XMLoadFloat3(&vertices[i].normal), v);
		tan = XMVectorSetW(tan, -1.0f);
		XMStoreFloat4(&vertices[i].tangent, tan);

		if ((i + 1) % n > i && i + 1 < datapoints)
		{
			XMVECTOR p1 = XMLoadFloat3(&vertices[i].position);
			XMVECTOR p2 = XMLoadFloat3(&vertices[i + 1].position);
			float distance = XMVectorGetX(XMVector3Length(p1 - p2));
			vertices[i + 1].texcoord.x = vertices[i].position.x + (n / 2.0f) + distance;
			//if (distance > 1.1f)
			//{
			//	for (int j = 0; j < n - ((i + 1) % n); ++j)
			//	{
			//		vertices[j + i + 1].texcoord.x += distance - 1.0f;
			//	}
			//}
		}
		if (i + n < datapoints)
		{
			XMVECTOR p1 = XMLoadFloat3(&vertices[i].position);
			XMVECTOR p2 = XMLoadFloat3(&vertices[i + n].position);
			float distance = XMVectorGetX(XMVector3Length(p1 - p2));
			vertices[i + n].texcoord.y = vertices[i].position.z + (n / 2.0f) + distance;
			if (distance > 1.0f)
			{
				//for (int j = i + n; j < datapoints; j += n)
				//{
				//	//vertices[j].texcoord.y += (vertices[j].texcoord.y - vertices[j - n].texcoord.y) * distance;
				//	vertices[j].texcoord.y = (vertices[j].position.z + n / 2.0f) * distance;
				//}
				//vertices[i].texcoord.y -= 0.5f * distance;
				//vertices[i + n].texcoord.y += 0.5f*distance;
			}
		}
		
	}

	const Core* core = Core::GetInstance();
	//Mesh m;
	//m.vertexBuffer = core->GetDirect3D11()->CreateVertexBuffer(vertices, datapoints);
	//m.vertexCount = datapoints;
	//m.indexBuffer = core->GetDirect3D11()->CreateIndexBuffer(indices, (n - 1)*(n - 1) * 6);
	//m.indexCount = (n - 1)*(n - 1) * 6;

	//_meshes.push_back(m);

	//const GameObject& go = core->GetGameObject(gameObject);
	//const_cast<GameObject&>(go).components[Components::MESH] = _meshes.size() - 1;

	//_filenameToIndex[filename] = _meshes.size() - 1;

	_terrains.push_back(TerrainQuadTree(vertices, indices, n));
	const GameObject& go = core->GetGameObject(gameObject);
	const_cast<GameObject&>(go).components[Components::TERRAIN] = _terrains.size() - 1;

	delete[] vertices;
	delete[] indices;

	return 0;
}

Mesh MeshManager::GetMesh(unsigned id) const
{
	if(id < _meshes.size())
		return _meshes[id];
	return Mesh();
}

void MeshManager::GetTerrain(unsigned id, std::vector<Mesh>& meshes, const XMMATRIX& scale) const
{
	_terrains[id].GetMeshes(meshes, scale);
}

void MeshManager::_Filter3x3(float** heightmap, const size_t n)
{
	float* filtered = new float[n*n];
	for (size_t i = 0; i < n*n; ++i)
	{
		filtered[i] = _Sample3x3(*heightmap, i % n, i / n, n);
	}
	delete[](*heightmap);
	(*heightmap) = filtered;
}

float MeshManager::_Sample3x3(float * heightmap, unsigned x, unsigned z, const size_t n)
{
	float sum = 0.0f;
	unsigned samples = 0;
	const size_t max = n * n;

	for (int i = -1; i < 2; ++i)
	{
		for (int j = -1; j < 2; ++j)
		{
			unsigned index = (x + i)*n + (z + j);
			if (index >= 0 && index < max)
			{
				sum += heightmap[index];
				++samples;
			}
		}
	}
	return sum / static_cast<float>(samples);
}


bool IsPowerOf2(size_t n)
{
	if (n % 2 != 0)
		return false;
	if (n == 2)
		return true;
	return IsPowerOf2(n / 2);
}