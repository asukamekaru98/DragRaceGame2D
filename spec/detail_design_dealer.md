# 詳細設計 - カーディーラー画面

## 1. 使用リソース

| 変数名（案） | ファイルパス（案） | 内容 |
|--------------|-------------------|------|
| `g_hBgDealer` | `resource/garage/bg_dealer.png` | 背景（マイガレージ画面と共有） |
| `g_hCarSprite[]` | `resource/cars/car_XX.png` | 車のスプライト（車種ごと） |

---

## 2. 画面レイアウト

```
┌────────────────────────────────────────────────────────────┐
│                   背景（ディーラー）                        │
│                                                            │
│  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐       │
│  │      │  │      │  │▓▓▓▓▓│  │      │  │      │       │
│  │ 車A  │  │ 車B  │  │▓車C▓│  │ 車D  │  │ 車E  │       │
│  │      │  │      │  │▓▓▓▓▓│  │      │  │      │       │
│  │$1000 │  │$2000 │  │$3000 │  │$4000 │  │$5000 │       │
│  └──────┘  └──────┘  └──────┘  └──────┘  └──────┘       │
│                ← ▼選択中▼ →                               │
│                                                            │
│         ┌──────────────────────────────────┐              │
│         │ 車名    : SAMPLE CAR C           │              │
│         │ 最高速度: 280 km/h              │              │
│         │ 加速力  : ★★★★☆            │              │
│         │ ギア数  : 6                      │              │
│         │ 価格    : $3,000                 │              │
│         │ 所持金  : $2,500  ← 不足        │              │
│         │                                  │              │
│         │  [購入する]  [戻る]              │              │
│         └──────────────────────────────────┘              │
└────────────────────────────────────────────────────────────┘
```

- 画面中央に現在選択中のカードを配置し、左右に隣の車がはみ出して見える
- 所持金が足りない車は**スプライトに影（暗い半透明オーバーレイ）**をかけて表示する

---

## 3. カードのスクロール仕様

### 3.1 表示方式

- 画面中央に選択中の車カードを1枚フォーカス表示する
- 左右に隣接する車カードが半分ほど見える（← 車 | 選択中 | 車 →）
- ←→キーで選択中の車が切り替わるとき、カードが横にスライドするアニメーションを行う

### 3.2 スクロールアニメーション

```
s_scrollX : カード列全体のX方向オフセット（float）
s_targetX : 目標オフセット（選択インデックスに応じて決まる）

毎フレーム: s_scrollX += (s_targetX - s_scrollX) * SCROLL_LERP
```

| 定数 | 値（案） | 説明 |
|------|----------|------|
| `SCROLL_LERP` | 0.2f | 補間係数。1.0で即座、0に近いほど滑らか |
| `CARD_WIDTH` | 160 | カード1枚の幅（px） |
| `CARD_MARGIN` | 20 | カード間の余白（px） |
| `CARD_STEP` | `CARD_WIDTH + CARD_MARGIN` | カード1枚分のステップ幅 |

目標オフセットの計算：
```cpp
s_targetX = -(s_carIndex * CARD_STEP);  // 選択中のカードが中央に来るよう調整
```

---

## 4. 車データ構造

```cpp
// header/car_data.h
typedef struct {
    const char* name;           // 車名
    float       maxSpeed;       // 最高速度 (km/h)
    float       acceleration;   // 加速力 (0.0〜5.0)
    int         gearCount;      // ギア数
    int         price;          // 購入価格
    int         spriteIndex;    // g_hCarSprite[] のインデックス
} CarData;
```

全車種データはテーブルとして定義し、ディーラー・ガレージ・ゲームで共有する。

```cpp
// source/car_data.cpp
const CarData CAR_TABLE[] = {
    { "SAMPLE CAR A", 200.0f, 2.0f, 4, 1000, 0 },
    { "SAMPLE CAR B", 240.0f, 3.0f, 5, 2000, 1 },
    { "SAMPLE CAR C", 280.0f, 4.0f, 6, 3000, 2 },
    // ...
};
const int CAR_TABLE_COUNT = sizeof(CAR_TABLE) / sizeof(CAR_TABLE[0]);
```

---

## 5. 所持金不足の表示

購入不可の車には、スプライトの上に暗い半透明の矩形を重ねて影をかける。

```cpp
bool CanAfford(int carIndex) {
    return g_gameData.money >= CAR_TABLE[carIndex].price;
}

// カード描画時
DrawGraph(cardX, cardY, g_hCarSprite[car.spriteIndex], TRUE);

if (!CanAfford(i)) {
    // 暗い半透明オーバーレイ
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);
    DrawFillBox(cardX, cardY, cardX + CARD_WIDTH, cardY + CARD_HEIGHT,
                GetColor(0, 0, 0));
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
```

---

## 6. 状態変数

```cpp
// source/dealer.cpp (static変数)
static int   s_carIndex  = 0;       // 選択中の車インデックス
static float s_scrollX   = 0.0f;    // カード列の現在のXオフセット
static float s_targetX   = 0.0f;    // スクロールの目標Xオフセット
static int   s_cursorPos = 0;       // 下部カーソル位置（0: 購入する, 1: 戻る）
```

---

## 7. 状態遷移

```
カード選択中
  ├─> [←→キー]  別の車を選択（スクロールアニメーション）
  └─> [Enterキー / Zキー]  下部メニューへフォーカス移動

下部メニュー（購入する / 戻る）
  ├─> [←→キー]  カーソル移動（購入する ⇔ 戻る）
  ├─> [Enter / Z]（購入する選択中）
  │    ├─> 所持金 >= 価格 → 購入処理 → マイガレージへ戻る
  │    └─> 所持金 < 価格  → 購入不可メッセージ表示
  ├─> [Enter / Z]（戻る選択中）→ マイガレージへ戻る
  └─> [Xキー / ESCキー]  → マイガレージへ戻る
```

---

## 8. 購入処理

```cpp
void BuyCar(int carIndex) {
    g_gameData.money -= CAR_TABLE[carIndex].price;
    // g_gameData.garage に購入した車を追加
    // g_gameData.carCount++
}
```

---

## 9. 更新処理（UpdateDealer）

```cpp
void UpdateDealer(void) {

    // カードスクロールアニメーション（毎フレーム）
    s_scrollX += (s_targetX - s_scrollX) * SCROLL_LERP;

    // カード選択中の操作
    if (IsKeyTriggered(KEY_INPUT_LEFT)) {
        s_carIndex = (s_carIndex - 1 + CAR_TABLE_COUNT) % CAR_TABLE_COUNT;
        s_targetX  = -(float)(s_carIndex * CARD_STEP);
    }
    if (IsKeyTriggered(KEY_INPUT_RIGHT)) {
        s_carIndex = (s_carIndex + 1) % CAR_TABLE_COUNT;
        s_targetX  = -(float)(s_carIndex * CARD_STEP);
    }

    // 下部メニュー操作
    if (IsKeyTriggered(KEY_INPUT_LEFT) || IsKeyTriggered(KEY_INPUT_RIGHT)) {
        s_cursorPos = 1 - s_cursorPos;  // 0 ⇔ 1 トグル
    }

    // 決定
    if (IsKeyTriggered(KEY_INPUT_Z) || IsKeyTriggered(KEY_INPUT_RETURN)) {
        if (s_cursorPos == 0) {
            // 「購入する」
            if (CanAfford(s_carIndex)) {
                BuyCar(s_carIndex);
                ChangeScreen(UpdateGarage, DrawGarage);
            }
            // 不可の場合は購入不可メッセージ表示（フラグを立てる）
        } else {
            // 「戻る」
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

## 10. 描画処理（DrawDealer）

```cpp
void DrawDealer(void) {

    // Layer 1: 背景
    DrawExtendGraph(0, 0, 800, 600, g_hBgDealer, TRUE);

    // Layer 2: 車カード一覧（スクロールオフセット適用）
    int centerX = 400;  // 画面中央
    for (int i = 0; i < CAR_TABLE_COUNT; i++) {
        int cardX = centerX + (int)s_scrollX + i * CARD_STEP - CARD_WIDTH / 2;
        int cardY = CARD_Y;

        // スプライト
        DrawGraph(cardX, cardY, g_hCarSprite[CAR_TABLE[i].spriteIndex], TRUE);

        // 所持金不足オーバーレイ
        if (!CanAfford(i)) {
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);
            DrawFillBox(cardX, cardY, cardX + CARD_WIDTH, cardY + CARD_HEIGHT,
                        GetColor(0, 0, 0));
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }

        // 選択中のカードは枠を強調
        if (i == s_carIndex) {
            DrawBox(cardX, cardY, cardX + CARD_WIDTH, cardY + CARD_HEIGHT,
                    GetColor(255, 255, 0), FALSE);
        }

        // 価格
        DrawFormatString(cardX, cardY + CARD_HEIGHT + 5, GetColor(255, 255, 255),
                         "$%d", CAR_TABLE[i].price);
    }

    // Layer 3: 選択中の車の詳細パネル
    const CarData* car = &CAR_TABLE[s_carIndex];
    DrawFormatString(DETAIL_X, DETAIL_Y +  0, GetColor(255,255,255), "車名    : %s",   car->name);
    DrawFormatString(DETAIL_X, DETAIL_Y + 25, GetColor(255,255,255), "最高速度: %.0f km/h", car->maxSpeed);
    DrawFormatString(DETAIL_X, DETAIL_Y + 50, GetColor(255,255,255), "ギア数  : %d",   car->gearCount);
    DrawFormatString(DETAIL_X, DETAIL_Y + 75, GetColor(255,255,255), "価格    : $%d",  car->price);
    DrawFormatString(DETAIL_X, DETAIL_Y +100,
                     CanAfford(s_carIndex) ? GetColor(255,255,255) : GetColor(255,80,80),
                     "所持金  : $%d%s",
                     g_gameData.money,
                     CanAfford(s_carIndex) ? "" : "  ← 不足");

    // Layer 4: 下部メニュー
    int buyColor  = (s_cursorPos == 0) ? GetColor(255,255,0) : GetColor(200,200,200);
    int backColor = (s_cursorPos == 1) ? GetColor(255,255,0) : GetColor(200,200,200);
    DrawString(BTN_BUY_X,  BTN_Y, "購入する", buyColor);
    DrawString(BTN_BACK_X, BTN_Y, "戻る",     backColor);
}
```

---

## 11. 座標・定数まとめ（暫定値・要調整）

| 定数名 | 暫定値 | 説明 |
|--------|--------|------|
| `CARD_WIDTH` | 160 | カード幅（px） |
| `CARD_HEIGHT` | 120 | カード高さ（px） |
| `CARD_MARGIN` | 20 | カード間余白（px） |
| `CARD_STEP` | 180 | `CARD_WIDTH + CARD_MARGIN` |
| `CARD_Y` | 150 | カード描画Y座標 |
| `SCROLL_LERP` | 0.2f | スクロール補間係数 |
| `DETAIL_X` | 250 | 詳細パネルX座標 |
| `DETAIL_Y` | 350 | 詳細パネルY座標 |
| `BTN_BUY_X` | 280 | 「購入する」ボタンX座標 |
| `BTN_BACK_X` | 430 | 「戻る」ボタンX座標 |
| `BTN_Y` | 540 | ボタンY座標 |
