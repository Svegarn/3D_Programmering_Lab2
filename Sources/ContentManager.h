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
};

#endif