#include "MeshManager.h"
#include <fstream>
#include <sstream>
#include "Core.h"

using namespace DirectX;

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

const Mesh & MeshManager::GetMesh(unsigned id) const
{
	return _meshes[id];
}
