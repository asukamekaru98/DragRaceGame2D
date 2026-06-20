# 詳細設計 - 裏モード（DxLibサンプル）

> このファイルに記載する全ての実装は `#ifdef _DEBUG` で囲み、リリースビルドには含めない。

---

## 1. 画面構成

```
タイトル画面（デバッグビルドのみボタン表示）
  └─> デバッグメニュー画面
       ├─> 描画サンプル画面（ページ切替あり）
       └─> 入力サンプル画面
```

---

## 2. デバッグメニュー画面（SCR_DEBUG_MENU）

### 2.1 レイアウト

```
┌────────────────────────────────────────────────────────────┐
│                                                            │
│                  DXLIB SAMPLE                              │
│                                                            │
│              > 描画サンプル                                 │
│                入力サンプル                                 │
│                                                            │
│                  [ESC] タイトルへ戻る                      │
└────────────────────────────────────────────────────────────┘
```

### 2.2 状態変数

```cpp
#ifdef _DEBUG
static int s_debugMenuIndex = 0;  // 0: 描画サンプル, 1: 入力サンプル
#endif
```

### 2.3 更新・描画

```cpp
#ifdef _DEBUG
void UpdateDebugMenu(void) {
    if (IsKeyTriggered(KEY_INPUT_UP)) {
        s_debugMenuIndex = (s_debugMenuIndex - 1 + 2) % 2;
    }
    if (IsKeyTriggered(KEY_INPUT_DOWN)) {
        s_debugMenuIndex = (s_debugMenuIndex + 1) % 2;
    }
    if (IsKeyTriggered(KEY_INPUT_Z) || IsKeyTriggered(KEY_INPUT_RETURN)) {
        if (s_debugMenuIndex == 0) ChangeScreen(UpdateDebugDraw,  DrawDebugDraw);
        else                       ChangeScreen(UpdateDebugInput, DrawDebugInput);
    }
    if (IsKeyTriggered(KEY_INPUT_ESCAPE)) {
        ChangeScreen(UpdateTitle, DrawTitle);
    }
}

void DrawDebugMenu(void) {
    ClearDrawScreen();
    DrawString(320, 150, "DXLIB SAMPLE", GetColor(255, 255, 0));

    const char* items[] = { "描画サンプル", "入力サンプル" };
    for (int i = 0; i < 2; i++) {
        int col = (i == s_debugMenuIndex) ? GetColor(255,255,0) : GetColor(200,200,200);
        DrawFormatString(320, 260 + i * 50, col, "%s%s",
                         i == s_debugMenuIndex ? "> " : "  ", items[i]);
    }
    DrawString(260, 500, "[ESC] タイトルへ戻る", GetColor(150,150,150));
}
#endif
```

---

## 3. 描画サンプル画面（SCR_DEBUG_DRAW）

### 3.1 ページ構成

←→キーでページを切り替える。

| ページ | 内容 |
|--------|------|
| 0 | 基本図形（点・線・矩形・円・三角・四角形） |
| 1 | 塗りつぶし図形 |
| 2 | 文字列描画 |
| 3 | ブレンドモード |

### 3.2 状態変数

```cpp
#ifdef _DEBUG
static int s_drawPage = 0;
#define DRAW_PAGE_COUNT 4
#endif
```

### 3.3 更新処理

```cpp
#ifdef _DEBUG
void UpdateDebugDraw(void) {
    if (IsKeyTriggered(KEY_INPUT_LEFT)) {
        s_drawPage = (s_drawPage - 1 + DRAW_PAGE_COUNT) % DRAW_PAGE_COUNT;
    }
    if (IsKeyTriggered(KEY_INPUT_RIGHT)) {
        s_drawPage = (s_drawPage + 1) % DRAW_PAGE_COUNT;
    }
    if (IsKeyTriggered(KEY_INPUT_ESCAPE)) {
        ChangeScreen(UpdateDebugMenu, DrawDebugMenu);
    }
}
#endif
```

### 3.4 描画処理

```cpp
#ifdef _DEBUG
void DrawDebugDraw(void) {
    ClearDrawScreen();

    // ヘッダー
    DrawFormatString(10, 10, GetColor(255,255,0),
                     "描画サンプル  [← →] ページ切替  [ESC] 戻る  (%d/%d)",
                     s_drawPage + 1, DRAW_PAGE_COUNT);
    DrawLine(0, 35, 800, 35, GetColor(100,100,100));

    switch (s_drawPage) {
    case 0: DrawPageBasic();   break;
    case 1: DrawPageFill();    break;
    case 2: DrawPageString();  break;
    case 3: DrawPageBlend();   break;
    }
}
#endif
```

---

### 3.5 ページ0：基本図形

各関数の出力を名前付きで並べる。

```
┌────────────────────────────────────────────────────────────┐
│ 描画サンプル  [←→] ページ切替  [ESC] 戻る  (1/4)        │
├────────────────────────────────────────────────────────────┤
│                                                            │
│  DrawPixel          DrawLine         DrawBox               │
│      ●               ／              □                    │
│                                                            │
│  DrawCircle         DrawOval         DrawTriangle          │
│      ○              (楕円)            △                   │
│                                                            │
│  DrawQuadrangle     DrawRoundRect                          │
│      ◇             (角丸□)                               │
│                                                            │
└────────────────────────────────────────────────────────────┘
```

```cpp
#ifdef _DEBUG
static void DrawPageBasic(void) {
    int col = GetColor(255, 255, 255);
    int y   = 80;

    // DrawPixel
    DrawString(60, y, "DrawPixel", col);
    for (int i = 0; i < 5; i++)
        DrawPixel(80 + i * 3, y + 40 + i, GetColor(255, 100, 100));

    // DrawLine
    DrawString(220, y, "DrawLine", col);
    DrawLine(230, y + 30, 310, y + 60, GetColor(100, 255, 100));

    // DrawBox（枠のみ）
    DrawString(400, y, "DrawBox", col);
    DrawBox(410, y + 30, 490, y + 60, GetColor(100, 100, 255), FALSE);

    y += 130;

    // DrawCircle（枠のみ）
    DrawString(60, y, "DrawCircle", col);
    DrawCircle(100, y + 40, 30, GetColor(255, 200, 0), FALSE);

    // DrawOval（枠のみ）
    DrawString(220, y, "DrawOval", col);
    DrawOval(280, y + 40, 50, 25, GetColor(200, 100, 255), FALSE);

    // DrawTriangle（枠のみ）
    DrawString(400, y, "DrawTriangle", col);
    DrawTriangle(440, y + 65, 470, y + 20, 510, y + 65, GetColor(0, 200, 200), FALSE);

    y += 130;

    // DrawQuadrangle（枠のみ）
    DrawString(60, y, "DrawQuadrangle", col);
    DrawQuadrangle(70, y + 65, 110, y + 20, 160, y + 35, 140, y + 65,
                   GetColor(255, 150, 0), FALSE);

    // DrawRoundRect（枠のみ）
    DrawString(260, y, "DrawRoundRect", col);
    DrawRoundRect(270, y + 20, 400, y + 65, 15, 15, GetColor(150, 255, 150), FALSE);
}
#endif
```

---

### 3.6 ページ1：塗りつぶし図形

ページ0と同じ関数を `TRUE`（塗りつぶし）で描画し、関数名の末尾に `(fill)` を付けて表示。

```cpp
#ifdef _DEBUG
static void DrawPageFill(void) {
    int y = 80;

    DrawString(60,  y, "DrawBox (fill)",      GetColor(255,255,255));
    DrawBox(60, y+30, 180, y+75, GetColor(100,100,255), TRUE);

    DrawString(240, y, "DrawCircle (fill)",   GetColor(255,255,255));
    DrawCircle(300, y+52, 30, GetColor(255,200,0), TRUE);

    DrawString(420, y, "DrawOval (fill)",     GetColor(255,255,255));
    DrawOval(500, y+52, 50, 25, GetColor(200,100,255), TRUE);

    y += 140;

    DrawString(60,  y, "DrawTriangle (fill)", GetColor(255,255,255));
    DrawTriangle(80, y+70, 130, y+20, 180, y+70, GetColor(0,200,200), TRUE);

    DrawString(260, y, "DrawQuadrangle (fill)", GetColor(255,255,255));
    DrawQuadrangle(270, y+70, 310, y+20, 380, y+35, 360, y+70,
                   GetColor(255,150,0), TRUE);

    DrawString(470, y, "DrawRoundRect (fill)", GetColor(255,255,255));
    DrawRoundRect(480, y+20, 620, y+70, 15, 15, GetColor(150,255,150), TRUE);
}
#endif
```

---

### 3.7 ページ2：文字列描画

```cpp
#ifdef _DEBUG
static void DrawPageString(void) {
    int col = GetColor(255,255,255);
    int y   = 80;

    DrawString(30, y, "DrawString", col);
    DrawString(30, y+30, "Hello, DxLib!", GetColor(255,255,0));

    y += 90;
    DrawString(30, y, "DrawFormatString", col);
    DrawFormatString(30, y+30, GetColor(100,255,100),
                     "int=%d  float=%.2f  str=%s", 42, 3.14f, "test");

    y += 90;
    DrawString(30, y, "DrawStringToHandle（フォントハンドル）", col);
    DrawString(30, y+30, "※ フォントハンドルは CreateFontToHandle() で作成",
               GetColor(180,180,180));

    y += 90;
    DrawString(30, y, "GetDrawStringWidth（文字列の幅を取得）", col);
    const char* sample = "SAMPLE TEXT";
    int w = GetDrawStringWidth(sample, strlen(sample));
    DrawFormatString(30, y+30, GetColor(255,200,0), "\"%s\"  幅=%dpx", sample, w);
    DrawBox(30, y+60, 30+w, y+65, GetColor(255,100,100), TRUE);  // 幅を可視化
}
#endif
```

---

### 3.8 ページ3：ブレンドモード

```cpp
#ifdef _DEBUG
static void DrawPageBlend(void) {
    int y = 70;

    // 背景として色付きボックスを描画
    DrawFillBox(0, y-10, 800, y+290, GetColor(30, 60, 120));

    const struct { int mode; const char* name; } blends[] = {
        { DX_BLENDMODE_NOBLEND,  "NOBLEND（通常）"    },
        { DX_BLENDMODE_ALPHA,    "ALPHA（半透明）"     },
        { DX_BLENDMODE_ADD,      "ADD（加算）"         },
        { DX_BLENDMODE_SUB,      "SUB（減算）"         },
        { DX_BLENDMODE_MUL,      "MUL（乗算）"         },
    };
    int blendCount = sizeof(blends) / sizeof(blends[0]);

    for (int i = 0; i < blendCount; i++) {
        int x = 60 + i * 140;
        DrawString(x, y, blends[i].name, GetColor(255,255,255));
        SetDrawBlendMode(blends[i].mode, 180);
        DrawFillBox(x, y+25, x+100, y+125, GetColor(255, 100, 0));
        DrawCircle(x+50, y+175, 40, GetColor(0, 200, 255), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    DrawString(30, y+230, "※ アルファ値=180 で描画しています",
               GetColor(180,180,180));
}
#endif
```

---

## 4. 入力サンプル画面（SCR_DEBUG_INPUT）

### 4.1 レイアウト

```
┌────────────────────────────────────────────────────────────┐
│ 入力サンプル  [ESC] 戻る                                   │
├──────────────────────────┬─────────────────────────────────┤
│    キーボード             │    コントローラー               │
│                          │                                 │
│  KEY_INPUT_UP    : ●    │  PAD_INPUT_UP    : ○           │
│  KEY_INPUT_DOWN  : ○    │  PAD_INPUT_DOWN  : ○           │
│  KEY_INPUT_LEFT  : ○    │  PAD_INPUT_LEFT  : ○           │
│  KEY_INPUT_RIGHT : ●    │  PAD_INPUT_RIGHT : ○           │
│  KEY_INPUT_Z     : ○    │  PAD_INPUT_1     : ○           │
│  KEY_INPUT_X     : ○    │  PAD_INPUT_2     : ○           │
│  KEY_INPUT_A     : ○    │  PAD_INPUT_3     : ○           │
│  KEY_INPUT_D     : ○    │  PAD_INPUT_4     : ○           │
│  KEY_INPUT_RETURN: ○    │                                 │
│  KEY_INPUT_ESCAPE: ○    │  アナログスティック              │
│                          │  Left  X: +0.00  Y: +0.00     │
│  最後に押したキー:        │  Right X: +0.00  Y: +0.00     │
│  KEY_INPUT_RIGHT         │                                 │
│                          │  接続状態: 未接続               │
└──────────────────────────┴─────────────────────────────────┘
```

- `●` = 押下中、`○` = 未入力

### 4.2 監視対象キー一覧

```cpp
#ifdef _DEBUG
typedef struct {
    int         keyCode;
    const char* label;
} KeyEntry;

static const KeyEntry KEY_LIST[] = {
    { KEY_INPUT_UP,     "KEY_INPUT_UP    " },
    { KEY_INPUT_DOWN,   "KEY_INPUT_DOWN  " },
    { KEY_INPUT_LEFT,   "KEY_INPUT_LEFT  " },
    { KEY_INPUT_RIGHT,  "KEY_INPUT_RIGHT " },
    { KEY_INPUT_Z,      "KEY_INPUT_Z     " },
    { KEY_INPUT_X,      "KEY_INPUT_X     " },
    { KEY_INPUT_A,      "KEY_INPUT_A     " },
    { KEY_INPUT_D,      "KEY_INPUT_D     " },
    { KEY_INPUT_RETURN, "KEY_INPUT_RETURN" },
    { KEY_INPUT_ESCAPE, "KEY_INPUT_ESCAPE" },
};
#define KEY_LIST_COUNT (sizeof(KEY_LIST) / sizeof(KEY_LIST[0]))
#endif
```

### 4.3 コントローラーボタン一覧

```cpp
#ifdef _DEBUG
typedef struct {
    int         padBit;
    const char* label;
} PadEntry;

static const PadEntry PAD_LIST[] = {
    { PAD_INPUT_UP,   "PAD_INPUT_UP   " },
    { PAD_INPUT_DOWN, "PAD_INPUT_DOWN " },
    { PAD_INPUT_LEFT, "PAD_INPUT_LEFT " },
    { PAD_INPUT_RIGHT,"PAD_INPUT_RIGHT" },
    { PAD_INPUT_1,    "PAD_INPUT_1    " },
    { PAD_INPUT_2,    "PAD_INPUT_2    " },
    { PAD_INPUT_3,    "PAD_INPUT_3    " },
    { PAD_INPUT_4,    "PAD_INPUT_4    " },
};
#define PAD_LIST_COUNT (sizeof(PAD_LIST) / sizeof(PAD_LIST[0]))
#endif
```

### 4.4 状態変数

```cpp
#ifdef _DEBUG
static int   s_lastKey    = -1;    // 最後に押されたキーのインデックス
#endif
```

### 4.5 更新処理

```cpp
#ifdef _DEBUG
void UpdateDebugInput(void) {
    // 最後に押したキーを更新
    for (int i = 0; i < (int)KEY_LIST_COUNT; i++) {
        if (IsKeyTriggered(KEY_LIST[i].keyCode)) {
            s_lastKey = i;
        }
    }

    // ESC以外でESCが押されたら戻る（ESC自体は監視するが遷移優先）
    if (IsKeyTriggered(KEY_INPUT_ESCAPE)) {
        ChangeScreen(UpdateDebugMenu, DrawDebugMenu);
    }
}
#endif
```

### 4.6 描画処理

```cpp
#ifdef _DEBUG
void DrawDebugInput(void) {
    ClearDrawScreen();

    // ヘッダー
    DrawString(10, 10, "入力サンプル  [ESC] 戻る", GetColor(255,255,0));
    DrawLine(0, 35, 800, 35, GetColor(100,100,100));
    DrawLine(400, 35, 400, 590, GetColor(100,100,100));

    // ── 左半分：キーボード ──────────────────
    DrawString(30, 45, "キーボード", GetColor(200,200,200));

    for (int i = 0; i < (int)KEY_LIST_COUNT; i++) {
        int isOn = CheckHitKey(KEY_LIST[i].keyCode);
        int col  = isOn ? GetColor(255,255,0) : GetColor(160,160,160);
        char mark = isOn ? (char)0x81 : 'o';  // ● or o （環境依存の場合は記号で代替）

        DrawFormatString(30, 70 + i * 28, col, "%s : %s",
                         KEY_LIST[i].label, isOn ? "●" : "○");
    }

    // 最後に押したキー
    DrawString(30, 360, "最後に押したキー:", GetColor(200,200,200));
    if (s_lastKey >= 0) {
        DrawString(30, 385, KEY_LIST[s_lastKey].label, GetColor(255,220,0));
    }

    // ── 右半分：コントローラー ──────────────
    DrawString(420, 45, "コントローラー", GetColor(200,200,200));

    int padInput = GetJoypadInputState(DX_INPUT_PAD1);
    int isConnected = (GetJoypadNum() > 0);

    for (int i = 0; i < (int)PAD_LIST_COUNT; i++) {
        int isOn = isConnected && (padInput & PAD_LIST[i].padBit);
        int col  = isOn ? GetColor(255,255,0) : GetColor(160,160,160);
        DrawFormatString(420, 70 + i * 28, col, "%s : %s",
                         PAD_LIST[i].label, isOn ? "●" : "○");
    }

    // アナログスティック
    DrawString(420, 310, "アナログスティック", GetColor(200,200,200));
    if (isConnected) {
        int lx = 0, ly = 0, rx = 0, ry = 0;
        GetJoypadAnalogInput(&lx, &ly, DX_INPUT_PAD1);
        GetJoypadAnalogInput2(&rx, &ry, DX_INPUT_PAD1);
        DrawFormatString(420, 335, GetColor(255,255,255),
                         "Left  X: %+.2f  Y: %+.2f", lx / 1000.0f, ly / 1000.0f);
        DrawFormatString(420, 360, GetColor(255,255,255),
                         "Right X: %+.2f  Y: %+.2f", rx / 1000.0f, ry / 1000.0f);
    }

    // 接続状態
    DrawFormatString(420, 410, isConnected ? GetColor(100,255,100) : GetColor(255,100,100),
                     "接続状態: %s", isConnected ? "接続中" : "未接続");
}
#endif
```

---

## 5. ファイル構成

```
header/
└── debug.h       // 全関数宣言を #ifdef _DEBUG で保護

source/
└── debug.cpp     // 全実装を #ifdef _DEBUG で保護
```

```cpp
// header/debug.h
#pragma once

#ifdef _DEBUG
void UpdateDebugMenu(void);
void DrawDebugMenu(void);
void UpdateDebugDraw(void);
void DrawDebugDraw(void);
void UpdateDebugInput(void);
void DrawDebugInput(void);
#endif
```

---

## 6. タイトル画面のデバッグボタン（再掲・実装箇所）

```cpp
// source/title.cpp の DrawTitle() 内
#ifdef _DEBUG
DrawString(680, 570, "DXLIB SAMPLE", GetColor(255, 255, 0));
#endif

// source/title.cpp の UpdateTitle() 内
#ifdef _DEBUG
// 右下ボタン付近でEnterを押したとき（簡易実装）
if (IsKeyTriggered(KEY_INPUT_F1)) {
    ChangeScreen(UpdateDebugMenu, DrawDebugMenu);
}
#endif
```

> F1キーをデバッグメニューへのショートカットとする案。マウスクリック対応はDxLibの `GetMousePoint` + `GetMouseInput` で実装できる。
