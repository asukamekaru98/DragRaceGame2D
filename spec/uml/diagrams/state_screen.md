# 画面遷移図(実装リバース)

実装元: 各画面 .cpp の `ChangeScreen()` 呼び出し。デバッグ系画面は `_DEBUG` ビルド限定。

実装状況メモ:

- ゲーム画面はダミー実装: 入力なし(updateInput=NULL)で60フレーム待機後、ダミーのリザルトデータ(1位/12.34秒)を `g_gameData` に書いてリザルトへ遷移する
- ディーラー: 車両購入決定 or キャンセルでガレージへ戻る
- カスタマイズ: パーツ購入装備 or キャンセルでガレージへ戻る

```mermaid
stateDiagram-v2
    state "オープニング" as Opening
    state "タイトル" as Title
    state "ガレージ" as Garage
    state "ディーラー" as Dealer
    state "カスタマイズ" as Customize
    state "ゲーム(ダミー)" as Game
    state "リザルト" as Result
    state "デバッグメニュー" as DebugMenu
    state "デバッグ描画" as DebugDraw
    state "デバッグ入力" as DebugInput
    state "デバッグメーター" as DebugMeter

    [*] --> Opening : 起動(TitleResource読込後)

    Opening --> Title : 演出終了

    Title --> Garage : 任意キー → シャッター演出後
    Title --> DebugMenu : F1(_DEBUG)
    Title --> DebugMeter : F2(_DEBUG)

    Garage --> Dealer : メニュー「ディーラー」決定
    Garage --> Customize : メニュー「カスタマイズ」決定
    Garage --> Game : メニュー「レース」決定

    Dealer --> Garage : 購入決定 / キャンセル(ESC)
    Customize --> Garage : 装備完了 / キャンセル(ESC)

    Game --> Result : 60フレーム経過(ダミー実装)
    Result --> Garage : 任意キー

    DebugMenu --> DebugDraw : 項目0決定
    DebugMenu --> DebugInput : 項目1決定
    DebugMenu --> Title : ESC
    DebugDraw --> DebugMenu : ESC
    DebugInput --> DebugMenu : ESC
    DebugMeter --> Title : ESC
```
