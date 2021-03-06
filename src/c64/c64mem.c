/*
 * c64mem.c -- C64 memory handling.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "c64-resources.h"
#include "c64_256k.h"
#include "c64cart.h"
#include "c64cia.h"
#include "c64io.h"
#include "c64mem.h"
#include "c64meminit.h"
#include "c64memlimit.h"
#include "c64memrom.h"
#include "c64pla.h"
#include "cart/c64cartmem.h"
#include "cartridge.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "mmc64.h"
#include "monitor.h"
#include "plus256k.h"
#include "plus60k.h"
#include "ram.h"
#include "ramcart.h"
#include "reu.h"
#include "sid.h"
#include "vicii-mem.h"
#include "vicii-phi1.h"
#include "vicii.h"

#ifdef HAVE_TFE
#include "tfe.h"
#endif


/* C64 memory-related resources.  */

/* Adjust this pointer when the MMU changes banks.  */
static BYTE **bank_base;
static int *bank_limit = NULL;
unsigned int mem_old_reg_pc;

/* ------------------------------------------------------------------------- */

/* Number of possible memory configurations.  */
#define NUM_CONFIGS     32
/* Number of possible video banks (16K each).  */
#define NUM_VBANKS      4

/* The C64 memory.  */
BYTE mem_ram[C64_RAM_SIZE];
BYTE mem_chargen_rom[C64_CHARGEN_ROM_SIZE];

/* Internal color memory.  */
static BYTE mem_color_ram[0x400];
BYTE *mem_color_ram_cpu, *mem_color_ram_vicii;

/* Pointer to the chargen ROM.  */
BYTE *mem_chargen_rom_ptr;

/* Pointers to the currently used memory read and write tables.  */
read_func_ptr_t *_mem_read_tab_ptr;
store_func_ptr_t *_mem_write_tab_ptr;
BYTE **_mem_read_base_tab_ptr;
int *mem_read_limit_tab_ptr;

/* Memory read and write tables.  */
static store_func_ptr_t mem_write_tab[NUM_VBANKS][NUM_CONFIGS][0x101];
static read_func_ptr_t mem_read_tab[NUM_CONFIGS][0x101];
static BYTE *mem_read_base_tab[NUM_CONFIGS][0x101];
static int mem_read_limit_tab[NUM_CONFIGS][0x101];

static store_func_ptr_t mem_write_tab_watch[0x101];
static read_func_ptr_t mem_read_tab_watch[0x101];

/* Current video bank (0, 1, 2 or 3).  */
static int vbank;

/* Current memory configuration.  */
static int mem_config;

/* Tape sense status: 1 = some button pressed, 0 = no buttons pressed.  */
static int tape_sense = 0;

/* ------------------------------------------------------------------------- */

static BYTE REGPARM1 read_watch(WORD addr)
{
    monitor_watch_push_load_addr(addr, e_comp_space);
    return mem_read_tab[mem_config][addr >> 8](addr);
}

static void REGPARM2 store_watch(WORD addr, BYTE value)
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

void mem_pla_config_changed(void)
{
    mem_config = (((~pport.dir | pport.data) & 0x7) | (export.exrom << 3)
                  | (export.game << 4));

    c64pla_config_changed(tape_sense, 1, 0x17);

    if (any_watchpoints(e_comp_space)) {
        _mem_read_tab_ptr = mem_read_tab_watch;
        _mem_write_tab_ptr = mem_write_tab_watch;
    } else {
        _mem_read_tab_ptr = mem_read_tab[mem_config];
        _mem_write_tab_ptr = mem_write_tab[vbank][mem_config];
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

BYTE REGPARM1 zero_read(WORD addr)
{
    addr &= 0xff;

    switch ((BYTE)addr) {
      case 0:
        return pport.dir_read;
      case 1:
        return pport.data_read;
    }

    if (c64_256k_enabled)
        return c64_256k_ram_segment0_read(addr);
    else {
        if (plus256k_enabled)
            return plus256k_ram_low_read(addr);
        else
            return mem_ram[addr & 0xff];
    }
}

void REGPARM2 zero_store(WORD addr, BYTE value)
{
    addr &= 0xff;

    switch ((BYTE)addr) {
      case 0:
        if (vbank == 0) {
            if (c64_256k_enabled)
               c64_256k_ram_segment0_store((WORD)0, vicii_read_phi1_lowlevel());
            else {
                if (plus256k_enabled)
                    plus256k_ram_low_store((WORD)0, vicii_read_phi1_lowlevel());
                else
                    vicii_mem_vbank_store((WORD)0, vicii_read_phi1_lowlevel());
            }
        } else {
            mem_ram[0] = vicii_read_phi1_lowlevel();
            machine_handle_pending_alarms(maincpu_rmw_flag + 1);
        }
        if (pport.dir != value) {
            pport.dir = value;
            mem_pla_config_changed();
        }
        break;
      case 1:
        if (vbank == 0) {
            if (c64_256k_enabled)
                c64_256k_ram_segment0_store((WORD)1, vicii_read_phi1_lowlevel());
            else {
                if (plus256k_enabled)
                    plus256k_ram_low_store((WORD)1, vicii_read_phi1_lowlevel());
                else
                    vicii_mem_vbank_store((WORD)1, vicii_read_phi1_lowlevel());
            }
        } else {
            mem_ram[1] = vicii_read_phi1_lowlevel();
            machine_handle_pending_alarms(maincpu_rmw_flag + 1);
        }
        if (pport.data != value) {
            pport.data = value;
            mem_pla_config_changed();
        }
        break;
      default:
        if (vbank == 0) {
            if (c64_256k_enabled)
                c64_256k_ram_segment0_store(addr, value);
            else {
                if (plus256k_enabled)
                    plus256k_ram_low_store(addr, value);
                else
                    vicii_mem_vbank_store(addr, value);
            }
        } else {
            mem_ram[addr] = value;
        }
    }
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 chargen_read(WORD addr)
{
    return mem_chargen_rom[addr & 0xfff];
}

void REGPARM2 chargen_store(WORD addr, BYTE value)
{
    mem_chargen_rom[addr & 0xfff] = value;
}

BYTE REGPARM1 ram_read(WORD addr)
{
    return mem_ram[addr];
}

void REGPARM2 ram_store(WORD addr, BYTE value)
{
    mem_ram[addr] = value;
}

void REGPARM2 ram_hi_store(WORD addr, BYTE value)
{
    if (vbank == 3)
        vicii_mem_vbank_3fxx_store(addr, value);
    else
        mem_ram[addr] = value;

    if (addr == 0xff00)
        reu_dma(-1);
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

void REGPARM2 colorram_store(WORD addr, BYTE value)
{
    mem_color_ram[addr & 0x3ff] = value & 0xf;
}

BYTE REGPARM1 colorram_read(WORD addr)
{
    return mem_color_ram[addr & 0x3ff] | (vicii_read_phi1() & 0xf0);
}

/* ------------------------------------------------------------------------- */

/* init 256k memory table changes */

static int check_256k_ram_write(int k, int i, int j)
{
  if (mem_write_tab[k][i][j]==vicii_mem_vbank_39xx_store ||
      mem_write_tab[k][i][j]==vicii_mem_vbank_3fxx_store ||
      mem_write_tab[k][i][j]==vicii_mem_vbank_store ||
      mem_write_tab[k][i][j]==ram_hi_store ||
      mem_write_tab[k][i][j]==ram_store)
    return 1;
  else
    return 0;
}

void c64_256k_init_config(void)
{
  int i,j,k;

  if (c64_256k_enabled)
  {
    mem_limit_256k_init(mem_read_limit_tab);
    for (i = 0; i < NUM_CONFIGS; i++)
    {
      for (j = 1; j <= 0xff; j++)
      {
        for (k = 0; k < NUM_VBANKS; k++)
        {
          if (check_256k_ram_write(k,i,j)==1)
          {
            if (j<0x40)
              mem_write_tab[k][i][j]=c64_256k_ram_segment0_store;
            if (j>0x3f && j<0x80)
              mem_write_tab[k][i][j]=c64_256k_ram_segment1_store;
            if (j>0x7f && j<0xc0)
              mem_write_tab[k][i][j]=c64_256k_ram_segment2_store;
            if (j>0xbf)
              mem_write_tab[k][i][j]=c64_256k_ram_segment3_store;
          }
        }
        if (mem_read_tab[i][j]==ram_read)
        {
          if (j<0x40)
            mem_read_tab[i][j]=c64_256k_ram_segment0_read;
          if (j>0x3f && j<0x80)
            mem_read_tab[i][j]=c64_256k_ram_segment1_read;
          if (j>0x7f && j<0xc0)
            mem_read_tab[i][j]=c64_256k_ram_segment2_read;
          if (j>0xbf)
            mem_read_tab[i][j]=c64_256k_ram_segment3_read;
        }
      }
    }
  }
}

/* ------------------------------------------------------------------------- */

/* init plus256k memory table changes */

void plus256k_init_config(void)
{
  int i,j,k;

  if (plus256k_enabled)
  {
    mem_limit_256k_init(mem_read_limit_tab);
    for (i = 0; i < NUM_CONFIGS; i++)
    {
      for (j = 1; j <= 0xff; j++)
      {
        for (k = 0; k < NUM_VBANKS; k++)
        {
          if (check_256k_ram_write(k,i,j)==1)
          {
            if (j<0x10)
              mem_write_tab[k][i][j]=plus256k_ram_low_store;
            else
              mem_write_tab[k][i][j]=plus256k_ram_high_store;
          }
          if (mem_write_tab[k][i][j]==vicii_store && j==0xd1)
            mem_write_tab[k][i][j]=plus256k_vicii_store;
          if (mem_write_tab[k][i][j]==vicii_store && j>0xd1)
            mem_write_tab[k][i][j]=plus256k_vicii_store0;
        }
        if (mem_read_tab[i][j]==ram_read)
        {
          if (j<0x10)
            mem_read_tab[i][j]=plus256k_ram_low_read;
          else
            mem_read_tab[i][j]=plus256k_ram_high_read;
        }
        if (mem_read_tab[i][j]==vicii_read && j==0xd1)
          mem_read_tab[i][j]=plus256k_vicii_read;
        if (mem_read_tab[i][j]==vicii_read && j>0xd1)
          mem_read_tab[i][j]=plus256k_vicii_read0;
      }
    }
  }
}

/* init plus60k memory table changes */

static void plus60k_init_config(void)
{
  int i,j,k;

  if (plus60k_enabled)
  {
    mem_limit_plus60k_init(mem_read_limit_tab);
    for (i = 0; i < NUM_CONFIGS; i++)
    {
      for (j = 0x10; j <= 0xff; j++)
      {
        for (k = 0; k < NUM_VBANKS; k++)
        {
          if (mem_write_tab[k][i][j]==vicii_mem_vbank_39xx_store)
            mem_write_tab[k][i][j]=plus60k_vicii_mem_vbank_39xx_store;
          if (mem_write_tab[k][i][j]==vicii_mem_vbank_3fxx_store)
            mem_write_tab[k][i][j]=plus60k_vicii_mem_vbank_3fxx_store;
          if (mem_write_tab[k][i][j]==vicii_mem_vbank_store)
            mem_write_tab[k][i][j]=plus60k_vicii_mem_vbank_store;
          if (mem_write_tab[k][i][j]==ram_hi_store)
            mem_write_tab[k][i][j]=plus60k_ram_hi_store;
          if (mem_write_tab[k][i][j]==vicii_store && j==0xd0 && plus60k_base==0xd040)
            mem_write_tab[k][i][j]=plus60k_vicii_store_old;
          if (mem_write_tab[k][i][j]==vicii_store && j==0xd1 && plus60k_base==0xd100)
            mem_write_tab[k][i][j]=plus60k_vicii_store;
          if (mem_write_tab[k][i][j]==vicii_store && j==0xd1 && plus60k_base==0xd040)
            mem_write_tab[k][i][j]=plus60k_vicii_store0;
          if (mem_write_tab[k][i][j]==vicii_store && j>0xd1)
            mem_write_tab[k][i][j]=plus60k_vicii_store0;
          if (mem_write_tab[k][i][j]==ram_store)
            mem_write_tab[k][i][j]=plus60k_ram_store;
        }
        if (mem_read_tab[i][j]==vicii_read && j==0xd0 && plus60k_base==0xd040)
          mem_read_tab[i][j]=plus60k_vicii_read_old;
        if (mem_read_tab[i][j]==vicii_read && j==0xd1 && plus60k_base==0xd100)
          mem_read_tab[i][j]=plus60k_vicii_read;
        if (mem_read_tab[i][j]==vicii_read && j==0xd1 && plus60k_base==0xd040)
          mem_read_tab[i][j]=plus60k_vicii_read0;
        if (mem_read_tab[i][j]==vicii_read && j>0xd1)
          mem_read_tab[i][j]=plus60k_vicii_read0;
        if (mem_read_tab[i][j]==ram_read)
          mem_read_tab[i][j]=plus60k_ram_read;
      }
    }
  }
}

/* ------------------------------------------------------------------------- */

/* init mmc64 memory table changes */

static void mmc64_init_config(void)
{
  int i,j,k;

  if (mmc64_enabled)
  {
    for (i = 0; i < NUM_CONFIGS; i++)
    {
      for (j = 0x80; j <= 0x9f; j++)
      {
        for (k = 0; k < NUM_VBANKS; k++)
        {
          if (mem_read_tab[i][j]==roml_read)
            mem_write_tab[k][i][j]=mmc64_roml_store;
        }
      }
    }
    mmc64_init_card_config();
  }
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

void mem_initialize_memory(void)
{
    int i, j, k;

    /* ROML is enabled at memory configs 11, 15, 27, 31 and Ultimax.  */
    const int roml_config[32] = { 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 1, 0, 0, 0, 1,
                                  1, 1, 1, 1, 1, 1, 1, 1,
                                  0, 0, 0, 1, 0, 0, 0, 1 };

    mem_chargen_rom_ptr = mem_chargen_rom;
    mem_color_ram_cpu = mem_color_ram;
    mem_color_ram_vicii = mem_color_ram;

    mem_limit_init(mem_read_limit_tab);

    /* Default is RAM.  */
    for (i = 0; i <= 0x100; i++) {
        mem_read_tab_watch[i] = read_watch;
        mem_write_tab_watch[i] = store_watch;
    }

    for (i = 0; i < NUM_CONFIGS; i++) {
        mem_set_write_hook(i, 0, zero_store);
        mem_read_tab[i][0] = zero_read;
        mem_read_base_tab[i][0] = mem_ram;
        for (j = 1; j <= 0xfe; j++) {
            mem_read_tab[i][j] = ram_read;
            mem_read_base_tab[i][j] = mem_ram + (j << 8);
            for (k = 0; k < NUM_VBANKS; k++) {
                if ((j & 0xc0) == (k << 6)) {
                    switch (j & 0x3f) {
                      case 0x39:
                        mem_write_tab[k][i][j] = vicii_mem_vbank_39xx_store;
                        break;
                      case 0x3f:
                        mem_write_tab[k][i][j] = vicii_mem_vbank_3fxx_store;
                        break;
                      default:
                        mem_write_tab[k][i][j] = vicii_mem_vbank_store;
                    }
                } else {
                    mem_write_tab[k][i][j] = ram_store;
                }
            }
        }
        mem_read_tab[i][0xff] = ram_read;
        mem_read_base_tab[i][0xff] = mem_ram + 0xff00;

        /* vbank access is handled within `ram_hi_store()'.  */
        mem_set_write_hook(i, 0xff, ram_hi_store);
    }

    /* Setup character generator ROM at $D000-$DFFF (memory configs 1, 2,
       3, 9, 10, 11, 25, 26, 27).  */
    for (i = 0xd0; i <= 0xdf; i++) {
        mem_read_tab[1][i] = chargen_read;
        mem_read_tab[2][i] = chargen_read;
        mem_read_tab[3][i] = chargen_read;
        mem_read_tab[9][i] = chargen_read;
        mem_read_tab[10][i] = chargen_read;
        mem_read_tab[11][i] = chargen_read;
        mem_read_tab[25][i] = chargen_read;
        mem_read_tab[26][i] = chargen_read;
        mem_read_tab[27][i] = chargen_read;
        mem_read_base_tab[1][i] = mem_chargen_rom + ((i & 0x0f) << 8);
        mem_read_base_tab[2][i] = mem_chargen_rom + ((i & 0x0f) << 8);
        mem_read_base_tab[3][i] = mem_chargen_rom + ((i & 0x0f) << 8);
        mem_read_base_tab[9][i] = mem_chargen_rom + ((i & 0x0f) << 8);
        mem_read_base_tab[10][i] = mem_chargen_rom + ((i & 0x0f) << 8);
        mem_read_base_tab[11][i] = mem_chargen_rom + ((i & 0x0f) << 8);
        mem_read_base_tab[25][i] = mem_chargen_rom + ((i & 0x0f) << 8);
        mem_read_base_tab[26][i] = mem_chargen_rom + ((i & 0x0f) << 8);
        mem_read_base_tab[27][i] = mem_chargen_rom + ((i & 0x0f) << 8);
    }

    c64meminit(0);

    for (i = 0; i < NUM_CONFIGS; i++) {
        mem_read_tab[i][0x100] = mem_read_tab[i][0];
        for (j = 0; j < NUM_VBANKS; j++) {
            mem_write_tab[j][i][0x100] = mem_write_tab[j][i][0];
        }
        mem_read_base_tab[i][0x100] = mem_read_base_tab[i][0];
    }

    _mem_read_tab_ptr = mem_read_tab[7];
    _mem_write_tab_ptr = mem_write_tab[vbank][7];
    _mem_read_base_tab_ptr = mem_read_base_tab[7];
    mem_read_limit_tab_ptr = mem_read_limit_tab[7];

    /*
     * Change address decoding.
     */
    if (mem_cartridge_type == CARTRIDGE_EXPERT) {
        /* Allow writing at ROML at $8000-$9FFF.  */
        for (j = 0; j < NUM_CONFIGS; j++) {
            if (roml_config[j]) {
                for (i = 0x80; i <= 0x9f; i++) {
                    mem_set_write_hook(j, i, roml_store);
                }
            }
        }

        /* Allow ROML being visible independent of charen, hiram & loram */
        for (j = 8; j < 16; j++) {
            for (i = 0x80; i <= 0x9f; i++) {
                mem_read_tab[j][i] = roml_read;
                mem_read_limit_tab[j][i] = -1;
                mem_read_base_tab[j][i] = NULL;
                mem_set_write_hook(j, i, roml_store);
            }
        }

        /*
         * Copy settings from "normal" operation mode into "ultimax"
         * configuration.
         */
        for (j = 16; j < 24; j++) {
            for (i = 0x10; i <= 0x7f; i++) {
                mem_read_tab[j][i] = mem_read_tab[j - 16][i];
                mem_set_write_hook(j, i, mem_write_tab[0][j - 16][i]);
                mem_read_base_tab[j][i] = mem_read_base_tab[j - 16][i];
            }
            for (i = 0xa0; i <= 0xbf; i++) {
                mem_read_tab[j][i] = mem_read_tab[j - 16][i];
                mem_set_write_hook(j, i, mem_write_tab[0][j - 16][i]);
            }
            for (i = 0xc0; i <= 0xcf; i++) {
                mem_read_tab[j][i] = mem_read_tab[j - 16][i];
                mem_set_write_hook(j, i, mem_write_tab[0][j - 16][i]);
                mem_read_base_tab[j][i] = mem_read_base_tab[j - 16][i];
            }
        }
    }

    vicii_set_chargen_addr_options(0x7000, 0x1000);

    c64pla_pport_reset();
    export.exrom = 0;
    export.game = 0;

    /* Setup initial memory configuration.  */
    mem_pla_config_changed();
    cartridge_init_config();
    ramcart_init_config();
    plus60k_init_config();
    plus256k_init_config();
    c64_256k_init_config();
    mmc64_init_config();
}

/* ------------------------------------------------------------------------- */

/* Initialize RAM for power-up.  */
void mem_powerup(void)
{
    ram_init(mem_ram, 0x10000);

    memset(export_ram0, 0xff, C64CART_RAM_LIMIT); /* Clean cartridge ram too */
}

/* ------------------------------------------------------------------------- */

/* Change the current video bank.  Call this routine only when the vbank
   has really changed.  */
void mem_set_vbank(int new_vbank)
{
    vbank = new_vbank;
    _mem_write_tab_ptr = mem_write_tab[new_vbank][mem_config];
    vicii_set_vbank(new_vbank);
}

/* Set the tape sense status.  */
void mem_set_tape_sense(int sense)
{
    tape_sense = sense;
    mem_pla_config_changed();
}

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
    if (addr >= 0xe000) {
        switch (mem_config) {
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

    return 0;
}

/* ------------------------------------------------------------------------- */

/* Banked memory access functions for the monitor.  */

/* FIXME: peek, cartridge support */

void REGPARM2 store_bank_io(WORD addr, BYTE byte)
{
    switch (addr & 0xff00) {
      case 0xd000:
      case 0xd100:
      case 0xd200:
      case 0xd300:
        vicii_store(addr, byte);
        break;
      case 0xd400:
      case 0xd500:
      case 0xd600:
      case 0xd700:
        sid_store(addr, byte);
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

BYTE REGPARM1 read_bank_io(WORD addr)
{
    switch (addr & 0xff00) {
      case 0xd000:
      case 0xd100:
      case 0xd200:
      case 0xd300:
        return vicii_read(addr);
      case 0xd400:
      case 0xd500:
      case 0xd600:
      case 0xd700:
        return sid_read(addr);
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
      case 0xd500:
      case 0xd600:
      case 0xd700:
        return sid_read(addr);
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

/* ------------------------------------------------------------------------- */

/* Exported banked memory access functions for the monitor.  */

static const char *banknames[] =
{
    "default", "cpu", "ram", "rom", "io", "cart", NULL
};

static const int banknums[] = { 1, 0, 1, 2, 3, 4 };

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
      case 3:                   /* io */
        if (addr >= 0xd000 && addr < 0xe000) {
            return read_bank_io(addr);
        }
      case 4:                   /* cart */
        if (addr >= 0x8000 && addr <= 0x9fff) {
            return roml_banks[addr & 0x1fff];
        }
        if (addr >= 0xa000 && addr <= 0xbfff) {
            return romh_banks[addr & 0x1fff];
        }
      case 2:                   /* rom */
        if (addr >= 0xa000 && addr <= 0xbfff) {
            return c64memrom_basic64_rom[addr & 0x1fff];
        }
        if (addr >= 0xd000 && addr <= 0xdfff) {
            return mem_chargen_rom[addr & 0x0fff];
        }
        if (addr >= 0xe000) {
            return c64memrom_kernal64_rom[addr & 0x1fff];
        }
      case 1:                   /* ram */
        break;
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
      case 3:                   /* io */
        if (addr >= 0xd000 && addr < 0xe000) {
            store_bank_io(addr, byte);
            return;
        }
      case 2:                   /* rom */
        if (addr >= 0xa000 && addr <= 0xbfff) {
            return;
        }
        if (addr >= 0xd000 && addr <= 0xdfff) {
            return;
        }
        if (addr >= 0xe000) {
            return;
        }
      case 1:                   /* ram */
        break;
    }
    mem_ram[addr] = byte;
}

mem_ioreg_list_t *mem_ioreg_list_get(void *context)
{
    mem_ioreg_list_t *mem_ioreg_list = NULL;

    mon_ioreg_add_list(&mem_ioreg_list, "VIC-II", 0xd000, 0xd02e);
    mon_ioreg_add_list(&mem_ioreg_list, "SID", 0xd400, 0xd41f);
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

void mem_set_exrom(int active)
{
    export.exrom = active ? 0 : 1;

    mem_pla_config_changed();
}

void mem_color_ram_to_snapshot(BYTE *color_ram)
{
    memcpy(color_ram, mem_color_ram, 0x400);
}

void mem_color_ram_from_snapshot(BYTE *color_ram)
{
    memcpy(mem_color_ram, color_ram, 0x400);
}

