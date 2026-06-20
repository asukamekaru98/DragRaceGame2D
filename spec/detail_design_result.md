# 詳細設計 - リザルト画面

## 1. 使用リソース

| 変数名（案） | ファイルパス（案） | 内容 |
|--------------|-------------------|------|
| `g_hBgResult` | `resource/result/bg_result.png` | リザルト背景 |

---

## 2. 画面レイアウト

```
┌────────────────────────────────────────────────────────────┐
│                   背景                                     │
│                                                            │
│              ★ WIN ★   or   LOSE...                      │  ← ① 勝敗
│                                                            │
│          タイム        :  00:12.34                         │  ← ②
│          ベストタイム  :  00:11.80  (更新！)               │  ← ③
│          獲得賞金      :  + $1,500                        │  ← ④
│          所持金        :  $4,000                          │  ← ⑤
│                                                            │
│                                                            │
│                [ PRESS ANY KEY ]                           │
└────────────────────────────────────────────────────────────┘
```

項目①〜⑤が順番にフェードインで出てくる。
全項目が表示され終わったら「PRESS ANY KEY」が点滅表示される。

---

## 3. 表示項目と順序

| 順番 | 項目 | 内容 |
|------|------|------|
| ① | 勝敗 | "WIN" または "LOSE" |
| ② | タイム | 今回のレースタイム |
| ③ | ベストタイム | 自己ベスト（更新時は「更新！」を付ける） |
| ④ | 獲得賞金 | 勝敗・タイムに応じた賞金額 |
| ⑤ | 所持金 | 賞金加算後の所持金 |

---

## 4. 賞金計算

```cpp
int CalcPrize(int rank, float raceTime) {
    if (rank == 1) {
        // 勝利：基本賞金 + タイムボーナス
        int base  = PRIZE_WIN_BASE;
        int bonus = (int)((PRIZE_TIME_BONUS_BASE / raceTime));  // 速いほど高い
        return base + bonus;
    } else {
        // 敗北：参加賞
        return PRIZE_LOSE;
    }
}
```

| 定数 | 値（案） | 説明 |
|------|----------|------|
| `PRIZE_WIN_BASE` | 1000 | 勝利基本賞金 |
| `PRIZE_TIME_BONUS_BASE` | 5000 | タイムボーナスの分子（小さい時間ほど高くなる） |
| `PRIZE_LOSE` | 200 | 敗北時の参加賞 |

---

## 5. ベストタイム更新判定

```cpp
// g_gameData にベストタイムを保持
if (g_gameData.lastRank == 1) {
    if (g_gameData.bestTime < 0.0f || g_gameData.lastTime < g_gameData.bestTime) {
        g_gameData.bestTime = g_gameData.lastTime;
        s_isBestUpdated = TRUE;  // 「更新！」表示フラグ
    }
}
```

> `g_gameData.bestTime` の初期値は `-1.0f`（未記録）。

---

## 6. フェードイン演出

### 6.1 設計方針

表示する項目を配列で管理し、一定フレームごとに次の項目のアルファ値を上げていく。

```cpp
// source/result.cpp
#define RESULT_ITEM_COUNT  5

static float s_itemAlpha[RESULT_ITEM_COUNT];  // 各項目の不透明度 (0.0〜255.0)
static int   s_currentItem;                   // 現在フェードイン中の項目インデックス
static int   s_allShown;                      // 全項目表示完了フラグ
static int   s_blinkTimer;                    // 「PRESS ANY KEY」点滅タイマー
```

### 6.2 フェードイン更新

```cpp
void UpdateResultFade(void) {
    if (s_currentItem >= RESULT_ITEM_COUNT) {
        s_allShown = TRUE;
        return;
    }

    s_itemAlpha[s_currentItem] += FADE_SPEED;

    if (s_itemAlpha[s_currentItem] >= 255.0f) {
        s_itemAlpha[s_currentItem] = 255.0f;
        // 一定時間待ってから次の項目へ
        s_waitTimer++;
        if (s_waitTimer >= ITEM_WAIT_FRAMES) {
            s_currentItem++;
            s_waitTimer = 0;
        }
    }
}
```

| 定数 | 値（案） | 説明 |
|------|----------|------|
| `FADE_SPEED` | 5.0f | 毎フレームのアルファ加算量（255÷5=51フレーム≒0.85秒） |
| `ITEM_WAIT_FRAMES` | 20 | 次の項目が出るまでの待機フレーム数（約0.33秒） |

---

## 7. 「PRESS ANY KEY」点滅

```cpp
// s_allShown が TRUE のときだけ動かす
s_blinkTimer++;
int visible = (s_blinkTimer / BLINK_INTERVAL) % 2;  // 0か1で交互
// visible == 1 のときだけ描画
```

| 定数 | 値（案） | 説明 |
|------|----------|------|
| `BLINK_INTERVAL` | 30 | 点滅の半周期（フレーム）。30フレームON/30フレームOFFで1秒点滅 |

---

## 8. 状態変数（初期化）

```cpp
void InitResult(void) {
    // フェードイン初期化
    for (int i = 0; i < RESULT_ITEM_COUNT; i++) {
        s_itemAlpha[i] = 0.0f;
    }
    s_currentItem = 0;
    s_waitTimer   = 0;
    s_allShown    = FALSE;
    s_blinkTimer  = 0;
    s_isBestUpdated = FALSE;

    // 賞金計算・加算
    s_prize = CalcPrize(g_gameData.lastRank, g_gameData.lastTime);
    g_gameData.money += s_prize;

    // ベストタイム更新判定
    if (g_gameData.lastRank == 1) {
        if (g_gameData.bestTime < 0.0f || g_gameData.lastTime < g_gameData.bestTime) {
            g_gameData.bestTime = g_gameData.lastTime;
            s_isBestUpdated = TRUE;
        }
    }
}
```

> `InitResult()` はリザルト画面に遷移した直後（`ChangeScreen` の後、または `UpdateResult` の初回）に1回だけ呼ぶ。

---

## 9. 更新処理（UpdateResult）

```cpp
void UpdateResult(void) {
    UpdateResultFade();

    if (s_allShown) {
        s_blinkTimer++;

        if (CheckHitKeyAll() != 0) {
            ChangeScreen(UpdateGarage, DrawGarage);
        }
    }
}
```

---

## 10. 描画処理（DrawResult）

```cpp
void DrawResult(void) {

    // Layer 1: 背景
    DrawExtendGraph(0, 0, 800, 600, g_hBgResult, TRUE);

    // ── 各項目をフェードイン描画 ──────────────────

    // ① 勝敗
    if (s_itemAlpha[0] > 0) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)s_itemAlpha[0]);
        if (g_gameData.lastRank == 1) {
            DrawString(300, 120, "★ WIN ★",  GetColor(255, 220, 0));
        } else {
            DrawString(310, 120, "LOSE...", GetColor(150, 150, 150));
        }
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // ② タイム
    if (s_itemAlpha[1] > 0) {
        int min  = (int)(g_gameData.lastTime / 60);
        int sec  = (int)(g_gameData.lastTime) % 60;
        int msec = (int)((g_gameData.lastTime - (int)g_gameData.lastTime) * 100);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)s_itemAlpha[1]);
        DrawFormatString(RESULT_LABEL_X, RESULT_Y + RESULT_LINE_H * 0,
                         GetColor(255,255,255), "タイム        :  %02d:%02d.%02d", min, sec, msec);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // ③ ベストタイム
    if (s_itemAlpha[2] > 0) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)s_itemAlpha[2]);
        if (g_gameData.bestTime < 0.0f) {
            DrawString(RESULT_LABEL_X, RESULT_Y + RESULT_LINE_H * 1,
                       GetColor(255,255,255), "ベストタイム  :  --:--.--");
        } else {
            int bmin  = (int)(g_gameData.bestTime / 60);
            int bsec  = (int)(g_gameData.bestTime) % 60;
            int bmsec = (int)((g_gameData.bestTime - (int)g_gameData.bestTime) * 100);
            DrawFormatString(RESULT_LABEL_X, RESULT_Y + RESULT_LINE_H * 1,
                             GetColor(255,255,255), "ベストタイム  :  %02d:%02d.%02d%s",
                             bmin, bsec, bmsec,
                             s_isBestUpdated ? "  (更新！)" : "");
        }
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // ④ 獲得賞金
    if (s_itemAlpha[3] > 0) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)s_itemAlpha[3]);
        DrawFormatString(RESULT_LABEL_X, RESULT_Y + RESULT_LINE_H * 2,
                         GetColor(255, 220, 0), "獲得賞金      :  + $%d", s_prize);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // ⑤ 所持金
    if (s_itemAlpha[4] > 0) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)s_itemAlpha[4]);
        DrawFormatString(RESULT_LABEL_X, RESULT_Y + RESULT_LINE_H * 3,
                         GetColor(255,255,255), "所持金        :  $%d", g_gameData.money);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    // PRESS ANY KEY（点滅）
    if (s_allShown) {
        int visible = (s_blinkTimer / BLINK_INTERVAL) % 2;
        if (visible) {
            DrawString(280, 520, "PRESS ANY KEY", GetColor(255,255,255));
        }
    }
}
```

---

## 11. 座標・定数まとめ（暫定値・要調整）

| 定数名 | 暫定値 | 説明 |
|--------|--------|------|
| `RESULT_LABEL_X` | 180 | 各項目テキストのX座標 |
| `RESULT_Y` | 240 | 項目リストの先頭Y座標 |
| `RESULT_LINE_H` | 50 | 項目間の行間（px） |
| `FADE_SPEED` | 5.0f | 毎フレームのアルファ加算量 |
| `ITEM_WAIT_FRAMES` | 20 | 次項目が出るまでの待機フレーム |
| `BLINK_INTERVAL` | 30 | 点滅の半周期（フレーム） |
| `PRIZE_WIN_BASE` | 1000 | 勝利基本賞金 |
| `PRIZE_TIME_BONUS_BASE` | 5000 | タイムボーナス計算用定数 |
| `PRIZE_LOSE` | 200 | 敗北参加賞 |
