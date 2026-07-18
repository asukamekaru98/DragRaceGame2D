# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## プロジェクト概要

DxLib(Ver 3.18e、リポジトリ内 `DxLib/` に同梱)を使ったC++製の2Dドラッグレースゲーム(ゼロヨン)。Windows専用のVisual Studioプロジェクト。仕様書は `spec/` 配下の日本語Markdownに集約されており、READMEは実質空。

## ビルド

Visual Studio 2022(PlatformToolset v143)。テストは存在しない。動作確認は生成されたexeの実行による。

```
msbuild DragRaceGame2D.sln /p:Configuration=Debug /p:Platform=x64
```

- 構成: Debug / Release、プラットフォーム: x64 / Win32(主に x64 Debug を使用)
- CharacterSet=MultiByte、静的CRT(/MTd, /MT)
- DxLibは `./DxLib` をインクルード/ライブラリパスに指定し、`#pragma comment(lib,...)` で自動リンク。`DxLib/` はライブラリ本体ごとgit追跡されている — 削除・改変しないこと

## 文字エンコーディング(重要)

ソースファイルは Shift-JIS(CP932)・UTF-8 BOM・ASCII が混在している。

- 日本語コメントを含むファイルを新規作成・編集する場合は **CP932 で保存する**こと。UTF-8(BOMなし)で保存するとMSVCが日本語コメントを誤読しビルドエラーになる
- 既存ファイルを編集する際は、そのファイルの既存エンコーディングを維持する
- 行末は CRLF

## アーキテクチャ

### 画面ステートマシン(source/screen_manager.h, main.cpp)

画面は「UpdateInput / Update / Draw の関数ポインタ3点セット」で表現される。

- `g_screen`(ScreenManager構造体のグローバル)が現在画面の関数ポインタを保持
- `g_ScreenFuncs[]`(実体は screen_manager.cpp)が `SCREEN_NAME` 列挙(SCREEN_OPENING〜SCREEN_RESULT、`_DEBUG` 時は SCREEN_DEBUG_* も)に対応する関数テーブル。キー入力を持たない画面(オープニング、ゲーム)の updateInput は NULL
- 画面遷移は各画面のUpdate内から `ChangeScreen(updateInput, update, draw)` を呼んで `next*` に予約し、main.cppのメインループ末尾でフレーム境界に反映される
- メインループ: `ProcessMessage` → `ClearDrawScreen` → `g_screen.updateInput()`(NULLならスキップ) → `g_screen.update()` → `g_screen.draw()` → 遷移反映 → `ScreenFlip`

### データ共有

画面間のデータ共有はグローバル変数(`g_gameData` in source/game_data.h)。グローバルは `g_` プレフィクス、画像ハンドルは `g_hXxx`。

### 入力処理(クラス型に統一済み)

- 抽象基底 `source/input/key_input.h` — `KeyInput`(`RefreshKeyState` / `IsPressed` / `IsTriggered` は protected)。派生クラスが `Update()` で意味付けフラグを更新し、const getter を公開する
- 画面ごとの派生クラス(source/input/ 配下): `TitleInput` / `GarageInput` / `DealerInput` / `CustomizeInput` / `ResultInput`。デバッグ用に `DebugMeterInput`(メーター画面)と `DebugRawInput`(生キー公開、デバッグサンプル3画面共用)
- 各画面 .cpp が static インスタンスと `UpdateXxxInput()` を持ち、メインループから `g_screen.updateInput()` 経由で毎フレーム呼ばれる
- 画面突入時の init ブロックで `s_input.Update()` をもう一度呼んでいる — 画面をまたいで押しっぱなしのキーがトリガー誤発火するのを防ぐため。削除しないこと
- 旧手続き型 `input.cpp`/`input.h` は削除済み。UML上の `GameKeyInput` はゲーム画面のレースロジック実装時に追加予定(未実装)

### その他の構造

- リソース管理: `source/manager/resourceManager.h` — 抽象 `ResourceManager` を継承し、`GetResources()` で `{ハンドルポインタ, ファイルパス}` 配列(末尾 `{nullptr,nullptr}` 番兵)を返すと `LoadResources`/`UnloadResources` が共通処理される。例: `source/title_resource.h`
- メーター: `source/car_meter/` — 抽象基底 `GaugeBase` からの継承
- クラスのprivateメンバは末尾アンダースコア(例 `fRPM_`)+ ハンガリアン風接頭辞(f=float, c=char, h=handle等)、定数は UPPER_SNAKE

## 仕様書(spec/)

要件 → 基本設計 → 画面別詳細設計 → UML の階層。実装より仕様が先行しており、UML(spec/uml/diagrams/)には未実装のクラス(GameKeyInput, CarEngine の完全形など)が含まれる — 仕様と実装は必ずしも一致しない。

- `spec/requirements.md` — ゲーム要件(画面一覧、操作: ↑/Z=加速, A=ギアアップ, D=ギアダウン, ESC=終了、解像度800×600固定)
- `spec/basic_design.md` — 画面ID・遷移トリガー・ゲームデータ・レースロジック(RPM×ギア→速度、400mでゴール)
- `spec/detail_design_screen.md` — 画面管理アーキテクチャの規定(最重要)
- `spec/detail_design_*.md` — 各画面の詳細設計
- `spec/uml/diagrams/` — mermaidクラス図・シーケンス図
