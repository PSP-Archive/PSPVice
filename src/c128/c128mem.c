/*
 * c128mem.c -- Memory handling for the C128 emulator.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *
 * Based on the original work in VICE 0.11.0 by
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c128-resources.h"
#include "c128.h"
#include "c128mem.h"
#include "c128meminit.h"
#include "c128memlimit.h"
#include "c128memrom.h"
#include "c128mmu.h"
#include "c64cart.h"
#include "c64cia.h"
#include "c64io.h"
#include "c64meminit.h"
#include "c64memrom.h"
#include "c64pla.h"
#include "c64tpi.h"
#include "functionrom.h"
#include "keyboard.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "monitor.h"
#include "ram.h"
#include "reu.h"
#include "georam.h"
#include "sid.h"
#include "types.h"
#include "vdc-mem.h"
#include "vdc.h"
#include "vicii-mem.h"
#include "vicii-phi1.h"
#include "vicii.h"
#include "z80mem.h"

#ifdef HAVE_TFE
#include "tfe.h"
#endif

/* #define DEBUG_MMU */

#ifdef DEBUG_MMU
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x)
#endif

/* ------------------------------------------------------------------------- */

/* Number of possible video banks (16K each).  */
#define NUM_VBANKS      4

/* The C128 memory.  */
BYTE mem_ram[C128_RAM_SIZE];
BYTE mem_chargen_rom[C128_CHARGEN_ROM_SIZE];

/* Internal color memory.  */
static BYTE mem_color_ram[0x800];
BYTE *mem_color_ram_cpu, *mem_color_ram_vicii;

/* Pointer to the chargen ROM.  */
BYTE *mem_chargen_rom_ptr;

/* Currently selected RAM bank.  */
BYTE *ram_bank;

/* Shared memory.  */
static WORD top_shared_limit, bottom_shared_limit;

/* Pointers to pages 0 and 1 (which can be physically placed anywhere).  */
BYTE *mem_page_zero, *mem_page_one;

/* Adjust this pointer when the MMU changes banks.  */
static BYTE **bank_base;
static int *bank_limit = NULL;
unsigned int mem_old_reg_pc;

/* Pointers to the currently used memory read and write tables.  */
read_func_ptr_t *_mem_read_tab_ptr;
store_func_ptr_t *_mem_write_tab_ptr;
BYTE **_mem_read_base_tab_ptr;
int *mem_read_limit_tab_ptr;

#define NUM_CONFIGS 256

/* Memory read and write tables.  */
static store_func_ptr_t mem_write_tab[NUM_VBANKS][NUM_CONFIGS][0x101];
static read_func_ptr_t mem_read_tab[NUM_CONFIGS][0x101];
static BYTE *mem_read_base_tab[NUM_CONFIGS][0x101];
static int mem_read_limit_tab[NUM_CONFIGS][0x101];

static store_func_ptr_t mem_write_tab_watch[0x101];
static read_func_ptr_t mem_read_tab_watch[0x101];

/* Current video bank (0, 1, 2 or 3 in the first bank,
   4, 5, 6 or 7 in the second bank).  */
static int vbank = 0;

/* Tape sense status: 1 = some button pressed, 0 = no buttons pressed.  */
static int tape_sense = 0;

/* Current memory configuration.  */
static int mem_config;

/* Current machine type.  */
static unsigned int mem_machine_type;

/* Logging goes here.  */
static log_t c128_mem_log = LOG_DEFAULT;

/* Status of the CAPS key (ASCII/DIN).  */
static int caps_sense = 1;

/* ------------------------------------------------------------------------- */

static BYTE REGPARM1 watch_read(WORD addr)
{
    monitor_watch_push_load_addr(addr, e_comp_space);
    return mem_read_tab[mem_config][addr >> 8](addr);
}

static void REGPARM2 watch_store(WORD addr, BYTE value)
{
    monitor_watch_push_store_addr(addr, e_comp_space);
    mem_write_tab[vbank][mem_config][addr >> 8](addr, value);
}

void mem_toggle_watchpoints(int flag, void *context)
{
    if (flag) {
        _mem_read_tab_ptr = mem_read_tab_watch;
        _mem_write_tab_ptr = mem_write_tab_watch;
    } else {
        _mem_read_tab_ptr = mem_read_tab[mem_config];
        _mem_write_tab_ptr = mem_write_tab[vbank][mem_config];
    }
}

/* ------------------------------------------------------------------------- */

static void mem_update_chargen(unsigned int chargen_high)
{
    BYTE *old_chargen_rom_ptr;

    old_chargen_rom_ptr = mem_chargen_rom_ptr;

    if (chargen_high) {
        mem_chargen_rom_ptr = mem_chargen_rom;
    } else {
        mem_chargen_rom_ptr = &mem_chargen_rom[0x1000];
    }
    if (old_chargen_rom_ptr != mem_chargen_rom_ptr) {
        machine_update_memory_ptrs();
    }
}

void mem_update_config(int config)
{
    mem_config = config;

    if (any_watchpoints(e_comp_space)) {
        _mem_read_tab_ptr = mem_read_tab_watch;
        _mem_write_tab_ptr = mem_write_tab_watch;
    } else {
        _mem_read_tab_ptr = mem_read_tab[mem_config];
        _mem_write_tab_ptr = mem_write_tab[vbank][mem_config];
    }

    _mem_read_base_tab_ptr = mem_read_base_tab[config];
    mem_read_limit_tab_ptr = mem_read_limit_tab[config];

    if (bank_limit != NULL) {
        *bank_base = _mem_read_base_tab_ptr[mem_old_reg_pc >> 8];
        if (*bank_base != 0)
            *bank_base = _mem_read_base_tab_ptr[mem_old_reg_pc >> 8]
                         - (mem_old_reg_pc & 0xff00);
        *bank_limit = mem_read_limit_tab_ptr[mem_old_reg_pc >> 8];
    }

    if (config >= 0x80) {
        if (mem_machine_type == C128_MACHINE_INT)
            mem_update_chargen(0);
        mem_color_ram_cpu = mem_color_ram;
        mem_color_ram_vicii = mem_color_ram;
        vicii_set_chargen_addr_options(0x7000, 0x1000);
    } else {
        if (mem_machine_type == C128_MACHINE_INT)
            mem_update_chargen(1);
        if (pport.data_read & 1)
            mem_color_ram_cpu = mem_color_ram;
        else
            mem_color_ram_cpu = &mem_color_ram[0x400];
        if (pport.data_read & 2)
            mem_color_ram_vicii = mem_color_ram;
        else
            mem_color_ram_vicii = &mem_color_ram[0x400];
        if (pport.data_read & 4)
            vicii_set_chargen_addr_options(0xffff, 0xffff);
        else
            vicii_set_chargen_addr_options(0x3000, 0x1000);
    }
}

void mem_set_machine_type(unsigned type)
{
    mem_machine_type = type;
}

void mem_set_bank_pointer(BYTE **base, int *limit)
{
    bank_base = base;
    bank_limit = limit;
}

/* Change the current video bank.  Call this routine only when the vbank
   has really changed.  */
void mem_set_vbank(int new_vbank)
{
    vbank = (vbank & ~3) | new_vbank;
    vicii_set_vbank(new_vbank & 3);
}

/*
static void mem_set_vbank_extended(int new_ext_vbank)
{
    vbank = (vbank & 3) | new_ext_vbank << 2;
}
*/

void mem_set_ram_config(BYTE value)
{
    unsigned int shared_size;

    /* XXX: We only support 128K here.  */
    vicii_set_ram_base(mem_ram + ((value & 0x40) << 10));
    /*mem_set_vbank_extended((value & 0x40) >> 6);*/

    DEBUG_PRINT(("MMU: Store RCR = $%02x\n", value));
    DEBUG_PRINT(("MMU: VIC-II base at $%05X\n", ((value & 0xc0) << 2)));

    if ((value & 0x3) == 0)
        shared_size = 1024;
    else
        shared_size = 0x1000 << ((value & 0x3) - 1);

    /* Share high memory?  */
    if (value & 0x8) {
        top_shared_limit = 0xffff - shared_size;
        DEBUG_PRINT(("MMU: Sharing high RAM from $%04X\n",
                    top_shared_limit + 1));
    } else {
        top_shared_limit = 0xffff;
        DEBUG_PRINT(("MMU: No high shared RAM\n"));
    }

    /* Share low memory?  */
    if (value & 0x4) {
        bottom_shared_limit = shared_size;
        DEBUG_PRINT(("MMU: Sharing low RAM up to $%04X\n",
                    bottom_shared_limit - 1));
    } else {
        bottom_shared_limit = 0;
        DEBUG_PRINT(("MMU: No low shared RAM\n"));
    }
}

/* ------------------------------------------------------------------------- */

void mem_pla_config_changed(void)
{
    c64pla_config_changed(tape_sense, caps_sense, 0x57);

    mmu_set_config64(((~pport.dir | pport.data) & 0x7)
                     | (export.exrom << 3) | (export.game << 4));

    if (mem_machine_type != C128_MACHINE_INT)
        mem_update_chargen(pport.data_read & 0x40);
}

static void mem_toggle_caps_key(void)
{
    caps_sense = (caps_sense) ? 0 : 1;
    mem_pla_config_changed();
    log_message(c128_mem_log, "CAPS key (ASCII/DIN) %s.",
                (caps_sense) ? "released" : "pressed");
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 zero_read(WORD addr)
{
    addr &= 0xff;

    switch ((BYTE)addr) {
      case 0:
        return pport.dir_read;
      case 1:
        return pport.data_read;
    }

    return mem_page_zero[addr];
}

void REGPARM2 zero_store(WORD addr, BYTE value)
{
    addr &= 0xff;

    switch ((BYTE)addr) {
      case 0:
        if (/*vbank ==*/ 0) {
            vicii_mem_vbank_store((WORD)0, vicii_read_phi1_lowlevel());
        } else {
            mem_page_zero[0] = vicii_read_phi1_lowlevel();
            machine_handle_pending_alarms(maincpu_rmw_flag + 1);
        }
        if (pport.dir != value) {
            pport.dir = value;
            mem_pla_config_changed();
        }
        break;
      case 1:
        if (/*vbank ==*/ 0) {
            vicii_mem_vbank_store((WORD)1, vicii_read_phi1_lowlevel());
        } else {
            mem_page_zero[1] = vicii_read_phi1_lowlevel();
            machine_handle_pending_alarms(maincpu_rmw_flag + 1);
        }
        if (pport.data != value) {
            pport.data = value;
            mem_pla_config_changed();
        }
        break;
      default:
        if (/*vbank ==*/ 0)
            vicii_mem_vbank_store(addr, value);
        else
            mem_page_zero[addr] = value;
    }
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 one_read(WORD addr)
{
    return mem_page_one[addr - 0x100];
}

void REGPARM2 one_store(WORD addr, BYTE value)
{
    mem_page_one[addr - 0x100] = value;
}

/* ------------------------------------------------------------------------- */

/* External memory access functions.  */

BYTE REGPARM1 chargen_read(WORD addr)
{
    return mem_chargen_rom_ptr[addr & 0x0fff];
}

void REGPARM2 chargen_store(WORD addr, BYTE value)
{
    mem_chargen_rom_ptr[addr & 0x0fff] = value;
}

/* ------------------------------------------------------------------------- */

/* Generic memory access.  */

void REGPARM2 mem_store(WORD addr, BYTE value)
{
    _mem_write_tab_ptr[addr >> 8](addr, value);
}

BYTE REGPARM1 mem_read(WORD addr)
{
    return _mem_read_tab_ptr[addr >> 8](addr);
}

void REGPARM2 mem_store_without_ultimax(WORD addr, BYTE value)
{
    store_func_ptr_t *write_tab_ptr;

    write_tab_ptr = mem_write_tab[vbank][mem_config & 7];

    write_tab_ptr[addr >> 8](addr, value);
}

BYTE REGPARM1 mem_read_without_ultimax(WORD addr)
{
    read_func_ptr_t *read_tab_ptr;

    read_tab_ptr = mem_read_tab[mem_config & 7];

    return read_tab_ptr[addr >> 8](addr);
}

/* ------------------------------------------------------------------------- */

/* CPU Memory interface.  */

/* The MMU can basically do the following:

   - select one of the two (four) memory banks as the standard
   (non-shared) memory;

   - turn ROM and I/O on and off;

   - enable/disable top/bottom shared RAM (from 1K to 16K, so bottom
   shared RAM cannot go beyond $3FFF and top shared RAM cannot go
   under $C000);

   - move pages 0 and 1 to any physical address.  */

#define READ_TOP_SHARED(addr)                     \
    ((addr) > top_shared_limit ? mem_ram[(addr)]  \
                               : ram_bank[(addr)])

#define STORE_TOP_SHARED(addr, value)                        \
    ((addr) > top_shared_limit ? (mem_ram[(addr)] = (value)) \
                               : (ram_bank[(addr)] = (value)))

#define READ_BOTTOM_SHARED(addr)                     \
    ((addr) < bottom_shared_limit ? mem_ram[(addr)]  \
                                  : ram_bank[(addr)])

#define STORE_BOTTOM_SHARED(addr, value)                         \
    ((addr) < bottom_shared_limit ? (mem_ram[(addr)] = (value))  \
                                  : (ram_bank[(addr)] = (value)))

/* $0200 - $3FFF: RAM (normal or shared).  */
BYTE REGPARM1 lo_read(WORD addr)
{
    return READ_BOTTOM_SHARED(addr);
}

void REGPARM2 lo_store(WORD addr, BYTE value)
{
    STORE_BOTTOM_SHARED(addr, value);
}

BYTE REGPARM1 ram_read(WORD addr)
{
    return ram_bank[addr];
}

void REGPARM2 ram_store(WORD addr, BYTE value)
{
    ram_bank[addr] = value;
}

void REGPARM2 ram_hi_store(WORD addr, BYTE value)
{
    if (vbank == 3)
        vicii_mem_vbank_3fxx_store(addr, value);
    else
        ram_bank[addr] = value;

    if (addr == 0xff00)
        reu_dma(-1);
}

/* $4000 - $7FFF: RAM or low BASIC ROM.  */
BYTE REGPARM1 basic_lo_read(WORD addr)
{
    return c128memrom_basic_rom[addr - 0x4000];
}

void REGPARM2 basic_lo_store(WORD addr, BYTE value)
{
    ram_bank[addr] = value;
}


/* $8000 - $BFFF: RAM or high BASIC ROM.  */
BYTE REGPARM1 basic_hi_read(WORD addr)
{
    return c128memrom_basic_rom[addr - 0x4000];
}

void REGPARM2 basic_hi_store(WORD addr, BYTE value)
{
    ram_bank[addr] = value;
}


/* $C000 - $CFFF: RAM (normal or shared) or Editor ROM.  */
BYTE REGPARM1 editor_read(WORD addr)
{
    return c128memrom_basic_rom[addr - 0x4000];
}

void REGPARM2 editor_store(WORD addr, BYTE value)
{
    STORE_TOP_SHARED(addr, value);
}

static BYTE REGPARM1 d5xx_read(WORD addr)
{
    return vicii_read_phi1();
}

static void REGPARM2 d5xx_store(WORD addr, BYTE value)
{
}

BYTE REGPARM1 d7xx_read(WORD addr)
{
    return vicii_read_phi1();
}

void REGPARM2 d7xx_store(WORD addr, BYTE value)
{
}

/* $E000 - $FFFF: RAM or Kernal.  */
BYTE REGPARM1 hi_read(WORD addr)
{
    return c128memrom_kernal_rom[addr & 0x1fff];
}

void REGPARM2 hi_store(WORD addr, BYTE value)
{
    STORE_TOP_SHARED(addr, value);
}

BYTE REGPARM1 top_shared_read(WORD addr)
{
    return READ_TOP_SHARED(addr);
}

void REGPARM2 top_shared_store(WORD addr, BYTE value)
{
    STORE_TOP_SHARED(addr, value);
}

/* ------------------------------------------------------------------------- */

void REGPARM2 colorram_store(WORD addr, BYTE value)
{
    mem_color_ram_cpu[addr & 0x3ff] = value & 0xf;
}

BYTE REGPARM1 colorram_read(WORD addr)
{
    return mem_color_ram_cpu[addr & 0x3ff] | (vicii_read_phi1() & 0xf0);
}

/* ------------------------------------------------------------------------- */

void mem_set_write_hook(int config, int page, store_func_t *f)
{
    int i;

    for (i = 0; i < NUM_VBANKS; i++) {
        mem_write_tab[i][config][page] = f;
    }
}

void mem_read_tab_set(unsigned int base, unsigned int index,
                      read_func_ptr_t read_func)
{
    mem_read_tab[base][index] = read_func;
}

void mem_read_base_set(unsigned int base, unsigned int index, BYTE *mem_ptr)
{
    mem_read_base_tab[base][index] = mem_ptr;
}

#ifdef _MSC_VER
#pragma optimize("",off)
#endif

void mem_initialize_memory(void)
{
    int i, j, k;

    mem_chargen_rom_ptr = mem_chargen_rom;
    mem_color_ram_cpu = mem_color_ram;
    mem_color_ram_vicii = mem_color_ram;

    mem_limit_init(mem_read_limit_tab);

    for (i = 0; i <= 0x100; i++) {
        mem_read_tab_watch[i] = watch_read;
        mem_write_tab_watch[i] = watch_store;
    }

    c128meminit();

    /* C64 mode configuration.  */

    for (i = 0; i < 32; i++) {
        mem_set_write_hook(128 + i, 0, zero_store);
        mem_read_tab[128 + i][0] = zero_read;
        mem_read_base_tab[128 + i][0] = mem_ram;
        mem_set_write_hook(128 + i, 1, one_store);
        mem_read_tab[128 + i][1] = one_read;
        mem_read_base_tab[128 + i][1] = mem_ram;
        for (j = 2; j <= 0xfe; j++) {
            mem_read_tab[128 + i][j] = ram_read;
            mem_read_base_tab[128 + i][j] = mem_ram + (j << 8);
            for (k = 0; k < NUM_VBANKS; k++) {
                if ((j & 0xc0) == (k << 6)) {
                    switch (j & 0x3f) {
                      case 0x39:
                        mem_write_tab[k][128+i][j] = vicii_mem_vbank_39xx_store;
                        break;
                      case 0x3f:
                        mem_write_tab[k][128+i][j] = vicii_mem_vbank_3fxx_store;
                        break;
                      default:
                        mem_write_tab[k][128+i][j] = vicii_mem_vbank_store;
                    }
                } else {
                    mem_write_tab[k][128+i][j] = ram_store;
                }
            }
        }
        mem_read_tab[128 + i][0xff] = ram_read;
        mem_read_base_tab[128 + i][0xff] = mem_ram + 0xff00;

        /* vbank access is handled within `ram_hi_store()'.  */
        mem_set_write_hook(128 + i, 0xff, ram_hi_store);
    }

    /* Setup character generator ROM at $D000-$DFFF (memory configs 1, 2,
       3, 9, 10, 11, 25, 26, 27).  */
    for (i = 0xd0; i <= 0xdf; i++) {
        mem_read_tab[128 + 1][i] = chargen_read;
        mem_read_tab[128 + 2][i] = chargen_read;
        mem_read_tab[128 + 3][i] = chargen_read;
        mem_read_tab[128 + 9][i] = chargen_read;
        mem_read_tab[128 + 10][i] = chargen_read;
        mem_read_tab[128 + 11][i] = chargen_read;
        mem_read_tab[128 + 25][i] = chargen_read;
        mem_read_tab[128 + 26][i] = chargen_read;
        mem_read_tab[128 + 27][i] = chargen_read;
        mem_read_base_tab[128 + 1][i] = NULL;
        mem_read_base_tab[128 + 2][i] = NULL;
        mem_read_base_tab[128 + 3][i] = NULL;
        mem_read_base_tab[128 + 9][i] = NULL;
        mem_read_base_tab[128 + 10][i] = NULL;
        mem_read_base_tab[128 + 11][i] = NULL;
        mem_read_base_tab[128 + 25][i] = NULL;
        mem_read_base_tab[128 + 26][i] = NULL;
        mem_read_base_tab[128 + 27][i] = NULL;
    }

    c64meminit(128);

    /* Setup C128 specific I/O at $D000-$DFFF.  */
    for (j = 0; j < 32; j++) {
        if (c64meminit_io_config[j]) {
            mem_read_tab[128 + j][0xd0] = c128_vicii_read;
            mem_set_write_hook(128 + j, 0xd0, c128_vicii_store);
            mem_read_tab[128 + j][0xd1] = c128_vicii_read;
            mem_set_write_hook(128 + j, 0xd1, c128_vicii_store);
            mem_read_tab[128 + j][0xd2] = c128_vicii_read;
            mem_set_write_hook(128 + j, 0xd2, c128_vicii_store);
            mem_read_tab[128 + j][0xd3] = c128_vicii_read;
            mem_set_write_hook(128 + j, 0xd3, c128_vicii_store);
            mem_read_tab[128 + j][0xd4] = c128_sid_read;
            mem_set_write_hook(128 + j, 0xd4, c128_sid_store);
            mem_read_tab[128 + j][0xd5] = c128_d5xx_read;
            mem_set_write_hook(128 + j, 0xd5, c128_d5xx_store);
            mem_read_tab[128 + j][0xd6] = c128_vdc_read;
            mem_set_write_hook(128 + j, 0xd6, c128_vdc_store);
            mem_read_tab[128 + j][0xd7] = c128_d7xx_read;
            mem_set_write_hook(128 + j, 0xd7, c128_d7xx_store);
            mem_read_tab[128 + j][0xd8] = c128_colorram_read;
            mem_set_write_hook(128 + j, 0xd8, c128_colorram_store);
            mem_read_tab[128 + j][0xd9] = c128_colorram_read;
            mem_set_write_hook(128 + j, 0xd9, c128_colorram_store);
            mem_read_tab[128 + j][0xda] = c128_colorram_read;
            mem_set_write_hook(128 + j, 0xda, c128_colorram_store);
            mem_read_tab[128 + j][0xdb] = c128_colorram_read;
            mem_set_write_hook(128 + j, 0xdb, c128_colorram_store);
            mem_read_tab[128 + j][0xdc] = c128_cia1_read;
            mem_set_write_hook(128 + j, 0xdc, c128_cia1_store);
            mem_read_tab[128 + j][0xdd] = c128_cia2_read;
            mem_set_write_hook(128 + j, 0xdd, c128_cia2_store);
            mem_read_tab[128 + j][0xde] = c128_c64io1_read;
            mem_set_write_hook(128 + j, 0xde, c128_c64io1_store);
            mem_read_tab[128 + j][0xdf] = c128_c64io2_read;
            mem_set_write_hook(128 + j, 0xdf, c128_c64io2_store);
        }
    }

    for (i = 128; i < 128 + 32; i++) {
        mem_read_tab[i][0x100] = mem_read_tab[i][0];
        for (j = 0; j < NUM_VBANKS; j++) {
            mem_write_tab[j][i][0x100] = mem_write_tab[j][i][0];
        }
        mem_read_base_tab[i][0x100] = mem_read_base_tab[i][0];
    }

    vicii_set_chargen_addr_options(0xffff, 0xffff);

    mmu_reset();

    keyboard_register_caps_key(mem_toggle_caps_key);

    top_shared_limit = 0xffff;
    bottom_shared_limit = 0x0000;
    ram_bank = mem_ram;
    mem_page_zero = mem_ram;
    mem_page_one = mem_ram + 0x100;

    _mem_read_tab_ptr = mem_read_tab[3];
    _mem_write_tab_ptr = mem_write_tab[vbank][3];
    _mem_read_base_tab_ptr = mem_read_base_tab[3];
    mem_read_limit_tab_ptr = mem_read_limit_tab[3];

    c64pla_pport_reset();

    cartridge_init_config();
}

#ifdef _MSC_VER
#pragma optimize("",on)
#endif

/* ------------------------------------------------------------------------- */

/* Initialize RAM for power-up.  */
void mem_powerup(void)
{
    ram_init(mem_ram, C128_RAM_SIZE);
}

/* ------------------------------------------------------------------------- */

/* Set the tape sense status.  */
void mem_set_tape_sense(int sense)
{
    tape_sense = sense;
    mem_pla_config_changed();
}

/* ------------------------------------------------------------------------- */

void mem_get_basic_text(WORD *start, WORD *end)
{
    if (start != NULL)
        *start = mem_ram[0x2b] | (mem_ram[0x2c] << 8);
    if (end != NULL)
        *end = mem_ram[0x1210] | (mem_ram[0x1211] << 8);
}

void mem_set_basic_text(WORD start, WORD end)
{
    mem_ram[0x2b] = mem_ram[0xac] = start & 0xff;
    mem_ram[0x2c] = mem_ram[0xad] = start >> 8;
    mem_ram[0x1210] = end & 0xff;
    mem_ram[0x1211] = end >> 8;
}

/* ------------------------------------------------------------------------- */

int mem_rom_trap_allowed(WORD addr)
{
    if (addr >= 0xe000) {
        if (mem_config >= 128) {
            switch (mem_config - 128) {
              case 2:
              case 3:
              case 6:
              case 7:
              case 10:
              case 11:
              case 14:
              case 15:
              case 26:
              case 27:
              case 30:
              case 31:
                return 1;
              default:
                return 0;
            }
        }
        return 1;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */

/* Banked memory access functions for the monitor */

/* FIXME: peek, cartridge support */

void store_bank_io(WORD addr, BYTE byte)
{
    switch (addr & 0xff00) {
      case 0xd000:
      case 0xd100:
      case 0xd200:
      case 0xd300:
        vicii_store(addr, byte);
        break;
      case 0xd400:
        sid_store(addr, byte);
        break;
      case 0xd500:
        mmu_store(addr, byte);
        break;
      case 0xd600:
        vdc_store(addr, byte);
        break;
      case 0xd700:
        d7xx_store(addr, byte);
        break;
      case 0xd800:
      case 0xd900:
      case 0xda00:
      case 0xdb00:
        colorram_store(addr, byte);
        break;
      case 0xdc00:
        cia1_store(addr, byte);
        break;
      case 0xdd00:
        cia2_store(addr, byte);
        break;
      case 0xde00:
        c64io1_store(addr, byte);
        break;
      case 0xdf00:
        c64io2_store(addr, byte);
        break;
    }
    return;
}

BYTE read_bank_io(WORD addr)
{
    switch (addr & 0xff00) {
      case 0xd000:
      case 0xd100:
      case 0xd200:
      case 0xd300:
        return vicii_read(addr);
      case 0xd400:
        return sid_read(addr);
      case 0xd500:
        return mmu_read(addr);
      case 0xd600:
        return vdc_read(addr);
      case 0xd700:
        return d7xx_read(addr);
      case 0xd800:
      case 0xd900:
      case 0xda00:
      case 0xdb00:
        return colorram_read(addr);
      case 0xdc00:
        return cia1_read(addr);
      case 0xdd00:
        return cia2_read(addr);
      case 0xde00:
        return c64io1_read(addr);
      case 0xdf00:
        return c64io2_read(addr);
    }
    return 0xff;
}

static BYTE peek_bank_io(WORD addr)
{
    switch (addr & 0xff00) {
      case 0xd000:
      case 0xd100:
      case 0xd200:
      case 0xd300:
        return vicii_peek(addr);
      case 0xd400:
        return sid_read(addr); /* FIXME */
      case 0xd500:
        return mmu_read(addr);
      case 0xd600:
        return vdc_read(addr); /* FIXME */
      case 0xd700:
        return d7xx_read(addr); /* FIXME */
      case 0xd800:
      case 0xd900:
      case 0xda00:
      case 0xdb00:
        return colorram_read(addr);
      case 0xdc00:
        return cia1_peek(addr);
      case 0xdd00:
        return cia2_peek(addr);
      case 0xde00:
        return c64io1_read(addr);  /* FIXME */
      case 0xdf00:
        return c64io2_read(addr);  /* FIXME */
    }
    return 0xff;
}

/* Exported banked memory access functions for the monitor.  */

static const char *banknames[] = {
    "default", "cpu", "ram", "rom", "io", "ram1", "intfunc", "extfunc", "cart",
    "c64rom", "vdc", NULL
};

static const int banknums[] = {
    1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
};

const char **mem_bank_list(void)
{
    return banknames;
}

int mem_bank_from_name(const char *name)
{
    int i = 0;

    while (banknames[i]) {
        if (!strcmp(name, banknames[i])) {
            return banknums[i];
        }
        i++;
    }
    return -1;
}

BYTE mem_bank_read(int bank, WORD addr, void *context)
{
    switch (bank) {
      case 0:                   /* current */
        return mem_read(addr);
        break;
      case 4:                   /* ram1 */
        return mem_ram[addr + 0x10000];
      case 3:                   /* io */
        if (addr >= 0xd000 && addr < 0xe000) {
            return read_bank_io(addr);
        }
      case 2:                   /* rom */
        if (addr <= 0x0fff) {
            return bios_read(addr);
        }
        if (addr >= 0x4000 && addr <= 0xcfff) {
            return c128memrom_basic_rom[addr - 0x4000];
        }
        if (addr >= 0xd000 && addr <= 0xdfff) {
            return mem_chargen_rom[addr & 0x0fff];
        }
        if (addr >= 0xe000) {
            return c128memrom_kernal_rom[addr & 0x1fff];
        }
      case 1:                   /* ram */
        break;
      case 5:
        if (addr >= 0x8000) {
            return int_function_rom[addr & 0x7fff];
        }
        break;
      case 6:
        if (addr >= 0x8000 && addr <= 0xbfff) {
            return ext_function_rom[addr & 0x3fff];
        }
        break;
      case 7:
        if (addr >= 0x8000 && addr <= 0x9fff) {
            return roml_banks[addr & 0x1fff];
        }
        if (addr >= 0xa000 && addr <= 0xbfff) {
            return romh_banks[addr & 0x1fff];
        }
      case 8:
        if (addr >= 0xa000 && addr <= 0xbfff) {
            return c64memrom_basic64_rom[addr & 0x1fff];
        }
        if (addr >= 0xd000 && addr <= 0xdfff) {
            return mem_chargen_rom[addr & 0x0fff];
        }
        if (addr >= 0xe000) {
            return c64memrom_kernal64_rom[addr & 0x1fff];
        }
        break;
      case 9:
        return vdc_ram_read(addr);
    }
    return mem_ram[addr];
}

BYTE mem_bank_peek(int bank, WORD addr, void *context)
{
    switch (bank) {
      case 0:                   /* current */
        return mem_read(addr);  /* FIXME */
        break;
      case 3:                   /* io */
        if (addr >= 0xd000 && addr < 0xe000) {
            return peek_bank_io(addr);
        }
    }
    return mem_bank_read(bank, addr, context);
}

void mem_bank_write(int bank, WORD addr, BYTE byte, void *context)
{
    switch (bank) {
      case 0:                   /* current */
        mem_store(addr, byte);
        return;
      case 4:                   /* ram1 */
        mem_ram[addr + 0x10000] = byte;
        return;
      case 3:                   /* io */
        if (addr >= 0xd000 && addr < 0xe000) {
            store_bank_io(addr, byte);
            return;
        }
      case 2:                   /* rom */
        if (addr >= 0x4000 && addr <= 0xcfff) {
            return;
        }
        if (addr >= 0xe000) {
            return;
        }
      case 1:                   /* ram */
        break;
      case 5:
        if (addr >= 0x8000) {
            return;
        }
        break;
      case 6:
        if (addr >= 0x8000 && addr <= 0xbfff) {
            return;
        }
        break;
      case 7:
        if (addr >= 0x8000 && addr <= 0x9fff) {
            return;
        }
        if (addr >= 0xa000 && addr <= 0xbfff) {
            return;
        }
      case 8:
        if (addr >= 0xa000 && addr <= 0xbfff) {
            return;
        }
        if (addr >= 0xd000 && addr <= 0xdfff) {
            return;
        }
        if (addr >= 0xe000) {
            return;
        }
        break;
      case 9:
        vdc_ram_store(addr, byte);
        break;
    }
    mem_ram[addr] = byte;
}

mem_ioreg_list_t *mem_ioreg_list_get(void *context)
{
    mem_ioreg_list_t *mem_ioreg_list = NULL;

    mon_ioreg_add_list(&mem_ioreg_list, "VIC-IIe", 0xd000, 0xd030);
    mon_ioreg_add_list(&mem_ioreg_list, "SID", 0xd400, 0xd41f);
    mon_ioreg_add_list(&mem_ioreg_list, "MMU", 0xd500, 0xd50b);
    mon_ioreg_add_list(&mem_ioreg_list, "CIA1", 0xdc00, 0xdc0f);
    mon_ioreg_add_list(&mem_ioreg_list, "CIA2", 0xdd00, 0xdd0f);

    c64io_ioreg_add_list(&mem_ioreg_list);

    return mem_ioreg_list;
}

void mem_get_screen_parameter(WORD *base, BYTE *rows, BYTE *columns)
{
    *base = ((vicii_peek(0xd018) & 0xf0) << 6)
            | ((~cia2_peek(0xdd00) & 0x03) << 14);
    *rows = 25;
    *columns = 40;
}

/* ------------------------------------------------------------------------- */

/* FIXME: is called from c64tpi.c to enable/disable C64 IEEE488 ROM module */
void mem_set_exrom(int active)
{
}

void mem_color_ram_to_snapshot(BYTE *color_ram)
{
    unsigned int i;

    for (i = 0; i < 0x400; i++)
        color_ram[i] = (mem_color_ram[i] & 15)
                       | (mem_color_ram[i + 0x400] << 4);
}

void mem_color_ram_from_snapshot(BYTE *color_ram)
{
    unsigned int i;

    for (i = 0; i < 0x400; i++) {
        mem_color_ram[i] = color_ram[i] & 15;
        mem_color_ram[i + 0x400] = color_ram[i] >> 4;
    }
}

/* ------------------------------------------------------------------------- */

/* 8502 specific I/O function wrappers for 2mhz mode cycle stretching */

BYTE REGPARM1 c128_vicii_read(WORD addr)
{
  BYTE temp_value;

  temp_value=vicii_read(addr);
  vicii_clock_read_stretch();
  return temp_value;
}

void REGPARM2 c128_vicii_store(WORD addr, BYTE value)
{
  vicii_clock_write_stretch();
  vicii_store(addr, value);
}

BYTE REGPARM1 c128_sid_read(WORD addr)
{
  BYTE temp_value;

  temp_value=sid_read(addr);
  vicii_clock_read_stretch();
  return temp_value;
}

void REGPARM2 c128_sid_store(WORD addr, BYTE value)
{
  vicii_clock_write_stretch();
  sid_store(addr, value);
}

BYTE REGPARM1 c128_mmu_read(WORD addr)
{
  BYTE temp_value;

  temp_value=mmu_read(addr);
  vicii_clock_read_stretch();
  return temp_value;
}

void REGPARM2 c128_mmu_store(WORD addr, BYTE value)
{
  vicii_clock_write_stretch();
  mmu_store(addr, value);
}

BYTE REGPARM1 c128_d5xx_read(WORD addr)
{
  BYTE temp_value;

  temp_value=d5xx_read(addr);
  vicii_clock_read_stretch();
  return temp_value;
}

void REGPARM2 c128_d5xx_store(WORD addr, BYTE value)
{
  vicii_clock_write_stretch();
  d5xx_store(addr, value);
}

BYTE REGPARM1 c128_vdc_read(WORD addr)
{
  BYTE temp_value;

  temp_value=vdc_read(addr);
  vicii_clock_read_stretch();
  return temp_value;
}

void REGPARM2 c128_vdc_store(WORD addr, BYTE value)
{
  vicii_clock_write_stretch();
  vdc_store(addr, value);
}

BYTE REGPARM1 c128_d7xx_read(WORD addr)
{
  BYTE temp_value;

  temp_value=d7xx_read(addr);
  vicii_clock_read_stretch();
  return temp_value;
}

void REGPARM2 c128_d7xx_store(WORD addr, BYTE value)
{
  vicii_clock_write_stretch();
  d7xx_store(addr, value);
}

BYTE REGPARM1 c128_colorram_read(WORD addr)
{
  BYTE temp_value;

  temp_value=colorram_read(addr);
  vicii_clock_read_stretch();
  return temp_value;
}

void REGPARM2 c128_colorram_store(WORD addr, BYTE value)
{
  vicii_clock_write_stretch();
  colorram_store(addr, value);
}

BYTE REGPARM1 c128_cia1_read(WORD addr)
{
  BYTE temp_value;

  temp_value=cia1_read(addr);
  vicii_clock_read_stretch();
  return temp_value;
}

void REGPARM2 c128_cia1_store(WORD addr, BYTE value)
{
  vicii_clock_write_stretch();
  cia1_store(addr, value);
}

BYTE REGPARM1 c128_cia2_read(WORD addr)
{
  BYTE temp_value;

  temp_value=cia2_read(addr);
  vicii_clock_read_stretch();
  return temp_value;
}

void REGPARM2 c128_cia2_store(WORD addr, BYTE value)
{
  vicii_clock_write_stretch();
  cia2_store(addr, value);
}

BYTE REGPARM1 c128_c64io1_read(WORD addr)
{
  BYTE temp_value;

  temp_value=c64io1_read(addr);
  vicii_clock_read_stretch();
  return temp_value;
}

void REGPARM2 c128_c64io1_store(WORD addr, BYTE value)
{
  vicii_clock_write_stretch();
  c64io1_store(addr, value);
}

BYTE REGPARM1 c128_c64io2_read(WORD addr)
{
  BYTE temp_value;

  temp_value=c64io2_read(addr);
  vicii_clock_read_stretch();
  return temp_value;
}

void REGPARM2 c128_c64io2_store(WORD addr, BYTE value)
{
  vicii_clock_write_stretch();
  c64io2_store(addr, value);
}
