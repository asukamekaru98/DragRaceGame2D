# 詳細設計 - ゲーム画面

## 1. 使用リソース

| 変数名（案） | ファイルパス（案） | 内容 |
|--------------|-------------------|------|
| `g_hBgGame` | `resource/game/bg_game.png` | 走行背景（ループスクロール） |
| `g_hPlayerCar` | `resource/cars/car_XX.png` | プレイヤー車スプライト |
| `g_hCpuCar` | `resource/cars/cpu_car.png` | CPU車スプライト |
| `g_hRoad` | `resource/game/road.png` | 路面スプライト（オプション） |

---

## 2. 画面レイアウト

```
┌────────────────────────────────────────────────────────────┐  ↑
│  走行距離: 123m / 400m    順位: 1位    タイム: 00:03.21   │  │
├────────────────────────────────────────────────────────────┤  │
│                                                            │  │
│  [背景：ループスクロール]                                   │ 450px
│                                                            │  │
│  CPU車  ───────────────────────────────────────────        │  │
│  自車   ──────────────────────────────────────────         │  │
│                                                            │  │
├────────────────────────────────────────────────────────────┤  ↓
│  [タコメーター]  [スピードメーター]  [燃料計]  [水温計]    │ 150px
│  RPM: 6500   Speed: 180km/h   Gear: 4   Time: 00:03.21   │
└────────────────────────────────────────────────────────────┘
  ← 800px →
```

- ゲームエリア：800 × 450 px（上部）
- メーターパネル：800 × 150 px（下部）
- 車は画面左から約1/3の位置に固定表示し、背景がスクロールする

---

## 3. ゲームの状態管理

```cpp
// header/game.h
typedef enum {
    GAME_STATE_COUNTDOWN,   // カウントダウン中
    GAME_STATE_RUNNING,     // レース中
    GAME_STATE_GOAL,        // ゴール演出中
} GAME_STATE;
```

---

## 4. カウントダウン仕様

### 4.1 流れ

```
ゲーム画面に入る
  └─> GAME_STATE_COUNTDOWN 開始
       └─> "3" 表示（1秒）
            └─> "2" 表示（1秒）
                 └─> "1" 表示（1秒）
                      └─> "GO!" 表示（0.5秒）
                           └─> GAME_STATE_RUNNING へ
```

### 4.2 状態変数

```cpp
static int   s_countNum    = 3;     // 現在の数字（3→2→1→0=GO!）
static float s_countTimer  = 0.0f;  // 経過フレーム数
```

```cpp
// UpdateGame 内（GAME_STATE_COUNTDOWN）
s_countTimer++;
if (s_countNum > 0 && s_countTimer >= FPS) {   // 1秒 = 60フレーム
    s_countNum--;
    s_countTimer = 0.0f;
} else if (s_countNum == 0 && s_countTimer >= FPS * 0.5f) {
    s_state = GAME_STATE_RUNNING;
}
```

| 定数 | 値 | 説明 |
|------|----|------|
| `FPS` | 60 | 1秒あたりのフレーム数 |

---

## 5. 走行物理モデル

### 5.1 概要

```
アクセル入力
  → RPM上昇
    → 現在ギアのギア比でタイヤ回転数に変換
      → 速度(km/h)に変換
        → 速度を積分 → 走行距離(m)を加算
```

### 5.2 状態変数（レースデータ）

```cpp
// source/game.cpp (static変数)
static GAME_STATE  s_state       = GAME_STATE_COUNTDOWN;
static float       s_rpm         = 0.0f;    // エンジン回転数 (0〜10000)
static float       s_speed       = 0.0f;    // 車速 (km/h)
static float       s_distance    = 0.0f;    // 走行距離 (m)
static int         s_gear        = 1;       // 現在ギア (1〜最大ギア数)
static float       s_fuel        = 1.0f;    // 燃料残量 (0.0〜1.0)
static float       s_waterTemp   = 60.0f;   // 水温 (℃)
static float       s_raceTimer   = 0.0f;    // レース経過時間 (秒)
static float       s_bgScrollX   = 0.0f;    // 背景スクロールX座標
static float       s_cpuDistance = 0.0f;    // CPU走行距離 (m)
```

### 5.3 RPMの計算

```cpp
// アクセルON時
float targetRpm = MAX_RPM_PER_GEAR[s_gear];  // そのギアの最大RPM
s_rpm += (targetRpm - s_rpm) * RPM_RISE_RATE;

// アクセルOFF時
s_rpm -= s_rpm * RPM_FALL_RATE;
```

| 定数 | 値（案） | 説明 |
|------|----------|------|
| `RPM_RISE_RATE` | 0.05f | RPM上昇係数（毎フレーム目標値に近づく速度） |
| `RPM_FALL_RATE` | 0.08f | RPM低下係数 |
| `MAX_RPM_PER_GEAR[]` | `{4000, 6000, 7500, 8500, 9000, 9500}` | ギアごとの最大RPM（6段の例） |

### 5.4 速度の計算

```cpp
float GEAR_RATIO[] = { 0.0f, 0.5f, 0.9f, 1.3f, 1.6f, 1.85f, 2.0f };
// インデックス0は未使用、1〜6がギア段数に対応

float targetSpeed = (s_rpm / 10000.0f) * car->maxSpeed * GEAR_RATIO[s_gear];
s_speed += (targetSpeed - s_speed) * SPEED_LERP;
```

| 定数 | 値（案） | 説明 |
|------|----------|------|
| `SPEED_LERP` | 0.03f | 速度変化の補間係数 |

### 5.5 走行距離の加算

```cpp
// km/h → m/frame に変換
// 1km/h = 1000m/3600s ÷ 60frame/s = 1/216 m/frame
s_distance += s_speed / 216.0f;
```

### 5.6 ギアチェンジ

```cpp
if (IsKeyTriggered(KEY_INPUT_A) && s_gear < car->gearCount) {
    s_gear++;
    s_rpm *= 0.7f;  // ギアアップでRPMを落とす
}
if (IsKeyTriggered(KEY_INPUT_D) && s_gear > 1) {
    s_gear--;
    s_rpm *= 1.3f;  // ギアダウンでRPM上昇（レブリミット超えたらクリップ）
    if (s_rpm > 10000.0f) s_rpm = 10000.0f;
}
```

### 5.7 燃料・水温

```cpp
// アクセルON時に燃料消費・水温上昇
if (IsKeyPressed(KEY_INPUT_Z) || IsKeyPressed(KEY_INPUT_UP)) {
    s_fuel     -= FUEL_CONSUMPTION;
    s_waterTemp += WATER_TEMP_RISE;
}
// 水温は自然冷却
s_waterTemp -= (s_waterTemp - WATER_TEMP_AMBIENT) * WATER_TEMP_COOL;

if (s_fuel < 0.0f) s_fuel = 0.0f;
```

| 定数 | 値（案） | 説明 |
|------|----------|------|
| `FUEL_CONSUMPTION` | 0.0002f | 毎フレームの燃料消費量 |
| `WATER_TEMP_RISE` | 0.02f | アクセルON時の水温上昇量（℃/frame） |
| `WATER_TEMP_COOL` | 0.002f | 自然冷却係数 |
| `WATER_TEMP_AMBIENT` | 60.0f | 水温の下限（外気温相当） |

---

## 6. 背景スクロール

車を画面上で固定し、背景を速度に応じてスクロールさせる。

```cpp
// 背景スクロール速度は車速に比例
s_bgScrollX -= s_speed * BG_SCROLL_RATE;

// 画像幅でループ（DrawModiGraphなどで対応、または手動でループ）
if (s_bgScrollX <= -BG_WIDTH) {
    s_bgScrollX += BG_WIDTH;
}
```

```cpp
// 描画時
DrawGraph((int)s_bgScrollX,             0, g_hBgGame, FALSE);
DrawGraph((int)s_bgScrollX + BG_WIDTH,  0, g_hBgGame, FALSE);  // つなぎ目を埋める
```

| 定数 | 値（案） | 説明 |
|------|----------|------|
| `BG_SCROLL_RATE` | 0.3f | 速度に対するスクロール倍率 |
| `BG_WIDTH` | 背景画像の横幅 | ループ計算に使用 |

---

## 7. CPU対戦相手

CPUは自動でアクセルを踏み続け、最適なタイミングでギアチェンジする。

```cpp
// CPU専用の状態変数
static float s_cpuRpm      = 0.0f;
static float s_cpuSpeed    = 0.0f;
static int   s_cpuGear     = 1;

// 毎フレーム更新
s_cpuRpm += (MAX_RPM_PER_GEAR[s_cpuGear] - s_cpuRpm) * RPM_RISE_RATE;
float cpuTarget = (s_cpuRpm / 10000.0f) * cpuCar.maxSpeed * GEAR_RATIO[s_cpuGear];
s_cpuSpeed += (cpuTarget - s_cpuSpeed) * SPEED_LERP;
s_cpuDistance += s_cpuSpeed / 216.0f;

// 最大RPMに達したら自動ギアアップ
if (s_cpuRpm >= MAX_RPM_PER_GEAR[s_cpuGear] * 0.98f && s_cpuGear < cpuCar.gearCount) {
    s_cpuGear++;
    s_cpuRpm *= 0.7f;
}
```

---

## 8. ゴール判定

```cpp
if (s_distance >= GOAL_DISTANCE) {
    s_state = GAME_STATE_GOAL;
    // 順位・タイムをリザルト用データに書き込む
    g_gameData.lastTime  = s_raceTimer;
    g_gameData.lastRank  = (s_distance >= s_cpuDistance) ? 1 : 2;
}
```

| 定数 | 値 | 説明 |
|------|----|------|
| `GOAL_DISTANCE` | 400.0f | ゴールまでの距離（m） |

ゴール演出（`GAME_STATE_GOAL`）中は一定時間（2秒程度）"GOAL!" を表示してからリザルト画面へ遷移。

---

## 9. HUD（ゲームエリア上部）

```cpp
void DrawHUD(void) {
    // 走行距離
    DrawFormatString(10, 10, GetColor(255,255,255),
                     "%.0fm / %.0fm", s_distance, GOAL_DISTANCE);

    // 順位
    int rank = (s_distance >= s_cpuDistance) ? 1 : 2;
    DrawFormatString(300, 10, GetColor(255,255,0), "%d位", rank);

    // タイム
    int min  = (int)(s_raceTimer / 60);
    int sec  = (int)(s_raceTimer) % 60;
    int msec = (int)((s_raceTimer - (int)s_raceTimer) * 100);
    DrawFormatString(550, 10, GetColor(255,255,255), "%02d:%02d.%02d", min, sec, msec);
}
```

---

## 10. 更新処理（UpdateGame）

```cpp
void UpdateGame(void) {

    switch (s_state) {

    case GAME_STATE_COUNTDOWN:
        // カウントダウン更新（セクション4参照）
        UpdateCountdown();
        break;

    case GAME_STATE_RUNNING:
        // タイマー更新
        s_raceTimer += 1.0f / FPS;

        // アクセル入力
        int accel = IsKeyPressed(KEY_INPUT_Z) || IsKeyPressed(KEY_INPUT_UP);
        UpdatePlayerPhysics(accel);
        UpdateCpuPhysics();
        UpdateBackground();

        // ギアチェンジ入力
        if (IsKeyTriggered(KEY_INPUT_A)) GearUp();
        if (IsKeyTriggered(KEY_INPUT_D)) GearDown();

        // ゴール判定
        if (s_distance >= GOAL_DISTANCE) {
            s_state = GAME_STATE_GOAL;
            s_goalTimer = 0.0f;
            g_gameData.lastTime = s_raceTimer;
            g_gameData.lastRank = (s_distance >= s_cpuDistance) ? 1 : 2;
        }

        // リタイア
        if (IsKeyTriggered(KEY_INPUT_ESCAPE)) {
            g_gameData.lastRank = 2;  // リタイア = 負け扱い
            ChangeScreen(UpdateResult, DrawResult);
        }
        break;

    case GAME_STATE_GOAL:
        s_goalTimer += 1.0f / FPS;
        if (s_goalTimer >= GOAL_DISPLAY_TIME) {
            ChangeScreen(UpdateResult, DrawResult);
        }
        break;
    }
}
```

| 定数 | 値（案） | 説明 |
|------|----------|------|
| `GOAL_DISPLAY_TIME` | 2.0f | ゴール演出の表示秒数 |

---

## 11. 描画処理（DrawGame）

```cpp
void DrawGame(void) {

    // ── ゲームエリア（0〜450px） ──────────────────
    // Layer 1: 背景スクロール
    DrawGraph((int)s_bgScrollX,            0, g_hBgGame, FALSE);
    DrawGraph((int)s_bgScrollX + BG_WIDTH, 0, g_hBgGame, FALSE);

    // Layer 2: CPU車（プレイヤーとの距離差を画面X座標に反映）
    float distDiff = s_cpuDistance - s_distance;   // +なら自車より前
    int cpuScreenX = PLAYER_CAR_X + (int)(distDiff * DIST_TO_PX);
    DrawGraph(cpuScreenX, CPU_CAR_Y, g_hCpuCar, TRUE);

    // Layer 3: プレイヤー車（固定位置）
    DrawGraph(PLAYER_CAR_X, PLAYER_CAR_Y, g_hPlayerCar, TRUE);

    // Layer 4: HUD（上部情報）
    DrawHUD();

    // カウントダウン表示
    if (s_state == GAME_STATE_COUNTDOWN) {
        DrawCountdown();
    }

    // ゴール表示
    if (s_state == GAME_STATE_GOAL) {
        DrawFormatString(320, 200, GetColor(255, 220, 0), "GOAL!");
    }

    // ── メーターパネル（450〜600px） ──────────────
    DrawMeterPanel();
}
```

| 定数 | 値（案） | 説明 |
|------|----------|------|
| `PLAYER_CAR_X` | 200 | 自車の固定X座標 |
| `PLAYER_CAR_Y` | 320 | 自車のY座標 |
| `CPU_CAR_Y` | 270 | CPU車のY座標（自車より少し上の列） |
| `DIST_TO_PX` | 2.0f | 距離差(m)→画面X座標(px)の変換係数 |

---

## 12. 座標・定数まとめ

| 定数名 | 暫定値 | 説明 |
|--------|--------|------|
| `FPS` | 60 | フレームレート |
| `GOAL_DISTANCE` | 400.0f | ゴール距離（m） |
| `GOAL_DISPLAY_TIME` | 2.0f | ゴール演出時間（秒） |
| `RPM_RISE_RATE` | 0.05f | RPM上昇係数 |
| `RPM_FALL_RATE` | 0.08f | RPM低下係数 |
| `SPEED_LERP` | 0.03f | 速度補間係数 |
| `BG_SCROLL_RATE` | 0.3f | 背景スクロール倍率 |
| `DIST_TO_PX` | 2.0f | 距離差→X座標変換係数 |
| `PLAYER_CAR_X` | 200 | 自車固定X座標 |
| `PLAYER_CAR_Y` | 320 | 自車Y座標 |
| `CPU_CAR_Y` | 270 | CPU車Y座標 |
| `FUEL_CONSUMPTION` | 0.0002f | 燃料消費量/frame |
| `WATER_TEMP_RISE` | 0.02f | 水温上昇量（℃/frame） |
| `WATER_TEMP_COOL` | 0.002f | 水温冷却係数 |
