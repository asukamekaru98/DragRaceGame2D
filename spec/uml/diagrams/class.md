# エンジン・メーター・データ層クラス図(実装リバース)

実装元: `source/car_engine/`、`source/car_meter/`、`source/car_data.h`、`source/parts_data.h`。

実装状況メモ:

- `CarEngine` はスケルトン。`Update()` は中身が空で、引数の入力クラス(構想上の `GameKeyInput`)はコメントアウト中。`GetData()` はメンバをそのまま詰めて返すのみ
- `GaugeBase` は純粋仮想3メソッドの抽象基底として実装済み
- `CarMeter` は `GaugeBase` を継承する**表示専用クラス**として実装済み(`car_meter.h`/`car_meter.cpp` に集約)。`Update(CarEngineData)` で受け取った値を保持し針の表示値イージングだけ行い、`Draw()` でタコdial・針・デジタル表示を描画する。RPM/速度の物理計算はクラス内に持たない。`PlayOpening()` は現状スタブ。旧2重定義(`carMeter.cpp`)と空ファイル(`car_meter.cpp` 旧・`a.cpp`)は削除済み
- デバッグメーター画面(F2)は `debug.cpp` の static `CarMeter s_meter` を使って描画する。スロットル→RPM→速度の簡易計算は同画面が手続き型で持ち、`CarEngineData` に詰めて `s_meter.Update()` に渡す
- `TurboGauge` / `OilTempGauge` は未実装(構想のみ)

```mermaid
classDiagram
    %% ===== エンジン層 =====
    class CarEngineData {
        <<struct>>
        +fRpm float
        +fSpeed float
        +iGear int
        +fBoostPressure float
        +bIsBraking bool
    }

    class CarEngine {
        <<スケルトン>>
        -fRPM_ float
        -fSpeed_ float
        -iGear_ int
        -fBoostPressure_ float
        -carData_ CarData
        -iEquippedParts_ int[]
        +Update() void
        +GetData() CarEngineData
    }

    CarEngine ..> CarData       : uses
    CarEngine ..> CarEngineData : outputs

    %% ===== メーター層 =====
    class GaugeBase {
        <<abstract>>
        +Update(data CarEngineData)* void
        +Draw()* void
        +PlayOpening()* void
    }

    class CarMeter {
        -fNeedleRpm_ float
        -fRpm_ float
        -fSpeed_ float
        -iGear_ int
        +Update(data CarEngineData) void
        +Draw() void
        +PlayOpening() void
        +Reset() void
        -DialPoint(fRadius float, fFrac float, iOutX int*, iOutY int*) void
    }

    class TurboGauge {
        <<未実装・構想>>
    }

    class OilTempGauge {
        <<未実装・構想>>
    }

    GaugeBase <|-- CarMeter
    GaugeBase <|-- TurboGauge
    GaugeBase <|-- OilTempGauge
    GaugeBase ..> CarEngineData : reads

    %% ===== データ層 =====
    class CarData {
        <<struct>>
        +name char*
        +maxSpeed float
        +acceleration float
        +gearCount int
        +price int
    }

    class PartsData {
        <<struct>>
        +pName char*
        +eCategory PARTS_CATEGORY
        +eGaugeType GAUGE_TYPE
        +iPrice int
        +fMaxSpeedBonus float
        +fAccelBonus float
    }

    class GAUGE_TYPE {
        <<enumeration>>
        GAUGE_NONE
        GAUGE_TURBO
        GAUGE_OIL_TEMP
    }

    class PARTS_CATEGORY {
        <<enumeration>>
        PARTS_CAT_ENGINE
        PARTS_CAT_TIRE
        PARTS_CAT_BODY
        PARTS_CAT_EXTERIOR
        PARTS_CAT_COUNT
    }

    PartsData --> GAUGE_TYPE
    PartsData --> PARTS_CATEGORY
```

マスターデータ(グローバル定数):

- `CAR_TABLE[]` / `CAR_TABLE_COUNT` — 車両マスタ(`car_data.cpp`)
- `PARTS_TABLE[]` / `PARTS_TABLE_COUNT` — パーツマスタ(`parts_data.cpp`)
