/*
 * plus4mem.c -- Plus4 memory handling.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Tibor Biczo <crown@axelero.hu>
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
#include <string.h>

#include "datasette.h"
#include "iecbus.h"
#include "maincpu.h"
#include "mem.h"
#include "monitor.h"
#include "plus4acia.h"
#include "plus4iec.h"
#include "plus4mem.h"
#include "plus4memcsory256k.h"
#include "plus4memhannes256k.h"
#include "plus4memlimit.h"
#include "plus4memrom.h"
#include "plus4pio1.h"
#include "plus4pio2.h"
#include "plus4tcbm.h"
#include "ram.h"
#include "resources.h"
#include "sid-resources.h"
#include "sid.h"
#include "ted.h"
#include "ted-mem.h"
#include "types.h"

static int hard_reset_flag = 0;

/* Adjust this pointer when the MMU changes banks.  */
static BYTE **bank_base;
static int *bank_limit = NULL;
unsigned int mem_old_reg_pc;

/* ------------------------------------------------------------------------- */

/* Number of possible memory configurations.  */
#define NUM_CONFIGS     32

/* The Plus4 memory.  */
BYTE mem_ram[PLUS4_RAM_SIZE];
BYTE extromlo1[PLUS4_BASIC_ROM_SIZE];
BYTE extromlo2[PLUS4_BASIC_ROM_SIZE];
BYTE extromlo3[PLUS4_BASIC_ROM_SIZE];
BYTE extromhi1[PLUS4_KERNAL_ROM_SIZE];
BYTE extromhi2[PLUS4_KERNAL_ROM_SIZE];
BYTE extromhi3[PLUS4_KERNAL_ROM_SIZE];

/* Pointers to the currently used memory read and write tables.  */
read_func_ptr_t *_mem_read_tab_ptr;
store_func_ptr_t *_mem_write_tab_ptr;
BYTE **_mem_read_base_tab_ptr;
int *mem_read_limit_tab_ptr;

/* Memory read and write tables.  */
static store_func_ptr_t mem_write_tab[NUM_CONFIGS][0x101];
static read_func_ptr_t mem_read_tab[NUM_CONFIGS][0x101];
static BYTE *mem_read_base_tab[NUM_CONFIGS][0x101];
static int mem_read_limit_tab[NUM_CONFIGS][0x101];

static store_func_ptr_t mem_write_tab_watch[0x101];
static read_func_ptr_t mem_read_tab_watch[0x101];

/* Processor port.  */
static struct {
    BYTE dir, data, data_out;
} pport;

/* Current memory configuration.  */
unsigned int mem_config;

/* ------------------------------------------------------------------------- */

#define RAM0 mem_ram + 0x0000
#define RAM4 mem_ram + 0x4000
#define RAM8 mem_ram + 0x8000
#define RAMC mem_ram + 0xc000

static BYTE *chargen_tab[8][16] = {
    /* 0000-3fff, RAM selected  */
    {       RAM0,       RAM0,       RAM0,       RAM0,
            RAM0,       RAM0,       RAM0,       RAM0,
            RAM0,       RAM0,       RAM0,       RAM0,
            RAM0,       RAM0,       RAM0,       RAM0 },
    /* 4000-7fff, RAM selected  */
    {       RAM4,       RAM4,       RAM4,       RAM4,
            RAM4,       RAM4,       RAM4,       RAM4,
            RAM4,       RAM4,       RAM4,       RAM4,
            RAM4,       RAM4,       RAM4,       RAM4 },
    /* 8000-bfff, RAM selected  */
    {       RAM8,       RAM8,       RAM8,       RAM8,
            RAM8,       RAM8,       RAM8,       RAM8,
            RAM8,       RAM8,       RAM8,       RAM8,
            RAM8,       RAM8,       RAM8,       RAM8 },
    /* c000-ffff, RAM selected  */
    {       RAMC,       RAMC,       RAMC,       RAMC,
            RAMC,       RAMC,       RAMC,       RAMC,
            RAMC,       RAMC,       RAMC,       RAMC,
            RAMC,       RAMC,       RAMC,       RAMC },

    /* 0000-3fff, ROM selected  */
    {       RAM0,       RAM0,       RAM0,       RAM0,
            RAM0,       RAM0,       RAM0,       RAM0,
            RAM0,       RAM0,       RAM0,       RAM0,
            RAM0,       RAM0,       RAM0,       RAM0 },
    /* 4000-7fff, ROM selected  */
    {       RAM4,       RAM4,       RAM4,       RAM4,
            RAM4,       RAM4,       RAM4,       RAM4,
            RAM4,       RAM4,       RAM4,       RAM4,
            RAM4,       RAM4,       RAM4,       RAM4 },
    /* 8000-bfff, ROM selected  */
    {  plus4memrom_basic_rom,  extromlo1,      extromlo2,      extromlo3,
       plus4memrom_basic_rom,  extromlo1,      extromlo2,      extromlo3,
       plus4memrom_basic_rom,  extromlo1,      extromlo2,      extromlo3,
       plus4memrom_basic_rom,  extromlo1,      extromlo2,      extromlo3 },
    /* c000-ffff, ROM selected  */
    {  plus4memrom_kernal_rom, plus4memrom_kernal_rom,
       plus4memrom_kernal_rom, plus4memrom_kernal_rom,
       extromhi1,      extromhi1,      extromhi1,      extromhi1,
       extromhi2,      extromhi2,      extromhi2,      extromhi2,
       extromhi3,      extromhi3,      extromhi3,      extromhi3 }
};


BYTE *mem_get_tedmem_base(unsigned int segment)
{
    return chargen_tab[segment][mem_config >> 1];
}

/* ------------------------------------------------------------------------- */

/* Tape motor status.  */
static BYTE old_port_data_out = 0xff;

/* Tape read input.  */
static BYTE tape_read = 0xff;

inline static void mem_proc_port_store(void)
{

    /*  Correct clock */
    ted_handle_pending_alarms(maincpu_rmw_flag + 1);

    pport.data_out = (pport.data_out & ~pport.dir)
                     | (pport.data & pport.dir);

    (*iecbus_callback_write)((BYTE)~pport.data_out, last_write_cycle);

    if (((pport.dir & pport.data) & 0x08) != old_port_data_out) {
        old_port_data_out = (pport.dir & pport.data) & 0x08;
        datasette_set_motor(!old_port_data_out);
    }
}

inline static BYTE mem_proc_port_read(WORD addr)
{
    BYTE tmp;
    BYTE input;

    /*  Correct clock */
    ted_handle_pending_alarms(0);

    if (addr == 0)
        return pport.dir;

    input = ((*iecbus_callback_read)(maincpu_clk) & 0xc0);
    if (tape_read) {
        input |= 0x10;
    } else {
        input &= ~0x10;
    }

    tmp = ((input & ~pport.dir) | (pport.data_out & pport.dir)) & 0xdf ;

    return tmp;
}

void mem_proc_port_trigger_flux_change(unsigned int on)
{
   /*printf("FLUXCHANGE\n");*/
   tape_read = on; 
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 zero_read(WORD addr)
{
    addr &= 0xff;

    switch ((BYTE)addr) {
      case 0:
      case 1:
        return mem_proc_port_read(addr);
    }
    if (!cs256k_enabled)
        return mem_ram[addr];
    else
        return cs256k_read(addr);
}

void REGPARM2 zero_store(WORD addr, BYTE value)
{
    addr &= 0xff;

    switch ((BYTE)addr) {
      case 0:
        if (pport.dir != value) {
            pport.dir = value & 0xdf;
            mem_proc_port_store();
        }
        if (!cs256k_enabled)
            mem_ram[addr] = value;
        else
            cs256k_store(addr,value);
        break;
      case 1:
        if (pport.data != value) {
            pport.data = value;
            mem_proc_port_store();
        }
        if (!cs256k_enabled)
            mem_ram[addr] = value;
        else
            cs256k_store(addr,value);
        break;
      default:
        mem_ram[addr] = value;
    }
}

/* ------------------------------------------------------------------------- */

static void mem_config_set(unsigned int config)
{
    mem_config = config;

    if (any_watchpoints(e_comp_space)) {
        _mem_read_tab_ptr = mem_read_tab_watch;
        _mem_write_tab_ptr = mem_write_tab_watch;
    } else {
        _mem_read_tab_ptr = mem_read_tab[mem_config];
        _mem_write_tab_ptr = mem_write_tab[mem_config];
    }

    _mem_read_base_tab_ptr = mem_read_base_tab[mem_config];
    mem_read_limit_tab_ptr = mem_read_limit_tab[mem_config];

    if (bank_limit != NULL) {
        *bank_base = _mem_read_base_tab_ptr[mem_old_reg_pc >> 8];
        if (*bank_base != 0)
            *bank_base = _mem_read_base_tab_ptr[mem_old_reg_pc >> 8]
                         - (mem_old_reg_pc & 0xff00);
        *bank_limit = mem_read_limit_tab_ptr[mem_old_reg_pc >> 8];
    }
}

void mem_config_ram_set(unsigned int config)
{
    mem_config_set((mem_config & ~0x01) | config);
}

static void mem_config_rom_set(unsigned int config)
{
    mem_config_set((mem_config & ~0x1e) | config);
}

/* ------------------------------------------------------------------------- */

static BYTE REGPARM1 read_watch(WORD addr)
{
    monitor_watch_push_load_addr(addr, e_comp_space);
    return mem_read_tab[mem_config][addr >> 8](addr);
}


static void REGPARM2 store_watch(WORD addr, BYTE value)
{
    monitor_watch_push_store_addr(addr, e_comp_space);
    mem_write_tab[mem_config][addr >> 8](addr, value);
}

void mem_toggle_watchpoints(int flag, void *context)
{
    if (flag) {
        _mem_read_tab_ptr = mem_read_tab_watch;
        _mem_write_tab_ptr = mem_write_tab_watch;
    } else {
        _mem_read_tab_ptr = mem_read_tab[mem_config];
        _mem_write_tab_ptr = mem_write_tab[mem_config];
    }
}

/* ------------------------------------------------------------------------- */

static BYTE REGPARM1 ram_read(WORD addr)
{
    return mem_ram[addr];
}

static BYTE REGPARM1 ram_read_32k(WORD addr)
{
    return mem_ram[addr & 0x7fff];
}

static BYTE REGPARM1 ram_read_16k(WORD addr)
{
    return mem_ram[addr & 0x3fff];
}

static void REGPARM2 ram_store(WORD addr, BYTE value)
{
    mem_ram[addr] = value;
}

static void REGPARM2 ram_store_32k(WORD addr, BYTE value)
{
    mem_ram[addr & 0x7fff] = value;
}

static void REGPARM2 ram_store_16k(WORD addr, BYTE value)
{
    mem_ram[addr & 0x3fff] = value;
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

/* ------------------------------------------------------------------------- */

static BYTE REGPARM1 fdxx_read(WORD addr)
{
#ifdef HAVE_RS232
    if (addr >= 0xfd00 && addr <= 0xfd0f)
        return acia_read(addr);
#endif

    if (addr == 0xfd16 && h256k_enabled)
        return h256k_reg_read(addr);

    if (addr == 0xfd15 && cs256k_enabled)
        return cs256k_reg_read(addr);

    if (addr >= 0xfd10 && addr <= 0xfd1f)
        return pio1_read(addr);

    if (addr >= 0xfd30 && addr <= 0xfd3f)
        return pio2_read(addr);

    if (sidcart_enabled && sidcart_address==0 && addr >= 0xfd40 && addr <= 0xfd5f)
        return sid_read(addr);

    return 0;
}

static void REGPARM2 fdxx_store(WORD addr, BYTE value)
{
#ifdef HAVE_RS232
    if (addr >= 0xfd00 && addr <= 0xfd0f) {
        acia_store(addr, value);
        return;
    }
#endif
    if (addr == 0xfd16 && h256k_enabled) {
        h256k_reg_store(addr, value);
        return;
    }
    if (addr == 0xfd15 && cs256k_enabled) {
        cs256k_reg_store(addr, value);
        return;
    }
    if (addr >= 0xfd10 && addr <= 0xfd1f) {
        pio1_store(addr, value);
        return;
    }
    if (addr >= 0xfd30 && addr <= 0xfd3f) {
        pio2_store(addr, value);
        return;
    }
    if (sidcart_enabled && sidcart_address==0 && addr >= 0xfd40 && addr <= 0xfd5f) {
        sid_store(addr, value);
        return;
    }
    if (addr >= 0xfdd0 && addr <= 0xfddf) {
        mem_config_rom_set((addr & 0xf) << 1);
        return;
    }
}

static BYTE REGPARM1 fexx_read(WORD addr)
{
    if (addr >= 0xfec0 && addr <= 0xfedf)
        return plus4tcbm2_read(addr);

    if (addr >= 0xfee0 && addr <= 0xfeff)
        return plus4tcbm1_read(addr);

    if (sidcart_enabled && sidcart_address==1 && addr >= 0xfe80 && addr <= 0xfe9f)
        return sid_read(addr);

    return 0;
}

static void REGPARM2 fexx_store(WORD addr, BYTE value)
{
    if (addr >= 0xfec0 && addr <= 0xfedf) {
        plus4tcbm2_store(addr, value);
        return;
    }
    if (addr >= 0xfee0 && addr <= 0xfeff) {
        plus4tcbm1_store(addr, value);
        return;
    }
    if (sidcart_enabled && sidcart_address==1 && addr >= 0xfe80 && addr <= 0xfe9f) {
        sid_store(addr, value);
        return;
    }
}

static BYTE REGPARM1 ram_ffxx_read(WORD addr)
{
    if ((addr >= 0xff20) && (addr != 0xff3e) && (addr != 0xff3f))
        return ram_read(addr);

    return ted_read(addr);
}

static BYTE REGPARM1 ram_ffxx_read_32k(WORD addr)
{
    if ((addr >= 0xff20) && (addr != 0xff3e) && (addr != 0xff3f))
        return ram_read_32k(addr);

    return ted_read(addr);
}

static BYTE REGPARM1 ram_ffxx_read_16k(WORD addr)
{
    if ((addr >= 0xff20) && (addr != 0xff3e) && (addr != 0xff3f))
        return ram_read_16k(addr);

    return ted_read(addr);
}


static void REGPARM2 ram_ffxx_store(WORD addr, BYTE value)
{
    if (addr < 0xff20 || addr == 0xff3e || addr == 0xff3f) {
        ted_store(addr, value);
    } else {
        ram_store(addr, value);
    }
}

static void REGPARM2 ram_ffxx_store_32k(WORD addr, BYTE value)
{
    if (addr < 0xff20 || addr == 0xff3e || addr == 0xff3f) {
        ted_store(addr, value);
    } else {
        ram_store_32k(addr, value);
    }
}

static void REGPARM2 ram_ffxx_store_16k(WORD addr, BYTE value)
{
    if (addr < 0xff20 || addr == 0xff3e || addr == 0xff3f) {
        ted_store(addr, value);
    } else {
        ram_store_16k(addr, value);
    }
}

static BYTE REGPARM1 rom_ffxx_read(WORD addr)
{
    if ((addr >= 0xff20) && (addr != 0xff3e) && (addr != 0xff3f))
        return plus4memrom_rom_read(addr);

    return ted_read(addr);
}

static void REGPARM2 rom_ffxx_store(WORD addr, BYTE value)
{
    if (addr < 0xff20 || addr == 0xff3e || addr == 0xff3f) {
        ted_store(addr, value);
    } else {
        ram_store(addr, value);
    }
}

static void REGPARM2 rom_ffxx_store_32k(WORD addr, BYTE value)
{
    if (addr < 0xff20 || addr == 0xff3e || addr == 0xff3f) {
        ted_store(addr, value);
    } else {
        ram_store_32k(addr, value);
    }
}

static void REGPARM2 rom_ffxx_store_16k(WORD addr, BYTE value)
{
    if (addr < 0xff20 || addr == 0xff3e || addr == 0xff3f) {
        ted_store(addr, value);
    } else {
        ram_store_16k(addr, value);
    }
}

/* ------------------------------------------------------------------------- */

static void set_write_hook(int config, int page, store_func_t *f)
{
     mem_write_tab[config][page] = f;
}

void mem_initialize_memory(void)
{
    int i, j;
    int ram_size;

    if (resources_get_int("RamSize", &ram_size) < 0)
        return;

    switch (ram_size) {
      default:
      case 256:
      case 64:
        for (i = 0; i < 16; i++) {
            chargen_tab[1][i] = RAM4;
            chargen_tab[2][i] = RAM8;
            chargen_tab[3][i] = RAMC;
            chargen_tab[5][i] = RAM4;
        }
        break;
      case 32:
        for (i = 0; i < 16; i++) {
            chargen_tab[1][i] = RAM4;
            chargen_tab[2][i] = RAM0;
            chargen_tab[3][i] = RAM4;
            chargen_tab[5][i] = RAM4;
        }
        break;
      case 16:
        for (i = 0; i < 16; i++) {
            chargen_tab[1][i] = RAM0;
            chargen_tab[2][i] = RAM0;
            chargen_tab[3][i] = RAM0;
            chargen_tab[5][i] = RAM0;
        }
        break;
    }

    mem_limit_init(mem_read_limit_tab);

    /* Default is RAM.  */
    for (i = 0; i <= 0x100; i++) {
        mem_read_tab_watch[i] = read_watch;
        mem_write_tab_watch[i] = store_watch;
    }

    for (i = 0; i < NUM_CONFIGS; i++) {
        set_write_hook(i, 0, zero_store);
        mem_read_tab[i][0] = zero_read;
        mem_read_base_tab[i][0] = mem_ram;
        for (j = 1; j <= 0xff; j++) {
            switch (ram_size) {
              case 4096:
              case 1024:
              case 256:
                if (h256k_enabled && j<0x10) {
                    mem_read_tab[i][j] = ram_read;
                    mem_write_tab[i][j] = ted_mem_vbank_store;
                }
                if (h256k_enabled && j>=0x10) {
                    mem_read_tab[i][j] = h256k_read;
                    mem_write_tab[i][j] = h256k_store;
                }
                if (cs256k_enabled) {
                    mem_read_tab[i][j] = cs256k_read;
                    mem_write_tab[i][j] = cs256k_store;
                }
                mem_read_base_tab[i][j] = mem_ram + (j << 8);
                break;
              default:
              case 64:
                mem_read_tab[i][j] = ram_read;
                mem_read_base_tab[i][j] = mem_ram + (j << 8);
                mem_write_tab[i][j] = ted_mem_vbank_store;
                break;
              case 32:
                mem_read_tab[i][j] = ram_read_32k;
                mem_read_base_tab[i][j] = mem_ram + ((j & 0x7f) << 8);
                mem_write_tab[i][j] = ted_mem_vbank_store_32k;
                break;
              case 16:
                mem_read_tab[i][j] = ram_read_16k;
                mem_read_base_tab[i][j] = mem_ram + ((j & 0x3f) << 8);
                mem_write_tab[i][j] = ted_mem_vbank_store_16k;
                break;
            }
#if 0
            if ((j & 0xc0) == (k << 6)) {
                switch (j & 0x3f) {
                  case 0x39:
                    mem_write_tab[i][j] = ted_mem_vbank_39xx_store;
                    break;
                  case 0x3f:
                    mem_write_tab[i][j] = ted_mem_vbank_3fxx_store;
                    break;
                  default:
                    mem_write_tab[i][j] = ted_mem_vbank_store;
                }
            } else {
#endif
#if 0
            }
#endif
        }
#if 0
        mem_read_tab[i][0xff] = ram_read;
        mem_read_base_tab[i][0xff] = ram + 0xff00;
        set_write_hook(i, 0xff, ram_store);
#endif
    }

    /* Setup BASIC ROM and extension ROMs at $8000-$BFFF.  */
    for (i = 0x80; i <= 0xbf; i++) {
        mem_read_tab[1][i] = plus4memrom_basic_read;
        mem_read_base_tab[1][i] = plus4memrom_basic_rom + ((i & 0x3f) << 8);
        mem_read_tab[3][i] = plus4memrom_extromlo1_read;
        mem_read_base_tab[3][i] = extromlo1 + ((i & 0x3f) << 8);
        mem_read_tab[5][i] = plus4memrom_extromlo2_read;
        mem_read_base_tab[5][i] = extromlo2 + ((i & 0x3f) << 8);
        mem_read_tab[7][i] = plus4memrom_extromlo3_read;
        mem_read_base_tab[7][i] = extromlo3 + ((i & 0x3f) << 8);
        mem_read_tab[9][i] = plus4memrom_basic_read;
        mem_read_base_tab[9][i] = plus4memrom_basic_rom + ((i & 0x3f) << 8);
        mem_read_tab[11][i] = plus4memrom_extromlo1_read;
        mem_read_base_tab[11][i] = extromlo1 + ((i & 0x3f) << 8);
        mem_read_tab[13][i] = plus4memrom_extromlo2_read;
        mem_read_base_tab[13][i] = extromlo2 + ((i & 0x3f) << 8);
        mem_read_tab[15][i] = plus4memrom_extromlo3_read;
        mem_read_base_tab[15][i] = extromlo3 + ((i & 0x3f) << 8);
        mem_read_tab[17][i] = plus4memrom_basic_read;
        mem_read_base_tab[17][i] = plus4memrom_basic_rom + ((i & 0x3f) << 8);
        mem_read_tab[19][i] = plus4memrom_extromlo1_read;
        mem_read_base_tab[19][i] = extromlo1 + ((i & 0x3f) << 8);
        mem_read_tab[21][i] = plus4memrom_extromlo2_read;
        mem_read_base_tab[21][i] = extromlo2 + ((i & 0x3f) << 8);
        mem_read_tab[23][i] = plus4memrom_extromlo3_read;
        mem_read_base_tab[23][i] = extromlo3 + ((i & 0x3f) << 8);
        mem_read_tab[25][i] = plus4memrom_basic_read;
        mem_read_base_tab[25][i] = plus4memrom_basic_rom + ((i & 0x3f) << 8);
        mem_read_tab[27][i] = plus4memrom_extromlo1_read;
        mem_read_base_tab[27][i] = extromlo1 + ((i & 0x3f) << 8);
        mem_read_tab[29][i] = plus4memrom_extromlo2_read;
        mem_read_base_tab[29][i] = extromlo2 + ((i & 0x3f) << 8);
        mem_read_tab[31][i] = plus4memrom_extromlo3_read;
        mem_read_base_tab[31][i] = extromlo3 + ((i & 0x3f) << 8);
    }

    /* Setup Kernal ROM and extension ROMs at $E000-$FFFF.  */
    for (i = 0xc0; i <= 0xff; i++) {
        mem_read_tab[1][i] = plus4memrom_kernal_read;
        mem_read_base_tab[1][i] = plus4memrom_kernal_trap_rom
                                  + ((i & 0x3f) << 8);
        mem_read_tab[3][i] = plus4memrom_kernal_read;
        mem_read_base_tab[3][i] = plus4memrom_kernal_trap_rom
                                  + ((i & 0x3f) << 8);
        mem_read_tab[5][i] = plus4memrom_kernal_read;
        mem_read_base_tab[5][i] = plus4memrom_kernal_trap_rom
                                  + ((i & 0x3f) << 8);
        mem_read_tab[7][i] = plus4memrom_kernal_read;
        mem_read_base_tab[7][i] = plus4memrom_kernal_trap_rom
                                  + ((i & 0x3f) << 8);
        mem_read_tab[9][i] = plus4memrom_extromhi1_read;
        mem_read_base_tab[9][i] = extromhi1 + ((i & 0x3f) << 8);
        mem_read_tab[11][i] = plus4memrom_extromhi1_read;
        mem_read_base_tab[11][i] = extromhi1 + ((i & 0x3f) << 8);
        mem_read_tab[13][i] = plus4memrom_extromhi1_read;
        mem_read_base_tab[13][i] = extromhi1 + ((i & 0x3f) << 8);
        mem_read_tab[15][i] = plus4memrom_extromhi1_read;
        mem_read_base_tab[15][i] = extromhi1 + ((i & 0x3f) << 8);
        mem_read_tab[17][i] = plus4memrom_extromhi2_read;
        mem_read_base_tab[17][i] = extromhi2 + ((i & 0x3f) << 8);
        mem_read_tab[19][i] = plus4memrom_extromhi2_read;
        mem_read_base_tab[19][i] = extromhi2 + ((i & 0x3f) << 8);
        mem_read_tab[21][i] = plus4memrom_extromhi2_read;
        mem_read_base_tab[21][i] = extromhi2 + ((i & 0x3f) << 8);
        mem_read_tab[23][i] = plus4memrom_extromhi2_read;
        mem_read_base_tab[23][i] = extromhi2 + ((i & 0x3f) << 8);
        mem_read_tab[25][i] = plus4memrom_extromhi3_read;
        mem_read_base_tab[25][i] = extromhi3 + ((i & 0x3f) << 8);
        mem_read_tab[27][i] = plus4memrom_extromhi3_read;
        mem_read_base_tab[27][i] = extromhi3 + ((i & 0x3f) << 8);
        mem_read_tab[29][i] = plus4memrom_extromhi3_read;
        mem_read_base_tab[29][i] = extromhi3 + ((i & 0x3f) << 8);
        mem_read_tab[31][i] = plus4memrom_extromhi3_read;
        mem_read_base_tab[31][i] = extromhi3 + ((i & 0x3f) << 8);
    }

    for (i = 0; i < NUM_CONFIGS; i += 2) {
        mem_read_tab[i + 1][0xfc] = plus4memrom_kernal_read;
        mem_read_base_tab[i + 1][0xfc] = plus4memrom_kernal_trap_rom
                                         + ((0xfc & 0x3f) << 8);

        mem_read_tab[i + 0][0xfd] = fdxx_read;
        mem_write_tab[i + 0][0xfd] = fdxx_store;
        mem_read_base_tab[i + 0][0xfd] = NULL;
        mem_read_tab[i + 1][0xfd] = fdxx_read;
        mem_write_tab[i + 1][0xfd] = fdxx_store;
        mem_read_base_tab[i + 1][0xfd] = NULL;

        mem_read_tab[i + 0][0xfe] = fexx_read;
        mem_write_tab[i + 0][0xfe] = fexx_store;
        mem_read_base_tab[i + 0][0xfe] = NULL;
        mem_read_tab[i + 1][0xfe] = fexx_read;
        mem_write_tab[i + 1][0xfe] = fexx_store;
        mem_read_base_tab[i + 1][0xfe] = NULL;

        switch (ram_size) {
          default:
          case 64:
            mem_read_tab[i + 0][0xff] = ram_ffxx_read;
            mem_write_tab[i + 0][0xff] = ram_ffxx_store;
            mem_read_base_tab[i + 0][0xff] = NULL;
            mem_read_tab[i + 1][0xff] = rom_ffxx_read;
            mem_write_tab[i + 1][0xff] = rom_ffxx_store;
            mem_read_base_tab[i + 1][0xff] = NULL;
            break;
          case 32:
            mem_read_tab[i + 0][0xff] = ram_ffxx_read_32k;
            mem_write_tab[i + 0][0xff] = ram_ffxx_store_32k;
            mem_read_base_tab[i + 0][0xff] = NULL;
            mem_read_tab[i + 1][0xff] = rom_ffxx_read;
            mem_write_tab[i + 1][0xff] = rom_ffxx_store_32k;
            mem_read_base_tab[i + 1][0xff] = NULL;
            break;
          case 16:
            mem_read_tab[i + 0][0xff] = ram_ffxx_read_16k;
            mem_write_tab[i + 0][0xff] = ram_ffxx_store_16k;
            mem_read_base_tab[i + 0][0xff] = NULL;
            mem_read_tab[i + 1][0xff] = rom_ffxx_read;
            mem_write_tab[i + 1][0xff] = rom_ffxx_store_16k;
            mem_read_base_tab[i + 1][0xff] = NULL;
            break;
        }

        mem_read_tab[i + 0][0x100] = mem_read_tab[i + 0][0];
        mem_write_tab[i + 0][0x100] = mem_write_tab[i + 0][0];
        mem_read_base_tab[i + 0][0x100] = mem_read_base_tab[i + 0][0];
        mem_read_tab[i + 1][0x100] = mem_read_tab[i + 1][0];
        mem_write_tab[i + 1][0x100] = mem_write_tab[i + 1][0];
        mem_read_base_tab[i + 1][0x100] = mem_read_base_tab[i + 1][0];
    }
    if (hard_reset_flag) {
        hard_reset_flag=0;
        mem_config = 1;
    }
    _mem_read_tab_ptr = mem_read_tab[mem_config];
    _mem_write_tab_ptr = mem_write_tab[mem_config];
    _mem_read_base_tab_ptr = mem_read_base_tab[mem_config];
    mem_read_limit_tab_ptr = mem_read_limit_tab[mem_config];
}

/* ------------------------------------------------------------------------- */

/* Initialize RAM for power-up.  */
void mem_powerup(void)
{
    ram_init(mem_ram, 0x10000);

    hard_reset_flag = 1;
}

/* ------------------------------------------------------------------------- */

void mem_set_bank_pointer(BYTE **base, int *limit)
{
    bank_base = base;
    bank_limit = limit;
}

/* ------------------------------------------------------------------------- */

/* FIXME: this part needs to be checked.  */

void mem_get_basic_text(WORD *start, WORD *end)
{
    if (start != NULL)
        *start = mem_ram[0x2b] | (mem_ram[0x2c] << 8);
    if (end != NULL)
        *end = mem_ram[0x2d] | (mem_ram[0x2e] << 8);
}

void mem_set_basic_text(WORD start, WORD end)
{
    mem_ram[0x2b] = mem_ram[0xac] = start & 0xff;
    mem_ram[0x2c] = mem_ram[0xad] = start >> 8;
    mem_ram[0x2d] = mem_ram[0x2f] = mem_ram[0x31] = mem_ram[0xae] = end & 0xff;
    mem_ram[0x2e] = mem_ram[0x30] = mem_ram[0x32] = mem_ram[0xaf] = end >> 8;
}

/* ------------------------------------------------------------------------- */

int mem_rom_trap_allowed(WORD addr)
{
    return addr >= 0x8000 && (mem_config & 0x1);
}

/* ------------------------------------------------------------------------- */

/* Exported banked memory access functions for the monitor.  */

static const char *banknames[] = {
    "default", "cpu", "ram", "rom", "funcrom", "cart1rom", "cart2rom", NULL };

static const int banknums[] = { 1, 0, 1, 2, 3, 4, 5 };

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
      case 2:                   /* rom */
        if (addr >= 0x8000 && addr <= 0xbfff) {
            return plus4memrom_basic_rom[addr & 0x3fff];
        }
        if (addr >= 0xc000) {
            return plus4memrom_kernal_rom[addr & 0x3fff];
        }
        break;
      case 3:                   /* funcrom */
        if (addr >= 0x8000 && addr <= 0xbfff) {
            return extromlo1[addr & 0x3fff];
        }
        if (addr >= 0xc000) {
            return extromhi1[addr & 0x3fff];
        }
        break;
      case 4:                   /* cart1rom */
        if (addr >= 0x8000 && addr <= 0xbfff) {
            return extromlo2[addr & 0x3fff];
        }
        if (addr >= 0xc000) {
            return extromhi2[addr & 0x3fff];
        }
        break;
      case 5:                   /* cart2rom */
        if (addr >= 0x8000 && addr <= 0xbfff) {
            return extromlo3[addr & 0x3fff];
        }
        if (addr >= 0xc000) {
            return extromhi3[addr & 0x3fff];
        }
        break;
      case 1:                   /* ram */
        break;
    }
    return mem_ram[addr];
}

BYTE mem_bank_peek(int bank, WORD addr, void *context)
{
    return mem_bank_read(bank, addr, context);
}

void mem_bank_write(int bank, WORD addr, BYTE byte, void *context)
{
    switch (bank) {
      case 0:                   /* current */
        mem_store(addr, byte);
        return;
      case 2:                   /* rom */
        if (addr >= 0x8000 && addr <= 0xbfff) {
            return;
        }
        if (addr >= 0xc000) {
            return;
        }
        break;
      case 3:                   /* funcrom */
        if (addr >= 0x8000 && addr <= 0xbfff) {
            return;
        }
        if (addr >= 0xc000) {
            return;
        }
        break;
      case 4:                   /* cart1rom */
        if (addr >= 0x8000 && addr <= 0xbfff) {
            return;
        }
        if (addr >= 0xc000) {
            return;
        }
        break;
      case 5:                   /* cart2rom */
        if (addr >= 0x8000 && addr <= 0xbfff) {
            return;
        }
        if (addr >= 0xc000) {
            return;
        }
        break;
      case 1:                   /* ram */
        break;
    }
    mem_ram[addr] = byte;
}

mem_ioreg_list_t *mem_ioreg_list_get(void *context)
{
    mem_ioreg_list_t *mem_ioreg_list = NULL;

    mon_ioreg_add_list(&mem_ioreg_list, "ACIA", 0xfd00, 0xfd0f);
    mon_ioreg_add_list(&mem_ioreg_list, "TED", 0xff00, 0xff3f);

    return mem_ioreg_list;
}

void mem_get_screen_parameter(WORD *base, BYTE *rows, BYTE *columns)
{
    *base = (ted_peek(0xff14) & 0xf8) << 8 | 0x400;
    *rows = 25;
    *columns = 40;
}

