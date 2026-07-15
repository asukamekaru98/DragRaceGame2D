# シーケンス図(実装リバース)

## 1. メインループ(main.cpp — 実装済み)

1フレームの処理順。`updateInput` が NULL の画面(オープニング、ゲーム)は入力更新をスキップする。
画面遷移は `ChangeScreen()` で `next*` に予約され、フレーム末尾でまとめて反映される。

```mermaid
sequenceDiagram
    participant Main as WinMain(メインループ)
    participant Screen as g_screen
    participant Input as s_input(画面ごとのKeyInput派生)
    participant Update as 画面Update
    participant Draw as 画面Draw

    loop 毎フレーム(ProcessMessage() == SUCCESS の間)
        Main->>Main: ClearDrawScreen()
        alt updateInput != NULL
            Main->>Screen: updateInput()
            Screen->>Input: s_input.Update()
            Input->>Input: RefreshKeyState()<br>意味付けフラグ更新
        end
        Main->>Screen: update()
        Screen->>Update: 画面ロジック
        Update->>Input: IsXxxTriggered() 等で入力参照
        opt 遷移条件成立
            Update->>Screen: ChangeScreen(updateInput, update, draw)
            Note over Screen: next* に予約(即時遷移はしない)
        end
        Main->>Screen: draw()
        Screen->>Draw: 描画
        opt nextUpdate != NULL
            Main->>Screen: next* を現在の関数ポインタへ反映
        end
        Main->>Main: ScreenFlip()
    end
```

## 2. 画面突入時の入力再同期(実装済み・重要)

各画面の Update は初回フレームに init ブロックを持ち、そこで `s_input.Update()` をもう一度呼ぶ。
前画面から押しっぱなしのキーが新画面でトリガー誤発火するのを防ぐためで、削除してはならない。

```mermaid
sequenceDiagram
    participant Main as メインループ
    participant Update as 画面Update(初回フレーム)
    participant Input as s_input

    Main->>Input: updateInput() → s_input.Update()
    Main->>Update: update()
    Update->>Update: !s_initialized → 画面状態リセット
    Update->>Input: s_input.Update()(再同期)
    Note over Input: prev==curr となり<br>持ち越しキーのIsTriggeredがfalseに
    Update->>Update: s_initialized = 1
```

## 3. エンジン→メーターのデータフロー(未実装・構想)

レースロジック(ゲーム画面)実装時の構想。現状は `CarEngine` / `CarMeter` ともスケルトンで、
針アニメーションのプロトタイプは `debug.cpp` のデバッグメーター画面に手続き型で存在する。

```mermaid
sequenceDiagram
    participant Input as GameKeyInput(未実装)
    participant CarEngine
    participant CarEngineData
    participant CarMeter
    participant TurboGauge as TurboGauge(未実装)

    Note over CarEngine: Update()
    Input->>CarEngine: キー入力(アクセル/ギア)
    CarEngine->>CarEngine: RPM・速度・ブースト圧を計算
    CarEngine->>CarEngineData: データを詰める

    Note over CarMeter,TurboGauge: Update(data)
    CarEngineData->>CarMeter: fRpm, fSpeed, iGear
    CarEngineData->>TurboGauge: fBoostPressure

    Note over CarMeter,TurboGauge: Draw()
    CarMeter->>CarMeter: 針のアニメーション描画
    TurboGauge->>TurboGauge: ターボ計描画
```
