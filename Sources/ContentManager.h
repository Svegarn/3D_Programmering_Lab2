#ifndef CONTENTMANAGER_H
#define CONTENTMANAGER_H

#include "StaticMesh.h"

class StaticMesh;
class ContentManager
{
private:
	ContentManager();
	~ContentManager();

	std::vector<StaticMesh*> staticMeshes;
	std::vector<DataStructures::Material*> materials;

public:
	static ContentManager& getInstance()
	{
		static ContentManager instance;
		return instance;
	}

	ContentManager(ContentManager const&) = delete;
	void operator=(ContentManager const&) = delete;

	void initializeContent();
	std::vector<StaticMesh*>& getStaticMeshes();

	void SetTexture(DataStructures::Material* material, std::wstring texturePath);
	void SetCbuffer(DataStructures::Material* material);
};

#endif