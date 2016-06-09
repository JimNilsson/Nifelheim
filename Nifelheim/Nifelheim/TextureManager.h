#ifndef _TEXTURE_MANAGER_H_
#define _TEXTURE_MANAGER_H_

#include "Structs.h"
#include <vector>
#include <unordered_map>

class TextureManager
{
public:
	TextureManager();
	~TextureManager();
	void GiveTexture(int gameObject,const std::string& filename, TextureTypes type);
	const Textures& GetTextures(int index) const;
private:
	std::vector<Textures> _textures;
	std::unordered_map<std::string, int> _filenameToIndex;
};


#endif
