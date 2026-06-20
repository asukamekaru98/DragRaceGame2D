# 詳細設計 - カスタマイズ画面

## 1. 使用リソース

| 変数名（案） | ファイルパス（案） | 内容 |
|--------------|-------------------|------|
| `g_hBgGarage` | `resource/garage/bg_garage.png` | 背景（マイガレージ画面と共有） |
| `g_hCarSprite[]` | `resource/cars/car_XX.png` | 車スプライト（マイガレージ・ディーラーと共有） |

---

## 2. 画面レイアウト

```
┌────────────────────────────────────────────────────────────┐
│                   背景（自分のガレージ）                    │
│                                                            │
│  ┌─────────────────────┐  ┌──────────────────────────┐   │
│  │                     │  │ [エンジン][タイヤ][ボディ][外装] │
│  │    車スプライト      │  ├──────────────────────────┤   │
│  │                     │  │ > パーツA  $500  ★装着中 │   │
│  │                     │  │   パーツB  $800          │   │
│  ├─────────────────────┤  │   パーツC  $1200         │   │
│  │ 性能バーグラフ       │  │   (なし)                 │   │
│  │ 最高速度 ██████░░   │  ├──────────────────────────┤   │
│  │ 加速力   ████░░░░   │  │ パーツB の効果           │   │
│  │ （現在→変更後）      │  │ 最高速度 +20 km/h        │   │
│  └─────────────────────┘  │ 価格     $800            │   │
│                            │ 所持金   $2,500          │   │
│                            │ [装着する]  [戻る]        │   │
│                            └──────────────────────────┘   │
└────────────────────────────────────────────────────────────┘
```

---

## 3. パーツデータ構造

### 3.1 パーツカテゴリ

```cpp
// header/parts_data.h
typedef enum {
    PARTS_CAT_ENGINE  = 0,  // エンジン
    PARTS_CAT_TIRE    = 1,  // タイヤ
    PARTS_CAT_BODY    = 2,  // ボディ
    PARTS_CAT_EXTERIOR= 3,  // 外装
    PARTS_CAT_COUNT   = 4,
} PARTS_CATEGORY;
```

### 3.2 パーツデータ

```cpp
typedef struct {
    const char*    name;            // パーツ名
    PARTS_CATEGORY category;        // カテゴリ
    int            price;           // 価格
    float          maxSpeedBonus;   // 最高速度補正 (km/h)
    float          accelBonus;      // 加速力補正
} PartsData;
```

### 3.3 パーツテーブル

全パーツを1つのテーブルで管理する。カテゴリでフィルタリングして表示する。

```cpp
// source/parts_data.cpp
const PartsData PARTS_TABLE[] = {
    // エンジン
    { "スポーツエンジンA",  PARTS_CAT_ENGINE,   500,  10.0f, 0.2f },
    { "ターボエンジンB",    PARTS_CAT_ENGINE,  1200,  30.0f, 0.5f },
    // タイヤ
    { "スポーツタイヤA",   PARTS_CAT_TIRE,     400,   0.0f, 0.3f },
    { "レーシングタイヤB",  PARTS_CAT_TIRE,    1000,   5.0f, 0.8f },
    // ボディ
    { "軽量ボディA",       PARTS_CAT_BODY,     600,  10.0f, 0.4f },
    // 外装
    { "カスタムペイントA", PARTS_CAT_EXTERIOR,  300,   0.0f, 0.0f },
    // ...
};
const int PARTS_TABLE_COUNT = sizeof(PARTS_TABLE) / sizeof(PARTS_TABLE[0]);
```

### 3.4 車両への装着状態

車両データ（`CarData` または `PlayerCar`）にカテゴリごとの装着パーツを持たせる。
`-1` は「未装着（なし）」を表す。

```cpp
// header/car_data.h に追加
typedef struct {
    int         dataIndex;              // CAR_TABLE のインデックス
    int         equippedParts[PARTS_CAT_COUNT]; // 各カテゴリの装着パーツインデックス（-1=なし）
} PlayerCar;
```

---

## 4. 状態変数

```cpp
// source/customize.cpp (static変数)
static int  s_categoryIndex = 0;    // 選択中のカテゴリ（0〜3）
static int  s_partsIndex    = 0;    // 選択中のパーツ（リスト内インデックス）
static int  s_filteredList[32];     // 現在カテゴリでフィルタしたパーツインデックス
static int  s_filteredCount = 0;    // フィルタ後のパーツ数
static int  s_cursorPos     = 0;    // 下部カーソル位置（0: 装着する, 1: 戻る）
```

### フィルタリング処理（カテゴリ切替時に実行）

```cpp
void RebuildFilteredList(void) {
    s_filteredCount = 0;
    for (int i = 0; i < PARTS_TABLE_COUNT; i++) {
        if (PARTS_TABLE[i].category == s_categoryIndex) {
            s_filteredList[s_filteredCount++] = i;
        }
    }
    s_partsIndex = 0;
}
```

---

## 5. 操作フロー

```
カテゴリタブ選択中（上段）
  ├─> [←→キー]  カテゴリを切り替え → フィルタリスト再構築
  └─> [↓キー]   パーツリストへフォーカス移動

パーツリスト選択中（右パネル）
  ├─> [↑↓キー]  パーツを選択
  ├─> [↑キー]（先頭で押した場合）  カテゴリタブへ戻る
  └─> [Enter / Zキー]  下部メニューへフォーカス移動

下部メニュー（装着する / 戻る）
  ├─> [←→キー]  カーソル移動（装着する ⇔ 戻る）
  ├─> [Enter / Z]（装着する）
  │    ├─> 所持金 >= 価格 → 装着処理 → マイガレージへ戻る
  │    └─> 所持金 < 価格  → 購入不可メッセージ
  ├─> [Enter / Z]（戻る）  → マイガレージへ戻る
  └─> [X / ESC]  → マイガレージへ戻る
```

---

## 6. 装着処理

```cpp
void EquipParts(int partsTableIndex) {
    int cat = PARTS_TABLE[partsTableIndex].category;
    PlayerCar* car = &g_gameData.garage[g_gameData.selectedCarIndex];

    // 既存パーツの代金は返金しない（差し替えのみ）
    g_gameData.money -= PARTS_TABLE[partsTableIndex].price;
    car->equippedParts[cat] = partsTableIndex;
}
```

> 同カテゴリに既にパーツが装着されている場合は差し替え（古いパーツは外れる）。返金なし。

---

## 7. 性能バーグラフの計算

選択中のパーツを仮装着したときの性能を計算し、現在値と並べて表示する。

```cpp
typedef struct {
    float maxSpeed;
    float acceleration;
} CarStats;

CarStats CalcStats(const PlayerCar* car) {
    const CarData* base = &CAR_TABLE[car->dataIndex];
    CarStats s = { base->maxSpeed, base->acceleration };
    for (int i = 0; i < PARTS_CAT_COUNT; i++) {
        int pi = car->equippedParts[i];
        if (pi >= 0) {
            s.maxSpeed    += PARTS_TABLE[pi].maxSpeedBonus;
            s.acceleration += PARTS_TABLE[pi].accelBonus;
        }
    }
    return s;
}

// プレビュー用（選択中パーツを仮装着）
CarStats CalcPreviewStats(const PlayerCar* car, int previewPartsIndex) {
    PlayerCar temp = *car;
    int cat = PARTS_TABLE[previewPartsIndex].category;
    temp.equippedParts[cat] = previewPartsIndex;
    return CalcStats(&temp);
}
```

### バーグラフの描画

```
最高速度  [████████░░░░]  → [██████████░░]   200 → 230 km/h
加速力    [██████░░░░░░]  → [████████░░░░]   3.0 → 4.0
```

- 現在値のバーを白・灰色で描画
- 変更後の値のバーを黄色で重ねて描画
- バー最大値（フルスケール）は車種によらず固定値（例：最高速度 300km/h、加速力 5.0）

```cpp
void DrawStatBar(int x, int y, const char* label, float current, float preview, float maxVal) {
    int barMaxW = 150;  // バー最大幅(px)
    int curW    = (int)(current / maxVal * barMaxW);
    int preW    = (int)(preview / maxVal * barMaxW);

    DrawString(x, y, label, GetColor(255, 255, 255));

    // 現在値バー（白）
    DrawFillBox(x + 80, y, x + 80 + curW, y + 12, GetColor(200, 200, 200));

    // 変更後バー（黄、性能アップ分のみ）
    if (preW > curW) {
        DrawFillBox(x + 80 + curW, y, x + 80 + preW, y + 12, GetColor(255, 220, 0));
    }

    // 数値
    DrawFormatString(x + 80 + barMaxW + 5, y, GetColor(255,255,255), "%.0f", preview);
}
```

---

## 8. 更新処理（UpdateCustomize）

```cpp
void UpdateCustomize(void) {

    // カテゴリ切替
    if (IsKeyTriggered(KEY_INPUT_LEFT)) {
        s_categoryIndex = (s_categoryIndex - 1 + PARTS_CAT_COUNT) % PARTS_CAT_COUNT;
        RebuildFilteredList();
    }
    if (IsKeyTriggered(KEY_INPUT_RIGHT)) {
        s_categoryIndex = (s_categoryIndex + 1) % PARTS_CAT_COUNT;
        RebuildFilteredList();
    }

    // パーツ選択
    if (IsKeyTriggered(KEY_INPUT_UP)) {
        s_partsIndex = (s_partsIndex - 1 + s_filteredCount) % s_filteredCount;
    }
    if (IsKeyTriggered(KEY_INPUT_DOWN)) {
        s_partsIndex = (s_partsIndex + 1) % s_filteredCount;
    }

    // 下部メニュー操作
    if (IsKeyTriggered(KEY_INPUT_LEFT) || IsKeyTriggered(KEY_INPUT_RIGHT)) {
        s_cursorPos = 1 - s_cursorPos;
    }

    // 決定
    if (IsKeyTriggered(KEY_INPUT_Z) || IsKeyTriggered(KEY_INPUT_RETURN)) {
        if (s_cursorPos == 0) {
            int pi = s_filteredList[s_partsIndex];
            if (g_gameData.money >= PARTS_TABLE[pi].price) {
                EquipParts(pi);
                ChangeScreen(UpdateGarage, DrawGarage);
            }
        } else {
            ChangeScreen(UpdateGarage, DrawGarage);
        }
    }

    // キャンセル
    if (IsKeyTriggered(KEY_INPUT_X) || IsKeyTriggered(KEY_INPUT_ESCAPE)) {
        ChangeScreen(UpdateGarage, DrawGarage);
    }
}
```

---

## 9. 描画処理（DrawCustomize）

```cpp
void DrawCustomize(void) {
    PlayerCar* car = &g_gameData.garage[g_gameData.selectedCarIndex];
    int pi = s_filteredList[s_partsIndex];  // 選択中パーツのテーブルインデックス

    // Layer 1: 背景
    DrawExtendGraph(0, 0, 800, 600, g_hBgGarage, TRUE);

    // Layer 2: 左側 - 車スプライト
    DrawGraph(CAR_X, CAR_Y, g_hCarSprite[CAR_TABLE[car->dataIndex].spriteIndex], TRUE);

    // Layer 3: 左側 - 性能バーグラフ
    CarStats cur  = CalcStats(car);
    CarStats prev = CalcPreviewStats(car, pi);
    DrawStatBar(BAR_X, BAR_Y,      "最高速度", cur.maxSpeed,     prev.maxSpeed,     300.0f);
    DrawStatBar(BAR_X, BAR_Y + 30, "加速力",   cur.acceleration, prev.acceleration,   5.0f);

    // Layer 4: 右側 - カテゴリタブ
    const char* catLabels[PARTS_CAT_COUNT] = { "エンジン", "タイヤ", "ボディ", "外装" };
    for (int i = 0; i < PARTS_CAT_COUNT; i++) {
        int col = (i == s_categoryIndex) ? GetColor(255,255,0) : GetColor(200,200,200);
        DrawString(TAB_X + i * TAB_W, TAB_Y, catLabels[i], col);
    }

    // Layer 5: 右側 - パーツリスト
    for (int i = 0; i < s_filteredCount; i++) {
        int idx     = s_filteredList[i];
        int isEquip = (car->equippedParts[s_categoryIndex] == idx);
        int isSel   = (i == s_partsIndex);
        int col     = isSel ? GetColor(255,255,0) : GetColor(200,200,200);

        DrawFormatString(LIST_X, LIST_Y + i * LIST_LINE_H, col,
                         "%s  $%d%s",
                         PARTS_TABLE[idx].name,
                         PARTS_TABLE[idx].price,
                         isEquip ? "  ★装着中" : "");
    }

    // Layer 6: 右側 - 選択中パーツの詳細
    DrawFormatString(DETAIL_X, DETAIL_Y,      GetColor(255,255,255), "%s の効果", PARTS_TABLE[pi].name);
    DrawFormatString(DETAIL_X, DETAIL_Y + 25, GetColor(255,255,255), "最高速度 %+.0f km/h", PARTS_TABLE[pi].maxSpeedBonus);
    DrawFormatString(DETAIL_X, DETAIL_Y + 50, GetColor(255,255,255), "価格     $%d",        PARTS_TABLE[pi].price);
    DrawFormatString(DETAIL_X, DETAIL_Y + 75,
                     g_gameData.money >= PARTS_TABLE[pi].price ? GetColor(255,255,255) : GetColor(255,80,80),
                     "所持金   $%d%s",
                     g_gameData.money,
                     g_gameData.money >= PARTS_TABLE[pi].price ? "" : "  ← 不足");

    // Layer 7: 下部メニュー
    int equipCol = (s_cursorPos == 0) ? GetColor(255,255,0) : GetColor(200,200,200);
    int backCol  = (s_cursorPos == 1) ? GetColor(255,255,0) : GetColor(200,200,200);
    DrawString(BTN_EQUIP_X, BTN_Y, "装着する", equipCol);
    DrawString(BTN_BACK_X,  BTN_Y, "戻る",     backCol);
}
```

---

## 10. 座標・定数まとめ（暫定値・要調整）

| 定数名 | 暫定値 | 説明 |
|--------|--------|------|
| `CAR_X` | 50 | 車スプライトX座標 |
| `CAR_Y` | 80 | 車スプライトY座標 |
| `BAR_X` | 50 | バーグラフX座標 |
| `BAR_Y` | 320 | バーグラフ先頭Y座標 |
| `TAB_X` | 420 | カテゴリタブX座標 |
| `TAB_Y` | 60 | カテゴリタブY座標 |
| `TAB_W` | 90 | タブ1個の幅 |
| `LIST_X` | 420 | パーツリストX座標 |
| `LIST_Y` | 110 | パーツリスト先頭Y座標 |
| `LIST_LINE_H` | 30 | リスト行間（px） |
| `DETAIL_X` | 420 | 詳細パネルX座標 |
| `DETAIL_Y` | 380 | 詳細パネルY座標 |
| `BTN_EQUIP_X` | 430 | 「装着する」ボタンX座標 |
| `BTN_BACK_X` | 570 | 「戻る」ボタンX座標 |
| `BTN_Y` | 550 | ボタンY座標 |
