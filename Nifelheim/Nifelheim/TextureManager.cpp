#include "Core.h"
#include "TextureManager.h"
#include <locale>
#include <codecvt>
#include <string>


TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

void TextureManager::GiveTexture(int gameObject, const std::string& filename, TextureTypes type)
{
	const GameObject& go = Core::GetInstance()->GetGameObject(gameObject);
	if (go.components[Components::TEXTURES] == -1)
	{
		_textures.push_back(Textures());
		const_cast<GameObject&>(go).components[Components::TEXTURES] = _textures.size() - 1;
	}

	auto exists = _filenameToIndex.find(filename);
	if (exists != _filenameToIndex.end())
	{
		_textures[go.components[Components::TEXTURES]].textures[type] = exists->second;
		return;
	}


	

	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::wstring wfilename = conv.from_bytes(filename.c_str());
	int d3dindex = Core::GetInstance()->GetDirect3D11()->CreateTexture(wfilename.c_str());
	_textures[go.components[Components::TEXTURES]].textures[type] = d3dindex;
	_filenameToIndex[filename] = d3dindex;
}

const Textures & TextureManager::GetTextures(int index) const
{
	if(index < _textures.size())
		return _textures[index];
	return Textures();
}
