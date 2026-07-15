# エンジン・メーター・データ層クラス図(実装リバース)

実装元: `source/car_engine/`、`source/car_meter/`、`source/car_data.h`、`source/parts_data.h`。

実装状況メモ:

- `CarEngine` はスケルトン。`Update()` は中身が空で、引数の入力クラス(構想上の `GameKeyInput`)はコメントアウト中。`GetData()` はメンバをそのまま詰めて返すのみ
- `GaugeBase` は純粋仮想3メソッドの抽象基底として実装済み(処理実体はまだない)
- `CarMeter` は空クラスが2重定義されている: `car_meter.h` の `CarMeter`(GaugeBase を継承していない)と `carMeter.cpp` 内の `CarMeter : public GaugeBase`。統合が必要
- `TurboGauge` / `OilTempGauge` は未実装(構想のみ)
- メーターの針アニメーション等の実挙動は `debug.cpp` のデバッグメーター画面に手続き型でプロトタイプ実装されている(クラス化は未着手)

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
        <<スケルトン・2重定義あり>>
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
