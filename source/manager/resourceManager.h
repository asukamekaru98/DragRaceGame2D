#pragma once

// 画像ハンドルとその読み込み元ファイルの対応
typedef struct {
	int*        pHandle;     // 画像ハンドル変数へのポインタ
	const char* pFilePath;   // 読み込み元の画像ファイルパス
} RESOURCE_ITEM;

// リソースの読み込み・解放を共通化する基底クラス。
// 派生クラスは GetResources() で自分が管理するリソース配列を返す。
// （配列の末尾は番兵 { nullptr, nullptr } で終端すること）
class ResourceManager {
protected:
	virtual RESOURCE_ITEM* GetResources() = 0;

public:
	virtual ~ResourceManager() {}

	void LoadResources();
	void UnloadResources();
};
