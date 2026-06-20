# 詳細設計 - 画面管理・遷移

## 1. 設計方針

「現在アクティブな画面」を関数ポインタで表現する。
画面ごとに **更新関数（Update）** と **描画関数（Draw）** をペアで持ち、
遷移時にポインタを差し替えることで画面を切り替える。

---

## 2. データ構造

### 2.1 画面関数の型定義

```cpp
// typedef.h に追加
typedef void (*UpdateFunc)(void);   // 更新関数の型
typedef void (*DrawFunc)(void);     // 描画関数の型
```

### 2.2 画面管理構造体

```cpp
// header/screen_manager.h
typedef struct {
    UpdateFunc  update;     // 現在の画面の更新関数
    DrawFunc    draw;       // 現在の画面の描画関数
} ScreenManager;
```

### 2.3 遷移リクエスト

画面関数の中から直接ポインタを書き換えると管理が難しくなるため、
「次の画面」を別の変数に予約し、フレームの終わりに反映する。

```cpp
// header/screen_manager.h
typedef struct {
    UpdateFunc  update;
    DrawFunc    draw;
    UpdateFunc  nextUpdate;     // 次フレームで切り替える更新関数（NULLなら遷移なし）
    DrawFunc    nextDraw;       // 次フレームで切り替える描画関数（NULLなら遷移なし）
} ScreenManager;
```

---

## 3. 各画面の関数一覧

画面ごとに Update / Draw のペアを定義する。

| 画面 | 更新関数 | 描画関数 |
|------|----------|----------|
| オープニング | `UpdateOpening()` | `DrawOpening()` |
| タイトル | `UpdateTitle()` | `DrawTitle()` |
| マイガレージ | `UpdateGarage()` | `DrawGarage()` |
| カーディーラー | `UpdateDealer()` | `DrawDealer()` |
| カスタマイズ | `UpdateCustomize()` | `DrawCustomize()` |
| ゲーム | `UpdateGame()` | `DrawGame()` |
| リザルト | `UpdateResult()` | `DrawResult()` |
| 裏モードメニュー | `UpdateDebugMenu()` | `DrawDebugMenu()` |
| 描画サンプル | `UpdateDebugDraw()` | `DrawDebugDraw()` |
| 入力サンプル | `UpdateDebugInput()` | `DrawDebugInput()` |

---

## 4. メインループの構造

```cpp
// source/main.cpp
int WINAPI WinMain(...) {

    InitProgram();

    // 初期画面をオープニングに設定
    ScreenManager screen;
    screen.update    = UpdateOpening;
    screen.draw      = DrawOpening;
    screen.nextUpdate = NULL;
    screen.nextDraw   = NULL;

    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {

        ClearDrawScreen();

        // 更新
        screen.update();

        // 描画
        screen.draw();

        // 遷移リクエストがあれば反映
        if (screen.nextUpdate != NULL) {
            screen.update    = screen.nextUpdate;
            screen.draw      = screen.nextDraw;
            screen.nextUpdate = NULL;
            screen.nextDraw   = NULL;
        }

        ScreenFlip();
    }

    DxLib_End();
    return 0;
}
```

---

## 5. 遷移のやり方

各画面の Update 関数は、遷移条件を満たしたとき `ScreenManager` に次の画面をセットする。

`ScreenManager` をグローバル変数、またはポインタ渡しで Update 関数に渡す。

### 5.1 グローバル変数方式（シンプル）

```cpp
// source/screen_manager.cpp
ScreenManager g_screen;

// 遷移関数
void ChangeScreen(UpdateFunc update, DrawFunc draw) {
    g_screen.nextUpdate = update;
    g_screen.nextDraw   = draw;
}
```

各画面の Update 関数から `ChangeScreen()` を呼ぶだけで遷移できる。

```cpp
// 例：タイトル画面でスタートが押されたらガレージへ遷移
void UpdateTitle(void) {
    if (CheckHitKey(KEY_INPUT_RETURN)) {
        ChangeScreen(UpdateGarage, DrawGarage);
    }

#ifdef _DEBUG
    if (/* DXLIBサンプルボタンが押された */) {
        ChangeScreen(UpdateDebugMenu, DrawDebugMenu);
    }
#endif
}
```

---

## 6. 画面をまたぐデータの受け渡し

遷移時に前の画面の結果を次の画面に渡したい場合（例：ガレージで選んだ車をゲームに渡す）、
グローバルなゲームデータ構造体を経由して受け渡す。
関数の引数には乗せない（関数ポインタの型を統一するため）。

```cpp
// header/game_data.h
typedef struct {
    // プレイヤーデータ（画面をまたいで共有）
    SI_4    money;              // 所持金
    // Car  garage[MAX_CAR];   // 所有車リスト（Car型は別途定義）
    // Car* selectedCar;       // 選択中の車
} GameData;

extern GameData g_gameData;    // 全画面から参照できるグローバル
```

---

## 7. ファイル構成

```
header/
├── typedef.h           # 型定義（UpdateFunc / DrawFunc を追加）
├── screen_manager.h    # ScreenManager 構造体・ChangeScreen 宣言
├── game_data.h         # GameData 構造体（画面間共有データ）
├── opening.h
├── title.h
├── garage.h
├── dealer.h
├── customize.h
├── game.h
├── result.h
└── debug.h             # 裏モード（#ifdef _DEBUG で保護）

source/
├── main.cpp            # WinMain・メインループ
├── screen_manager.cpp  # ChangeScreen・g_screen の実体
├── game_data.cpp       # g_gameData の実体
├── opening.cpp
├── title.cpp
├── garage.cpp
├── dealer.cpp
├── customize.cpp
├── game.cpp
├── result.cpp
└── debug.cpp           # 裏モード（#ifdef _DEBUG で保護）
```

---

## 8. 遷移フロー（関数ポインタの動き）

```
起動時
  g_screen.update = UpdateOpening
  g_screen.draw   = DrawOpening

オープニングで任意キー押下
  ChangeScreen(UpdateTitle, DrawTitle)
  → 次フレームから update/draw が Title に切り替わる

タイトルでスタート選択
  ChangeScreen(UpdateGarage, DrawGarage)

ガレージでレース開始
  ChangeScreen(UpdateGame, DrawGame)

ゲームでゴール
  ChangeScreen(UpdateResult, DrawResult)

リザルトで任意キー
  ChangeScreen(UpdateGarage, DrawGarage)

[デバッグビルドのみ]
タイトルで DXLIBサンプルボタン選択
  ChangeScreen(UpdateDebugMenu, DrawDebugMenu)
```
