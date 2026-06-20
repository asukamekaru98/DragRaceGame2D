# 詳細設計 - オープニング画面・タイトル画面

## 1. 使用リソース

| 変数名（案） | ファイルパス | 内容 |
|--------------|--------------|------|
| `g_hBg` | `resource/title/tite_sprite1.png` | ガレージ背景 |
| `g_hShutter` | `resource/title/tite_sprite2.png` | シャッター |
| `g_hLogo` | `resource/logo.png` | ゲームロゴ |

画像は `LoadGraph()` で読み込み、`int` 型のハンドルで管理する。
オープニング・タイトルで共用するため、モジュールレベルのスタティック変数として保持する。

---

## 2. 描画レイヤー構成

3枚の画像を以下の順で重ねて描画する（後に描くほど手前に表示される）。

```
Layer 1（奥）: ガレージ背景 (tite_sprite1.png)
Layer 2      : シャッター   (tite_sprite2.png)  ← Y座標アニメーション
Layer 3（手前）: ロゴ        (logo.png)          ← 不透明度アニメーション
```

---

## 3. オープニング画面

### 3.1 概要

ロゴが徐々に浮かび上がり、完全に表示されたらタイトル画面へ自動遷移する。
背景・シャッターは静止している。

### 3.2 状態変数

```cpp
// source/opening.cpp (static変数)
static int   s_hBg      = -1;   // 背景ハンドル
static int   s_hShutter = -1;   // シャッターハンドル
static int   s_hLogo    = -1;   // ロゴハンドル
static int   s_logoAlpha = 0;   // ロゴの不透明度 (0〜255)
```

### 3.3 座標定義

```
画面サイズ: 800 x 600

ガレージ背景:
  描画座標: (0, 0) ※画面全体に合わせてスケールして描画

シャッター:
  描画座標: 背景のガレージ開口部に合わせて配置（要調整）
  オープニング中は静止

ロゴ:
  描画座標: 画面中央に配置
  X = (800 - ロゴ幅) / 2
  Y = (600 - ロゴ高) / 2
```

### 3.4 初期化処理（LoadOpening）

```cpp
void LoadOpening(void) {
    s_hBg      = LoadGraph("resource/title/tite_sprite1.png");
    s_hShutter = LoadGraph("resource/title/tite_sprite2.png");
    s_hLogo    = LoadGraph("resource/logo.png");
    s_logoAlpha = 0;
}
```

### 3.5 更新処理（UpdateOpening）

```cpp
void UpdateOpening(void) {
    // ロゴをフェードイン
    s_logoAlpha += LOGO_FADE_SPEED;   // 毎フレーム加算

    if (s_logoAlpha >= 255) {
        s_logoAlpha = 255;
        // 完全に表示されたらタイトルへ遷移
        ChangeScreen(UpdateTitle, DrawTitle);
    }
}
```

| 定数 | 値（案） | 説明 |
|------|----------|------|
| `LOGO_FADE_SPEED` | 2 | 毎フレームの不透明度加算量。255÷2 ≒ 約128フレーム（約2秒）でフェード完了 |

### 3.6 描画処理（DrawOpening）

```cpp
void DrawOpening(void) {
    // Layer 1: 背景
    DrawExtendGraph(0, 0, 800, 600, s_hBg, TRUE);

    // Layer 2: シャッター（静止）
    DrawGraph(SHUTTER_X, SHUTTER_Y, s_hShutter, TRUE);

    // Layer 3: ロゴ（フェードイン）
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, s_logoAlpha);
    DrawGraph(LOGO_X, LOGO_Y, s_hLogo, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}
```

---

## 4. タイトル画面

### 4.1 概要

オープニング終了時の見た目（背景＋シャッター閉＋ロゴ表示）をそのまま引き継ぐ。
任意キー押下でシャッターが上方向にスライドし、上がり切ったらマイガレージへ遷移する。

### 4.2 状態変数

```cpp
// source/title.cpp (static変数)
typedef enum {
    TITLE_STATE_WAIT,       // キー入力待ち
    TITLE_STATE_SHUTTER_UP, // シャッター上昇中
} TITLE_STATE;

static TITLE_STATE  s_state      = TITLE_STATE_WAIT;
static float        s_shutterY   = SHUTTER_Y;   // シャッターのY座標（float で滑らかに動かす）

#ifdef _DEBUG
static int s_debugBtnHandle = -1;   // デバッグボタン画像（またはフォントで代替）
#endif
```

### 4.3 状態遷移

```
TITLE_STATE_WAIT
  └─> [任意キー押下] → TITLE_STATE_SHUTTER_UP

TITLE_STATE_SHUTTER_UP
  └─> [シャッターが画面外に出た] → ChangeScreen(UpdateGarage, DrawGarage)
```

### 4.4 更新処理（UpdateTitle）

```cpp
void UpdateTitle(void) {

    switch (s_state) {

    case TITLE_STATE_WAIT:
        if (CheckHitKeyAll() != 0) {
            s_state = TITLE_STATE_SHUTTER_UP;
        }

#ifdef _DEBUG
        if (/* デバッグボタンがクリックされた */) {
            ChangeScreen(UpdateDebugMenu, DrawDebugMenu);
        }
#endif
        break;

    case TITLE_STATE_SHUTTER_UP:
        s_shutterY -= SHUTTER_SPEED;   // 毎フレームY座標を上へ

        // シャッターが画面外（上）に出たら遷移
        if (s_shutterY + SHUTTER_HEIGHT < 0) {
            ChangeScreen(UpdateGarage, DrawGarage);
        }
        break;
    }
}
```

| 定数 | 値（案） | 説明 |
|------|----------|------|
| `SHUTTER_SPEED` | 8.0f | 毎フレームのシャッター上昇量（px） |
| `SHUTTER_HEIGHT` | シャッター画像の高さ | 画面外判定に使用 |

### 4.5 描画処理（DrawTitle）

```cpp
void DrawTitle(void) {
    // Layer 1: 背景
    DrawExtendGraph(0, 0, 800, 600, s_hBg, TRUE);

    // Layer 2: シャッター（Y座標が変化）
    DrawGraph(SHUTTER_X, (int)s_shutterY, s_hShutter, TRUE);

    // Layer 3: ロゴ（不透明度 255 = 完全表示）
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawGraph(LOGO_X, LOGO_Y, s_hLogo, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // 「PRESS ANY KEY」テキスト（WAIT状態のみ表示、点滅させてもよい）
    if (s_state == TITLE_STATE_WAIT) {
        DrawString(PRESS_ANY_KEY_X, PRESS_ANY_KEY_Y, "PRESS ANY KEY", GetColor(255, 255, 255));
    }

#ifdef _DEBUG
    // デバッグボタン（右下に表示）
    DrawString(700, 570, "DXLIB SAMPLE", GetColor(255, 255, 0));
#endif
}
```

---

## 5. リソースの共有方法

オープニングとタイトルはリソース（背景・シャッター・ロゴ）を共用する。
`opening.cpp` で読み込んだハンドルをタイトル画面でも使えるよう、
`title.cpp` から参照できる形で管理する。

### 方針

`title_resource.h / title_resource.cpp` に共有ハンドルをまとめる。

```cpp
// header/title_resource.h
extern int g_hBg;
extern int g_hShutter;
extern int g_hLogo;

void LoadTitleResource(void);    // オープニング開始時に1回だけ呼ぶ
void UnloadTitleResource(void);  // マイガレージ遷移後に解放
```

---

## 6. 座標・定数のまとめ（暫定値・要画像確認）

| 定数名 | 暫定値 | 説明 |
|--------|--------|------|
| `SHUTTER_X` | 要調整 | シャッターの描画X座標 |
| `SHUTTER_Y` | 要調整 | シャッターの初期描画Y座標 |
| `SHUTTER_HEIGHT` | 要調整 | シャッター画像の高さ（px） |
| `SHUTTER_SPEED` | 8.0f | シャッター上昇速度（px/frame） |
| `LOGO_X` | 要調整 | ロゴの描画X座標（中央揃え） |
| `LOGO_Y` | 要調整 | ロゴの描画Y座標 |
| `LOGO_FADE_SPEED` | 2 | ロゴフェードイン速度（/frame） |
| `PRESS_ANY_KEY_X` | 要調整 | テキストX座標 |
| `PRESS_ANY_KEY_Y` | 要調整 | テキストY座標 |

> 座標の暫定値は画像サイズを `GetGraphSize()` で取得してから計算する。
