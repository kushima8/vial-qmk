SRC += a2j/translate_ansi_to_jis.c
SRC += nicola/nicola.c
SRC += nicola/jtu.c
SRC += nicola/key_duration.c

# 16-bit timer (TIMER1) ���g�p�����^�C���A�E�g
# ��莞�Ԍo�߂���ƁAkey down ��Ԃ̂܂܂ł��L�[���o�͂����
OPT_DEFS = -DTIMEOUT_INTERRUPT

# NICOLA key down timre requires TIMER1 so Backlight LED must be turned off
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
