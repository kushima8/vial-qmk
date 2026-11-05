# gbc24 (RP2040版 / Vial)

24キー + ロータリーエンコーダ + ポインティングデバイス(ジョイスティック / PMW3360トラックボール 交換式)のマクロパッド。
ATmega32u4 版 (`keyboards/gbc/gbc24`) を RP2040 (KB2040 互換ピン配置) に移植し、Vial に対応させたもの。

* Keyboard Maintainer: [kushima8](https://github.com/kushima8)
* MCU: RP2040 / Bootloader: rp2040 (UF2)
* 対応マイコン: Adafruit KB2040 / RP2040 Kee Boar Driver (KB2040 互換ピン配置)
* ベースファームウェア: [kushima8/vial-qmk](https://github.com/kushima8/vial-qmk) (`kb2040` ブランチ)
* キーマップ: `vial` (`keymaps/vial/`、Vial の定義は `vial.json`)

## Vial 固有の設定

* `VIAL_ENABLE = yes` (`rules.mk`)。追加キーコードは `vial.json` の `customKeycodes` で定義している(QMK版の `gbc24.json` と同じ名前・同じ順序)。
* Vial のアンロック操作: マトリクス (0,0) と (0,1) のキー(左上から1番目と2番目のキー)を同時に押す(`VIAL_UNLOCK_COMBO_ROWS` / `VIAL_UNLOCK_COMBO_COLS`)。
* `QMK_SETTINGS = yes` のため、Vial の「QMK Settings」で Tapping Term などを変更できる。トラックボール構成の自動マウスレイヤーで、キー入力後にレイヤーが切り替わらない時間 (`AUTO_MOUSE_DELAY`) は、この Tapping Term の値に連動する(初期値 200ms)。
* `COMBO_ENABLE = yes` (Vial のコンボ機能を使える)。

## ピン割り当て (AVR → RP2040 / KB2040換算)

Adafruit KB2040 のピン対応表に従って変換している。

| 機能 | AVR (Pro Micro) | RP2040 (KB2040) |
|---|---|---|
| ROW | D4, C6, D7, E6, B4 | GP4, GP5, GP6, GP7, GP8 |
| COL | B5, F4, F5, B6 | GP9, GP29, GP28, GP10 |
| エンコーダ A/B | D3 / D2 | GP0 / GP1 |
| DIPスイッチ (信号/GND) | B2 / B1 | GP19 / GP18 |
| ジョイスティック X/Y | F6 / F7 (ADC) | GP27 / GP26 (ADC1/ADC0) |
| PMW3360 SCK/MOSI/MISO/NCS | B1/B2/B3/F7 | GP18/GP19/GP20/GP26 (SPI0) |

RP2040 の ADC は 12bit だが、QMK が 10bit に換算して返すため、`config.h` のジョイスティック可動域 (`JOY_X_SPAN_*` / `JOY_Y_SPAN_*`) は AVR 版と同じ値を使う。

`EXTRAKEY_ENABLE` / `NKRO_ENABLE` は AVR 版と同じく無効(`no`)のままにしている。

## ポインティング入力の切り替え (コンパイル時)

AVR 版と同じく `rules.mk` の【A】ジョイスティック /【B】トラックボール ブロックのコメントアウトで切り替える。

* **ジョイスティックを使う**: 【A】を有効化し、【B】を全てコメントアウト(デフォルト状態)
* **トラックボールを使う**: 【A】を全てコメントアウトし、【B】の `#` を外す

トラックボール構成の例:

    # 【A】アナログジョイスティック構成 (デフォルト)
    #JOYSTICK_ENABLE = yes

    # 【B】PMW3360 トラックボール構成 (lib/gbc 経由)
    JOYSTICK_ENABLE = no
    POINTING_DEVICE_ENABLE = yes
    POINTING_DEVICE_DRIVER = custom
    SRC += drivers/pmw3360/pmw3360.c
    SRC += lib/gbc/gbc.c
    QUANTUM_LIB_SRC += spi_master.c
    DIP_SWITCH_ENABLE = no
    OPT_DEFS += -DGBC_CORE_ENABLE

ビルド:

    make gbc_rp2040/gbc24:vial

### trackball ビルドでの注意点

* ハードウェア SPI0 が GP18(SCK)/GP19(MOSI) を使うため、**DIP スイッチ (GP19, GND=GP18) は併用不可**(【B】ブロックで無効化)。
* `JOYSTICK_ENABLE = no` になるため、JOY_DIGI / JOY_MOUS / JOY_ANLG / JOY_SPD_UP / JOY_SPD_DN / JOY_DZ_UP / JOY_DZ_DN / JOY_DZ_RST キーコードは無効(キーマップは共通のままコンパイル可能)。
* センサーへのアクセスとトラックボール用キーコード (REC_RST など) の処理は `lib/gbc/gbc.c` が行う。gbc.c が custom pointing device driver を提供するため、`drivers/pmw3360/pmw3360_pointing_device_driver.c` は使わない。
* `GBC_CORE_ENABLE` は gbc24.c 側の軸補正を無効化するためのフラグ (gbc.c 側で同じ向きの補正を行うため、二重に補正しないよう必ず定義する)。
* RP2040 はフラッシュに余裕があるため LTO は不要。

## Bootloader

* **Physical reset button**: 基板上面のリセットボタンを素早く2回押す(`RP2040_BOOTLOADER_DOUBLE_TAP_RESET` 有効、受付時間 200ms)。または、マイコンの BOOT ボタンを押しながら USB 接続する。
* **Keycode in layout**: `QK_BOOT` を割り当てたキーを押す。Vial はロック中に `QK_BOOT` をキーへ割り当てられないため、先にアンロック操作をしてから割り当てる。

ブートローダーに入ると `RPI-RP2` ドライブとして認識されるので、UF2 ファイルをそのドライブへコピーして書き込む。
