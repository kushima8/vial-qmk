/*
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "quantum.h"
#include "pmw3360.h"

// Include SROM definitions.
#include "srom_0x04.c"
#include "srom_0x81.c"

#define PMW3360_SPI_MODE 3
//#define PMW3360_SPI_DIVISOR (F_CPU / PMW3360_CLOCKS)
#define PMW3360_SPI_DIVISOR 64
#define PMW3360_CLOCKS 2000000

static const pin_t pins[] = PMW3360_NCS_PINS;
#define NUMBER_OF_SENSORS (sizeof(pins) / sizeof(pin_t))

static bool motion_bursting[NUMBER_OF_SENSORS] = {0};

bool pmw3360_spi_start(int8_t index) {
    return spi_start(pins[index], false, PMW3360_SPI_MODE, PMW3360_SPI_DIVISOR);
}

uint8_t pmw3360_reg_read(int8_t index,uint8_t addr) {
    pmw3360_spi_start(index);
    spi_write(addr & 0x7f);
    wait_us(160);
    uint8_t data = spi_read();
    wait_us(1);
    spi_stop();
    wait_us(19);
    // Reset motion_bursting mode if read from a register other than motion
    // burst register.
    if (addr != pmw3360_Motion_Burst) {
        motion_bursting[index] = false;
    }
    return data;
}

void pmw3360_reg_write(int8_t index,uint8_t addr, uint8_t data) {
    pmw3360_spi_start(index);
    spi_write(addr | 0x80);
    spi_write(data);
    wait_us(35);
    spi_stop();
    wait_us(145);
}

uint8_t pmw3360_cpi_get(void) {
    return pmw3360_reg_read(0,pmw3360_Config1);
}

void pmw3360_cpi_set(uint8_t cpi) {
    if (cpi > pmw3360_MAXCPI) {
        cpi = pmw3360_MAXCPI;
    }
    for (size_t i = 0; i < NUMBER_OF_SENSORS; i++) {
        pmw3360_reg_write(i,pmw3360_Config1, cpi);
    }
}

static uint32_t pmw3360_timer      = 0;
static uint32_t pmw3360_scan_count = 0;
static uint32_t pmw3360_last_count = 0;

void pmw3360_scan_perf_task(void) {
    pmw3360_scan_count++;
    uint32_t now = timer_read32();
    if (TIMER_DIFF_32(now, pmw3360_timer) > 1000) {
#if defined(CONSOLE_ENABLE)
        dprintf("pmw3360 scan frequency: %lu\n", pmw3360_scan_count);
#endif
        pmw3360_last_count = pmw3360_scan_count;
        pmw3360_scan_count = 0;
        pmw3360_timer      = now;
    }
}

uint32_t pmw3360_scan_rate_get(void) {
    return pmw3360_last_count;
}

bool pmw3360_motion_read(int8_t index,pmw3360_motion_t *d) {
#ifdef DEBUG_PMW3360_SCAN_RATE
    pmw3360_scan_perf_task();
#endif
    uint8_t mot = pmw3360_reg_read(index,pmw3360_Motion);
    if ((mot & 0x88) != 0x80) {
        return false;
    }
    d->x = pmw3360_reg_read(index,pmw3360_Delta_X_L);
    d->x |= pmw3360_reg_read(index,pmw3360_Delta_X_H) << 8;
    d->y = pmw3360_reg_read(index,pmw3360_Delta_Y_L);
    d->y |= pmw3360_reg_read(index,pmw3360_Delta_Y_H) << 8;
    return true;
}

bool pmw3360_motion_burst(int8_t index,pmw3360_motion_t *d) {
    if (index >= NUMBER_OF_SENSORS) {
        return false;
    }
#ifdef DEBUG_PMW3360_SCAN_RATE
    pmw3360_scan_perf_task();
#endif
    // Start motion burst if motion burst mode is not started.
    if (!motion_bursting[index]) {
        pmw3360_reg_write(index,pmw3360_Motion_Burst, 0);
        motion_bursting[index] = true;
    }

    pmw3360_spi_start(index);
    spi_write(pmw3360_Motion_Burst);
    wait_us(35);
    spi_read(); // skip MOT
    spi_read(); // skip Observation
    d->x = spi_read();
    d->x |= spi_read() << 8;
    d->y = spi_read();
    d->y |= spi_read() << 8;
    spi_stop();
    // Required NCS in 500ns after motion burst.
    wait_us(1);
    return true;
}

bool pmw3360_init(int8_t index) {
    if (index >= NUMBER_OF_SENSORS) {
        return false;
    }
    spi_init();
    gpio_set_pin_output(pins[index]);
    // reboot
    pmw3360_spi_start(index);
    pmw3360_reg_write(index,pmw3360_Power_Up_Reset, 0x5a);
    wait_ms(50);
    // read five registers of motion and discard those values
    pmw3360_reg_read(index,pmw3360_Motion);
    pmw3360_reg_read(index,pmw3360_Delta_X_L);
    pmw3360_reg_read(index,pmw3360_Delta_X_H);
    pmw3360_reg_read(index,pmw3360_Delta_Y_L);
    pmw3360_reg_read(index,pmw3360_Delta_Y_H);
    // configuration
    pmw3360_reg_write(index,pmw3360_Config2, 0x00);
    // check product ID and revision ID
    uint8_t pid = pmw3360_reg_read(index,pmw3360_Product_ID);
    uint8_t rev = pmw3360_reg_read(index,pmw3360_Revision_ID);
    spi_stop();
    return pid == 0x42 && rev == 0x01;
}

uint8_t pmw3360_srom_id = 0;

void pmw3360_srom_upload(int8_t index,pmw3360_srom_t srom) {
    pmw3360_reg_write(index,pmw3360_Config2, 0x00);
    pmw3360_reg_write(index,pmw3360_SROM_Enable, 0x1d);
    wait_us(10);
    pmw3360_reg_write(index,pmw3360_SROM_Enable, 0x18);

    // SROM upload (download for PMW3360) with burst mode
    pmw3360_spi_start(index);
    spi_write(pmw3360_SROM_Load_Burst | 0x80);
    wait_us(15);
    for (size_t i = 0; i < srom.len; i++) {
        spi_write(pgm_read_byte(srom.data + i));
        wait_us(15);
    }
    spi_stop();
    wait_us(200);

    pmw3360_srom_id = pmw3360_reg_read(index,pmw3360_SROM_ID);
    pmw3360_reg_write(index,pmw3360_Config2, 0x00);
    wait_ms(10);
}
