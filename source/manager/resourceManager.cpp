#include "../../DxLib/DxLib.h"
#include "resourceManager.h"

void ResourceManager::LoadResources()
{
	RESOURCE_ITEM* res = GetResources();
	for (int i = 0; res[i].pHandle != nullptr; i++) {
		*(res[i].pHandle) = LoadGraph(res[i].pFilePath);
	}
}

void ResourceManager::UnloadResources()
{
	RESOURCE_ITEM* res = GetResources();
	for (int i = 0; res[i].pHandle != nullptr; i++) {
		DeleteGraph(*(res[i].pHandle));
		*(res[i].pHandle) = -1;
	}
}
