# 詳細設計 - マイガレージ画面

## 1. 使用リソース

| 変数名（案） | ファイルパス（案） | 内容 |
|--------------|-------------------|------|
| `g_hBgGarage` | `resource/garage/bg_garage.png` | カスタム選択時の背景（自分のガレージ内部） |
| `g_hBgDealer` | `resource/garage/bg_dealer.png` | 車屋選択時の背景 |
| `g_hBgStreet` | `resource/garage/bg_street.png` | レース開始選択時の背景（ストリート） |
| `g_hCarSprite[]` | `resource/cars/car_XX.png` | 車のスプライト（車種ごと） |

> 背景画像は3枚用意する。車スプライトは車種の数だけ用意する。

---

## 2. 画面レイアウト

```
┌────────────────────────────────────────────────────────────┐
│                                                            │
│              背景画像（選択メニューに応じて切替）            │
│                                                            │
│                   [← 車スプライト →]                       │
│                  （所有車が複数のとき横移動）               │
│                                                            │
│         ┌─────────────────────────────────┐               │
│         │   ▲                             │               │
│         │   [車屋 / カスタム / レース開始] │               │
│         │   ▼                             │               │
│         └─────────────────────────────────┘               │
│                                                            │
└────────────────────────────────────────────────────────────┘
```

---

## 3. メニュー仕様

### 3.1 縦移動（メニュー項目の選択）

```cpp
typedef enum {
    GARAGE_MENU_DEALER  = 0,  // 車屋
    GARAGE_MENU_CUSTOM  = 1,  // カスタム
    GARAGE_MENU_RACE    = 2,  // レース開始
    GARAGE_MENU_COUNT   = 3,
} GARAGE_MENU;
```

| 操作 | 動作 |
|------|------|
| ↑キー | 選択を1つ上へ（車屋の状態で↑ → レース開始へループ） |
| ↓キー | 選択を1つ下へ（レース開始の状態で↓ → 車屋へループ） |

ループの計算：
```cpp
s_menuIndex = (s_menuIndex - 1 + GARAGE_MENU_COUNT) % GARAGE_MENU_COUNT;  // ↑
s_menuIndex = (s_menuIndex + 1)                      % GARAGE_MENU_COUNT;  // ↓
```

### 3.2 横移動（所有車の選択）

- 所有車が2台以上のとき、←→キーで選択中の車を切り替える
- 所有車が1台のときは横移動を受け付けない

```cpp
// ←
s_carIndex = (s_carIndex - 1 + g_gameData.carCount) % g_gameData.carCount;
// →
s_carIndex = (s_carIndex + 1)                        % g_gameData.carCount;
```

### 3.3 初期選択

マイガレージに入ったとき、メニューの初期選択は **カスタム（GARAGE_MENU_CUSTOM）**。

---

## 4. 背景の切り替え

選択中のメニューに応じて描画する背景を切り替える。

| GARAGE_MENU | 背景 |
|-------------|------|
| `GARAGE_MENU_DEALER` | `g_hBgDealer`（車屋） |
| `GARAGE_MENU_CUSTOM` | `g_hBgGarage`（自分のガレージ） |
| `GARAGE_MENU_RACE`   | `g_hBgStreet`（ストリート） |

```cpp
// DrawGarage 内の背景描画
int hBgTable[GARAGE_MENU_COUNT] = {
    g_hBgDealer,
    g_hBgGarage,
    g_hBgStreet,
};
DrawExtendGraph(0, 0, 800, 600, hBgTable[s_menuIndex], TRUE);
```

---

## 5. 状態変数

```cpp
// source/garage.cpp (static変数)
static int  s_menuIndex = GARAGE_MENU_CUSTOM;  // 現在のメニュー選択（初期値：カスタム）
static int  s_carIndex  = 0;                   // 現在の車選択インデックス
```

---

## 6. 遷移仕様

決定キー（Enter / Zキー）を押したとき、選択中のメニューに応じて遷移する。

| 選択中メニュー | 遷移先 |
|----------------|--------|
| `GARAGE_MENU_DEALER` | `ChangeScreen(UpdateDealer, DrawDealer)` |
| `GARAGE_MENU_CUSTOM` | `ChangeScreen(UpdateCustomize, DrawCustomize)` |
| `GARAGE_MENU_RACE`   | `ChangeScreen(UpdateGame, DrawGame)` |

---

## 7. 更新処理（UpdateGarage）

```cpp
void UpdateGarage(void) {

    // 縦移動（メニュー選択）
    if (CheckHitKey(KEY_INPUT_UP)) {
        s_menuIndex = (s_menuIndex - 1 + GARAGE_MENU_COUNT) % GARAGE_MENU_COUNT;
    }
    if (CheckHitKey(KEY_INPUT_DOWN)) {
        s_menuIndex = (s_menuIndex + 1) % GARAGE_MENU_COUNT;
    }

    // 横移動（車選択）※複数台所有時のみ
    if (g_gameData.carCount > 1) {
        if (CheckHitKey(KEY_INPUT_LEFT)) {
            s_carIndex = (s_carIndex - 1 + g_gameData.carCount) % g_gameData.carCount;
        }
        if (CheckHitKey(KEY_INPUT_RIGHT)) {
            s_carIndex = (s_carIndex + 1) % g_gameData.carCount;
        }
    }

    // 決定
    if (CheckHitKey(KEY_INPUT_RETURN) || CheckHitKey(KEY_INPUT_Z)) {
        switch (s_menuIndex) {
        case GARAGE_MENU_DEALER:  ChangeScreen(UpdateDealer,   DrawDealer);   break;
        case GARAGE_MENU_CUSTOM:  ChangeScreen(UpdateCustomize, DrawCustomize); break;
        case GARAGE_MENU_RACE:    ChangeScreen(UpdateGame,      DrawGame);     break;
        }
    }
}
```

> **注意**: `CheckHitKey` は毎フレーム反応するため、キーの押し下げ瞬間だけ取る処理（エッジ検出）が必要。実装時に対応する。

---

## 8. 描画処理（DrawGarage）

```cpp
void DrawGarage(void) {

    // Layer 1: 背景（メニューに応じて切替）
    int hBgTable[GARAGE_MENU_COUNT] = { g_hBgDealer, g_hBgGarage, g_hBgStreet };
    DrawExtendGraph(0, 0, 800, 600, hBgTable[s_menuIndex], TRUE);

    // Layer 2: 選択中の車のスプライト
    DrawGraph(CAR_SPRITE_X, CAR_SPRITE_Y, g_hCarSprite[s_carIndex], TRUE);

    // 所有車が複数の場合、横移動の矢印を表示
    if (g_gameData.carCount > 1) {
        DrawString(CAR_SPRITE_X - 30, CAR_SPRITE_Y, "◀", GetColor(255, 255, 255));
        DrawString(CAR_SPRITE_X + CAR_SPRITE_W + 10, CAR_SPRITE_Y, "▶", GetColor(255, 255, 255));
    }

    // Layer 3: メニューUI
    const char* menuLabels[GARAGE_MENU_COUNT] = { "車屋", "カスタム", "レース開始" };
    for (int i = 0; i < GARAGE_MENU_COUNT; i++) {
        int color = (i == s_menuIndex)
            ? GetColor(255, 255, 0)    // 選択中：黄色
            : GetColor(200, 200, 200); // 非選択：グレー
        DrawString(MENU_X, MENU_Y + i * MENU_LINE_H, menuLabels[i], color);
    }
}
```

---

## 9. 座標・定数まとめ（暫定値・要調整）

| 定数名 | 暫定値 | 説明 |
|--------|--------|------|
| `CAR_SPRITE_X` | 要調整 | 車スプライトの描画X座標 |
| `CAR_SPRITE_Y` | 要調整 | 車スプライトの描画Y座標 |
| `CAR_SPRITE_W` | 要調整 | 車スプライトの幅（矢印位置計算用） |
| `MENU_X` | 要調整 | メニューテキストのX座標 |
| `MENU_Y` | 要調整 | メニューテキストの先頭Y座標 |
| `MENU_LINE_H` | 40 | メニュー項目の行間（px） |

---

## 10. キー入力のエッジ検出について

`CheckHitKey` は押し続けている間ずっと `1` を返すため、
1フレームに複数段移動してしまう。

前フレームのキー状態を保持し、「今フレームON かつ 前フレームOFF」のときだけ反応させる。

```cpp
// 共通ユーティリティとして用意する（案）
// header/input.h
void UpdateInput(void);              // 毎フレームメインループから呼ぶ
int  IsKeyTriggered(int keyCode);    // 押した瞬間だけ1を返す
int  IsKeyPressed(int keyCode);      // 押している間1を返す
```
