# MCU name
MCU = atmega32u4

# Bootloader selection
BOOTLOADER = atmel-dfu

# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = yes      # Enable Bootmagic Lite
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = no         # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
# Do not enable SLEEP_LED_ENABLE. it uses the same timer as BACKLIGHT_ENABLE
SLEEP_LED_ENABLE = no       # Breathing sleep LED during USB suspend
# if this doesn't work, see here: https://github.com/tmk/tmk_keyboard/wiki/FAQ#nkro-doesnt-work
NKRO_ENABLE = no            # USB Nkey Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = yes       # Enable keyboard RGB underglow
AUDIO_ENABLE = no           # Audio output
IOS_DEVICE_ENABLE = yes     # connect to IOS device (iPad,iPhone)
ENCODER_ENABLE = yes        # Add rotary encoder support
ENCODER_MAP_ENABLE = yes
OLED_ENABLE = yes
SPLIT_KEYBOARD = no
VIA_ENABLE = yes
CUSTOM_MATRIX = lite
SRC += lib/duplexmatrix/duplexmatrix.c
LTO_ENABLE = yes# MCU name
MCU = RP2040

# Bootloader selection
BOOTLOADER = rp2040

# Build Options
BOOTMAGIC_ENABLE = no       # Enable Bootmagic Lite
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = no         # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
NKRO_ENABLE = yes           # Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
AUDIO_ENABLE = no           # Audio output

# Duplex matrix.
CUSTOM_MATRIX = lite
SRC += lib/duplexmatrix/duplexmatrix.c

SPLIT_KEYBOARD = no

# This is unnecessary for processing KC_MS_BTN*.
MOUSEKEY_ENABLE = yes

# Enabled only one of RGBLIGHT and RGB_MATRIX if necessary.
RGBLIGHT_ENABLE = yes       # Enable RGBLIGHT
RGB_MATRIX_ENABLE = no      # Enable RGB_MATRIX (not work yet)
RGB_MATRIX_DRIVER = ws2812

# Do not enable SLEEP_LED_ENABLE. it uses the same timer as BACKLIGHT_ENABLE
SLEEP_LED_ENABLE = no       # Breathing sleep LED during USB suspend

# To support OLED
OLED_ENABLE = yes               # Please Enable this in each keymaps.

# Disable other features to squeeze firmware size
SPACE_CADET_ENABLE = yes
MAGIC_ENABLE = yes

VIA_ENABLE = yes

GRAVE_ESC_ENABLE = yes

ENCODER_ENABLE = yes

ENCODER_MAP_ENABLE = yes

WS2812_DRIVER = vendor
SERIAL_DRIVER = vendor

VIAL_ENABLE = yes
QMK_SETTINGS = yes 

TAP_DANCE_ENABLE = yes
COMBO_ENABLE = yes
KEY_OVERRIDE_ENABLE = yes