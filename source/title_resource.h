#pragma once

#include "manager/resourceManager.h"

extern int g_hBg;
extern int g_hShutter;
extern int g_hLogo;

// タイトル/オープニング画面で使う画像リソース
class TitleResource : public ResourceManager {
	RESOURCE_ITEM resources[4] = {
		{ &g_hBg,      "resource/title/tite_sprite1.png" },
		{ &g_hShutter, "resource/title/tite_sprite2.png" },
		{ &g_hLogo,    "resource/logo.png" },
		{ nullptr, nullptr },   // 番兵
	};

protected:
	RESOURCE_ITEM* GetResources() override { return resources; }
};
