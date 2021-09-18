/*
 * intl.c - Localization routines for Amiga.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef __VBCC__
#define __USE_INLINE__
#endif

#include <proto/locale.h>

#include "archdep.h"
#include "cmdline.h"
#include "intl.h"
#include "lib.h"
#include "resources.h"
#include "translate.h"
#include "util.h"
#include "ui.h"

#define countof(array) (sizeof(array) / sizeof((array)[0]))

typedef struct amiga_iso_s {
    char *amiga_locale_language;
    char *iso_language_code;
} amiga_iso_t;


/* this table holds only the currently present translation
   languages */

static amiga_iso_t amiga_to_iso[]={
  {"english.language", "en"},
  {"german.language", "de"},
  {"deutsch.language", "de"},
  {"french.language", "fr"},
  {"fran�ais.language", "fr"},
  {"hungarian.language", "hu"},
  {"italian.language", "it"},
  {"italiano.language", "it"},
  {"dutch.language", "nl"},
  {"nederlands.language", "nl"},
  {"polish.language", "pl"},
  {"polski.language", "pl"},
  {"swedish.language", "sv"},
  {"svenska.language", "sv"},
  {NULL, NULL}
};

/* The language table is duplicated in
   the translate.c, make sure they match
   when adding a new language */

static char *language_table[] = {

/* english */
  "en",

/* german */
  "de",

/* french */
  "fr",

/* hungarian */
  "hu",

/* italian */
  "it",

/* dutch */
  "nl",

/* polish */
  "pl",

/* swedish */
  "sv"
};

/* --------------------------------------------------------------------- */

typedef struct intl_translate_s {
    int resource_id;
    char *text;
} intl_translate_t;



intl_translate_t intl_string_table[] = {

/* ----------------------- AmigaOS Menu Strings ----------------------- */

/* en */ {IDMS_FILE,    "File"},
/* de */ {IDMS_FILE_DE, "Datei"},
/* fr */ {IDMS_FILE_FR, "Fichier"},
/* hu */ {IDMS_FILE_HU, "F�jl"},
/* it */ {IDMS_FILE_IT, "File"},
/* nl */ {IDMS_FILE_NL, "Bestand"},
/* pl */ {IDMS_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FILE_SV, "Arkiv"},

/* en */ {IDMS_AUTOSTART_IMAGE,    "Autostart disk/tape image..."},
/* de */ {IDMS_AUTOSTART_IMAGE_DE, "Autostart Disk/Band Image..."},
/* fr */ {IDMS_AUTOSTART_IMAGE_FR, "D�marrage automatique d'une image datassette..."},
/* hu */ {IDMS_AUTOSTART_IMAGE_HU, "Lemez/szalag k�pm�s automatikus ind�t�sa..."},
/* it */ {IDMS_AUTOSTART_IMAGE_IT, "Avvia automaticamente l'immagine di un disco/cassetta..."},
/* nl */ {IDMS_AUTOSTART_IMAGE_NL, "Autostart disk/tape bestand..."},
/* pl */ {IDMS_AUTOSTART_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_AUTOSTART_IMAGE_SV, "Autostarta disk-/bandavbildningsfil..."},

/* en */ {IDMS_ATTACH_DISK_IMAGE,    "Attach disk image"},
/* de */ {IDMS_ATTACH_DISK_IMAGE_DE, "Disk Image einlegen"},
/* fr */ {IDMS_ATTACH_DISK_IMAGE_FR, "Ins�rer une image de disque"},
/* hu */ {IDMS_ATTACH_DISK_IMAGE_HU, "Lemez k�pm�s csatol�sa"},
/* it */ {IDMS_ATTACH_DISK_IMAGE_IT, "Seleziona l'immagine di un disco"},
/* nl */ {IDMS_ATTACH_DISK_IMAGE_NL, "Koppel disk bestand"},
/* pl */ {IDMS_ATTACH_DISK_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATTACH_DISK_IMAGE_SV, "Anslut diskettavbildningsfil"},

/* en */ {IDMS_DRIVE_8,    "Drive 8"},
/* de */ {IDMS_DRIVE_8_DE, "Laufwerk 8"},
/* fr */ {IDMS_DRIVE_8_FR, "Lecteur #8"},
/* hu */ {IDMS_DRIVE_8_HU, "#8-as lemezegys�g"},
/* it */ {IDMS_DRIVE_8_IT, "Drive 8"},
/* nl */ {IDMS_DRIVE_8_NL, "Drive 8"},
/* pl */ {IDMS_DRIVE_8_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_8_SV, "Enhet 8"},

/* en */ {IDMS_DRIVE_9,    "Drive 9"},
/* de */ {IDMS_DRIVE_9_DE, "Laufwerk 9"},
/* fr */ {IDMS_DRIVE_9_FR, "Lecteur #9"},
/* hu */ {IDMS_DRIVE_9_HU, "#9-es lemezegys�g"},
/* it */ {IDMS_DRIVE_9_IT, "Drive 9"},
/* nl */ {IDMS_DRIVE_9_NL, "Drive 9"},
/* pl */ {IDMS_DRIVE_9_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_9_SV, "Enhet 9"},

/* en */ {IDMS_DRIVE_10,    "Drive 10"},
/* de */ {IDMS_DRIVE_10_DE, "Laufwerk 10"},
/* fr */ {IDMS_DRIVE_10_FR, "Lecteur #10"},
/* hu */ {IDMS_DRIVE_10_HU, "#10-es lemezegys�g"},
/* it */ {IDMS_DRIVE_10_IT, "Drive 10"},
/* nl */ {IDMS_DRIVE_10_NL, "Drive 10"},
/* pl */ {IDMS_DRIVE_10_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_10_SV, "Enhet 10"},

/* en */ {IDMS_DRIVE_11,    "Drive 11"},
/* de */ {IDMS_DRIVE_11_DE, "Laufwerk 11"},
/* fr */ {IDMS_DRIVE_11_FR, "Lecteur #11"},
/* hu */ {IDMS_DRIVE_11_HU, "#11-es lemezegys�g"},
/* it */ {IDMS_DRIVE_11_IT, "Drive 11"},
/* nl */ {IDMS_DRIVE_11_NL, "Drive 11"},
/* pl */ {IDMS_DRIVE_11_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_11_SV, "Enhet 11"},

/* en */ {IDMS_DETACH_DISK_IMAGE,    "Detach disk image"},
/* de */ {IDMS_DETACH_DISK_IMAGE_DE, "Disk Image entfernen"},
/* fr */ {IDMS_DETACH_DISK_IMAGE_FR, "Retirer une image de disque"},
/* hu */ {IDMS_DETACH_DISK_IMAGE_HU, "Lemezk�pm�s lev�laszt�sa"},
/* it */ {IDMS_DETACH_DISK_IMAGE_IT, "Rimuovi un immagine disco"},
/* nl */ {IDMS_DETACH_DISK_IMAGE_NL, "Ontkoppel disk bestand"},
/* pl */ {IDMS_DETACH_DISK_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DETACH_DISK_IMAGE_SV, "Koppla fr�n diskettavbildningsfil"},

/* en */ {IDMS_ALL,    "All"},
/* de */ {IDMS_ALL_DE, "Alle"},
/* fr */ {IDMS_ALL_FR, "Tout"},
/* hu */ {IDMS_ALL_HU, "Mindegyik"},
/* it */ {IDMS_ALL_IT, "Tutti"},
/* nl */ {IDMS_ALL_NL, "Alles"},
/* pl */ {IDMS_ALL_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ALL_SV, "Alla"},

/* en */ {IDMS_FLIP_LIST,    "Flip list"},
/* de */ {IDMS_FLIP_LIST_DE, "Flipliste"},
/* fr */ {IDMS_FLIP_LIST_FR, "Groupement de disques"},
/* hu */ {IDMS_FLIP_LIST_HU, "Lemezlista"},
/* it */ {IDMS_FLIP_LIST_IT, "Flip list"},
/* nl */ {IDMS_FLIP_LIST_NL, "Flip lijst"},
/* pl */ {IDMS_FLIP_LIST_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FLIP_LIST_SV, "Vallista"},

/* en */ {IDMS_ADD_CURRENT_IMAGE,    "Add current image"},
/* de */ {IDMS_ADD_CURRENT_IMAGE_DE, "Aktuelles Image hinzuf�gen"},
/* fr */ {IDMS_ADD_CURRENT_IMAGE_FR, "Ajouter l'image de disque courante"},
/* hu */ {IDMS_ADD_CURRENT_IMAGE_HU, "Az aktu�lis k�pm�s hozz�ad�sa"},
/* it */ {IDMS_ADD_CURRENT_IMAGE_IT, "Aggiungi l'immagine attuale"},
/* nl */ {IDMS_ADD_CURRENT_IMAGE_NL, "Voeg huidig bestand toe"},
/* pl */ {IDMS_ADD_CURRENT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ADD_CURRENT_IMAGE_SV, "L�gg till aktuell avbildning"},

/* en */ {IDMS_REMOVE_CURRENT_IMAGE,    "Remove current image"},
/* de */ {IDMS_REMOVE_CURRENT_IMAGE_DE, "Aktuelles Image entfernen"},
/* fr */ {IDMS_REMOVE_CURRENT_IMAGE_FR, "Retirer l'image de disque courante"},
/* hu */ {IDMS_REMOVE_CURRENT_IMAGE_HU, "Az aktu�lis k�pm�s elt�vol�t�sa"},
/* it */ {IDMS_REMOVE_CURRENT_IMAGE_IT, "Rimuovi l'immagine attuale"},
/* nl */ {IDMS_REMOVE_CURRENT_IMAGE_NL, "Verwijder huidig bestand"},
/* pl */ {IDMS_REMOVE_CURRENT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_REMOVE_CURRENT_IMAGE_SV, "Ta bort aktuell avbildning"},

/* en */ {IDMS_ATTACH_NEXT_IMAGE,    "Attach next image"},
/* de */ {IDMS_ATTACH_NEXT_IMAGE_DE, "N�chstes Image"},
/* fr */ {IDMS_ATTACH_NEXT_IMAGE_FR, "Ins�rer la prochaine image"},
/* hu */ {IDMS_ATTACH_NEXT_IMAGE_HU, "K�vetkez� k�pm�s csatol�sa"},
/* it */ {IDMS_ATTACH_NEXT_IMAGE_IT, "Seleziona l'immagine successiva"},
/* nl */ {IDMS_ATTACH_NEXT_IMAGE_NL, "Koppel volgend bestand"},
/* pl */ {IDMS_ATTACH_NEXT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATTACH_NEXT_IMAGE_SV, "Anslut n�sta avbildning"},

/* en */ {IDMS_ATTACH_PREVIOUS_IMAGE,    "Attach previous image"},
/* de */ {IDMS_ATTACH_PREVIOUS_IMAGE_DE, "Voriges Image"},
/* fr */ {IDMS_ATTACH_PREVIOUS_IMAGE_FR, "Ins�rer l'image pr�c�dente"},
/* hu */ {IDMS_ATTACH_PREVIOUS_IMAGE_HU, "El�z� k�pm�s csatol�sa"},
/* it */ {IDMS_ATTACH_PREVIOUS_IMAGE_IT, "Seleziona l'immagine precedente"},
/* nl */ {IDMS_ATTACH_PREVIOUS_IMAGE_NL, "Koppel vorig bestand"},
/* pl */ {IDMS_ATTACH_PREVIOUS_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATTACH_PREVIOUS_IMAGE_SV, "Anslut f�reg�ende avbildning"},

/* en */ {IDMS_LOAD_FLIP_LIST,    "Load flip list"},
/* de */ {IDMS_LOAD_FLIP_LIST_DE, "Fliplist Datei Laden"},
/* fr */ {IDMS_LOAD_FLIP_LIST_FR, "Charger un groupement de disques"},
/* hu */ {IDMS_LOAD_FLIP_LIST_HU, "Lemezlista bet�lt�se"},
/* it */ {IDMS_LOAD_FLIP_LIST_IT, "Carica flip list"},
/* nl */ {IDMS_LOAD_FLIP_LIST_NL, "Laad flip lijst"},
/* pl */ {IDMS_LOAD_FLIP_LIST_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LOAD_FLIP_LIST_SV, "L�s in vallistefil"},

/* en */ {IDMS_SAVE_FLIP_LIST,    "Save flip list"},
/* de */ {IDMS_SAVE_FLIP_LIST_DE, "Fliplist Datei Speichern"},
/* fr */ {IDMS_SAVE_FLIP_LIST_FR, "Enregistrer le groupement de disques"},
/* hu */ {IDMS_SAVE_FLIP_LIST_HU, "Lemezlista ment�se"},
/* it */ {IDMS_SAVE_FLIP_LIST_IT, "Salva fliplist"},
/* nl */ {IDMS_SAVE_FLIP_LIST_NL, "Opslaan flip lijst"},
/* pl */ {IDMS_SAVE_FLIP_LIST_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_FLIP_LIST_SV, "Spara vallistefil"},

/* en */ {IDMS_ATTACH_TAPE_IMAGE,    "Attach tape image..."},
/* de */ {IDMS_ATTACH_TAPE_IMAGE_DE, "Band Image einlegen..."},
/* fr */ {IDMS_ATTACH_TAPE_IMAGE_FR, "Ins�rer une image datassette..."},
/* hu */ {IDMS_ATTACH_TAPE_IMAGE_HU, "Szalag k�pm�s csatol�sa..."},
/* it */ {IDMS_ATTACH_TAPE_IMAGE_IT, "Seleziona l'immagine di una cassetta..."},
/* nl */ {IDMS_ATTACH_TAPE_IMAGE_NL, "Koppel tape bestand..."},
/* pl */ {IDMS_ATTACH_TAPE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATTACH_TAPE_IMAGE_SV, "Anslut en bandavbildningsfil..."},

/* en */ {IDMS_DETACH_TAPE_IMAGE,    "Detach tape image"},
/* de */ {IDMS_DETACH_TAPE_IMAGE_DE, "Band Image entfernen"},
/* fr */ {IDMS_DETACH_TAPE_IMAGE_FR, "Retirer une image datassette"},
/* hu */ {IDMS_DETACH_TAPE_IMAGE_HU, "Szalag k�pm�s lev�laszt�sa"},
/* it */ {IDMS_DETACH_TAPE_IMAGE_IT, "Rimuovi immagine cassetta"},
/* nl */ {IDMS_DETACH_TAPE_IMAGE_NL, "Ontkoppel tape image"},
/* pl */ {IDMS_DETACH_TAPE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DETACH_TAPE_IMAGE_SV, "Koppla fr�n bandavbildningsfil"},

/* en */ {IDMS_DATASSETTE_CONTROL,    "Datassette control"},
/* de */ {IDMS_DATASSETTE_CONTROL_DE, "Bandlaufwerk Kontrolle"},
/* fr */ {IDMS_DATASSETTE_CONTROL_FR, "Contr�le datassette"},
/* hu */ {IDMS_DATASSETTE_CONTROL_HU, "Magn� vez�rl�s"},
/* it */ {IDMS_DATASSETTE_CONTROL_IT, "Controlli del registratore"},
/* nl */ {IDMS_DATASSETTE_CONTROL_NL, "Datassette bediening"},
/* pl */ {IDMS_DATASSETTE_CONTROL_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DATASSETTE_CONTROL_SV, "Datasettestyrning"},

/* en */ {IDMS_STOP,    "Stop"},
/* de */ {IDMS_STOP_DE, "Stop"},
/* fr */ {IDMS_STOP_FR, "Arr�t"},
/* hu */ {IDMS_STOP_HU, "Le�ll�t�s"},
/* it */ {IDMS_STOP_IT, "Stop"},
/* nl */ {IDMS_STOP_NL, "Stop"},
/* pl */ {IDMS_STOP_PL, ""},  /* fuzzy */
/* sv */ {IDMS_STOP_SV, "Stoppa"},

/* en */ {IDMS_START,    "Start"},
/* de */ {IDMS_START_DE, "Start"},
/* fr */ {IDMS_START_FR, "D�marrer"},
/* hu */ {IDMS_START_HU, "Ind�t�s"},
/* it */ {IDMS_START_IT, "Avvia"},
/* nl */ {IDMS_START_NL, "Start"},
/* pl */ {IDMS_START_PL, ""},  /* fuzzy */
/* sv */ {IDMS_START_SV, "Starta"},

/* en */ {IDMS_FORWARD,    "Forward"},
/* de */ {IDMS_FORWARD_DE, "Forward"},
/* fr */ {IDMS_FORWARD_FR, "En avant"},
/* hu */ {IDMS_FORWARD_HU, "El�recs�v�l�s"},
/* it */ {IDMS_FORWARD_IT, "Avanti"},
/* nl */ {IDMS_FORWARD_NL, "Vooruit"},
/* pl */ {IDMS_FORWARD_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FORWARD_SV, "Spola fram�t"},

/* en */ {IDMS_REWIND,    "Rewind"},
/* de */ {IDMS_REWIND_DE, "Rewind"},
/* fr */ {IDMS_REWIND_FR, "En arri�re"},
/* hu */ {IDMS_REWIND_HU, "Visszacs�v�l�s"},
/* it */ {IDMS_REWIND_IT, "Indietro"},
/* nl */ {IDMS_REWIND_NL, "Terug"},
/* pl */ {IDMS_REWIND_PL, ""},  /* fuzzy */
/* sv */ {IDMS_REWIND_SV, "Spola bak�t"},

/* en */ {IDMS_RECORD,    "Record"},
/* de */ {IDMS_RECORD_DE, "Record"},
/* fr */ {IDMS_RECORD_FR, "Enregistrer"},
/* hu */ {IDMS_RECORD_HU, "Felv�tel"},
/* it */ {IDMS_RECORD_IT, "Registra"},
/* nl */ {IDMS_RECORD_NL, "Opname"},
/* pl */ {IDMS_RECORD_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RECORD_SV, "Spela in"},

/* en */ {IDMS_RESET,    "Reset"},
/* de */ {IDMS_RESET_DE, "Reset"},
/* fr */ {IDMS_RESET_FR, "R�initialiser"},
/* hu */ {IDMS_RESET_HU, "Reset"},
/* it */ {IDMS_RESET_IT, "Reset"},
/* nl */ {IDMS_RESET_NL, "Reset"},
/* pl */ {IDMS_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RESET_SV, "Nollst�ll"},

/* en */ {IDMS_RESET_COUNTER,    "Reset Counter"},
/* de */ {IDMS_RESET_COUNTER_DE, "Z�hler zur�cksetzen"},
/* fr */ {IDMS_RESET_COUNTER_FR, "R�inialiser le compteur"},
/* hu */ {IDMS_RESET_COUNTER_HU, "Sz�ml�l� null�z�sa"},
/* it */ {IDMS_RESET_COUNTER_IT, "Reset contantore"},
/* nl */ {IDMS_RESET_COUNTER_NL, "Reset teller"},
/* pl */ {IDMS_RESET_COUNTER_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RESET_COUNTER_SV, "Nollst�ll r�knare"},

/* en */ {IDMS_ATTACH_CART_IMAGE,    "Attach cartridge image..."},
/* de */ {IDMS_ATTACH_CART_IMAGE_DE, "Erweiterungsmodule einlegen..."},
/* fr */ {IDMS_ATTACH_CART_IMAGE_FR, "Ins�rer une cartouche..."},
/* hu */ {IDMS_ATTACH_CART_IMAGE_HU, "Cartridge k�pm�s csatol�sa..."},
/* it */ {IDMS_ATTACH_CART_IMAGE_IT, "Seleziona l'immagine di una cartuccia..."},
/* nl */ {IDMS_ATTACH_CART_IMAGE_NL, "Koppel cartridge bestand..."},
/* pl */ {IDMS_ATTACH_CART_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATTACH_CART_IMAGE_SV, "Anslut insticksmodulfil..."},

/* en */ {IDMS_4_8_16KB_AT_2000,    "4/8/16KB image at $2000..."},
/* de */ {IDMS_4_8_16KB_AT_2000_DE, "4/8/16KB Modul Image bei $2000..."},
/* fr */ {IDMS_4_8_16KB_AT_2000_FR, "Insertion d'une image 4/8/16Ko � $2000..."},
/* hu */ {IDMS_4_8_16KB_AT_2000_HU, "4/8/16KB k�pm�s $2000 c�men..."},
/* it */ {IDMS_4_8_16KB_AT_2000_IT, "Immagine di 4/8/16KB a $2000..."},
/* nl */ {IDMS_4_8_16KB_AT_2000_NL, "Koppel 4/8/16KB bestand in $2000..."},
/* pl */ {IDMS_4_8_16KB_AT_2000_PL, ""},  /* fuzzy */
/* sv */ {IDMS_4_8_16KB_AT_2000_SV, "4/8/16KB-fil vid $2000..."},

/* en */ {IDMS_4_8_16KB_AT_4000,    "4/8/16KB image at $4000..."},
/* de */ {IDMS_4_8_16KB_AT_4000_DE, "4/8/16KB Modul Image bei $4000..."},
/* fr */ {IDMS_4_8_16KB_AT_4000_FR, "Insertion d'une image 4/8/16Ko � $4000..."},
/* hu */ {IDMS_4_8_16KB_AT_4000_HU, "4/8/16KB k�pm�s $4000 c�men..."},
/* it */ {IDMS_4_8_16KB_AT_4000_IT, "Immagine di 4/8/16KB a $4000..."},
/* nl */ {IDMS_4_8_16KB_AT_4000_NL, "Koppel 4/8/16KB bestand in $4000..."},
/* pl */ {IDMS_4_8_16KB_AT_4000_PL, ""},  /* fuzzy */
/* sv */ {IDMS_4_8_16KB_AT_4000_SV, "4/8/16KB-fil vid $4000..."},

/* en */ {IDMS_4_8_16KB_AT_6000,    "4/8/16KB image at $6000..."},
/* de */ {IDMS_4_8_16KB_AT_6000_DE, "4/8/16KB Modul Image bei $6000..."},
/* fr */ {IDMS_4_8_16KB_AT_6000_FR, "Insertion d'une image 4/8/16Ko � $6000..."},
/* hu */ {IDMS_4_8_16KB_AT_6000_HU, "4/8/16KB k�pm�s $6000 c�men..."},
/* it */ {IDMS_4_8_16KB_AT_6000_IT, "Immagine di 4/8/16KB a $6000..."},
/* nl */ {IDMS_4_8_16KB_AT_6000_NL, "Koppel 4/8/16KB bestand in $6000..."},
/* pl */ {IDMS_4_8_16KB_AT_6000_PL, ""},  /* fuzzy */
/* sv */ {IDMS_4_8_16KB_AT_6000_SV, "4/8/16KB-fil vid $6000..."},

/* en */ {IDMS_4_8KB_AT_A000,    "4/8KB image at $A000..."},
/* de */ {IDMS_4_8KB_AT_A000_DE, "4/8KB Modul Image bei $A000..."},
/* fr */ {IDMS_4_8KB_AT_A000_FR, "Insertion d'une image 4/8Ko � $A000..."},
/* hu */ {IDMS_4_8KB_AT_A000_HU, "4/8KB k�pm�s $A000 c�men..."},
/* it */ {IDMS_4_8KB_AT_A000_IT, "Immagine di 4/8KB a $A000..."},
/* nl */ {IDMS_4_8KB_AT_A000_NL, "Koppel 4/8KB bestand in $A000..."},
/* pl */ {IDMS_4_8KB_AT_A000_PL, ""},  /* fuzzy */
/* sv */ {IDMS_4_8KB_AT_A000_SV, "4/8KB-fil vid $A000..."},

/* en */ {IDMS_4KB_AT_B000,    "4KB image at $B000..."},
/* de */ {IDMS_4KB_AT_B000_DE, "4KB Modul Image bei $B000..."},
/* fr */ {IDMS_4KB_AT_B000_FR, "Insertion d'une image 4Ko � $B000..."},
/* it */ {IDMS_4KB_AT_B000_IT, "Immagine di 4KB a $B000..."},
/* hu */ {IDMS_4KB_AT_B000_HU, "4KB k�pm�s $B000 c�men..."},
/* nl */ {IDMS_4KB_AT_B000_NL, "Koppel 4KB bestand in $B000..."},
/* pl */ {IDMS_4KB_AT_B000_PL, ""},  /* fuzzy */
/* sv */ {IDMS_4KB_AT_B000_SV, "4KB-fil vid $B000..."},

/* en */ {IDMS_DETACH_CART_IMAGE,    "Detach cartridge image"},
/* de */ {IDMS_DETACH_CART_IMAGE_DE, "Erweiterungsmodul Image entfernen"},
/* fr */ {IDMS_DETACH_CART_IMAGE_FR, "Retirer une cartouche"},
/* hu */ {IDMS_DETACH_CART_IMAGE_HU, "Cartridge k�pm�s lev�laszt�sa"},
/* it */ {IDMS_DETACH_CART_IMAGE_IT, "Rimuovi immagine cartuccia"},
/* nl */ {IDMS_DETACH_CART_IMAGE_NL, "Ontkoppel cartridge bestand"},
/* pl */ {IDMS_DETACH_CART_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DETACH_CART_IMAGE_SV, "Koppla fr�n insticksmodulfil"},

/* en */ {IDMS_C1_LOW_IMAGE,    "C1 low  image..."},
/* de */ {IDMS_C1_LOW_IMAGE_DE, "C1 low Image Datei..."},
/* fr */ {IDMS_C1_LOW_IMAGE_FR, "Image C1 basse..."},
/* hu */ {IDMS_C1_LOW_IMAGE_HU, "C1 als� k�pm�s..."},
/* it */ {IDMS_C1_LOW_IMAGE_IT, "Immagine nell'area bassa di C1..."},
/* nl */ {IDMS_C1_LOW_IMAGE_NL, "C1 low  bestand..."},
/* pl */ {IDMS_C1_LOW_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_C1_LOW_IMAGE_SV, "C1 l�g-avbildning..."},

/* en */ {IDMS_C1_HIGH_IMAGE,    "C1 high image..."},
/* de */ {IDMS_C1_HIGH_IMAGE_DE, "C1 high Image Datei..."},
/* fr */ {IDMS_C1_HIGH_IMAGE_FR, "Image C1 haute..."},
/* hu */ {IDMS_C1_HIGH_IMAGE_HU, "C1 fels� k�pm�s..."},
/* it */ {IDMS_C1_HIGH_IMAGE_IT, "Immagine nell'area alta di C1..."},
/* nl */ {IDMS_C1_HIGH_IMAGE_NL, "C1 high bestand..."},
/* pl */ {IDMS_C1_HIGH_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_C1_HIGH_IMAGE_SV, "C1 h�g-avbildning..."},

/* en */ {IDMS_C2_LOW_IMAGE,    "C2 low  image..."},
/* de */ {IDMS_C2_LOW_IMAGE_DE, "C2 low Image Datei..."},
/* fr */ {IDMS_C2_LOW_IMAGE_FR, "Image C2 basse..."},
/* it */ {IDMS_C2_LOW_IMAGE_IT, "Immagine nell'area bassa di C2..."},
/* hu */ {IDMS_C2_LOW_IMAGE_HU, "C2 als� k�pm�s..."},
/* nl */ {IDMS_C2_LOW_IMAGE_NL, "C2 low  bestand..."},
/* pl */ {IDMS_C2_LOW_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_C2_LOW_IMAGE_SV, "C2 l�g-avbildning..."},

/* en */ {IDMS_C2_HIGH_IMAGE,    "C2 high image..."},
/* de */ {IDMS_C2_HIGH_IMAGE_DE, "C2 high Image Datei..."},
/* fr */ {IDMS_C2_HIGH_IMAGE_FR, "Image C2 haute..."},
/* hu */ {IDMS_C2_HIGH_IMAGE_HU, "C2 fels� k�pm�s..."},
/* it */ {IDMS_C2_HIGH_IMAGE_IT, "Immagine nell'area alta di C2..."},
/* nl */ {IDMS_C2_HIGH_IMAGE_NL, "C2 high bestand..."},
/* pl */ {IDMS_C2_HIGH_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_C2_HIGH_IMAGE_SV, "C2 h�g-avbildning..."},

/* en */ {IDMS_FUNCTION_LOW_3PLUS1,    "Function low  image (3plus1)..."},
/* de */ {IDMS_FUNCTION_LOW_3PLUS1_DE, "Funktions ROM Image Datei low (3plus1)..."},
/* fr */ {IDMS_FUNCTION_LOW_3PLUS1_FR, "Image Fonction basse (3+1)..."},
/* hu */ {IDMS_FUNCTION_LOW_3PLUS1_HU, "Function als� k�pm�s (3plus1)..."},
/* it */ {IDMS_FUNCTION_LOW_3PLUS1_IT, "Function (3plus1) caricata nella memoria bassa..."},
/* nl */ {IDMS_FUNCTION_LOW_3PLUS1_NL, "Function low  bestand (3plus1)..."},
/* pl */ {IDMS_FUNCTION_LOW_3PLUS1_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FUNCTION_LOW_3PLUS1_SV, "Funktion l�g-avbildning (3plus1)..."},

/* en */ {IDMS_FUNCTION_HIGH_3PLUS1,    "Function high image (3plus1)..."},
/* de */ {IDMS_FUNCTION_HIGH_3PLUS1_DE, "Funktions ROM Image Datei high (3plus1)..."},
/* fr */ {IDMS_FUNCTION_HIGH_3PLUS1_FR, "Image Fonction haute (3+1)..."},
/* hu */ {IDMS_FUNCTION_HIGH_3PLUS1_HU, "Function fels� k�pm�s (3plus1)..."},
/* it */ {IDMS_FUNCTION_HIGH_3PLUS1_IT, "Function (3plus1) caricata nella memoria alta..."},
/* nl */ {IDMS_FUNCTION_HIGH_3PLUS1_NL, "Function high bestand (3plus1)..."},
/* pl */ {IDMS_FUNCTION_HIGH_3PLUS1_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FUNCTION_HIGH_3PLUS1_SV, "Funktion h�g-avbildning (3plus1)..."},

/* en */ {IDMS_CRT_IMAGE,    "CRT image..."},
/* de */ {IDMS_CRT_IMAGE_DE, "CRT Image Datei..."},
/* fr */ {IDMS_CRT_IMAGE_FR, "Nom du fichier image CRT..."},
/* hu */ {IDMS_CRT_IMAGE_HU, "CRT k�pm�s..."},
/* it */ {IDMS_CRT_IMAGE_IT, "Immagine CRT..."},
/* nl */ {IDMS_CRT_IMAGE_NL, "CRT bestand..."},
/* pl */ {IDMS_CRT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CRT_IMAGE_SV, "CRT-fil..."},

/* en */ {IDMS_GENERIC_8KB_IMAGE,    "Generic 8KB image..."},
/* de */ {IDMS_GENERIC_8KB_IMAGE_DE, "Generische 8Kb Image..."},
/* fr */ {IDMS_GENERIC_8KB_IMAGE_FR, "Ins�rer une image g�n�rique de 8Ko..."},
/* hu */ {IDMS_GENERIC_8KB_IMAGE_HU, "�ltal�nos 8KB k�pm�s..."},
/* it */ {IDMS_GENERIC_8KB_IMAGE_IT, "Immagine generica di 8KB..."},
/* nl */ {IDMS_GENERIC_8KB_IMAGE_NL, "Algemeen 8KB bestand..."},
/* pl */ {IDMS_GENERIC_8KB_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_GENERIC_8KB_IMAGE_SV, "Vanlig 8KB-fil..."},

/* en */ {IDMS_GENERIC_16KB_IMAGE,    "Generic 16KB image..."},
/* de */ {IDMS_GENERIC_16KB_IMAGE_DE, "Generische 16Kb Image..."},
/* fr */ {IDMS_GENERIC_16KB_IMAGE_FR, "Ins�rer une image g�n�rique de 16Ko..."},
/* hu */ {IDMS_GENERIC_16KB_IMAGE_HU, "�ltal�nos 16KB k�pm�s..."},
/* it */ {IDMS_GENERIC_16KB_IMAGE_IT, "Immagine generica di 16KB..."},
/* nl */ {IDMS_GENERIC_16KB_IMAGE_NL, "Algemeen 16KB bestand..."},
/* pl */ {IDMS_GENERIC_16KB_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_GENERIC_16KB_IMAGE_SV, "Vanlig 16KB-fil..."},

/* en */ {IDMS_ACTION_REPLAY_IMAGE,    "Action Replay image..."},
/* de */ {IDMS_ACTION_REPLAY_IMAGE_DE, "Action Replay Image..."},
/* fr */ {IDMS_ACTION_REPLAY_IMAGE_FR, "Ins�rer une cartouche Action Replay..."},
/* hu */ {IDMS_ACTION_REPLAY_IMAGE_HU, "Action Replay k�pm�s..."},
/* it */ {IDMS_ACTION_REPLAY_IMAGE_IT, "Immagine Action Replay..."},
/* nl */ {IDMS_ACTION_REPLAY_IMAGE_NL, "Action Replay bestand"},
/* pl */ {IDMS_ACTION_REPLAY_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ACTION_REPLAY_IMAGE_SV, "Action Replay-fil..."},

/* en */ {IDMS_ATOMIC_POWER_IMAGE,    "Atomic Power image..."},
/* de */ {IDMS_ATOMIC_POWER_IMAGE_DE, "Atomic Power Image..."},
/* fr */ {IDMS_ATOMIC_POWER_IMAGE_FR, "Ins�rer une cartouche Atomic Power..."},
/* hu */ {IDMS_ATOMIC_POWER_IMAGE_HU, "Atomic Power k�pm�s..."},
/* it */ {IDMS_ATOMIC_POWER_IMAGE_IT, "Immagine Atomic Power..."},
/* nl */ {IDMS_ATOMIC_POWER_IMAGE_NL, "Atomic Power bestand..."},
/* pl */ {IDMS_ATOMIC_POWER_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ATOMIC_POWER_IMAGE_SV, "Atomic Power-fil..."},

/* en */ {IDMS_EPYX_FASTLOAD_IMAGE,    "Epyx fastload image..."},
/* de */ {IDMS_EPYX_FASTLOAD_IMAGE_DE, "Epyx Fastload Image..."},
/* fr */ {IDMS_EPYX_FASTLOAD_IMAGE_FR, "Ins�rer une cartouche Epyx FastLoad..."},
/* hu */ {IDMS_EPYX_FASTLOAD_IMAGE_HU, "Epyx gyorst�lt� k�pm�s..."},
/* it */ {IDMS_EPYX_FASTLOAD_IMAGE_IT, "Immagine Epyx fastload..."},
/* nl */ {IDMS_EPYX_FASTLOAD_IMAGE_NL, "Epyx fastload bestand..."},
/* pl */ {IDMS_EPYX_FASTLOAD_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_EPYX_FASTLOAD_IMAGE_SV, "Epyx fastload-fil..."},

/* en */ {IDMS_IEEE488_INTERFACE_IMAGE,    "IEEE488 interface image..."},
/* de */ {IDMS_IEEE488_INTERFACE_IMAGE_DE, "IEEE 488 Schnittstellenmodul..."},
/* fr */ {IDMS_IEEE488_INTERFACE_IMAGE_FR, "Ins�rer une cartouche interface IEEE488..."},
/* hu */ {IDMS_IEEE488_INTERFACE_IMAGE_HU, "IEEE488 interf�sz k�pm�s..."},
/* it */ {IDMS_IEEE488_INTERFACE_IMAGE_IT, "Immagine dell'interfaccia IEEE488..."},
/* nl */ {IDMS_IEEE488_INTERFACE_IMAGE_NL, "IEEE488 interface bestand..."},
/* pl */ {IDMS_IEEE488_INTERFACE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_IEEE488_INTERFACE_IMAGE_SV, "IEEE488-gr�nssnittsfil..."},

/* en */ {IDMS_RETRO_REPLAY_IMAGE,    "Retro Replay image..."},
/* de */ {IDMS_RETRO_REPLAY_IMAGE_DE, "Retro Replay Image..."},
/* fr */ {IDMS_RETRO_REPLAY_IMAGE_FR, "Ins�rer une cartouche Retro Replay..."},
/* hu */ {IDMS_RETRO_REPLAY_IMAGE_HU, "Retro Replay k�pm�s..."},
/* it */ {IDMS_RETRO_REPLAY_IMAGE_IT, "Immagine Retro Replay..."},
/* nl */ {IDMS_RETRO_REPLAY_IMAGE_NL, "Retro Replay bestand..."},
/* pl */ {IDMS_RETRO_REPLAY_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RETRO_REPLAY_IMAGE_SV, "Retro Replay-fil..."},

/* en */ {IDMS_IDE64_INTERFACE_IMAGE,    "IDE64 interface image..."},
/* de */ {IDMS_IDE64_INTERFACE_IMAGE_DE, "IDE64 Schnittstellenmodul..."},
/* fr */ {IDMS_IDE64_INTERFACE_IMAGE_FR, "Ins�rer une cartouche interface IDE64..."},
/* hu */ {IDMS_IDE64_INTERFACE_IMAGE_HU, "IDE64 interf�sz k�pm�s..."},
/* it */ {IDMS_IDE64_INTERFACE_IMAGE_IT, "Immagine dell'interfaccia IDE64..."},
/* nl */ {IDMS_IDE64_INTERFACE_IMAGE_NL, "IDE64 interface bestand..."},
/* pl */ {IDMS_IDE64_INTERFACE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_IDE64_INTERFACE_IMAGE_SV, "IDE64-gr�nssnittsfil..."},

/* en */ {IDMS_SUPER_SNAPSHOT_4_IMAGE,    "Super Snapshot 4 image..."},
/* de */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_DE, "Super Snapshot 4 Image..."},
/* fr */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_FR, "Ins�rer une cartouche Super Snapshot 4..."},
/* hu */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_HU, "Super Snapshot 4 k�pm�s..."},
/* it */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_IT, "Immagine Super Snapshot 4..."},
/* nl */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_NL, "Super Snapshot 4 bestand..."},
/* pl */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SUPER_SNAPSHOT_4_IMAGE_SV, "Super Snapshot 4-fil..."},

/* en */ {IDMS_SUPER_SNAPSHOT_5_IMAGE,    "Super Snapshot 5 image..."},
/* de */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_DE, "Super Snapshot 5 Image..."},
/* fr */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_FR, "Ins�rer une cartouche Super Snapshot 5..."},
/* hu */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_HU, "Super Snapshot 5 k�pm�s..."},
/* it */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_IT, "Immagine Super Snapshot 5..."},
/* nl */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_NL, "Super Snapshot 5 bestand..."},
/* pl */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SUPER_SNAPSHOT_5_IMAGE_SV, "Super Snapshot 5-fil..."},

/* en */ {IDMS_STRUCTURED_BASIC_IMAGE,    "Structured Basic image..."},
/* de */ {IDMS_STRUCTURED_BASIC_IMAGE_DE, "Structured Basic Image..."},
/* fr */ {IDMS_STRUCTURED_BASIC_IMAGE_FR, "Ins�rer une cartouche Structured Basic..."},
/* hu */ {IDMS_STRUCTURED_BASIC_IMAGE_HU, "Structured Basic k�pm�s..."},
/* it */ {IDMS_STRUCTURED_BASIC_IMAGE_IT, "Immagine Structured Basic..."},
/* nl */ {IDMS_STRUCTURED_BASIC_IMAGE_NL, "Structured Basic bestand..."},
/* pl */ {IDMS_STRUCTURED_BASIC_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_STRUCTURED_BASIC_IMAGE_SV, "Structured Basic-fil..."},

/* en */ {IDMS_EXPERT_CART,    "Expert Cartridge"},
/* de */ {IDMS_EXPERT_CART_DE, "Expert Cartridge"},
/* fr */ {IDMS_EXPERT_CART_FR, "Activer les param�tres des cartouches experts"},
/* hu */ {IDMS_EXPERT_CART_HU, "Expert cartridge"},
/* it */ {IDMS_EXPERT_CART_IT, "Expert Cartridge"},
/* nl */ {IDMS_EXPERT_CART_NL, "Expert Cartridge"},
/* pl */ {IDMS_EXPERT_CART_PL, ""},  /* fuzzy */
/* sv */ {IDMS_EXPERT_CART_SV, "Expert Cartridge"},

/* en */ {IDMS_ENABLE,    "Enable"},
/* de */ {IDMS_ENABLE_DE, "Aktivieren"},
/* fr */ {IDMS_ENABLE_FR, "Activer"},
/* hu */ {IDMS_ENABLE_HU, "Enged�lyez�s"},
/* it */ {IDMS_ENABLE_IT, "Abilita"},
/* nl */ {IDMS_ENABLE_NL, "Aktiveer"},
/* pl */ {IDMS_ENABLE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ENABLE_SV, "Aktivera"},

/* en */ {IDMS_OFF,    "Off"},
/* de */ {IDMS_OFF_DE, "Aus"},
/* fr */ {IDMS_OFF_FR, "Off"},
/* hu */ {IDMS_OFF_HU, "Ki"},
/* it */ {IDMS_OFF_IT, "Off"},
/* nl */ {IDMS_OFF_NL, "Uit"},
/* pl */ {IDMS_OFF_PL, ""},  /* fuzzy */
/* sv */ {IDMS_OFF_SV, "Av"},

/* en */ {IDMS_PRG,    "Prg"},
/* de */ {IDMS_PRG_DE, "Prg"},
/* fr */ {IDMS_PRG_FR, "Prg"},
/* hu */ {IDMS_PRG_HU, "Prg"},
/* it */ {IDMS_PRG_IT, "Prg"},
/* nl */ {IDMS_PRG_NL, "Prg"},
/* pl */ {IDMS_PRG_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PRG_SV, "Prg"},

/* en */ {IDMS_ON,    "On"},
/* de */ {IDMS_ON_DE, "Ein"},
/* fr */ {IDMS_ON_FR, "On"},
/* hu */ {IDMS_ON_HU, "Be"},
/* it */ {IDMS_ON_IT, "On"},
/* nl */ {IDMS_ON_NL, "Aan"},
/* pl */ {IDMS_ON_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ON_SV, "P�"},

/* en */ {IDMS_SET_CART_AS_DEFAULT,    "Set cartridge as default"},
/* de */ {IDMS_SET_CART_AS_DEFAULT_DE, "Aktuelles Erweiterungsmodul als standard aktivieren"},
/* fr */ {IDMS_SET_CART_AS_DEFAULT_FR, "D�finir cette cartouche par d�faut"},
/* hu */ {IDMS_SET_CART_AS_DEFAULT_HU, "Cartridge alap�rtelmezett� t�tele"},
/* it */ {IDMS_SET_CART_AS_DEFAULT_IT, "Imposta la cartuccia come predefinita"},
/* nl */ {IDMS_SET_CART_AS_DEFAULT_NL, "Zet cartridge als standaard"},
/* pl */ {IDMS_SET_CART_AS_DEFAULT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SET_CART_AS_DEFAULT_SV, "V�lj insticksmodul som f�rval"},

/* en */ {IDMS_RESET_ON_CART_CHANGE,    "Reset on cart change"},
/* de */ {IDMS_RESET_ON_CART_CHANGE_DE, "Reset bei Wechsel von Erweiterungsmodulen"},
/* fr */ {IDMS_RESET_ON_CART_CHANGE_FR, "R�inialiser sur changement de cartouche"},
/* hu */ {IDMS_RESET_ON_CART_CHANGE_HU, "�jraind�t�s cartridge cser�n�l"},
/* it */ {IDMS_RESET_ON_CART_CHANGE_IT, "Reset al cambio di cartuccia"},
/* nl */ {IDMS_RESET_ON_CART_CHANGE_NL, "Reset bij cart wisseling"},
/* pl */ {IDMS_RESET_ON_CART_CHANGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RESET_ON_CART_CHANGE_SV, "Nollst�ll vid modulbyte"},

/* en */ {IDMS_CART_FREEZE,    "Cartridge freeze"},
/* de */ {IDMS_CART_FREEZE_DE, "Erweiterungsmodul Freeze"},
/* fr */ {IDMS_CART_FREEZE_FR, "Geler la cartouche"},
/* hu */ {IDMS_CART_FREEZE_HU, "Fagyaszt�s Cartridge-el"},
/* it */ {IDMS_CART_FREEZE_IT, "Freeze della cartuccia"},
/* nl */ {IDMS_CART_FREEZE_NL, "Cartridge freeze"},
/* pl */ {IDMS_CART_FREEZE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CART_FREEZE_SV, "Frys"},

/* en */ {IDMS_PAUSE,    "Pause"},
/* de */ {IDMS_PAUSE_DE, "Pause"},
/* fr */ {IDMS_PAUSE_FR, "Pause"},
/* hu */ {IDMS_PAUSE_HU, "Sz�net"},
/* it */ {IDMS_PAUSE_IT, "Pausa"},
/* nl */ {IDMS_PAUSE_NL, "Pauze"},
/* pl */ {IDMS_PAUSE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PAUSE_SV, "Paus"},

/* en */ {IDMS_MONITOR,    "Monitor"},
/* de */ {IDMS_MONITOR_DE, "Monitor"},
/* fr */ {IDMS_MONITOR_FR, "Tracer"},
/* hu */ {IDMS_MONITOR_HU, "Monitor"},
/* it */ {IDMS_MONITOR_IT, "Monitor"},
/* nl */ {IDMS_MONITOR_NL, "Monitor"},
/* pl */ {IDMS_MONITOR_PL, ""},  /* fuzzy */
/* sv */ {IDMS_MONITOR_SV, "Monitor"},

/* en */ {IDMS_HARD,    "Hard"},
/* de */ {IDMS_HARD_DE, "Hart"},
/* fr */ {IDMS_HARD_FR, "Physiquement"},
/* hu */ {IDMS_HARD_HU, "Hideg"},
/* it */ {IDMS_HARD_IT, "Hard"},
/* nl */ {IDMS_HARD_NL, "Hard"},
/* pl */ {IDMS_HARD_PL, ""},  /* fuzzy */
/* sv */ {IDMS_HARD_SV, "H�rd"},

/* en */ {IDMS_SOFT,    "Soft"},
/* de */ {IDMS_SOFT_DE, "Soft"},
/* fr */ {IDMS_SOFT_FR, "Logiciellement"},
/* hu */ {IDMS_SOFT_HU, "Meleg"},
/* it */ {IDMS_SOFT_IT, "Soft"},
/* nl */ {IDMS_SOFT_NL, "Zacht"},
/* pl */ {IDMS_SOFT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SOFT_SV, "Mjuk"},

/* en */ {IDMS_EXIT,    "Exit"},
/* de */ {IDMS_EXIT_DE, "Exit"},
/* fr */ {IDMS_EXIT_FR, "Quitter"},
/* hu */ {IDMS_EXIT_HU, "Kil�p�s"},
/* it */ {IDMS_EXIT_IT, "Esci"},
/* nl */ {IDMS_EXIT_NL, "Afsluiten"},
/* pl */ {IDMS_EXIT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_EXIT_SV, "Avsluta"},

/* en */ {IDMS_SNAPSHOT,    "Snapshot"},
/* de */ {IDMS_SNAPSHOT_DE, "Snapshot"},
/* fr */ {IDMS_SNAPSHOT_FR, "Sauvegarde"},
/* hu */ {IDMS_SNAPSHOT_HU, "Pillanatk�p"},
/* it */ {IDMS_SNAPSHOT_IT, "Snapshot"},
/* nl */ {IDMS_SNAPSHOT_NL, "Momentopname"},
/* pl */ {IDMS_SNAPSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SNAPSHOT_SV, "�gonblicksbild"},

/* en */ {IDMS_LOAD_SNAPSHOT_IMAGE,    "Load snapshot image..."},
/* de */ {IDMS_LOAD_SNAPSHOT_IMAGE_DE, "Snapshot Image Laden..."},
/* fr */ {IDMS_LOAD_SNAPSHOT_IMAGE_FR, "Charger une sauvegarde...."},
/* hu */ {IDMS_LOAD_SNAPSHOT_IMAGE_HU, "Pillanatk�p bet�lt�se..."},
/* it */ {IDMS_LOAD_SNAPSHOT_IMAGE_IT, "Carica immagine dello snapshot..."},
/* nl */ {IDMS_LOAD_SNAPSHOT_IMAGE_NL, "Laad momentopname bestand..."},
/* pl */ {IDMS_LOAD_SNAPSHOT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LOAD_SNAPSHOT_IMAGE_SV, "L�s in �gonblicksbild..."},

/* en */ {IDMS_SAVE_SNAPSHOT_IMAGE,    "Save snapshot image..."},
/* de */ {IDMS_SAVE_SNAPSHOT_IMAGE_DE, "Snapshot Image Speichern..."},
/* fr */ {IDMS_SAVE_SNAPSHOT_IMAGE_FR, "Enregistrer une sauvegarde..."},
/* hu */ {IDMS_SAVE_SNAPSHOT_IMAGE_HU, "Pillanatk�p ment�se..."},
/* it */ {IDMS_SAVE_SNAPSHOT_IMAGE_IT, "Salva l'immagine dello snapshot..."},
/* nl */ {IDMS_SAVE_SNAPSHOT_IMAGE_NL, "Opslaan momentopname bestand..."},
/* pl */ {IDMS_SAVE_SNAPSHOT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_SNAPSHOT_IMAGE_SV, "Spara �gonblicksbild..."},

/* en */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE,    "Load quicksnapshot image"},
/* de */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_DE, "Schnellladen von Snapshot Image"},
/* fr */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_FR, "Charger la sauvegarde rapide"},
/* hu */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_HU, "Gyors pillanatk�p bet�lt�se"},
/* it */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_IT, "Carica immagine dello snapshot"},
/* nl */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_NL, "Snellaad momentopname bestand"},
/* pl */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LOAD_QUICKSNAPSHOT_IMAGE_SV, "L�s in snabb�gonblicksbild"},

/* en */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE,    "Save quicksnapshot image"},
/* de */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_DE, "Schnellspeichern von Snapshot"},
/* fr */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_FR, "Enregistrer la sauvegarde rapide"},
/* hu */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_HU, "Gyors pillanatk�p f�jl ment�se"},
/* it */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_IT, "Salva immagine dello snapshot"},
/* nl */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_NL, "Snelopslaan momentopname bestand"},
/* pl */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_QUICKSNAPSHOT_IMAGE_SV, "Spara snabb�gonblicksbildfil"},

/* en */ {IDMS_START_STOP_RECORDING,    "Start/Stop Recording History"},
/* de */ {IDMS_START_STOP_RECORDING_DE, "Start/Stop Aufnahme History"},
/* fr */ {IDMS_START_STOP_RECORDING_FR, "D�marrer/Arr�ter l'historique de l'enregistrement"},
/* hu */ {IDMS_START_STOP_RECORDING_HU, "Esem�ny felv�tel ind�t�sa/meg�ll�t�sa"},
/* it */ {IDMS_START_STOP_RECORDING_IT, "Avvia/termina la registrazione della cronologia"},
/* nl */ {IDMS_START_STOP_RECORDING_NL, "Start/Stop Opname Geschiedenis"},
/* pl */ {IDMS_START_STOP_RECORDING_PL, ""},  /* fuzzy */
/* sv */ {IDMS_START_STOP_RECORDING_SV, "B�rja/sluta spela in h�ndelser"},

/* en */ {IDMS_START_STOP_PLAYBACK,    "Start/Stop Playback History"},
/* de */ {IDMS_START_STOP_PLAYBACK_DE, "Start/Stop Wiedergabe History"},
/* fr */ {IDMS_START_STOP_PLAYBACK_FR, "D�marrer/Arr�ter l'historique de la lecture"},
/* hu */ {IDMS_START_STOP_PLAYBACK_HU, "Esem�ny visszaj�tsz�s ind�t�sa/meg�ll�t�sa"},
/* it */ {IDMS_START_STOP_PLAYBACK_IT, "Avvia/termina la riproduzione della cronologia"},
/* nl */ {IDMS_START_STOP_PLAYBACK_NL, "Start/Stop Afspeel Geschiedenis"},
/* pl */ {IDMS_START_STOP_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDMS_START_STOP_PLAYBACK_SV, "B�rja/sluta spela upp h�ndelser"},

/* en */ {IDMS_SET_MILESTONE,    "Set Recording Milestone"},
/* de */ {IDMS_SET_MILESTONE_DE, "Setze Aufnahme Meilenstein"},
/* fr */ {IDMS_SET_MILESTONE_FR, "Lever le Signet d'enregistrement"},
/* hu */ {IDMS_SET_MILESTONE_HU, "Kil�m�terk� elhelyez�se a felv�telben"},
/* it */ {IDMS_SET_MILESTONE_IT, "Imposta segnalibro"},
/* nl */ {IDMS_SET_MILESTONE_NL, "Zet Opname Mijlpaal"},
/* pl */ {IDMS_SET_MILESTONE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SET_MILESTONE_SV, "Ange inspelningsbokm�rke"},

/* en */ {IDMS_RETURN_TO_MILESTONE,    "Return to Milestone"},
/* de */ {IDMS_RETURN_TO_MILESTONE_DE, "R�ckkehr zum Meilenstein"},
/* fr */ {IDMS_RETURN_TO_MILESTONE_FR, "Retourner au Signet d'enregistrement"},
/* hu */ {IDMS_RETURN_TO_MILESTONE_HU, "Visszaugr�s az el�z� kil�m�terk�h�z"},
/* it */ {IDMS_RETURN_TO_MILESTONE_IT, "Vai al segnalibro"},
/* nl */ {IDMS_RETURN_TO_MILESTONE_NL, "Ga terug naar Mijlpaal"},
/* pl */ {IDMS_RETURN_TO_MILESTONE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RETURN_TO_MILESTONE_SV, "�terg� till bokm�rke"},

/* en */ {IDMS_RECORDING_START_MODE,    "Recording start mode"},
/* de */ {IDMS_RECORDING_START_MODE_DE, "Aufnahme Start Modus"},
/* fr */ {IDMS_RECORDING_START_MODE_FR, "Mode de d�part d'enregistrement"},
/* hu */ {IDMS_RECORDING_START_MODE_HU, "R�gz�t�s ind�t�s�nak m�dja"},
/* it */ {IDMS_RECORDING_START_MODE_IT, "Modalit� di inizio registrazione"},
/* nl */ {IDMS_RECORDING_START_MODE_NL, "Opname start modus"},
/* pl */ {IDMS_RECORDING_START_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RECORDING_START_MODE_SV, "Spelar in startl�ge"},

/* en */ {IDMS_SAVE_NEW_SNAPSHOT,    "Save new snapshot"},
/* de */ {IDMS_SAVE_NEW_SNAPSHOT_DE, "Speichere neuen Snapshot"},
/* fr */ {IDMS_SAVE_NEW_SNAPSHOT_FR, "Enregistrer une nouvelle sauvegarde"},
/* hu */ {IDMS_SAVE_NEW_SNAPSHOT_HU, "�j pillanatk�p ment�se"},
/* it */ {IDMS_SAVE_NEW_SNAPSHOT_IT, "Salva nuovo snapshot"},
/* nl */ {IDMS_SAVE_NEW_SNAPSHOT_NL, "Opslaan nieuwe momentopname"},
/* pl */ {IDMS_SAVE_NEW_SNAPSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_NEW_SNAPSHOT_SV, "Spara ny �gonblicksbild"},

/* en */ {IDMS_LOAD_EXISTING_SNAPSHOT,    "Load existing snapshot"},
/* de */ {IDMS_LOAD_EXISTING_SNAPSHOT_DE, "Lade existierenden Snapshot"},
/* fr */ {IDMS_LOAD_EXISTING_SNAPSHOT_FR, "Charger une sauvegarde"},
/* hu */ {IDMS_LOAD_EXISTING_SNAPSHOT_HU, "L�tez� pillanatk�p bet�lt�se"},
/* it */ {IDMS_LOAD_EXISTING_SNAPSHOT_IT, "Carica snapshot esistente"},
/* nl */ {IDMS_LOAD_EXISTING_SNAPSHOT_NL, "Laad bestaande momentopname"},
/* pl */ {IDMS_LOAD_EXISTING_SNAPSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LOAD_EXISTING_SNAPSHOT_SV, "L�s in existerande �gonblicksbild"},

/* en */ {IDMS_START_WITH_RESET,    "Start with reset"},
/* de */ {IDMS_START_WITH_RESET_DE, "Mit Reset starten"},
/* fr */ {IDMS_START_WITH_RESET_FR, "R�initialiser au d�marrage"},
/* hu */ {IDMS_START_WITH_RESET_HU, "Indul�s RESET-tel"},
/* it */ {IDMS_START_WITH_RESET_IT, "Avvia con reset"},
/* nl */ {IDMS_START_WITH_RESET_NL, "Start bij reset"},
/* pl */ {IDMS_START_WITH_RESET_PL, ""},  /* fuzzy */
/* sv */ {IDMS_START_WITH_RESET_SV, "Starta med nollst�llning"},

/* en */ {IDMS_OVERWRITE_PLAYBACK,    "Overwrite Playback"},
/* de */ {IDMS_OVERWRITE_PLAYBACK_DE, "Laufende Aufnahme �berschreiben"},
/* fr */ {IDMS_OVERWRITE_PLAYBACK_FR, "�craser la lecture actuelle"},
/* hu */ {IDMS_OVERWRITE_PLAYBACK_HU, "Visszaj�tsz�s fel�l�r�sa"},
/* it */ {IDMS_OVERWRITE_PLAYBACK_IT, "Sovrascrivi la riproduzione"},
/* nl */ {IDMS_OVERWRITE_PLAYBACK_NL, "Overschrijf weergave"},
/* pl */ {IDMS_OVERWRITE_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDMS_OVERWRITE_PLAYBACK_SV, "Skriv �ver aktiv uppspelning"},

/* en */ {IDMS_SELECT_HISTORY_DIR,    "Select History files/directory"},
/* de */ {IDMS_SELECT_HISTORY_DIR_DE, "Verzeichnis f�r Event Aufnahme"},
/* fr */ {IDMS_SELECT_HISTORY_DIR_FR, "S�lectionnez le r�pertoire des captures"},
/* hu */ {IDMS_SELECT_HISTORY_DIR_HU, "V�lassza ki a felv�teleket/k�nyvt�rat"},
/* it */ {IDMS_SELECT_HISTORY_DIR_IT, "Seleziona i file/directory della cronologia"},
/* nl */ {IDMS_SELECT_HISTORY_DIR_NL, "Selecteer Geschiedenis directory"},
/* pl */ {IDMS_SELECT_HISTORY_DIR_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SELECT_HISTORY_DIR_SV, "V�lj historikfiler/-katalog"},

/* en */ {IDMS_SAVE_STOP_MEDIA_FILE,    "Save/stop media file..."},
/* de */ {IDMS_SAVE_STOP_MEDIA_FILE_DE, "Speichern/stop media Datei..."},
/* fr */ {IDMS_SAVE_STOP_MEDIA_FILE_FR, "Enregistrer/Arr�ter fichier m�dia..."},
/* hu */ {IDMS_SAVE_STOP_MEDIA_FILE_HU, "M�dia f�jl ment�se/meg�ll�t�sa..."},
/* it */ {IDMS_SAVE_STOP_MEDIA_FILE_IT, "Salva/arresta file multimediale..."},
/* nl */ {IDMS_SAVE_STOP_MEDIA_FILE_NL, "Opslaan/stop media bestand..."},
/* pl */ {IDMS_SAVE_STOP_MEDIA_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_STOP_MEDIA_FILE_SV, "Starta/stoppa mediafil..."},

/* en */ {IDMS_NETPLAY,    "Netplay (experimental)..."},
/* de */ {IDMS_NETPLAY_DE, "Netplay (experimentell)..."},
/* fr */ {IDMS_NETPLAY_FR, ""},  /* fuzzy */
/* hu */ {IDMS_NETPLAY_HU, "H�l�zati j�t�k (k�s�rleti)..."},
/* it */ {IDMS_NETPLAY_IT, "Gioco in rete (sperimentale)..."},
/* nl */ {IDMS_NETPLAY_NL, "Netplay (experimenteel)..."},
/* pl */ {IDMS_NETPLAY_PL, ""},  /* fuzzy */
/* sv */ {IDMS_NETPLAY_SV, "N�tverksspel (experimentellt)..."},

/* en */ {IDMS_OPTIONS,    "Options"},
/* de */ {IDMS_OPTIONS_DE, "Optionen"},
/* fr */ {IDMS_OPTIONS_FR, "Options"},
/* hu */ {IDMS_OPTIONS_HU, "Opci�k"},
/* it */ {IDMS_OPTIONS_IT, "Opzioni"},
/* nl */ {IDMS_OPTIONS_NL, "Opties"},
/* pl */ {IDMS_OPTIONS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_OPTIONS_SV, "Alternativ"},

/* en */ {IDMS_REFRESH_RATE,    "Refresh rate"},
/* de */ {IDMS_REFRESH_RATE_DE, "Wiederholrate"},
/* fr */ {IDMS_REFRESH_RATE_FR, "Taux de rafra�chissement"},
/* hu */ {IDMS_REFRESH_RATE_HU, "Friss�t�si r�ta"},
/* it */ {IDMS_REFRESH_RATE_IT, "Frequenza di aggiornamento"},
/* nl */ {IDMS_REFRESH_RATE_NL, "Vernieuwings snelheid"},
/* pl */ {IDMS_REFRESH_RATE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_REFRESH_RATE_SV, "Uppdateringshastighet"},

/* en */ {IDMS_AUTO,    "Auto"},
/* de */ {IDMS_AUTO_DE, "Automatisch"},
/* fr */ {IDMS_AUTO_FR, "Automatique"},
/* hu */ {IDMS_AUTO_HU, "Automatikus"},
/* it */ {IDMS_AUTO_IT, "Auto"},
/* nl */ {IDMS_AUTO_NL, "Auto"},
/* pl */ {IDMS_AUTO_PL, ""},  /* fuzzy */
/* sv */ {IDMS_AUTO_SV, "Auto"},

/* en */ {IDMS_1_1,    "1/1"},
/* de */ {IDMS_1_1_DE, "1/1"},
/* fr */ {IDMS_1_1_FR, "1/1"},
/* hu */ {IDMS_1_1_HU, "1/1"},
/* it */ {IDMS_1_1_IT, "1/1"},
/* nl */ {IDMS_1_1_NL, "1/1"},
/* pl */ {IDMS_1_1_PL, "1/1"},
/* sv */ {IDMS_1_1_SV, "1/1"},

/* en */ {IDMS_1_2,    "1/2"},
/* de */ {IDMS_1_2_DE, "1/2"},
/* fr */ {IDMS_1_2_FR, "1/2"},
/* hu */ {IDMS_1_2_HU, "1/2"},
/* it */ {IDMS_1_2_IT, "1/2"},
/* nl */ {IDMS_1_2_NL, "1/2"},
/* pl */ {IDMS_1_2_PL, "1/2"},
/* sv */ {IDMS_1_2_SV, "1/2"},

/* en */ {IDMS_1_3,    "1/3"},
/* de */ {IDMS_1_3_DE, "1/3"},
/* fr */ {IDMS_1_3_FR, "1/3"},
/* hu */ {IDMS_1_3_HU, "1/3"},
/* it */ {IDMS_1_3_IT, "1/3"},
/* nl */ {IDMS_1_3_NL, "1/3"},
/* pl */ {IDMS_1_3_PL, "1/3"},
/* sv */ {IDMS_1_3_SV, "1/3"},

/* en */ {IDMS_1_4,    "1/4"},
/* de */ {IDMS_1_4_DE, "1/4"},
/* fr */ {IDMS_1_4_FR, "1/4"},
/* hu */ {IDMS_1_4_HU, "1/4"},
/* it */ {IDMS_1_4_IT, "1/4"},
/* nl */ {IDMS_1_4_NL, "1/4"},
/* pl */ {IDMS_1_4_PL, "1/4"},
/* sv */ {IDMS_1_4_SV, "1/4"},

/* en */ {IDMS_1_5,    "1/5"},
/* de */ {IDMS_1_5_DE, "1/5"},
/* fr */ {IDMS_1_5_FR, "1/5"},
/* hu */ {IDMS_1_5_HU, "1/5"},
/* it */ {IDMS_1_5_IT, "1/5"},
/* nl */ {IDMS_1_5_NL, "1/5"},
/* pl */ {IDMS_1_5_PL, "1/5"},
/* sv */ {IDMS_1_5_SV, "1/5"},

/* en */ {IDMS_1_6,    "1/6"},
/* de */ {IDMS_1_6_DE, "1/6"},
/* fr */ {IDMS_1_6_FR, "1/6"},
/* hu */ {IDMS_1_6_HU, "1/6"},
/* it */ {IDMS_1_6_IT, "1/6"},
/* nl */ {IDMS_1_6_NL, "1/6"},
/* pl */ {IDMS_1_6_PL, "1/6"},
/* sv */ {IDMS_1_6_SV, "1/6"},

/* en */ {IDMS_1_7,    "1/7"},
/* de */ {IDMS_1_7_DE, "1/7"},
/* fr */ {IDMS_1_7_FR, "1/7"},
/* hu */ {IDMS_1_7_HU, "1/7"},
/* it */ {IDMS_1_7_IT, "1/7"},
/* nl */ {IDMS_1_7_NL, "1/7"},
/* pl */ {IDMS_1_7_PL, "1/7"},
/* sv */ {IDMS_1_7_SV, "1/7"},

/* en */ {IDMS_1_8,    "1/8"},
/* de */ {IDMS_1_8_DE, "1/8"},
/* fr */ {IDMS_1_8_FR, "1/8"},
/* hu */ {IDMS_1_8_HU, "1/8"},
/* it */ {IDMS_1_8_IT, "1/8"},
/* nl */ {IDMS_1_8_NL, "1/8"},
/* pl */ {IDMS_1_8_PL, "1/8"},
/* sv */ {IDMS_1_8_SV, "1/8"},

/* en */ {IDMS_1_9,    "1/9"},
/* de */ {IDMS_1_9_DE, "1/9"},
/* fr */ {IDMS_1_9_FR, "1/9"},
/* hu */ {IDMS_1_9_HU, "1/9"},
/* it */ {IDMS_1_9_IT, "1/9"},
/* nl */ {IDMS_1_9_NL, "1/9"},
/* pl */ {IDMS_1_9_PL, "1/9"},
/* sv */ {IDMS_1_9_SV, "1/9"},

/* en */ {IDMS_1_10,    "1/10"},
/* de */ {IDMS_1_10_DE, "1/10"},
/* fr */ {IDMS_1_10_FR, "1/10"},
/* hu */ {IDMS_1_10_HU, "1/10"},
/* it */ {IDMS_1_10_IT, "1/10"},
/* nl */ {IDMS_1_10_NL, "1/10"},
/* pl */ {IDMS_1_10_PL, "1/10"},
/* sv */ {IDMS_1_10_SV, "1/10"},

/* en */ {IDMS_MAXIMUM_SPEED,    "Maximum Speed"},
/* de */ {IDMS_MAXIMUM_SPEED_DE, "Maximale Geschwindigkeit"},
/* fr */ {IDMS_MAXIMUM_SPEED_FR, "Vitesse Maximale"},
/* hu */ {IDMS_MAXIMUM_SPEED_HU, "Maxim�lis sebess�g"},
/* it */ {IDMS_MAXIMUM_SPEED_IT, "Velocit� massima"},
/* nl */ {IDMS_MAXIMUM_SPEED_NL, "Maximale Snelheid"},
/* pl */ {IDMS_MAXIMUM_SPEED_PL, ""},  /* fuzzy */
/* sv */ {IDMS_MAXIMUM_SPEED_SV, "Maximal hastighet"},

/* en */ {IDMS_200_PERCENT,    "200%"},
/* de */ {IDMS_200_PERCENT_DE, "200%"},
/* fr */ {IDMS_200_PERCENT_FR, "200%"},
/* hu */ {IDMS_200_PERCENT_HU, "200%"},
/* it */ {IDMS_200_PERCENT_IT, "200%"},
/* nl */ {IDMS_200_PERCENT_NL, "200%"},
/* pl */ {IDMS_200_PERCENT_PL, "200%"},
/* sv */ {IDMS_200_PERCENT_SV, "200%"},

/* en */ {IDMS_100_PERCENT,    "100%"},
/* de */ {IDMS_100_PERCENT_DE, "100%"},
/* fr */ {IDMS_100_PERCENT_FR, "100%"},
/* hu */ {IDMS_100_PERCENT_HU, "100%"},
/* it */ {IDMS_100_PERCENT_IT, "100%"},
/* nl */ {IDMS_100_PERCENT_NL, "100%"},
/* pl */ {IDMS_100_PERCENT_PL, "100%"},
/* sv */ {IDMS_100_PERCENT_SV, "100%"},

/* en */ {IDMS_50_PERCENT,    "50%"},
/* de */ {IDMS_50_PERCENT_DE, "50%"},
/* fr */ {IDMS_50_PERCENT_FR, "50%"},
/* hu */ {IDMS_50_PERCENT_HU, "50%"},
/* it */ {IDMS_50_PERCENT_IT, "50%"},
/* nl */ {IDMS_50_PERCENT_NL, "50%"},
/* pl */ {IDMS_50_PERCENT_PL, "50%"},
/* sv */ {IDMS_50_PERCENT_SV, "50%"},

/* en */ {IDMS_20_PERCENT,    "20%"},
/* de */ {IDMS_20_PERCENT_DE, "20%"},
/* fr */ {IDMS_20_PERCENT_FR, "20%"},
/* hu */ {IDMS_20_PERCENT_HU, "20%"},
/* it */ {IDMS_20_PERCENT_IT, "20%"},
/* nl */ {IDMS_20_PERCENT_NL, "20%"},
/* pl */ {IDMS_20_PERCENT_PL, "20%"},
/* sv */ {IDMS_20_PERCENT_SV, "20%"},

/* en */ {IDMS_10_PERCENT,    "10%"},
/* de */ {IDMS_10_PERCENT_DE, "10%"},
/* fr */ {IDMS_10_PERCENT_FR, "10%"},
/* hu */ {IDMS_10_PERCENT_HU, "10%"},
/* it */ {IDMS_10_PERCENT_IT, "10%"},
/* nl */ {IDMS_10_PERCENT_NL, "10%"},
/* pl */ {IDMS_10_PERCENT_PL, "10%"},
/* sv */ {IDMS_10_PERCENT_SV, "10%"},

/* en */ {IDMS_NO_LIMIT,    "No limit"},
/* de */ {IDMS_NO_LIMIT_DE, "Kein Limit"},
/* fr */ {IDMS_NO_LIMIT_FR, "Aucune limite"},
/* hu */ {IDMS_NO_LIMIT_HU, "Nincs hat�r"},
/* it */ {IDMS_NO_LIMIT_IT, "Nessun limite"},
/* nl */ {IDMS_NO_LIMIT_NL, "Geen limiet"},
/* pl */ {IDMS_NO_LIMIT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_NO_LIMIT_SV, "Ingen gr�ns"},

/* en */ {IDMS_CUSTOM,    "Custom"},
/* de */ {IDMS_CUSTOM_DE, "Benutzerdefiniert"},
/* fr */ {IDMS_CUSTOM_FR, "Personnalis�"},
/* hu */ {IDMS_CUSTOM_HU, "Egy�ni"},
/* it */ {IDMS_CUSTOM_IT, "Personalizzato"},
/* nl */ {IDMS_CUSTOM_NL, "Eigen"},
/* pl */ {IDMS_CUSTOM_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CUSTOM_SV, "Egen"},

/* en */ {IDMS_WARP_MODE,    "Warp mode"},
/* de */ {IDMS_WARP_MODE_DE, "Warp modus"},
/* fr */ {IDMS_WARP_MODE_FR, "Mode turbo"},
/* hu */ {IDMS_WARP_MODE_HU, "Hipergyors m�d"},
/* it */ {IDMS_WARP_MODE_IT, "Modalit� turbo"},
/* nl */ {IDMS_WARP_MODE_NL, "Warp modus"},
/* pl */ {IDMS_WARP_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_WARP_MODE_SV, "Warpl�ge"},

/* en */ {IDMS_FULLSCREEN,    "Fullscreen"},
/* de */ {IDMS_FULLSCREEN_DE, "Vollbild"},
/* fr */ {IDMS_FULLSCREEN_FR, "P�riph�rique plein �cran"},
/* hu */ {IDMS_FULLSCREEN_HU, "Teljes k�perny�"},
/* it */ {IDMS_FULLSCREEN_IT, "Schermo intero"},
/* nl */ {IDMS_FULLSCREEN_NL, "Volscherm"},
/* pl */ {IDMS_FULLSCREEN_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FULLSCREEN_SV, "Fullsk�rm"},

/* en */ {IDMS_VIDEO_CACHE,    "Video cache"},
/* de */ {IDMS_VIDEO_CACHE_DE, "Video cache"},
/* fr */ {IDMS_VIDEO_CACHE_FR, "Cache vid�o"},
/* hu */ {IDMS_VIDEO_CACHE_HU, "K�p gyors�t�t�r"},
/* it */ {IDMS_VIDEO_CACHE_IT, "Cache video"},
/* nl */ {IDMS_VIDEO_CACHE_NL, "Video cache"},
/* pl */ {IDMS_VIDEO_CACHE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIDEO_CACHE_SV, "Grafikcache"},

/* en */ {IDMS_DOUBLE_SIZE,    "Double size"},
/* de */ {IDMS_DOUBLE_SIZE_DE, "Doppelte Gr��e"},
/* fr */ {IDMS_DOUBLE_SIZE_FR, "Taille double"},
/* hu */ {IDMS_DOUBLE_SIZE_HU, "Dupla m�ret"},
/* it */ {IDMS_DOUBLE_SIZE_IT, "Dimensione doppia"},
/* nl */ {IDMS_DOUBLE_SIZE_NL, "Dubbele grootte"},
/* pl */ {IDMS_DOUBLE_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DOUBLE_SIZE_SV, "Dubbel storlek"},

/* en */ {IDMS_DOUBLE_SCAN,    "Double scan"},
/* de */ {IDMS_DOUBLE_SCAN_DE, "Doppelt Scan"},
/* fr */ {IDMS_DOUBLE_SCAN_FR, "Double scan"},
/* hu */ {IDMS_DOUBLE_SCAN_HU, "Dupla p�szt�z�s"},
/* it */ {IDMS_DOUBLE_SCAN_IT, "Scansione doppia"},
/* nl */ {IDMS_DOUBLE_SCAN_NL, "Dubbele scan"},
/* pl */ {IDMS_DOUBLE_SCAN_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DOUBLE_SCAN_SV, "Dubbelskanning"},

/* en */ {IDMS_PAL_EMULATION,    "PAL Emulation"},
/* de */ {IDMS_PAL_EMULATION_DE, "PAL Emulation"},
/* fr */ {IDMS_PAL_EMULATION_FR, "�mulation PAL"},
/* hu */ {IDMS_PAL_EMULATION_HU, "PAL emul�ci�"},
/* it */ {IDMS_PAL_EMULATION_IT, "Emulazione PAL"},
/* nl */ {IDMS_PAL_EMULATION_NL, "PAL Emulatie"},
/* pl */ {IDMS_PAL_EMULATION_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PAL_EMULATION_SV, "PAL-emulering"},

/* en */ {IDMS_SCALE2X,    "Scale2x"},
/* de */ {IDMS_SCALE2X_DE, "Scale2x"},
/* fr */ {IDMS_SCALE2X_FR, "Scale2x"},
/* hu */ {IDMS_SCALE2X_HU, "2x m�retez"},
/* it */ {IDMS_SCALE2X_IT, "Scale2x"},
/* nl */ {IDMS_SCALE2X_NL, "Schaal2x"},
/* pl */ {IDMS_SCALE2X_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SCALE2X_SV, "Scale2x"},

/* en */ {IDMS_VDC_SETTINGS,    "VDC settings"},
/* de */ {IDMS_VDC_SETTINGS_DE, "VDC Einstellungen"},
/* fr */ {IDMS_VDC_SETTINGS_FR, "Param�tres VDC"},
/* it */ {IDMS_VDC_SETTINGS_IT, "Impostazioni VDC"},
/* hu */ {IDMS_VDC_SETTINGS_HU, "VDC be�ll�t�sai"},
/* nl */ {IDMS_VDC_SETTINGS_NL, "VDC instellingen"},
/* pl */ {IDMS_VDC_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VDC_SETTINGS_SV, "VDC-inst�llningar"},

/* en */ {IDMS_64KB_VIDEO_MEMORY,    "64KB video memory"},
/* de */ {IDMS_64KB_VIDEO_MEMORY_DE, "64KB Bildschirm Speicher"},
/* fr */ {IDMS_64KB_VIDEO_MEMORY_FR, "M�moire d'affichage de 64Ko"},
/* it */ {IDMS_64KB_VIDEO_MEMORY_IT, "64KB di memoria video"},
/* hu */ {IDMS_64KB_VIDEO_MEMORY_HU, "64KB k�perny� mem�ria"},
/* nl */ {IDMS_64KB_VIDEO_MEMORY_NL, "64KB video geheugen"},
/* pl */ {IDMS_64KB_VIDEO_MEMORY_PL, ""},  /* fuzzy */
/* sv */ {IDMS_64KB_VIDEO_MEMORY_SV, "64KB grafikminne"},

/* en */ {IDMS_SWAP_JOYSTICKS,    "Swap Joysticks"},
/* de */ {IDMS_SWAP_JOYSTICKS_DE, "Joystick ports austauschen"},
/* fr */ {IDMS_SWAP_JOYSTICKS_FR, "Interchanger les joysticks"},
/* hu */ {IDMS_SWAP_JOYSTICKS_HU, "Botkorm�nyok felcser�l�se"},
/* it */ {IDMS_SWAP_JOYSTICKS_IT, "Scambia joystick"},
/* nl */ {IDMS_SWAP_JOYSTICKS_NL, "Verwissel Joysticks"},
/* pl */ {IDMS_SWAP_JOYSTICKS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SWAP_JOYSTICKS_SV, "V�xla spelportarna"},

/* en */ {IDMS_SOUND_PLAYBACK,    "Sound playback"},
/* de */ {IDMS_SOUND_PLAYBACK_DE, "Sound Einschalten"},
/* fr */ {IDMS_SOUND_PLAYBACK_FR, "Lecture audio"},
/* hu */ {IDMS_SOUND_PLAYBACK_HU, "Hangok enged�lyez�se"},
/* it */ {IDMS_SOUND_PLAYBACK_IT, "Riproduzione del suono"},
/* nl */ {IDMS_SOUND_PLAYBACK_NL, "Geluidsuitvoer"},
/* pl */ {IDMS_SOUND_PLAYBACK_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SOUND_PLAYBACK_SV, "Aktivera ljud�tergivning"},

/* en */ {IDMS_TRUE_DRIVE_EMU,    "True drive emulation"},
/* de */ {IDMS_TRUE_DRIVE_EMU_DE, "Pr�zise Floppy Emulation"},
/* fr */ {IDMS_TRUE_DRIVE_EMU_FR, "Activer l'�mulation r�elle des lecteurs"},
/* hu */ {IDMS_TRUE_DRIVE_EMU_HU, "Val�s lemezegys�g emul�ci�"},
/* it */ {IDMS_TRUE_DRIVE_EMU_IT, "Attiva l'emulazione hardware dei drive"},
/* nl */ {IDMS_TRUE_DRIVE_EMU_NL, "Hardware drive emulatie"},
/* pl */ {IDMS_TRUE_DRIVE_EMU_PL, ""},  /* fuzzy */
/* sv */ {IDMS_TRUE_DRIVE_EMU_SV, "�kta diskettenhetsemulering"},

/* en */ {IDMS_VIRTUAL_DEVICE_TRAPS,    "Virtual device traps"},
/* de */ {IDMS_VIRTUAL_DEVICE_TRAPS_DE, "Virtuelle Ger�te Traps"},
/* fr */ {IDMS_VIRTUAL_DEVICE_TRAPS_FR, "Activer les p�riph�riques virtuels"},
/* hu */ {IDMS_VIRTUAL_DEVICE_TRAPS_HU, "*Virtu�lis eszk�z�k ciklus kihagy�sa"},
/* it */ {IDMS_VIRTUAL_DEVICE_TRAPS_IT, "Attiva le periferiche virtuali"},
/* nl */ {IDMS_VIRTUAL_DEVICE_TRAPS_NL, "Virtuele apparaat traps"},
/* pl */ {IDMS_VIRTUAL_DEVICE_TRAPS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIRTUAL_DEVICE_TRAPS_SV, "Virtuella enhetstrap:ar"},

/* en */ {IDMS_DRIVE_SYNC_FACTOR,    "Drive sync factor"},
/* de */ {IDMS_DRIVE_SYNC_FACTOR_DE, "Laufwerkt Sync Faktor"},
/* fr */ {IDMS_DRIVE_SYNC_FACTOR_FR, "Facteur de synchro du lecteur"},
/* hu */ {IDMS_DRIVE_SYNC_FACTOR_HU, "Lemezegys�g szinkron faktor"},
/* it */ {IDMS_DRIVE_SYNC_FACTOR_IT, "Fattore di sincronizzazione del drive"},
/* nl */ {IDMS_DRIVE_SYNC_FACTOR_NL, "Drive synchronisatie faktor"},
/* pl */ {IDMS_DRIVE_SYNC_FACTOR_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_SYNC_FACTOR_SV, "Synkfaktor f�r diskettstation"},

/* en */ {IDMS_PAL,    "PAL"},
/* de */ {IDMS_PAL_DE, "PAL"},
/* fr */ {IDMS_PAL_FR, "PAL"},
/* hu */ {IDMS_PAL_HU, "PAL"},
/* it */ {IDMS_PAL_IT, "PAL"},
/* nl */ {IDMS_PAL_NL, "PAL"},
/* pl */ {IDMS_PAL_PL, "PAL"},
/* sv */ {IDMS_PAL_SV, "PAL"},

/* en */ {IDMS_NTSC,    "NTSC"},
/* de */ {IDMS_NTSC_DE, "NTSC"},
/* fr */ {IDMS_NTSC_FR, "NTSC"},
/* hu */ {IDMS_NTSC_HU, "NTSC"},
/* it */ {IDMS_NTSC_IT, "NTSC"},
/* nl */ {IDMS_NTSC_NL, "NTSC"},
/* pl */ {IDMS_NTSC_PL, "NTSC"},
/* sv */ {IDMS_NTSC_SV, "NTSC"},

/* en */ {IDMS_VIDEO_STANDARD,    "Video standard"},
/* de */ {IDMS_VIDEO_STANDARD_DE, "Video standard"},
/* fr */ {IDMS_VIDEO_STANDARD_FR, "Standard vid�o"},
/* hu */ {IDMS_VIDEO_STANDARD_HU, "Vide� szabv�ny"},
/* it */ {IDMS_VIDEO_STANDARD_IT, "Standard video"},
/* nl */ {IDMS_VIDEO_STANDARD_NL, "Video standaard"},
/* pl */ {IDMS_VIDEO_STANDARD_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIDEO_STANDARD_SV, "TV-standard"},

/* en */ {IDMS_PAL_G,    "PAL-G"},
/* de */ {IDMS_PAL_G_DE, "PAL-G"},
/* fr */ {IDMS_PAL_G_FR, "PAL-G"},
/* hu */ {IDMS_PAL_G_HU, "PAL-G"},
/* it */ {IDMS_PAL_G_IT, "PAL-G"},
/* nl */ {IDMS_PAL_G_NL, "PAL-G"},
/* pl */ {IDMS_PAL_G_PL, "PAL-G"},
/* sv */ {IDMS_PAL_G_SV, "PAL-G"},

/* en */ {IDMS_NTSC_M,    "NTSC-M"},
/* de */ {IDMS_NTSC_M_DE, "NTSC-M"},
/* fr */ {IDMS_NTSC_M_FR, "NTSC-M"},
/* hu */ {IDMS_NTSC_M_HU, "NTSC-M"},
/* it */ {IDMS_NTSC_M_IT, "NTSC-M"},
/* nl */ {IDMS_NTSC_M_NL, "NTSC-M"},
/* pl */ {IDMS_NTSC_M_PL, "NTSC-M"},
/* sv */ {IDMS_NTSC_M_SV, "NTSC-M"},

/* en */ {IDMS_OLD_NTSC_M,    "Old NTSC-M"},
/* de */ {IDMS_OLD_NTSC_M_DE, "NTSC-M alt"},
/* fr */ {IDMS_OLD_NTSC_M_FR, "Ancien NTSC-M"},
/* hu */ {IDMS_OLD_NTSC_M_HU, "R�gi NTSC-M"},
/* it */ {IDMS_OLD_NTSC_M_IT, "NTSC-M vecchio"},
/* nl */ {IDMS_OLD_NTSC_M_NL, "Oud NTSC-M"},
/* pl */ {IDMS_OLD_NTSC_M_PL, ""},  /* fuzzy */
/* sv */ {IDMS_OLD_NTSC_M_SV, "Gammal NTSC-M"},

/* en */ {IDMS_EMU_ID,    "Emulator Identification"},
/* de */ {IDMS_EMU_ID_DE, "Emulator Identifikation"},
/* fr */ {IDMS_EMU_ID_FR, "Identification de l'�mulateur"},
/* hu */ {IDMS_EMU_ID_HU, "Emul�tor azonos�t�"},
/* it */ {IDMS_EMU_ID_IT, "Identificazione dell'emulatore"},
/* nl */ {IDMS_EMU_ID_NL, "Emulator Identificatie"},
/* pl */ {IDMS_EMU_ID_PL, ""},  /* fuzzy */
/* sv */ {IDMS_EMU_ID_SV, "Emulatoridentifiering"},

/* en */ {IDMS_VIC_1112_IEEE_488,    "VIC-1112 IEEE 488 module"},
/* de */ {IDMS_VIC_1112_IEEE_488_DE, "VIC-1112 IEEE 488 Modul"},
/* fr */ {IDMS_VIC_1112_IEEE_488_FR, "Module VIC-1112 IEEE 488"},
/* hu */ {IDMS_VIC_1112_IEEE_488_HU, "VIC-1112 IEEE 488 modul"},
/* it */ {IDMS_VIC_1112_IEEE_488_IT, "Modulo VIC-1112 IEEE 488"},
/* nl */ {IDMS_VIC_1112_IEEE_488_NL, "VIC-1112 IEEE 488 module"},
/* pl */ {IDMS_VIC_1112_IEEE_488_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIC_1112_IEEE_488_SV, "VIC-1112 IEEE 488-modul"},

/* en */ {IDMS_IEEE_488_INTERFACE,    "IEEE 488 Interface emulation"},
/* de */ {IDMS_IEEE_488_INTERFACE_DE, "IEEE488 Schnittstellen-Emulation"},
/* fr */ {IDMS_IEEE_488_INTERFACE_FR, "Interface d'�mulation IEEE488"},
/* hu */ {IDMS_IEEE_488_INTERFACE_HU, "IEEE488 interf�sz emul�ci�"},
/* it */ {IDMS_IEEE_488_INTERFACE_IT, "Emulazione dell'interfaccia IEEE 488"},
/* nl */ {IDMS_IEEE_488_INTERFACE_NL, "IEEE488 Interface emulatie"},
/* pl */ {IDMS_IEEE_488_INTERFACE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_IEEE_488_INTERFACE_SV, "Emulering av IEEE488-gr�nssnitt"},

/* en */ {IDMS_1351_MOUSE,    "1351 mouse emulation"},
/* de */ {IDMS_1351_MOUSE_DE, "1351 Mausemulation"},
/* fr */ {IDMS_1351_MOUSE_FR, "�mulation de la souris 1351"},
/* hu */ {IDMS_1351_MOUSE_HU, "1351 eg�r emul�ci�"},
/* it */ {IDMS_1351_MOUSE_IT, "Emulazione del mouse 1351"},
/* nl */ {IDMS_1351_MOUSE_NL, "1351 muis emulatie"},
/* pl */ {IDMS_1351_MOUSE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_1351_MOUSE_SV, "1351-musemulering"},

/* en */ {IDMS_SETTINGS,    "Settings"},
/* de */ {IDMS_SETTINGS_DE, "Einstellungen"},
/* fr */ {IDMS_SETTINGS_FR, "Param�tres"},
/* hu */ {IDMS_SETTINGS_HU, "Be�ll�t�sok"},
/* it */ {IDMS_SETTINGS_IT, "Impostazioni"},
/* nl */ {IDMS_SETTINGS_NL, "Instellingen"},
/* pl */ {IDMS_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SETTINGS_SV, "Inst�llningar"},

/* en */ {IDMS_VIDEO_SETTINGS,    "Video settings..."},
/* de */ {IDMS_VIDEO_SETTINGS_DE, "Video Einstellungen..."},
/* fr */ {IDMS_VIDEO_SETTINGS_FR, "Param�tres vid�o..."},
/* hu */ {IDMS_VIDEO_SETTINGS_HU, "Vide� be�ll�t�sai..."},
/* it */ {IDMS_VIDEO_SETTINGS_IT, "Impostazioni video..."},
/* nl */ {IDMS_VIDEO_SETTINGS_NL, "Video instellingen..."},
/* pl */ {IDMS_VIDEO_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIDEO_SETTINGS_SV, "Grafikinst�llningar..."},

/* en */ {IDMS_VIC_SETTINGS,    "VIC settings..."},
/* de */ {IDMS_VIC_SETTINGS_DE, "VIC Einstellungen..."},
/* fr */ {IDMS_VIC_SETTINGS_FR, "Param�tres VIC..."},
/* hu */ {IDMS_VIC_SETTINGS_HU, "VIC be�ll�t�sai..."},
/* it */ {IDMS_VIC_SETTINGS_IT, "Impostazioni VIC..."},
/* nl */ {IDMS_VIC_SETTINGS_NL, "VIC instellingen..."},
/* pl */ {IDMS_VIC_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIC_SETTINGS_SV, "VIC-inst�llningar..."},

/* en */ {IDMS_CBM2_SETTINGS,    "CBM2 settings..."},
/* de */ {IDMS_CBM2_SETTINGS_DE, "CBM2 Einstellungen..."},
/* fr */ {IDMS_CBM2_SETTINGS_FR, "Param�tres CBM2..."},
/* hu */ {IDMS_CBM2_SETTINGS_HU, "CBM be�ll�t�sai..."},
/* it */ {IDMS_CBM2_SETTINGS_IT, "Impostazioni CBM2..."},
/* nl */ {IDMS_CBM2_SETTINGS_NL, "CBM2 instellingen..."},
/* pl */ {IDMS_CBM2_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CBM2_SETTINGS_SV, "CBM2-inst�llningar..."},

/* en */ {IDMS_SET_PET_MODEL,    "Change PET model"},
/* de */ {IDMS_SET_PET_MODEL_DE, "PET Modell ver�ndern"},
/* fr */ {IDMS_SET_PET_MODEL_FR, ""},  /* fuzzy */
/* hu */ {IDMS_SET_PET_MODEL_HU, "PET modell kiv�laszt�sa"},
/* it */ {IDMS_SET_PET_MODEL_IT, "Cambia modello di PET"},
/* nl */ {IDMS_SET_PET_MODEL_NL, "Verander PET model"},
/* pl */ {IDMS_SET_PET_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SET_PET_MODEL_SV, "Byt PET-modell"},

/* en */ {IDMS_PET_SETTINGS,    "PET settings..."},
/* de */ {IDMS_PET_SETTINGS_DE, "PET Einstellungen..."},
/* fr */ {IDMS_PET_SETTINGS_FR, "Param�tres PET..."},
/* hu */ {IDMS_PET_SETTINGS_HU, "PET be�ll�t�sai..."},
/* it */ {IDMS_PET_SETTINGS_IT, "Impostazioni PET..."},
/* nl */ {IDMS_PET_SETTINGS_NL, "PET instellingen..."},
/* pl */ {IDMS_PET_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PET_SETTINGS_SV, "PET-inst�llningar..."},

/* en */ {IDMS_PERIPHERAL_SETTINGS,    "Peripheral settings..."},
/* de */ {IDMS_PERIPHERAL_SETTINGS_DE, "Peripherie Einstellungen..."},
/* fr */ {IDMS_PERIPHERAL_SETTINGS_FR, "Param�tres des p�riph�riques..."},
/* hu */ {IDMS_PERIPHERAL_SETTINGS_HU, "Perif�ri�k be�ll�t�sai..."},
/* it */ {IDMS_PERIPHERAL_SETTINGS_IT, "Impostazioni periferiche..."},
/* nl */ {IDMS_PERIPHERAL_SETTINGS_NL, "Rand Apparaten instellingen..."},
/* pl */ {IDMS_PERIPHERAL_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PERIPHERAL_SETTINGS_SV, "Inst�llningar f�r kringutrustning..."},

/* en */ {IDMS_DRIVE_SETTINGS,    "Drive settings..."},
/* de */ {IDMS_DRIVE_SETTINGS_DE, "Floppy Einstellungen..."},
/* fr */ {IDMS_DRIVE_SETTINGS_FR, "Param�tres des lecteurs..."},
/* hu */ {IDMS_DRIVE_SETTINGS_HU, "Lemezegys�g be�ll�t�sai..."},
/* it */ {IDMS_DRIVE_SETTINGS_IT, "Impostazioni drive..."},
/* nl */ {IDMS_DRIVE_SETTINGS_NL, "Drive instellingen..."},
/* pl */ {IDMS_DRIVE_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_SETTINGS_SV, "Diskettenhetsinst�llningar..."},

/* en */ {IDMS_DATASETTE_SETTINGS,    "Datasette settings..."},
/* de */ {IDMS_DATASETTE_SETTINGS_DE, "Bandlaufwerk Einstellungen..."},
/* fr */ {IDMS_DATASETTE_SETTINGS_FR, "Param�tres du datassette..."},
/* hu */ {IDMS_DATASETTE_SETTINGS_HU, "Magn� be�ll�t�sai..."},
/* it */ {IDMS_DATASETTE_SETTINGS_IT, "Impostazioni registratore..."},
/* nl */ {IDMS_DATASETTE_SETTINGS_NL, "Datasette instellingen..."},
/* pl */ {IDMS_DATASETTE_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DATASETTE_SETTINGS_SV, "Datasetteinst�llningar..."},

/* en */ {IDMS_PLUS4_SETTINGS,    "Plus4 settings..."},
/* de */ {IDMS_PLUS4_SETTINGS_DE, "Plus4 Einstellungen..."},
/* fr */ {IDMS_PLUS4_SETTINGS_FR, "Param�tres Plus4..."},
/* hu */ {IDMS_PLUS4_SETTINGS_HU, "Plus4 be�ll�t�sai..."},
/* it */ {IDMS_PLUS4_SETTINGS_IT, "Impostazioni Plus4..."},
/* nl */ {IDMS_PLUS4_SETTINGS_NL, "Plus4 instellingen"},
/* pl */ {IDMS_PLUS4_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PLUS4_SETTINGS_SV, "Plus4-inst�llningar..."},

/* en */ {IDMS_VICII_SETTINGS,    "VIC-II settings..."},
/* de */ {IDMS_VICII_SETTINGS_DE, "VIC-II Einstellungen..."},
/* fr */ {IDMS_VICII_SETTINGS_FR, "Param�tres VIC-II..."},
/* hu */ {IDMS_VICII_SETTINGS_HU, "VIC-II be�ll�t�sai..."},
/* it */ {IDMS_VICII_SETTINGS_IT, "Impostazioni VIC-II..."},
/* nl */ {IDMS_VICII_SETTINGS_NL, "VIC-II instellingen..."},
/* pl */ {IDMS_VICII_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VICII_SETTINGS_SV, "VIC-II-inst�llningar..."},

/* en */ {IDMS_JOYSTICK_SETTINGS,    "Joystick settings..."},
/* de */ {IDMS_JOYSTICK_SETTINGS_DE, "Joystick Einstellungen..."},
/* fr */ {IDMS_JOYSTICK_SETTINGS_FR, "Param�tres des joysticks..."},
/* hu */ {IDMS_JOYSTICK_SETTINGS_HU, "Botkorm�ny be�ll�t�sai..."},
/* it */ {IDMS_JOYSTICK_SETTINGS_IT, "Impostazioni joystick..."},
/* nl */ {IDMS_JOYSTICK_SETTINGS_NL, "Joystick instellingen..."},
/* pl */ {IDMS_JOYSTICK_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_JOYSTICK_SETTINGS_SV, "Inst�llningar f�r joystick..."},

/* en */ {IDMS_JOYSTICK_DEVICE_SELECT,    "Joystick device selection"},
/* de */ {IDMS_JOYSTICK_DEVICE_SELECT_DE, "Joystick Ger�te Auswahl"},
/* fr */ {IDMS_JOYSTICK_DEVICE_SELECT_FR, ""},  /* fuzzy */
/* hu */ {IDMS_JOYSTICK_DEVICE_SELECT_HU, "Botkorm�ny eszk�z kiv�laszt�sa"},
/* it */ {IDMS_JOYSTICK_DEVICE_SELECT_IT, "Selezione dispositivo joystick"},
/* nl */ {IDMS_JOYSTICK_DEVICE_SELECT_NL, "Joystick apparaat selectie"},
/* pl */ {IDMS_JOYSTICK_DEVICE_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_JOYSTICK_DEVICE_SELECT_SV, "enhetsinst�llningar f�r joystick"},

/* en */ {IDMS_JOYSTICK_FIRE_SELECT,    "Joystick fire button selection"},
/* de */ {IDMS_JOYSTICK_FIRE_SELECT_DE, "Joystick Feuerknopf Wahl"},
/* fr */ {IDMS_JOYSTICK_FIRE_SELECT_FR, ""},  /* fuzzy */
/* hu */ {IDMS_JOYSTICK_FIRE_SELECT_HU, "Botkorm�ny t�zgomb kiv�laszt�s"},
/* it */ {IDMS_JOYSTICK_FIRE_SELECT_IT, "Selezione pulsante di fuoco del joystick"},
/* nl */ {IDMS_JOYSTICK_FIRE_SELECT_NL, "Joystick vuur knop selectie"},
/* pl */ {IDMS_JOYSTICK_FIRE_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_JOYSTICK_FIRE_SELECT_SV, "val f�r joystickens fire-knapp"},

/* en */ {IDMS_KEYBOARD_SETTINGS,    "Keyboard settings..."},
/* de */ {IDMS_KEYBOARD_SETTINGS_DE, "Tastatur Einstellungen..."},
/* fr */ {IDMS_KEYBOARD_SETTINGS_FR, "Param�tres du clavier..."},
/* hu */ {IDMS_KEYBOARD_SETTINGS_HU, "Billenty�zet be�ll�t�sai..."},
/* it */ {IDMS_KEYBOARD_SETTINGS_IT, "Impostazioni tastiera..."},
/* nl */ {IDMS_KEYBOARD_SETTINGS_NL, "Toetsenbord instellingen..."},
/* pl */ {IDMS_KEYBOARD_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_KEYBOARD_SETTINGS_SV, "Tangentbordsinst�llningar..."},

/* en */ {IDMS_SOUND_SETTINGS,    "Sound settings..."},
/* de */ {IDMS_SOUND_SETTINGS_DE, "Sound Einstellungen..."},
/* fr */ {IDMS_SOUND_SETTINGS_FR, "Param�tres son..."},
/* hu */ {IDMS_SOUND_SETTINGS_HU, "Hang be�ll�t�sai..."},
/* it */ {IDMS_SOUND_SETTINGS_IT, "Impostazioni audio..."},
/* nl */ {IDMS_SOUND_SETTINGS_NL, "Geluid instellingen..."},
/* pl */ {IDMS_SOUND_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SOUND_SETTINGS_SV, "Ljudinst�llningar..."},

/* en */ {IDMS_SID_SETTINGS,    "SID settings..."},
/* de */ {IDMS_SID_SETTINGS_DE, "SID Einstellungen..."},
/* fr */ {IDMS_SID_SETTINGS_FR, "Param�tres SID..."},
/* hu */ {IDMS_SID_SETTINGS_HU, "SID be�ll�t�sai..."},
/* it */ {IDMS_SID_SETTINGS_IT, "Impostazioni SID..."},
/* nl */ {IDMS_SID_SETTINGS_NL, "SID instellingen..."},
/* pl */ {IDMS_SID_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SID_SETTINGS_SV, "SID-inst�llningar..."},

/* en */ {IDMS_COMPUTER_ROM_SETTINGS,    "Computer ROM settings..."},
/* de */ {IDMS_COMPUTER_ROM_SETTINGS_DE, "Computer ROM Einstellungen..."},
/* fr */ {IDMS_COMPUTER_ROM_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDMS_COMPUTER_ROM_SETTINGS_HU, "ROM-ok be�ll�t�sai..."},
/* it */ {IDMS_COMPUTER_ROM_SETTINGS_IT, "Impostazioni ROM del computer..."},
/* nl */ {IDMS_COMPUTER_ROM_SETTINGS_NL, "Computer ROM instellingen..."},
/* pl */ {IDMS_COMPUTER_ROM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_COMPUTER_ROM_SETTINGS_SV, "ROM-inst�llningar f�r dator..."},

/* en */ {IDMS_DRIVE_ROM_SETTINGS,    "Drive ROM settings..."},
/* de */ {IDMS_DRIVE_ROM_SETTINGS_DE, "Floppy ROM Einstellungen..."},
/* fr */ {IDMS_DRIVE_ROM_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDMS_DRIVE_ROM_SETTINGS_HU, "Lemezegys�g ROM be�ll�t�sai..."},
/* it */ {IDMS_DRIVE_ROM_SETTINGS_IT, "Impostazioni ROM del drive..."},
/* nl */ {IDMS_DRIVE_ROM_SETTINGS_NL, "Drive ROM instellingen..."},
/* pl */ {IDMS_DRIVE_ROM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DRIVE_ROM_SETTINGS_SV, "ROM-inst�llningar f�r diskettenhet..."},

/* en */ {IDMS_RAM_SETTINGS,    "RAM settings..."},
/* de */ {IDMS_RAM_SETTINGS_DE, "RAM Einstellungen..."},
/* fr */ {IDMS_RAM_SETTINGS_FR, "Param�tres de la RAM..."},
/* hu */ {IDMS_RAM_SETTINGS_HU, "RAM be�ll�t�sai..."},
/* it */ {IDMS_RAM_SETTINGS_IT, "Impostazioni RAM..."},
/* nl */ {IDMS_RAM_SETTINGS_NL, "RAM instellingen..."},
/* pl */ {IDMS_RAM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RAM_SETTINGS_SV, "RAM-inst�llningar..."},

/* en */ {IDMS_RS232_SETTINGS,    "RS232 settings..."},
/* de */ {IDMS_RS232_SETTINGS_DE, "RS232 Einstellungen..."},
/* fr */ {IDMS_RS232_SETTINGS_FR, "Param�tres RS232..."},
/* hu */ {IDMS_RS232_SETTINGS_HU, "RS232 be�ll�t�sai..."},
/* it */ {IDMS_RS232_SETTINGS_IT, "Impostazioni RS232..."},
/* nl */ {IDMS_RS232_SETTINGS_NL, "RS232 instellingen..."},
/* pl */ {IDMS_RS232_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RS232_SETTINGS_SV, "RS232-inst�llningar..."},

/* en */ {IDMS_C128_SETTINGS,    "C128 settings..."},
/* de */ {IDMS_C128_SETTINGS_DE, "C128 Einstellungen..."},
/* fr */ {IDMS_C128_SETTINGS_FR, "Param�tres C128..."},
/* hu */ {IDMS_C128_SETTINGS_HU, "C128 be�ll�t�sai..."},
/* it */ {IDMS_C128_SETTINGS_IT, "Impostazioni C128..."},
/* nl */ {IDMS_C128_SETTINGS_NL, "C128 instellingen..."},
/* pl */ {IDMS_C128_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_C128_SETTINGS_SV, "C128-inst�llningar..."},

/* en */ {IDMS_CART_IO_SETTINGS,    "Cartridge/IO settings"},
/* de */ {IDMS_CART_IO_SETTINGS_DE, "Erweiterungsmodul Einstellungen"},
/* fr */ {IDMS_CART_IO_SETTINGS_FR, "Param�tres E/S cartouche"},
/* hu */ {IDMS_CART_IO_SETTINGS_HU, "Cartridge/IO be�ll�t�sai"},
/* it */ {IDMS_CART_IO_SETTINGS_IT, "Impostazioni I/O della cartuccia"},
/* nl */ {IDMS_CART_IO_SETTINGS_NL, "Cartridge Instellingen"},
/* pl */ {IDMS_CART_IO_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CART_IO_SETTINGS_SV, "Inst�llningar f�r insticksmoduler..."},

/* en */ {IDMS_REU_SETTINGS,    "REU settings..."},
/* de */ {IDMS_REU_SETTINGS_DE, "REU Einstellungen..."},
/* fr */ {IDMS_REU_SETTINGS_FR, "Param�tres REU..."},
/* hu */ {IDMS_REU_SETTINGS_HU, "REU be�ll�t�sai..."},
/* it */ {IDMS_REU_SETTINGS_IT, "Impostazioni REU..."},
/* nl */ {IDMS_REU_SETTINGS_NL, "REU instellingen..."},
/* pl */ {IDMS_REU_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_REU_SETTINGS_SV, "REU-inst�llningar..."},

/* en */ {IDMS_GEORAM_SETTINGS,    "GEORAM settings..."},
/* de */ {IDMS_GEORAM_SETTINGS_DE, "GEORAM Einstellungen..."},
/* fr */ {IDMS_GEORAM_SETTINGS_FR, "Param�tres GEORAM..."},
/* hu */ {IDMS_GEORAM_SETTINGS_HU, "GEORAM be�ll�t�sai..."},
/* it */ {IDMS_GEORAM_SETTINGS_IT, "Impostazioni GEORAM..."},
/* nl */ {IDMS_GEORAM_SETTINGS_NL, "GEORAM instellingen..."},
/* pl */ {IDMS_GEORAM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_GEORAM_SETTINGS_SV, "GEORAM-inst�llningar..."},

/* en */ {IDMS_RAMCART_SETTINGS,    "RAMCART settings..."},
/* de */ {IDMS_RAMCART_SETTINGS_DE, "RAMCART Einstellungen..."},
/* fr */ {IDMS_RAMCART_SETTINGS_FR, "Param�tres RAMCART..."},
/* hu */ {IDMS_RAMCART_SETTINGS_HU, "RAMCART be�ll�t�sai..."},
/* it */ {IDMS_RAMCART_SETTINGS_IT, "Impostazioni RAMCART..."},
/* nl */ {IDMS_RAMCART_SETTINGS_NL, "RAMCART instellingen..."},
/* pl */ {IDMS_RAMCART_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RAMCART_SETTINGS_SV, "RAMCART-inst�llningar..."},

/* en */ {IDMS_PLUS60K_SETTINGS,    "PLUS60K settings..."},
/* de */ {IDMS_PLUS60K_SETTINGS_DE, "PLUS60K Einstellungen..."},
/* fr */ {IDMS_PLUS60K_SETTINGS_FR, "Param�tres PLUS60K..."},
/* hu */ {IDMS_PLUS60K_SETTINGS_HU, "PLUS60K be�ll�t�sai..."},
/* it */ {IDMS_PLUS60K_SETTINGS_IT, "Impostazioni PLUS60K..."},
/* nl */ {IDMS_PLUS60K_SETTINGS_NL, "PLUS60K instellingen..."},
/* pl */ {IDMS_PLUS60K_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PLUS60K_SETTINGS_SV, "PLUS60K-inst�llningar..."},

/* en */ {IDS_PLUS256K_FILENAME,    "PLUS256K file"},
/* de */ {IDS_PLUS256K_FILENAME_DE, "PLUS256K Datei"},
/* fr */ {IDS_PLUS256K_FILENAME_FR, "Fichier PLUS256K"},
/* hu */ {IDS_PLUS256K_FILENAME_HU, "PLUS256K f�jl"},
/* it */ {IDS_PLUS256K_FILENAME_IT, "File PLUS256K"},
/* nl */ {IDS_PLUS256K_FILENAME_NL, "PLUS256K bestand"},
/* pl */ {IDS_PLUS256K_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLUS256K_FILENAME_SV, "PLUS256K-fil"},

/* en */ {IDS_PLUS256K_FILENAME_SELECT,    "Select file for PLUS256K"},
/* de */ {IDS_PLUS256K_FILENAME_SELECT_DE, "Datei f�r PLUS256K selektieren"},
/* fr */ {IDS_PLUS256K_FILENAME_SELECT_FR, "S�lectionner fichier pour PLUS256K"},
/* hu */ {IDS_PLUS256K_FILENAME_SELECT_HU, "V�lasszon f�jlt a PLUS256K-hoz"},
/* it */ {IDS_PLUS256K_FILENAME_SELECT_IT, "Seleziona file per PLUS256K"},
/* nl */ {IDS_PLUS256K_FILENAME_SELECT_NL, "Selecteer bestand voor PLUS256K"},
/* pl */ {IDS_PLUS256K_FILENAME_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLUS256K_FILENAME_SELECT_SV, "Ange fil f�r PLUS256K"},

/* en */ {IDMS_PLUS256K_SETTINGS,    "PLUS256K settings..."},
/* de */ {IDMS_PLUS256K_SETTINGS_DE, "PLUS256K Einstellungen..."},
/* fr */ {IDMS_PLUS256K_SETTINGS_FR, "Param�tres PLUS256K..."},
/* hu */ {IDMS_PLUS256K_SETTINGS_HU, "PLUS256K be�ll�t�sai..."},
/* it */ {IDMS_PLUS256K_SETTINGS_IT, "Impostazioni PLUS256K..."},
/* nl */ {IDMS_PLUS256K_SETTINGS_NL, "PLUS256K instellingen..."},
/* pl */ {IDMS_PLUS256K_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PLUS256K_SETTINGS_SV, "PLUS256K-inst�llningar..."},

/* en */ {IDMS_256K_SETTINGS,    "256K settings..."},
/* de */ {IDMS_256K_SETTINGS_DE, "256K Einstellungen..."},
/* fr */ {IDMS_256K_SETTINGS_FR, "Param�tres 256K..."},
/* hu */ {IDMS_256K_SETTINGS_HU, "256K be�ll�t�sai..."},
/* it */ {IDMS_256K_SETTINGS_IT, "Impostazioni 256K..."},
/* nl */ {IDMS_256K_SETTINGS_NL, "256K instellingen..."},
/* pl */ {IDMS_256K_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_256K_SETTINGS_SV, "256K-inst�llningar..."},

/* en */ {IDMS_IDE64_SETTINGS,    "IDE64 settings..."},
/* de */ {IDMS_IDE64_SETTINGS_DE, "IDE64 Einstellungen..."},
/* fr */ {IDMS_IDE64_SETTINGS_FR, "Param�tres IDE64..."},
/* hu */ {IDMS_IDE64_SETTINGS_HU, "IDE64 be�ll�t�sai..."},
/* it */ {IDMS_IDE64_SETTINGS_IT, "Impostazioni IDE64..."},
/* nl */ {IDMS_IDE64_SETTINGS_NL, "IDE64 instellingen..."},
/* pl */ {IDMS_IDE64_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_IDE64_SETTINGS_SV, "IDE64-inst�llningar..."},

/* en */ {IDMS_MMC64_SETTINGS,    "MMC64 settings..."},
/* de */ {IDMS_MMC64_SETTINGS_DE, "MMC64 Einstellungen..."},
/* fr */ {IDMS_MMC64_SETTINGS_FR, "Param�tres MMC64..."},
/* hu */ {IDMS_MMC64_SETTINGS_HU, "MMC64 be�ll�t�sai..."},
/* it */ {IDMS_MMC64_SETTINGS_IT, "Impostazioni MMC64..."},
/* nl */ {IDMS_MMC64_SETTINGS_NL, "MMC64 instellingen..."},
/* pl */ {IDMS_MMC64_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_MMC64_SETTINGS_SV, "MMC64-inst�llningar..."},

/* en */ {IDMS_DIGIMAX_SETTINGS,    "Digimax settings..."},
/* de */ {IDMS_DIGIMAX_SETTINGS_DE, "Digimax Einstellungen..."},
/* fr */ {IDMS_DIGIMAX_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDMS_DIGIMAX_SETTINGS_HU, "Digimax be�ll�t�sai..."},
/* it */ {IDMS_DIGIMAX_SETTINGS_IT, "Impostazioni digimax..."},
/* nl */ {IDMS_DIGIMAX_SETTINGS_NL, "Digimax instellingen..."},
/* pl */ {IDMS_DIGIMAX_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DIGIMAX_SETTINGS_SV, "Digimaxinst�llningar..."},

/* en */ {IDMS_ETHERNET_SETTINGS,    "Ethernet settings..."},
/* de */ {IDMS_ETHERNET_SETTINGS_DE, "Ethernet Einstellungen..."},
/* fr */ {IDMS_ETHERNET_SETTINGS_FR, "�mulation Ethernet..."},
/* hu */ {IDMS_ETHERNET_SETTINGS_HU, "Ethernet be�ll�t�sai..."},
/* it */ {IDMS_ETHERNET_SETTINGS_IT, "Impostazioni Ethernet..."},
/* nl */ {IDMS_ETHERNET_SETTINGS_NL, "Ethernet instellingen..."},
/* pl */ {IDMS_ETHERNET_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ETHERNET_SETTINGS_SV, "Ethernetinst�llningar..."},

/* en */ {IDMS_ACIA_SETTINGS,    "ACIA settings..."},
/* de */ {IDMS_ACIA_SETTINGS_DE, "ACIA Einstellungen..."},
/* fr */ {IDMS_ACIA_SETTINGS_FR, "Param�tres ACIA..."},
/* hu */ {IDMS_ACIA_SETTINGS_HU, "ACIA be�ll�t�sai..."},
/* it */ {IDMS_ACIA_SETTINGS_IT, "Impostazioni ACIA..."},
/* nl */ {IDMS_ACIA_SETTINGS_NL, "ACIA instellingen..."},
/* pl */ {IDMS_ACIA_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ACIA_SETTINGS_SV, "ACIA-inst�llningar..."},

/* en */ {IDMS_PETREU_SETTINGS,    "PET REU settings..."},
/* de */ {IDMS_PETREU_SETTINGS_DE, "PET REU Einstellungen..."},
/* fr */ {IDMS_PETREU_SETTINGS_FR, "Param�tres PET REU..."},
/* hu */ {IDMS_PETREU_SETTINGS_HU, "PET REU be�ll�t�sai..."},
/* it */ {IDMS_PETREU_SETTINGS_IT, "Impostazioni PET REU..."},
/* nl */ {IDMS_PETREU_SETTINGS_NL, "PET REU instellingen..."},
/* pl */ {IDMS_PETREU_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_PETREU_SETTINGS_SV, "PET REU-inst�llningar..."},

/* en */ {IDMS_SIDCART_SETTINGS,    "SID cart settings..."},
/* de */ {IDMS_SIDCART_SETTINGS_DE, "SID Modul Einstellungen..."},
/* fr */ {IDMS_SIDCART_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDMS_SIDCART_SETTINGS_HU, "SID k�rtya be�ll�t�sai..."},
/* it */ {IDMS_SIDCART_SETTINGS_IT, "Impostazioni cartuccia SID..."},
/* nl */ {IDMS_SIDCART_SETTINGS_NL, "SID cart instellingen..."},
/* pl */ {IDMS_SIDCART_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SIDCART_SETTINGS_SV, "SID-modulinst�llningar..."},

/* en */ {IDMS_RS232_USERPORT_SETTINGS,    "RS232 userport settings..."},
/* de */ {IDMS_RS232_USERPORT_SETTINGS_DE, "RS232 Userport Einstellungen..."},
/* fr */ {IDMS_RS232_USERPORT_SETTINGS_FR, "Param�tres RS232 userport..."},
/* hu */ {IDMS_RS232_USERPORT_SETTINGS_HU, "RS232 userport be�ll�t�sai..."},
/* it */ {IDMS_RS232_USERPORT_SETTINGS_IT, "Impostazioni RS232 su userport..."},
/* nl */ {IDMS_RS232_USERPORT_SETTINGS_NL, "RS232 userport instellingen..."},
/* pl */ {IDMS_RS232_USERPORT_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_RS232_USERPORT_SETTINGS_SV, "RS232-anv�ndarportinst�llningar..."},

/* en */ {IDMS_SAVE_CURRENT_SETTINGS,    "Save current settings"},
/* de */ {IDMS_SAVE_CURRENT_SETTINGS_DE, "Einstellungen speichern"},
/* fr */ {IDMS_SAVE_CURRENT_SETTINGS_FR, "Enregistrer les param�tres courants"},
/* hu */ {IDMS_SAVE_CURRENT_SETTINGS_HU, "Jelenlegi be�ll�t�sok ment�se"},
/* it */ {IDMS_SAVE_CURRENT_SETTINGS_IT, "Salva le impostazioni attuali"},
/* nl */ {IDMS_SAVE_CURRENT_SETTINGS_NL, "Huidige instellingen opslaan"},
/* pl */ {IDMS_SAVE_CURRENT_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_CURRENT_SETTINGS_SV, "Spara nuvarande inst�llningar"},

/* en */ {IDMS_LOAD_SAVED_SETTINGS,    "Load saved settings"},
/* de */ {IDMS_LOAD_SAVED_SETTINGS_DE, "Einstellungen Laden"},
/* fr */ {IDMS_LOAD_SAVED_SETTINGS_FR, "Charger les param�tres"},
/* hu */ {IDMS_LOAD_SAVED_SETTINGS_HU, "Be�ll�t�sok bet�lt�se"},
/* it */ {IDMS_LOAD_SAVED_SETTINGS_IT, "Carica le impostazioni salvate"},
/* nl */ {IDMS_LOAD_SAVED_SETTINGS_NL, "Opgeslagen instelling laden"},
/* pl */ {IDMS_LOAD_SAVED_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LOAD_SAVED_SETTINGS_SV, "L�s sparade inst�llningar"},

/* en */ {IDMS_SET_DEFAULT_SETTINGS,    "Set default settings"},
/* de */ {IDMS_SET_DEFAULT_SETTINGS_DE, "Wiederherstellen Standard Einstellungen"},
/* fr */ {IDMS_SET_DEFAULT_SETTINGS_FR, "R�tablir les param�tres par d�faut"},
/* hu */ {IDMS_SET_DEFAULT_SETTINGS_HU, "Alap�rtelmez�s be�ll�t�sa"},
/* it */ {IDMS_SET_DEFAULT_SETTINGS_IT, "Ripristina le impostazioni originarie"},
/* nl */ {IDMS_SET_DEFAULT_SETTINGS_NL, "Herstel standaard instellingen"},
/* pl */ {IDMS_SET_DEFAULT_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SET_DEFAULT_SETTINGS_SV, "�terst�ll f�rvalda inst�llningar"},

/* en */ {IDMS_SAVE_SETTING_ON_EXIT,    "Save settings on exit"},
/* de */ {IDMS_SAVE_SETTING_ON_EXIT_DE, "Einstellungen beim Beenden speichern"},
/* fr */ {IDMS_SAVE_SETTING_ON_EXIT_FR, "Enregistrer les param�tres � la sortie"},
/* hu */ {IDMS_SAVE_SETTING_ON_EXIT_HU, "Be�ll�t�sok ment�se kil�p�skor"},
/* it */ {IDMS_SAVE_SETTING_ON_EXIT_IT, "Salva le impostazioni in uscita"},
/* nl */ {IDMS_SAVE_SETTING_ON_EXIT_NL, "Sla instellingen op bij afsluiten"},
/* pl */ {IDMS_SAVE_SETTING_ON_EXIT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_SAVE_SETTING_ON_EXIT_SV, "Spara inst�llningar vid avslut"},

/* en */ {IDMS_CONFIRM_ON_EXIT,    "Confirm on exit"},
/* de */ {IDMS_CONFIRM_ON_EXIT_DE, "Best�tigung beim Beenden"},
/* fr */ {IDMS_CONFIRM_ON_EXIT_FR, "Confirmation � la sortie"},
/* hu */ {IDMS_CONFIRM_ON_EXIT_HU, "Meger�s�t�s kil�p�skor"},
/* it */ {IDMS_CONFIRM_ON_EXIT_IT, "Conferma all'uscita"},
/* nl */ {IDMS_CONFIRM_ON_EXIT_NL, "Bevestigen bij afsluiten"},
/* pl */ {IDMS_CONFIRM_ON_EXIT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CONFIRM_ON_EXIT_SV, "Bekr�fta avslutning"},

/* en */ {IDMS_HELP,    "Help"},
/* de */ {IDMS_HELP_DE, "Hilfe"},
/* fr */ {IDMS_HELP_FR, "Aide"},
/* hu */ {IDMS_HELP_HU, "Seg�ts�g"},
/* it */ {IDMS_HELP_IT, "Aiuto"},
/* nl */ {IDMS_HELP_NL, "Help"},
/* pl */ {IDMS_HELP_PL, ""},  /* fuzzy */
/* sv */ {IDMS_HELP_SV, "Hj�lp"},

/* en */ {IDMS_ABOUT,    "About..."},
/* de */ {IDMS_ABOUT_DE, "�ber VICE..."},
/* fr */ {IDMS_ABOUT_FR, "� Propos..."},
/* hu */ {IDMS_ABOUT_HU, "A VICE-r�l..."},
/* it */ {IDMS_ABOUT_IT, "Informazioni..."},
/* nl */ {IDMS_ABOUT_NL, "Over VICE..."},
/* pl */ {IDMS_ABOUT_PL, ""},  /* fuzzy */
/* sv */ {IDMS_ABOUT_SV, "Om VICE..."},

/* en */ {IDMS_COMMAND_LINE_OPTIONS,    "Command line options"},
/* de */ {IDMS_COMMAND_LINE_OPTIONS_DE, "Kommandozeilen Optionen"},
/* fr */ {IDMS_COMMAND_LINE_OPTIONS_FR, "Options de ligne de commande"},
/* hu */ {IDMS_COMMAND_LINE_OPTIONS_HU, "Parancssori opci�k"},
/* it */ {IDMS_COMMAND_LINE_OPTIONS_IT, "Opzioni da riga di comando"},
/* nl */ {IDMS_COMMAND_LINE_OPTIONS_NL, "Commando invoer opties"},
/* pl */ {IDMS_COMMAND_LINE_OPTIONS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_COMMAND_LINE_OPTIONS_SV, "Kommandoradsflaggor"},

/* en */ {IDMS_CONTRIBUTORS,    "Contributors"},
/* de */ {IDMS_CONTRIBUTORS_DE, "VICE Hackers"},
/* fr */ {IDMS_CONTRIBUTORS_FR, "Contributeurs"},
/* hu */ {IDMS_CONTRIBUTORS_HU, "K�zrem�k�d�k"},
/* it */ {IDMS_CONTRIBUTORS_IT, "Collaboratori"},
/* nl */ {IDMS_CONTRIBUTORS_NL, "Medewerkers aan het VICE project"},
/* pl */ {IDMS_CONTRIBUTORS_PL, ""},  /* fuzzy */
/* sv */ {IDMS_CONTRIBUTORS_SV, "Bidragsl�mnare"},

/* en */ {IDMS_LICENSE,    "License"},
/* de */ {IDMS_LICENSE_DE, "Lizenz"},
/* fr */ {IDMS_LICENSE_FR, "License"},
/* hu */ {IDMS_LICENSE_HU, "Licensz"},
/* it */ {IDMS_LICENSE_IT, "Licenza"},
/* nl */ {IDMS_LICENSE_NL, "Licensie"},
/* pl */ {IDMS_LICENSE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LICENSE_SV, "Licens"},

/* en */ {IDMS_NO_WARRANTY,    "No warranty"},
/* de */ {IDMS_NO_WARRANTY_DE, "Keine Garantie!"},
/* fr */ {IDMS_NO_WARRANTY_FR, "Aucune garantie!"},
/* hu */ {IDMS_NO_WARRANTY_HU, "Nincs garancia!"},
/* it */ {IDMS_NO_WARRANTY_IT, "Nessuna garanzia"},
/* nl */ {IDMS_NO_WARRANTY_NL, "Geen garantie"},
/* pl */ {IDMS_NO_WARRANTY_PL, ""},  /* fuzzy */
/* sv */ {IDMS_NO_WARRANTY_SV, "Ingen garanti"},

/* en */ {IDMS_LANGUAGE_INTERNATIONAL,    "International"},
/* de */ {IDMS_LANGUAGE_INTERNATIONAL_DE, "International"},
/* fr */ {IDMS_LANGUAGE_INTERNATIONAL_FR, ""},  /* fuzzy */
/* hu */ {IDMS_LANGUAGE_INTERNATIONAL_HU, "Nemzetk�zi"},
/* it */ {IDMS_LANGUAGE_INTERNATIONAL_IT, "Internazionale"},
/* nl */ {IDMS_LANGUAGE_INTERNATIONAL_NL, "Internationaal"},
/* pl */ {IDMS_LANGUAGE_INTERNATIONAL_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_INTERNATIONAL_SV, "Internationell"},

/* en */ {IDMS_LANGUAGE_ENGLISH,    "English"},
/* de */ {IDMS_LANGUAGE_ENGLISH_DE, "Englisch"},
/* fr */ {IDMS_LANGUAGE_ENGLISH_FR, "Anglais"},
/* hu */ {IDMS_LANGUAGE_ENGLISH_HU, "Angol"},
/* it */ {IDMS_LANGUAGE_ENGLISH_IT, "Inglese"},
/* nl */ {IDMS_LANGUAGE_ENGLISH_NL, "Engels"},
/* pl */ {IDMS_LANGUAGE_ENGLISH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_ENGLISH_SV, "Engelska"},

/* en */ {IDMS_LANGUAGE_GERMAN,    "German"},
/* de */ {IDMS_LANGUAGE_GERMAN_DE, "Deutsch"},
/* fr */ {IDMS_LANGUAGE_GERMAN_FR, "Allemand"},
/* hu */ {IDMS_LANGUAGE_GERMAN_HU, "N�met"},
/* it */ {IDMS_LANGUAGE_GERMAN_IT, "Tedesco"},
/* nl */ {IDMS_LANGUAGE_GERMAN_NL, "Duits"},
/* pl */ {IDMS_LANGUAGE_GERMAN_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_GERMAN_SV, "Tyska"},

/* en */ {IDMS_LANGUAGE_FINNISH,    "Finnish"},
/* de */ {IDMS_LANGUAGE_FINNISH_DE, "Finnisch"},
/* fr */ {IDMS_LANGUAGE_FINNISH_FR, ""},  /* fuzzy */
/* hu */ {IDMS_LANGUAGE_FINNISH_HU, "Finn"},
/* it */ {IDMS_LANGUAGE_FINNISH_IT, "Finlandese"},
/* nl */ {IDMS_LANGUAGE_FINNISH_NL, "Fins"},
/* pl */ {IDMS_LANGUAGE_FINNISH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_FINNISH_SV, "Finska"},

/* en */ {IDMS_LANGUAGE_FRENCH,    "French"},
/* de */ {IDMS_LANGUAGE_FRENCH_DE, "Franz�sisch"},
/* fr */ {IDMS_LANGUAGE_FRENCH_FR, "Fran�ais"},
/* hu */ {IDMS_LANGUAGE_FRENCH_HU, "Francia"},
/* it */ {IDMS_LANGUAGE_FRENCH_IT, "Francese"},
/* nl */ {IDMS_LANGUAGE_FRENCH_NL, "Frans"},
/* pl */ {IDMS_LANGUAGE_FRENCH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_FRENCH_SV, "Franska"},

/* en */ {IDMS_LANGUAGE_HUNGARIAN,    "Hungarian"},
/* de */ {IDMS_LANGUAGE_HUNGARIAN_DE, "Ungarisch"},
/* fr */ {IDMS_LANGUAGE_HUNGARIAN_FR, ""},  /* fuzzy */
/* hu */ {IDMS_LANGUAGE_HUNGARIAN_HU, "Magyar"},
/* it */ {IDMS_LANGUAGE_HUNGARIAN_IT, "Ungherese"},
/* nl */ {IDMS_LANGUAGE_HUNGARIAN_NL, "Hongaars"},
/* pl */ {IDMS_LANGUAGE_HUNGARIAN_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_HUNGARIAN_SV, "Ungerska"},

/* en */ {IDMS_LANGUAGE_ITALIAN,    "Italian"},
/* de */ {IDMS_LANGUAGE_ITALIAN_DE, "Italienisch"},
/* fr */ {IDMS_LANGUAGE_ITALIAN_FR, "Italien"},
/* hu */ {IDMS_LANGUAGE_ITALIAN_HU, "Olasz"},
/* it */ {IDMS_LANGUAGE_ITALIAN_IT, "Italiano"},
/* nl */ {IDMS_LANGUAGE_ITALIAN_NL, "Italiaans"},
/* pl */ {IDMS_LANGUAGE_ITALIAN_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_ITALIAN_SV, "Italienska"},

/* en */ {IDMS_LANGUAGE_DUTCH,    "Dutch"},
/* de */ {IDMS_LANGUAGE_DUTCH_DE, "Holl�ndisch"},
/* fr */ {IDMS_LANGUAGE_DUTCH_FR, "Hollandais"},
/* hu */ {IDMS_LANGUAGE_DUTCH_HU, "Holland"},
/* it */ {IDMS_LANGUAGE_DUTCH_IT, "Olandese"},
/* nl */ {IDMS_LANGUAGE_DUTCH_NL, "Nederlands"},
/* pl */ {IDMS_LANGUAGE_DUTCH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_DUTCH_SV, "Nederl�ndska"},

/* en */ {IDMS_LANGUAGE_NORWEGIAN,    "Norwegian"},
/* de */ {IDMS_LANGUAGE_NORWEGIAN_DE, "Norwegisch"},
/* fr */ {IDMS_LANGUAGE_NORWEGIAN_FR, ""},  /* fuzzy */
/* hu */ {IDMS_LANGUAGE_NORWEGIAN_HU, "Norv�g"},
/* it */ {IDMS_LANGUAGE_NORWEGIAN_IT, "Norvegese"},
/* nl */ {IDMS_LANGUAGE_NORWEGIAN_NL, "Noors"},
/* pl */ {IDMS_LANGUAGE_NORWEGIAN_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_NORWEGIAN_SV, "Norska"},

/* en */ {IDMS_LANGUAGE_POLISH,    "Polish"},
/* de */ {IDMS_LANGUAGE_POLISH_DE, "Polnisch"},
/* fr */ {IDMS_LANGUAGE_POLISH_FR, "Polonais"},
/* hu */ {IDMS_LANGUAGE_POLISH_HU, "Lengyel"},
/* it */ {IDMS_LANGUAGE_POLISH_IT, "Polacco"},
/* nl */ {IDMS_LANGUAGE_POLISH_NL, "Pools"},
/* pl */ {IDMS_LANGUAGE_POLISH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_POLISH_SV, "Polska"},

/* en */ {IDMS_LANGUAGE_SWEDISH,    "Swedish"},
/* de */ {IDMS_LANGUAGE_SWEDISH_DE, "Schwedisch"},
/* fr */ {IDMS_LANGUAGE_SWEDISH_FR, "Su�dois"},
/* hu */ {IDMS_LANGUAGE_SWEDISH_HU, "Sv�d"},
/* it */ {IDMS_LANGUAGE_SWEDISH_IT, "Svedese"},
/* nl */ {IDMS_LANGUAGE_SWEDISH_NL, "Zweeds"},
/* pl */ {IDMS_LANGUAGE_SWEDISH_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_SWEDISH_SV, "Svenska"},

/* en */ {IDMS_LANGUAGE,    "Language"},
/* de */ {IDMS_LANGUAGE_DE, "Sprache"},
/* fr */ {IDMS_LANGUAGE_FR, "Langage"},
/* hu */ {IDMS_LANGUAGE_HU, "Nyelv"},
/* it */ {IDMS_LANGUAGE_IT, "Lingua"},
/* nl */ {IDMS_LANGUAGE_NL, "Taal"},
/* pl */ {IDMS_LANGUAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMS_LANGUAGE_SV, "Spr�k"},

/* en */ {IDMS_FULLSCREEN_STATUSBAR,    "Fullscreen Statusbar"},
/* de */ {IDMS_FULLSCREEN_STATUSBAR_DE, "Vollbild Statusleiste"},
/* fr */ {IDMS_FULLSCREEN_STATUSBAR_FR, ""},  /* fuzzy */
/* hu */ {IDMS_FULLSCREEN_STATUSBAR_HU, "�llapotsor teljes k�perny�n�l"},
/* it */ {IDMS_FULLSCREEN_STATUSBAR_IT, "Statusbar nella modalit� a tutto schermo"},
/* nl */ {IDMS_FULLSCREEN_STATUSBAR_NL, "Volscherm Statusbalk"},
/* pl */ {IDMS_FULLSCREEN_STATUSBAR_PL, ""},  /* fuzzy */
/* sv */ {IDMS_FULLSCREEN_STATUSBAR_SV, "Status rad vid fullsk�rm"},

/* en */ {IDMS_VIDEO_OVERLAY,    "Video overlay"},
/* de */ {IDMS_VIDEO_OVERLAY_DE, "Video Overlay"},
/* fr */ {IDMS_VIDEO_OVERLAY_FR, ""},  /* fuzzy */
/* hu */ {IDMS_VIDEO_OVERLAY_HU, "Vide� overlay"},
/* it */ {IDMS_VIDEO_OVERLAY_IT, "Overlay video"},
/* nl */ {IDMS_VIDEO_OVERLAY_NL, "Video overlay"},
/* pl */ {IDMS_VIDEO_OVERLAY_PL, ""},  /* fuzzy */
/* sv */ {IDMS_VIDEO_OVERLAY_SV, "Video overlay"},

/* en */ {IDMS_START_SOUND_RECORD,    "Start Sound Record..."},
/* de */ {IDMS_START_SOUND_RECORD_DE, "Tonaufnahme starten..."},
/* fr */ {IDMS_START_SOUND_RECORD_FR, ""},  /* fuzzy */
/* hu */ {IDMS_START_SOUND_RECORD_HU, "Hangfelv�tel ind�t�sa..."},
/* it */ {IDMS_START_SOUND_RECORD_IT, "Avvia la registrazione audio..."},
/* nl */ {IDMS_START_SOUND_RECORD_NL, "Start Geluid Opname..."},
/* pl */ {IDMS_START_SOUND_RECORD_PL, ""},  /* fuzzy */
/* sv */ {IDMS_START_SOUND_RECORD_SV, "Starta ljudinspelning..."},

/* en */ {IDMS_STOP_SOUND_RECORD,    "Stop Sound Record"},
/* de */ {IDMS_STOP_SOUND_RECORD_DE, "Tonaufnahme stoppen"},
/* fr */ {IDMS_STOP_SOUND_RECORD_FR, ""},  /* fuzzy */
/* hu */ {IDMS_STOP_SOUND_RECORD_HU, "Hangfelv�tel le�ll�t�sa..."},
/* it */ {IDMS_STOP_SOUND_RECORD_IT, "Interrompi la registrazione audio"},
/* nl */ {IDMS_STOP_SOUND_RECORD_NL, "Stop Geluid Opname"},
/* pl */ {IDMS_STOP_SOUND_RECORD_PL, ""},  /* fuzzy */
/* sv */ {IDMS_STOP_SOUND_RECORD_SV, "Stoppa ljudinspelning"},


/* ----------------------- AmigaOS Message/Error Strings ----------------------- */

/* en */ {IDMES_SETTINGS_SAVED_SUCCESS,    "Settings saved successfully."},
/* de */ {IDMES_SETTINGS_SAVED_SUCCESS_DE, "Einstellungen erfolgreich gespeichert."},
/* fr */ {IDMES_SETTINGS_SAVED_SUCCESS_FR, "Sauvegarde des param�tres effectu�e correctement."},
/* hu */ {IDMES_SETTINGS_SAVED_SUCCESS_HU, "Be�ll�t�sok sikeresen elmentve."},
/* it */ {IDMES_SETTINGS_SAVED_SUCCESS_IT, "Impostazioni salvate con successo."},
/* nl */ {IDMES_SETTINGS_SAVED_SUCCESS_NL, "Instellingen met succes opgeslagen."},
/* pl */ {IDMES_SETTINGS_SAVED_SUCCESS_PL, ""},  /* fuzzy */
/* sv */ {IDMES_SETTINGS_SAVED_SUCCESS_SV, "Inst�llningarna har sparats."},

/* en */ {IDMES_SETTINGS_LOAD_SUCCESS,    "Settings loaded successfully."},
/* de */ {IDMES_SETTINGS_LOAD_SUCCESS_DE, "Einstellungen erfolgreich geladen."},
/* fr */ {IDMES_SETTINGS_LOAD_SUCCESS_FR, "Chargement des param�tres r�ussi."},
/* hu */ {IDMES_SETTINGS_LOAD_SUCCESS_HU, "Be�ll�t�sok sikeresen elmentve."},
/* it */ {IDMES_SETTINGS_LOAD_SUCCESS_IT, "Impostazioni caricate con successo."},
/* nl */ {IDMES_SETTINGS_LOAD_SUCCESS_NL, "Instellingen met succes geladen."},
/* pl */ {IDMES_SETTINGS_LOAD_SUCCESS_PL, ""},  /* fuzzy */
/* sv */ {IDMES_SETTINGS_LOAD_SUCCESS_SV, "Inst�llningarna har l�sts in."},

/* en */ {IDMES_DFLT_SETTINGS_RESTORED,    "Default settings restored."},
/* de */ {IDMES_DFLT_SETTINGS_RESTORED_DE, "Standard wiederhergestellt."},
/* fr */ {IDMES_DFLT_SETTINGS_RESTORED_FR, "Param�tres par d�faut restaur�s."},
/* hu */ {IDMES_DFLT_SETTINGS_RESTORED_HU, "Alap�rtelmezett be�ll�t�sok vissza�ll�tva."},
/* it */ {IDMES_DFLT_SETTINGS_RESTORED_IT, "Impostazioni predefinite ripristinate."},
/* nl */ {IDMES_DFLT_SETTINGS_RESTORED_NL, "Standaard instellingen hersteld."},
/* pl */ {IDMES_DFLT_SETTINGS_RESTORED_PL, ""},  /* fuzzy */
/* sv */ {IDMES_DFLT_SETTINGS_RESTORED_SV, "F�rvalda inst�llningar �terst�llda."},

/* en */ {IDMES_VICE_MESSAGE,    "VICE Message"},
/* de */ {IDMES_VICE_MESSAGE_DE, "VICE Nachricht"},
/* fr */ {IDMES_VICE_MESSAGE_FR, "Message VICE"},
/* hu */ {IDMES_VICE_MESSAGE_HU, "VICE �zenet"},
/* it */ {IDMES_VICE_MESSAGE_IT, "Messaggio di VICE"},
/* nl */ {IDMES_VICE_MESSAGE_NL, "VICE Bericht"},
/* pl */ {IDMES_VICE_MESSAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_VICE_MESSAGE_SV, "VICE-meddelande"},

/* en */ {IDMES_OK,    "OK"},
/* de */ {IDMES_OK_DE, "OK"},
/* fr */ {IDMES_OK_FR, "OK"},
/* hu */ {IDMES_OK_HU, "OK"},
/* it */ {IDMES_OK_IT, "OK"},
/* nl */ {IDMES_OK_NL, "OK"},
/* pl */ {IDMES_OK_PL, "OK"},
/* sv */ {IDMES_OK_SV, "OK"},

/* en */ {IDMES_VICE_CONTRIBUTORS,    "VICE contributors"},
/* de */ {IDMES_VICE_CONTRIBUTORS_DE, "VICE Hackers"},
/* fr */ {IDMES_VICE_CONTRIBUTORS_FR, "VICE Contributeurs"},
/* hu */ {IDMES_VICE_CONTRIBUTORS_HU, "VICE - k�zrem�k�d�k"},
/* it */ {IDMES_VICE_CONTRIBUTORS_IT, "Collaboratori al progetto VICE"},
/* nl */ {IDMES_VICE_CONTRIBUTORS_NL, "Medewerkers aan het VICE project"},
/* pl */ {IDMES_VICE_CONTRIBUTORS_PL, ""},  /* fuzzy */
/* sv */ {IDMES_VICE_CONTRIBUTORS_SV, "VICE Bidragsl�mnare"},

/* en */ {IDMES_WHO_MADE_WHAT,    "Who made what?"},
/* de */ {IDMES_WHO_MADE_WHAT_DE, "Wer hat was gemacht?"},
/* fr */ {IDMES_WHO_MADE_WHAT_FR, "Qui fait quoi?"},
/* hu */ {IDMES_WHO_MADE_WHAT_HU, "Ki mit csin�lt?"},
/* it */ {IDMES_WHO_MADE_WHAT_IT, "Chi ha fatto cosa?"},
/* nl */ {IDMES_WHO_MADE_WHAT_NL, "Wie heeft wat gemaakt?"},
/* pl */ {IDMES_WHO_MADE_WHAT_PL, ""},  /* fuzzy */
/* sv */ {IDMES_WHO_MADE_WHAT_SV, "Vem gjorde vad?"},

/* en */ {IDMES_VICE_DIST_NO_WARRANTY,    "VICE is distributed WITHOUT ANY WARRANTY!"},
/* de */ {IDMES_VICE_DIST_NO_WARRANTY_DE, "VICE ist ohne Garantie"},
/* fr */ {IDMES_VICE_DIST_NO_WARRANTY_FR, "VICE est distribu� SANS AUCUN GARANTIE!"},
/* hu */ {IDMES_VICE_DIST_NO_WARRANTY_HU, "A VICE-t garancia n�lk�l terjesztj�k!"},
/* it */ {IDMES_VICE_DIST_NO_WARRANTY_IT, "VICE � distribuito SENZA NESSUNA GARANZIA!"},
/* nl */ {IDMES_VICE_DIST_NO_WARRANTY_NL, "VICE heeft ABSOLUUT GEEN GARANTIE!"},
/* pl */ {IDMES_VICE_DIST_NO_WARRANTY_PL, ""},  /* fuzzy */
/* sv */ {IDMES_VICE_DIST_NO_WARRANTY_SV, "VICE distribueras UTAN N�GON SOM HELST GARANTI!"},

/* en */ {IDMES_WHICH_COMMANDS_AVAILABLE,    "Which command line options are available?"},
/* de */ {IDMES_WHICH_COMMANDS_AVAILABLE_DE, "Welche Kommandozeilen Parameter sind verf�gber?"},
/* fr */ {IDMES_WHICH_COMMANDS_AVAILABLE_FR, "Quelles sont les lignes de commandes disponibles?"},
/* hu */ {IDMES_WHICH_COMMANDS_AVAILABLE_HU, "Milyen parancssori opci�k lehets�gesek?"},
/* it */ {IDMES_WHICH_COMMANDS_AVAILABLE_IT, "Quali parametri da riga di comando sono disponibili?"},
/* nl */ {IDMES_WHICH_COMMANDS_AVAILABLE_NL, "Welke commando opties zijn beschikbaar?"},
/* pl */ {IDMES_WHICH_COMMANDS_AVAILABLE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_WHICH_COMMANDS_AVAILABLE_SV, "Vilka kommandoradsflaggor �r tillg�ngliga?"},

/* en */ {IDMES_CANNOT_SAVE_SETTINGS,    "Cannot save settings."},
/* de */ {IDMES_CANNOT_SAVE_SETTINGS_DE, "Fehler beim Speichern der Einstellungen."},
/* fr */ {IDMES_CANNOT_SAVE_SETTINGS_FR, "Impossible d'enregistrer les param�tres."},
/* hu */ {IDMES_CANNOT_SAVE_SETTINGS_HU, "A be�ll�t�sok ment�se nem siker�lt."},
/* it */ {IDMES_CANNOT_SAVE_SETTINGS_IT, "Impossibile salvare le impostazioni."},
/* nl */ {IDMES_CANNOT_SAVE_SETTINGS_NL, "Kan de instellingen niet opslaan."},
/* pl */ {IDMES_CANNOT_SAVE_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMES_CANNOT_SAVE_SETTINGS_SV, "Kan inte spara inst�llningar."},

/* en */ {IDMES_CANNOT_LOAD_SETTINGS,    "Cannot load settings."},
/* de */ {IDMES_CANNOT_LOAD_SETTINGS_DE, "Fehler beim Laden der Einstellungen."},
/* fr */ {IDMES_CANNOT_LOAD_SETTINGS_FR, "Impossible de charger les param�tres."},
/* hu */ {IDMES_CANNOT_LOAD_SETTINGS_HU, "A be�ll�t�sok bet�lt�se nem siker�lt."},
/* it */ {IDMES_CANNOT_LOAD_SETTINGS_IT, "Non � possibile caricare le impostazioni."},
/* nl */ {IDMES_CANNOT_LOAD_SETTINGS_NL, "Kan de instellingen niet laden."},
/* pl */ {IDMES_CANNOT_LOAD_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDMES_CANNOT_LOAD_SETTINGS_SV, "Kan inte l�sa inst�llningar."},

/* en */ {IDMES_VICE_ERROR,    "VICE Error"},
/* de */ {IDMES_VICE_ERROR_DE, "VICE Fehler"},
/* fr */ {IDMES_VICE_ERROR_FR, "Erreur VICE"},
/* hu */ {IDMES_VICE_ERROR_HU, "VICE hiba"},
/* it */ {IDMES_VICE_ERROR_IT, "Errore di VICE"},
/* nl */ {IDMES_VICE_ERROR_NL, "Vice Fout"},
/* pl */ {IDMES_VICE_ERROR_PL, ""},  /* fuzzy */
/* sv */ {IDMES_VICE_ERROR_SV, "VICE-fel"},

/* en */ {IDMES_NO_JOY_ON_PORT_D,    "No joystick or joypad found on port %d"},
/* de */ {IDMES_NO_JOY_ON_PORT_D_DE, "Kein Joystick oder Joypad am Port %d gefunden"},
/* fr */ {IDMES_NO_JOY_ON_PORT_D_FR, ""},  /* fuzzy */
/* hu */ {IDMES_NO_JOY_ON_PORT_D_HU, "Nem tal�ltam botkorm�nyt a %d porton"},
/* it */ {IDMES_NO_JOY_ON_PORT_D_IT, "Non � stato trovato nessun joystick o joypad sulla porta %d"},
/* nl */ {IDMES_NO_JOY_ON_PORT_D_NL, "Geen joystick of joypad gevonden op poort %d"},
/* pl */ {IDMES_NO_JOY_ON_PORT_D_PL, ""},  /* fuzzy */
/* sv */ {IDMES_NO_JOY_ON_PORT_D_SV, "Ingen joystick eller joypad hittades p� port %d"},

/* en */ {IDMES_MOUSE_ON_PORT_D,    "A mouse was found on port %d"},
/* de */ {IDMES_MOUSE_ON_PORT_D_DE, "Eine Maus wurde in Port %d gefunden"},
/* fr */ {IDMES_MOUSE_ON_PORT_D_FR, ""},  /* fuzzy */
/* hu */ {IDMES_MOUSE_ON_PORT_D_HU, "Nem tal�ltam egeret a %d porton"},
/* it */ {IDMES_MOUSE_ON_PORT_D_IT, "E' stato trovato un mouse sulla porta %d"},
/* nl */ {IDMES_MOUSE_ON_PORT_D_NL, "Een muis was gevonden op poort %d"},
/* pl */ {IDMES_MOUSE_ON_PORT_D_PL, ""},  /* fuzzy */
/* sv */ {IDMES_MOUSE_ON_PORT_D_SV, "En mus hittades p� port %d"},

/* en */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D,    "Unknown device found on port %d"},
/* de */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_DE, "Unbekanntes Ger�t im Port %d gefunden"},
/* fr */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_FR, ""},  /* fuzzy */
/* hu */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_HU, "%d porton ismeretlen eszk�zt tal�ltam"},
/* it */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_IT, "Dispositivo sconosciuto trovato sulla porta %d"},
/* nl */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_NL, "Een onbekent apparaat was gevonden op poort %d"},
/* pl */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_PL, ""},  /* fuzzy */
/* sv */ {IDMES_UNKNOWN_DEVICE_ON_PORT_D_SV, "En ok�nd enhet hittades p� port %d"},

/* en */ {IDMES_DEVICE_NOT_GAMEPAD,    "The device on this port is not a gamepad"},
/* de */ {IDMES_DEVICE_NOT_GAMEPAD_DE, "Das Ger�t in diesem Port ist kein Gamepad"},
/* fr */ {IDMES_DEVICE_NOT_GAMEPAD_FR, ""},  /* fuzzy */
/* hu */ {IDMES_DEVICE_NOT_GAMEPAD_HU, "Az eszk�z ezen a porton nem gamepad"},
/* it */ {IDMES_DEVICE_NOT_GAMEPAD_IT, "Il dispositivo su questa porta non � un gamepad"},
/* nl */ {IDMES_DEVICE_NOT_GAMEPAD_NL, "Het apparaat op deze poort is niet een gamepad"},
/* pl */ {IDMES_DEVICE_NOT_GAMEPAD_PL, ""},  /* fuzzy */
/* sv */ {IDMES_DEVICE_NOT_GAMEPAD_SV, "Enheten p� denna port �r ingen gamepad"},

/* en */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT,    "The port is not mapped to an Amiga port"},
/* de */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_DE, "Dieses Port ist keinem Amiga Port zugeordnet"},
/* fr */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_FR, ""},  /* fuzzy */
/* hu */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_HU, "A port nincs lek�pezve egy Amiga portra"},
/* it */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_IT, "Questa porta non � mappata ad una porta dell'Amiga"},
/* nl */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_NL, "De poort is niet verbonden met een Amiga poort"},
/* pl */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_PL, ""},  /* fuzzy */
/* sv */ {IDMES_NOT_MAPPED_TO_AMIGA_PORT_SV, "Porten �r inte bunden till en Amiga port"},

/* en */ {IDMES_CANNOT_AUTOSTART_FILE,    "Cannot autostart specified file."},
/* de */ {IDMES_CANNOT_AUTOSTART_FILE_DE, "Autostart von angeforderter Datei fehlgeschlagen."},
/* fr */ {IDMES_CANNOT_AUTOSTART_FILE_FR, "Impossible de d�marrer automatiquement le fichier sp�cifi�"},
/* hu */ {IDMES_CANNOT_AUTOSTART_FILE_HU, "Nem lehet automatikusan elind�tani a megadott f�jlt."},
/* it */ {IDMES_CANNOT_AUTOSTART_FILE_IT, "Non � possibile avviare automaticamente il file specificato."},
/* nl */ {IDMES_CANNOT_AUTOSTART_FILE_NL, "Kan opgegeven bestand niet autostarten."},
/* pl */ {IDMES_CANNOT_AUTOSTART_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_CANNOT_AUTOSTART_FILE_SV, "Kan inte autostarta angiven fil."},

/* en */ {IDMES_CANNOT_ATTACH_FILE,    "Cannot attach specified file"},
/* de */ {IDMES_CANNOT_ATTACH_FILE_DE, "Kann Image Datei nicht einlegen"},
/* fr */ {IDMES_CANNOT_ATTACH_FILE_FR, "Impossible d'attacher le fichier sp�cifi�"},
/* hu */ {IDMES_CANNOT_ATTACH_FILE_HU, "Nem lehet csatolni a megadott f�jlt"},
/* it */ {IDMES_CANNOT_ATTACH_FILE_IT, "Non � possibile selezionare il file specificato"},
/* nl */ {IDMES_CANNOT_ATTACH_FILE_NL, "Kan het opgegeven bestand niet koppelen"},
/* pl */ {IDMES_CANNOT_ATTACH_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_CANNOT_ATTACH_FILE_SV, "Kan inte ansluta filen"},

/* en */ {IDMES_INVALID_CART,    "Invalid cartridge"},
/* de */ {IDMES_INVALID_CART_DE, "Ung�ltiges Erweiterungsmodul"},
/* fr */ {IDMES_INVALID_CART_FR, "Cartouche invalide"},
/* hu */ {IDMES_INVALID_CART_HU, "�rv�nytelen cartridge"},
/* it */ {IDMES_INVALID_CART_IT, "Cartuccia non valida"},
/* nl */ {IDMES_INVALID_CART_NL, "Ongeldige cartridge"},
/* pl */ {IDMES_INVALID_CART_PL, ""},  /* fuzzy */
/* sv */ {IDMES_INVALID_CART_SV, "Ogiltig insticksmodul"},

/* en */ {IDMES_BAD_CART_CONFIG_IN_UI,    "Bad cartridge config in UI!"},
/* de */ {IDMES_BAD_CART_CONFIG_IN_UI_DE, "Fehlerhafte Modul Konfiguration im UI!"},
/* fr */ {IDMES_BAD_CART_CONFIG_IN_UI_FR, "Mauvaise configuration de la cartouche dans l'interface utilisateur!"},
/* hu */ {IDMES_BAD_CART_CONFIG_IN_UI_HU, "Rossz cartridge konfigur�ci�!"},
/* it */ {IDMES_BAD_CART_CONFIG_IN_UI_IT, "Configurazione della cartuccia errata nell'IU!"},
/* nl */ {IDMES_BAD_CART_CONFIG_IN_UI_NL, "Slechte cartridge configuratie in UI!"},
/* pl */ {IDMES_BAD_CART_CONFIG_IN_UI_PL, ""},  /* fuzzy */
/* sv */ {IDMES_BAD_CART_CONFIG_IN_UI_SV, "Felaktiga insticksmodulsinst�llningar i UI!"},

/* en */ {IDMES_INVALID_CART_IMAGE,    "Invalid cartridge image"},
/* de */ {IDMES_INVALID_CART_IMAGE_DE, "Ung�ltiges Erweiterungsmodul Image"},
/* fr */ {IDMES_INVALID_CART_IMAGE_FR, "Image de cartouche invalide"},
/* hu */ {IDMES_INVALID_CART_IMAGE_HU, "�rv�nytelen cartridge k�pm�s"},
/* it */ {IDMES_INVALID_CART_IMAGE_IT, "Immagine della cartuccia non valida"},
/* nl */ {IDMES_INVALID_CART_IMAGE_NL, "Ongeldig cartridge bestand"},
/* pl */ {IDMES_INVALID_CART_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_INVALID_CART_IMAGE_SV, "Ogiltig insticksmodulfil"},

/* en */ {IDMES_CANNOT_CREATE_IMAGE,    "Cannot create image"},
/* de */ {IDMES_CANNOT_CREATE_IMAGE_DE, "Kann Image Datei nicht erzeugen"},
/* fr */ {IDMES_CANNOT_CREATE_IMAGE_FR, "Impossible de cr�er l'image"},
/* hu */ {IDMES_CANNOT_CREATE_IMAGE_HU, "Nem siker�lt a k�pm�st l�trehozni"},
/* it */ {IDMES_CANNOT_CREATE_IMAGE_IT, "Non � possibile creare l'immagine"},
/* nl */ {IDMES_CANNOT_CREATE_IMAGE_NL, "Kan bestand niet maken"},
/* pl */ {IDMES_CANNOT_CREATE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDMES_CANNOT_CREATE_IMAGE_SV, "Kan inte skapa avbildningsfil"},

/* en */ {IDMES_ERROR_STARTING_SERVER,    "An error occured starting the server."},
/* de */ {IDMES_ERROR_STARTING_SERVER_DE, "Fehler beim Starten des Netplay Servers."},
/* fr */ {IDMES_ERROR_STARTING_SERVER_FR, ""},  /* fuzzy */
/* hu */ {IDMES_ERROR_STARTING_SERVER_HU, "Hiba t�rt�nt a j�t�k kiszolg�l� ind�t�sakor."},
/* it */ {IDMES_ERROR_STARTING_SERVER_IT, "Si � verificato un errore all'avvio del server."},
/* nl */ {IDMES_ERROR_STARTING_SERVER_NL, "Er is een fout opgetreden bij het opstarten van de server"},
/* pl */ {IDMES_ERROR_STARTING_SERVER_PL, ""},  /* fuzzy */
/* sv */ {IDMES_ERROR_STARTING_SERVER_SV, "Fel vid anslutning till server."},

/* en */ {IDMES_ERROR_CONNECTING_CLIENT,    "An error occured connecting the client."},
/* de */ {IDMES_ERROR_CONNECTING_CLIENT_DE, "Fehler beim Verbindungsaufbau zum Client"},
/* fr */ {IDMES_ERROR_CONNECTING_CLIENT_FR, ""},  /* fuzzy */
/* hu */ {IDMES_ERROR_CONNECTING_CLIENT_HU, "Hiba t�rt�nt a kapcsol�d�skor."},
/* it */ {IDMES_ERROR_CONNECTING_CLIENT_IT, "Si � verificato un errore durante il collegamento col client."},
/* nl */ {IDMES_ERROR_CONNECTING_CLIENT_NL, "Een fout is opgetreden bij het verbinden."},
/* pl */ {IDMES_ERROR_CONNECTING_CLIENT_PL, ""},  /* fuzzy */
/* sv */ {IDMES_ERROR_CONNECTING_CLIENT_SV, "Fel vid anslutning till klient."},

/* en */ {IDMES_INVALID_PORT_NUMBER,    "Invalid port number"},
/* de */ {IDMES_INVALID_PORT_NUMBER_DE, "Ung�ltige Port Nummer"},
/* fr */ {IDMES_INVALID_PORT_NUMBER_FR, ""},  /* fuzzy */
/* hu */ {IDMES_INVALID_PORT_NUMBER_HU, "�rv�nytelen port sz�m"},
/* it */ {IDMES_INVALID_PORT_NUMBER_IT, "Numero di porta non valido"},
/* nl */ {IDMES_INVALID_PORT_NUMBER_NL, "Ongeldig poort nummer"},
/* pl */ {IDMES_INVALID_PORT_NUMBER_PL, ""},  /* fuzzy */
/* sv */ {IDMES_INVALID_PORT_NUMBER_SV, "Ogiltigt portnummer"},

/* en */ {IDMES_THIS_MACHINE_NO_SID,    "This machine may not have a SID"},
/* de */ {IDMES_THIS_MACHINE_NO_SID_DE, "Diese Maschine hat m�glicherweise keinen SID"},
/* fr */ {IDMES_THIS_MACHINE_NO_SID_FR, "Cette machine n'a peut-�tre pas de SID"},
/* hu */ {IDMES_THIS_MACHINE_NO_SID_HU, "Ennek a g�pnek nem lehet SIDje"},
/* it */ {IDMES_THIS_MACHINE_NO_SID_IT, "Questa macchina potrebbe non avere un SID"},
/* nl */ {IDMES_THIS_MACHINE_NO_SID_NL, "Deze machine heeft mogelijk geen SID"},
/* pl */ {IDMES_THIS_MACHINE_NO_SID_PL, ""},  /* fuzzy */
/* sv */ {IDMES_THIS_MACHINE_NO_SID_SV, "Denna maskin kan inte ha en SID"},

/* en */ {IDMES_SCREENSHOT_SAVE_S_FAILED,    "Screenshot save of %s failed"},
/* de */ {IDMES_SCREENSHOT_SAVE_S_FAILED_DE, "Screenshot speicherung von %s ist fehlgeschlagen"},
/* fr */ {IDMES_SCREENSHOT_SAVE_S_FAILED_FR, ""},  /* fuzzy */
/* hu */ {IDMES_SCREENSHOT_SAVE_S_FAILED_HU, "F�nyk�p f�jl ment�se %s f�jlba nem siker�lt"},
/* it */ {IDMES_SCREENSHOT_SAVE_S_FAILED_IT, "Il salvataggio della schermata %s � fallito"},
/* nl */ {IDMES_SCREENSHOT_SAVE_S_FAILED_NL, "Opslaan van schermafdruk %s niet gelukt."},
/* pl */ {IDMES_SCREENSHOT_SAVE_S_FAILED_PL, ""},  /* fuzzy */
/* sv */ {IDMES_SCREENSHOT_SAVE_S_FAILED_SV, "Fel vid skrivning av sk�rmdump \"%s\""},

/* en */ {IDMES_SCREENSHOT_S_SAVED,    "Screenshot %s saved."},
/* de */ {IDMES_SCREENSHOT_S_SAVED_DE, "Screenshots %s gespeicher."},
/* fr */ {IDMES_SCREENSHOT_S_SAVED_FR, ""},  /* fuzzy */
/* hu */ {IDMES_SCREENSHOT_S_SAVED_HU, "F�nyk�p %s f�jlba elmentve."},
/* it */ {IDMES_SCREENSHOT_S_SAVED_IT, "Schermata %s salvata."},
/* nl */ {IDMES_SCREENSHOT_S_SAVED_NL, "Schermafdruk %s opgeslagen."},
/* pl */ {IDMES_SCREENSHOT_S_SAVED_PL, ""},  /* fuzzy */
/* sv */ {IDMES_SCREENSHOT_S_SAVED_SV, "Sk�rmdump \"%s\" sparad."},

/* ----------------------- AmigaOS Strings ----------------------- */

/* en */ {IDS_PRESS_KEY_BUTTON,    "Press desired key/button, move stick or press ESC for no key."},
/* de */ {IDS_PRESS_KEY_BUTTON_DE, "Gew�nschte Taste/Knopf dr�cken, Kn�ppel bewegen oder ESC f�r keine Taste dr�cken."},
/* fr */ {IDS_PRESS_KEY_BUTTON_FR, ""},  /* fuzzy */
/* hu */ {IDS_PRESS_KEY_BUTTON_HU, "Nyomja meg a v�lasztott gombot, mozd�tsa a botkorm�nyt, vagy nyomjon ESC-et �reshez"},
/* it */ {IDS_PRESS_KEY_BUTTON_IT, "Premi il tasto/bottone desiderato, muovi la leva o premi ESC per non selezionare alcun tasto."},
/* nl */ {IDS_PRESS_KEY_BUTTON_NL, "Druk de toets/knop, beweeg de joystick of druk op ESC voor geen toets."},
/* pl */ {IDS_PRESS_KEY_BUTTON_PL, ""},  /* fuzzy */
/* sv */ {IDS_PRESS_KEY_BUTTON_SV, "Tryck �nskad tangent/knapp, r�r joysticken eller tryck ESC f�r att inte v�lja n�gon knapp."},

/* en */ {IDS_SAVE,    "Save"},
/* de */ {IDS_SAVE_DE, "Speichern"},
/* fr */ {IDS_SAVE_FR, "Enregistrer"},
/* hu */ {IDS_SAVE_HU, "Ment�s"},
/* it */ {IDS_SAVE_IT, "Salva"},
/* nl */ {IDS_SAVE_NL, "Opslaan"},
/* pl */ {IDS_SAVE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_SV, "Spara"},

/* en */ {IDS_BROWSE,    "Browse"},
/* de */ {IDS_BROWSE_DE, "St�bern"},
/* fr */ {IDS_BROWSE_FR, "Parcourir"},
/* hu */ {IDS_BROWSE_HU, "Tall�z"},
/* it */ {IDS_BROWSE_IT, "Sfoglia"},
/* nl */ {IDS_BROWSE_NL, "Bladeren"},
/* pl */ {IDS_BROWSE_PL, ""},  /* fuzzy */
/* sv */ {IDS_BROWSE_SV, "Bl�ddra"},

/* en */ {IDS_CURRENT_MODE,    "Current mode"},
/* de */ {IDS_CURRENT_MODE_DE, "Aktueller Modus"},
/* fr */ {IDS_CURRENT_MODE_FR, ""},  /* fuzzy */
/* hu */ {IDS_CURRENT_MODE_HU, "Jelenlegi m�d"},
/* it */ {IDS_CURRENT_MODE_IT, "Modalit� attuale"},
/* nl */ {IDS_CURRENT_MODE_NL, "Huidige modus"},
/* pl */ {IDS_CURRENT_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDS_CURRENT_MODE_SV, "Nuvarande l�ge"},

/* en */ {IDS_TCP_PORT,    "TCP-Port"},
/* de */ {IDS_TCP_PORT_DE, "TCP-Port"},
/* fr */ {IDS_TCP_PORT_FR, ""},  /* fuzzy */
/* hu */ {IDS_TCP_PORT_HU, "TCP-Port"},
/* it */ {IDS_TCP_PORT_IT, "Porta TCP"},
/* nl */ {IDS_TCP_PORT_NL, "TCP-Poort"},
/* pl */ {IDS_TCP_PORT_PL, ""},  /* fuzzy */
/* sv */ {IDS_TCP_PORT_SV, "TCP-port"},

/* en */ {IDS_START_SERVER,    "Start Server"},
/* de */ {IDS_START_SERVER_DE, "Server starten"},
/* fr */ {IDS_START_SERVER_FR, ""},  /* fuzzy */
/* hu */ {IDS_START_SERVER_HU, "Kiszolg�l� elind�t�sa"},
/* it */ {IDS_START_SERVER_IT, "Avvia server"},
/* nl */ {IDS_START_SERVER_NL, "Start server"},
/* pl */ {IDS_START_SERVER_PL, ""},  /* fuzzy */
/* sv */ {IDS_START_SERVER_SV, "Starta server"},

/* en */ {IDS_CONNECT_TO,    "Connect to"},
/* de */ {IDS_CONNECT_TO_DE, "Verbinden mit"},
/* fr */ {IDS_CONNECT_TO_FR, ""},  /* fuzzy */
/* hu */ {IDS_CONNECT_TO_HU, "Kapcsol�d�s ehhez:"},
/* it */ {IDS_CONNECT_TO_IT, "Connetti a"},
/* nl */ {IDS_CONNECT_TO_NL, "Maak verbinding naar"},
/* pl */ {IDS_CONNECT_TO_PL, ""},  /* fuzzy */
/* sv */ {IDS_CONNECT_TO_SV, "Anslut till"},

/* en */ {IDS_DISCONNECT,    "Disconnect"},
/* de */ {IDS_DISCONNECT_DE, "Verbindung Trennen"},
/* fr */ {IDS_DISCONNECT_FR, ""},  /* fuzzy */
/* hu */ {IDS_DISCONNECT_HU, "Sz�tkapcsol�d�s"},
/* it */ {IDS_DISCONNECT_IT, "Disconnetti"},
/* nl */ {IDS_DISCONNECT_NL, "Verbreek verbinding"},
/* pl */ {IDS_DISCONNECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_DISCONNECT_SV, "Koppla fr�n"},

/* en */ {IDS_IDLE,    "Idle"},
/* de */ {IDS_IDLE_DE, "Idle"},
/* fr */ {IDS_IDLE_FR, ""},  /* fuzzy */
/* hu */ {IDS_IDLE_HU, "T�tlen"},
/* it */ {IDS_IDLE_IT, "Inattivo"},
/* nl */ {IDS_IDLE_NL, "Idle"},
/* pl */ {IDS_IDLE_PL, ""},  /* fuzzy */
/* sv */ {IDS_IDLE_SV, "V�ntar"},

/* en */ {IDS_SERVER_LISTENING,    "Server listening"},
/* de */ {IDS_SERVER_LISTENING_DE, "Server wartet auf Verbindung"},
/* fr */ {IDS_SERVER_LISTENING_FR, ""},  /* fuzzy */
/* hu */ {IDS_SERVER_LISTENING_HU, "V�rakoz�s kliensre"},
/* it */ {IDS_SERVER_LISTENING_IT, "Server in ascolto"},
/* nl */ {IDS_SERVER_LISTENING_NL, "Server wacht op verbinding"},
/* pl */ {IDS_SERVER_LISTENING_PL, ""},  /* fuzzy */
/* sv */ {IDS_SERVER_LISTENING_SV, "Servern lyssnar"},

/* en */ {IDS_CONNECTED_SERVER,    "Connected server"},
/* de */ {IDS_CONNECTED_SERVER_DE, "Verbunden, Server"},
/* fr */ {IDS_CONNECTED_SERVER_FR, ""},  /* fuzzy */
/* hu */ {IDS_CONNECTED_SERVER_HU, "Kiszolg�l� a kapcsolatban"},
/* it */ {IDS_CONNECTED_SERVER_IT, "Server connesso"},
/* nl */ {IDS_CONNECTED_SERVER_NL, "Verbonden Server"},
/* pl */ {IDS_CONNECTED_SERVER_PL, ""},  /* fuzzy */
/* sv */ {IDS_CONNECTED_SERVER_SV, "Ansluten server"},

/* en */ {IDS_CONNECTED_CLIENT,    "Connected client"},
/* de */ {IDS_CONNECTED_CLIENT_DE, "Verbunden, Client"},
/* fr */ {IDS_CONNECTED_CLIENT_FR, ""},  /* fuzzy */
/* hu */ {IDS_CONNECTED_CLIENT_HU, "Kapcsol�d� kliens"},
/* it */ {IDS_CONNECTED_CLIENT_IT, "Client connesso"},
/* nl */ {IDS_CONNECTED_CLIENT_NL, "Verbonden Client"},
/* pl */ {IDS_CONNECTED_CLIENT_PL, ""},  /* fuzzy */
/* sv */ {IDS_CONNECTED_CLIENT_SV, "Ansluten klient"},

/* en */ {IDS_NETPLAY_SETTINGS,    "Netplay Settings"},
/* de */ {IDS_NETPLAY_SETTINGS_DE, "Netplay Einstellungen"},
/* fr */ {IDS_NETPLAY_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_NETPLAY_SETTINGS_HU, "H�l�zati j�t�k be�ll�t�sai"},
/* it */ {IDS_NETPLAY_SETTINGS_IT, "Impostazioni Netplay"},
/* nl */ {IDS_NETPLAY_SETTINGS_NL, "netplay Instellingen"},
/* pl */ {IDS_NETPLAY_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_NETPLAY_SETTINGS_SV, "Inst�llningar f�r n�tverksspel"},

/* en */ {IDS_SAVE_SCREENSHOT,    "Save Screenshot"},
/* de */ {IDS_SAVE_SCREENSHOT_DE, "Screenshot Speichern"},
/* fr */ {IDS_SAVE_SCREENSHOT_FR, "Enregistrer une capture d'�cran"},
/* hu */ {IDS_SAVE_SCREENSHOT_HU, "F�nyk�p ment�se"},
/* it */ {IDS_SAVE_SCREENSHOT_IT, "Salva schermata"},
/* nl */ {IDS_SAVE_SCREENSHOT_NL, "Scherm afdruk opslaan"},
/* pl */ {IDS_SAVE_SCREENSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_SCREENSHOT_SV, "Spara sk�rmdump"},

/* en */ {IDS_CANCEL,    "Cancel"},
/* de */ {IDS_CANCEL_DE, "Abbruch"},
/* fr */ {IDS_CANCEL_FR, "Annuler"},
/* hu */ {IDS_CANCEL_HU, "M�gsem"},
/* it */ {IDS_CANCEL_IT, "Annulla"},
/* nl */ {IDS_CANCEL_NL, "Annuleren"},
/* pl */ {IDS_CANCEL_PL, ""},  /* fuzzy */
/* sv */ {IDS_CANCEL_SV, "Avbryt"},

/* en */ {IDS_CHOOSE_SCREENSHOT_FORMAT,    "Choose screenshot format"},
/* de */ {IDS_CHOOSE_SCREENSHOT_FORMAT_DE, "W�hle Screenshot Format"},
/* fr */ {IDS_CHOOSE_SCREENSHOT_FORMAT_FR, ""},  /* fuzzy */
/* hu */ {IDS_CHOOSE_SCREENSHOT_FORMAT_HU, "V�lasszon f�nyk�p form�tumot"},
/* it */ {IDS_CHOOSE_SCREENSHOT_FORMAT_IT, "Seleziona il formato per il salvataggio della schermata"},
/* nl */ {IDS_CHOOSE_SCREENSHOT_FORMAT_NL, "Kies schermafdruk formaat"},
/* pl */ {IDS_CHOOSE_SCREENSHOT_FORMAT_PL, ""},  /* fuzzy */
/* sv */ {IDS_CHOOSE_SCREENSHOT_FORMAT_SV, "V�lj �nskat format f�r sk�rmdumpen"},

/* en */ {IDS_SAVE_S_SCREENSHOT,    "Save %s screenshot"},
/* de */ {IDS_SAVE_S_SCREENSHOT_DE, "Screenshot %s speichern"},
/* fr */ {IDS_SAVE_S_SCREENSHOT_FR, ""},  /* fuzzy */
/* hu */ {IDS_SAVE_S_SCREENSHOT_HU, "%s f�nyk�p ment�se"},
/* it */ {IDS_SAVE_S_SCREENSHOT_IT, "Salva schermata %s"},
/* nl */ {IDS_SAVE_S_SCREENSHOT_NL, "Sla schermafdruk %s op"},
/* pl */ {IDS_SAVE_S_SCREENSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_S_SCREENSHOT_SV, "Spara sk�rmdumpen i %s format"},

/* en */ {IDS_DETACHED_DEVICE_D,    "Detached device %d"},
/* de */ {IDS_DETACHED_DEVICE_D_DE, "Ger�t %d entfernt"},
/* fr */ {IDS_DETACHED_DEVICE_D_FR, "P�riph�rique d�tach� %d"},
/* hu */ {IDS_DETACHED_DEVICE_D_HU, "%d eszk�z lev�lasztva"},
/* it */ {IDS_DETACHED_DEVICE_D_IT, "Rimosso dispositivo %d"},
/* nl */ {IDS_DETACHED_DEVICE_D_NL, "Apparaat %d ontkoppelt"},
/* pl */ {IDS_DETACHED_DEVICE_D_PL, ""},  /* fuzzy */
/* sv */ {IDS_DETACHED_DEVICE_D_SV, "Kopplade fran enhet %d"},

/* en */ {IDS_ATTACHED_S_TO_DEVICE_D,    "Attached %s to device#%d"},
/* de */ {IDS_ATTACHED_S_TO_DEVICE_D_DE, "%s mit Ger�t #%d verbunden"},
/* fr */ {IDS_ATTACHED_S_TO_DEVICE_D_FR, "%s attach� au p�riph�rique #%s"},
/* hu */ {IDS_ATTACHED_S_TO_DEVICE_D_HU, "%s csatolva #%d eszk�zh�z"},
/* it */ {IDS_ATTACHED_S_TO_DEVICE_D_IT, "Selezionato %s per la periferica #%d"},
/* nl */ {IDS_ATTACHED_S_TO_DEVICE_D_NL, "%s aan apparaat#%d gekoppelt"},
/* pl */ {IDS_ATTACHED_S_TO_DEVICE_D_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACHED_S_TO_DEVICE_D_SV, "Ansl�t %s till enhet %d"},

/* en */ {IDS_VICE_QUESTION,    "VICE Question"},
/* de */ {IDS_VICE_QUESTION_DE, "VICE Frage"},
/* fr */ {IDS_VICE_QUESTION_FR, "Question VICE"},
/* hu */ {IDS_VICE_QUESTION_HU, "VICE k�rd�s"},
/* it */ {IDS_VICE_QUESTION_IT, "Domanda di VICE"},
/* nl */ {IDS_VICE_QUESTION_NL, "VICE vraag"},
/* pl */ {IDS_VICE_QUESTION_PL, ""},  /* fuzzy */
/* sv */ {IDS_VICE_QUESTION_SV, "VICE-fraga"},

/* en */ {IDS_EXTEND_TO_40_TRACK,    "Extend image to 40-track format?"},
/* de */ {IDS_EXTEND_TO_40_TRACK_DE, "Image Dateu auf 40-Spur Format erweitern?"},
/* fr */ {IDS_EXTEND_TO_40_TRACK_FR, "�tendre l'image au format 40 pistes?"},
/* hu */ {IDS_EXTEND_TO_40_TRACK_HU, "Kiterjesszem a k�pm�st 40 s�vos form�tumra?"},
/* it */ {IDS_EXTEND_TO_40_TRACK_IT, "Astendere l'immagine al formato 40 tracce?"},
/* nl */ {IDS_EXTEND_TO_40_TRACK_NL, "Bestand uitbreiden naar 40-sporen formaat?"},
/* pl */ {IDS_EXTEND_TO_40_TRACK_PL, ""},  /* fuzzy */
/* sv */ {IDS_EXTEND_TO_40_TRACK_SV, "Ut�ka avbildning till 40-sparsformat?"},

/* en */ {IDS_YES_NO,    "Yes|No"},
/* de */ {IDS_YES_NO_DE, "Ja|Nein"},
/* fr */ {IDS_YES_NO_FR, ""},  /* fuzzy */
/* hu */ {IDS_YES_NO_HU, "Igen|Nem"},
/* it */ {IDS_YES_NO_IT, "S�|No"},
/* nl */ {IDS_YES_NO_NL, "Ja|Nee"},
/* pl */ {IDS_YES_NO_PL, ""},  /* fuzzy */
/* sv */ {IDS_YES_NO_SV, "Ja|Nej"},

/* en */ {IDS_DETACHED_TAPE,    "Detached tape"},
/* de */ {IDS_DETACHED_TAPE_DE, "Band Image entfernt"},
/* fr */ {IDS_DETACHED_TAPE_FR, "Datassette d�tach�"},
/* hu */ {IDS_DETACHED_TAPE_HU, "Szalag lev�lasztva"},
/* it */ {IDS_DETACHED_TAPE_IT, "Cassetta rimossa"},
/* nl */ {IDS_DETACHED_TAPE_NL, "Tape ontkoppelt"},
/* pl */ {IDS_DETACHED_TAPE_PL, ""},  /* fuzzy */
/* sv */ {IDS_DETACHED_TAPE_SV, "Kopplade fran band"},

/* en */ {IDS_ATTACHED_TAPE_S,    "Attached tape %s"},
/* de */ {IDS_ATTACHED_TAPE_S_DE, "Band Image %s eingelegt"},
/* fr */ {IDS_ATTACHED_TAPE_S_FR, "Datassette %s attach�"},
/* hu */ {IDS_ATTACHED_TAPE_S_HU, "%s szalag csatolva"},
/* it */ {IDS_ATTACHED_TAPE_S_IT, "Cassetta %s selezionata"},
/* nl */ {IDS_ATTACHED_TAPE_S_NL, "Tape %s gekoppelt"},
/* pl */ {IDS_ATTACHED_TAPE_S_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACHED_TAPE_S_SV, "Ansl�t band %s"},

/* en */ {IDS_SELECT_START_SNAPSHOT,    "Select start snapshot for event history"},
/* de */ {IDS_SELECT_START_SNAPSHOT_DE, "Startsnapshot f�r Ereignishistory festlegen"},
/* fr */ {IDS_SELECT_START_SNAPSHOT_FR, "S�lectionner la sauvegarde de d�part pour l'historique des �v�nements"},
/* hu */ {IDS_SELECT_START_SNAPSHOT_HU, "V�lasszon kezdeti pillanatk�pet az esem�ny r�gz�t�shez"},
/* it */ {IDS_SELECT_START_SNAPSHOT_IT, "Seleziona lo snapshot iniziale per la cronologia degli eventi"},
/* nl */ {IDS_SELECT_START_SNAPSHOT_NL, "Selecteer start momentopname voor gebeurtenis geschiedenis"},
/* pl */ {IDS_SELECT_START_SNAPSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_START_SNAPSHOT_SV, "V�lj start�gonblicksmodul f�r h�ndelsehistorik"},

/* en */ {IDS_SELECT_END_SNAPSHOT,    "Select end snapshot for event history"},
/* de */ {IDS_SELECT_END_SNAPSHOT_DE, "Startsnapshot f�r Ereignishistory festlegen"},
/* fr */ {IDS_SELECT_END_SNAPSHOT_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_END_SNAPSHOT_HU, "V�lasszon v�gs� pillanatk�pet az esem�ny r�gz�t�shez"},
/* it */ {IDS_SELECT_END_SNAPSHOT_IT, "Seleziona lo snapshot finale per la cronologia degli eventi"},
/* nl */ {IDS_SELECT_END_SNAPSHOT_NL, "Selecteer eind momentopname voor gebeurtenis geschiedenis"},
/* pl */ {IDS_SELECT_END_SNAPSHOT_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_END_SNAPSHOT_SV, "V�lj slut�gonblicksmodul f�r h�ndelsehistorik"},

/* en */ {IDS_REALLY_EXIT,    "Do you really want to exit?\n\nAll the data present in the emulated RAM will be lost."},
/* de */ {IDS_REALLY_EXIT_DE, "Willst Du wirklich beenden?\n\nDaten im emulierten Speicher RAM gehen verloren."},
/* fr */ {IDS_REALLY_EXIT_FR, "D�sirez-vous vraiment quitter?\n\nToutes les donn�es pr�sentes dans la m�moire de l'�mulateur seront perdues."},
/* hu */ {IDS_REALLY_EXIT_HU, "Biztosan kil�p?\n\nAz emul�lt mem�ria teljes tartalma el fog veszni."},
/* it */ {IDS_REALLY_EXIT_IT, "Uscire davvero?\n\nTutti i dati presenti nella RAM emulata saranno persi."},
/* nl */ {IDS_REALLY_EXIT_NL, "Wilt U echt afsluiten?\n\nAlle data in ge�muleerd geheugen zal verloren gaan."},
/* pl */ {IDS_REALLY_EXIT_PL, ""},  /* fuzzy */
/* sv */ {IDS_REALLY_EXIT_SV, "Vill du verkligen avsluta?\n\nAlla data i emulerat RAM kommer f�rloras."},

/* en */ {IDS_ATTACH_CRT_IMAGE,    "Attach CRT cartridge image"},
/* de */ {IDS_ATTACH_CRT_IMAGE_DE, "CRT Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_CRT_IMAGE_FR, "Ins�rer une cartouche CRT"},
/* hu */ {IDS_ATTACH_CRT_IMAGE_HU, "CRT cartridge k�pm�s csatol�sa"},
/* it */ {IDS_ATTACH_CRT_IMAGE_IT, "Seleziona l'immagine di una cartuccia CRT"},
/* nl */ {IDS_ATTACH_CRT_IMAGE_NL, "Koppel CRT cartridge bestand"},
/* pl */ {IDS_ATTACH_CRT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_CRT_IMAGE_SV, "Anslut CRT-insticksmodulfil"},

/* en */ {IDS_ATTACH_RAW_8KB_IMAGE,    "Attach raw 8KB cartridge image"},
/* de */ {IDS_ATTACH_RAW_8KB_IMAGE_DE, "8KB Erweiterungsmodul (raw) Image einlegen"},
/* fr */ {IDS_ATTACH_RAW_8KB_IMAGE_FR, "Ins�rer une cartouche 8KO"},
/* hu */ {IDS_ATTACH_RAW_8KB_IMAGE_HU, "Nyers 8KB-os cartridge k�pm�s csatol�sa"},
/* it */ {IDS_ATTACH_RAW_8KB_IMAGE_IT, "Seleziona l'immagine di una cartuccia da 8KB"},
/* nl */ {IDS_ATTACH_RAW_8KB_IMAGE_NL, "Koppel binair 8KB cartridge bestand"},
/* pl */ {IDS_ATTACH_RAW_8KB_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_RAW_8KB_IMAGE_SV, "Anslut ra 8KB-insticksmodulavbilding"},

/* en */ {IDS_ATTACH_RAW_16KB_IMAGE,    "Attach raw 16KB cartridge image"},
/* de */ {IDS_ATTACH_RAW_16KB_IMAGE_DE, "16KB Erweiterungsmodul (raw) Image einlegen"},
/* fr */ {IDS_ATTACH_RAW_16KB_IMAGE_FR, "Ins�rer une cartouche 16KO"},
/* hu */ {IDS_ATTACH_RAW_16KB_IMAGE_HU, "Nyers 16KB-os cartridge k�pm�s csatol�sa"},
/* it */ {IDS_ATTACH_RAW_16KB_IMAGE_IT, "Seleziona l'immagine di una cartuccia da 16KB"},
/* nl */ {IDS_ATTACH_RAW_16KB_IMAGE_NL, "Koppel binair 16KB cartridge bestand"},
/* pl */ {IDS_ATTACH_RAW_16KB_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_RAW_16KB_IMAGE_SV, "Anslut ra 16KB-insticksmodulavbilding"},

/* en */ {IDS_ATTACH_ACTION_REPLAY_IMAGE,    "Attach Action Replay cartridge image"},
/* de */ {IDS_ATTACH_ACTION_REPLAY_IMAGE_DE, "Action Replay Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_ACTION_REPLAY_IMAGE_FR, "Ins�rer une cartouche Action Replay"},
/* hu */ {IDS_ATTACH_ACTION_REPLAY_IMAGE_HU, "Action Replay cartridge k�pm�s csatol�sa"},
/* it */ {IDS_ATTACH_ACTION_REPLAY_IMAGE_IT, "Seleziona l'immagine di una cartuccia Action Replay"},
/* nl */ {IDS_ATTACH_ACTION_REPLAY_IMAGE_NL, "Koppel Action Replay cartridge bestand"},
/* pl */ {IDS_ATTACH_ACTION_REPLAY_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_ACTION_REPLAY_IMAGE_SV, "Anslut Action Replay-avbildningsfil"},

/* en */ {IDS_ATTACH_ATOMIC_POWER_IMAGE,    "Attach Atomic Power cartridge image"},
/* de */ {IDS_ATTACH_ATOMIC_POWER_IMAGE_DE, "Atomic Power Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_ATOMIC_POWER_IMAGE_FR, "Ins�rer une cartouche Atomic Power"},
/* hu */ {IDS_ATTACH_ATOMIC_POWER_IMAGE_HU, "Atomic Power cartridge k�pm�s csatol�sa"},
/* it */ {IDS_ATTACH_ATOMIC_POWER_IMAGE_IT, "Seleziona l'immagine di una cartuccia Atomic Power"},
/* nl */ {IDS_ATTACH_ATOMIC_POWER_IMAGE_NL, "Koppel Atomic Power cartridge bestand"},
/* pl */ {IDS_ATTACH_ATOMIC_POWER_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_ATOMIC_POWER_IMAGE_SV, "Anslut Atomic Power-avbildningsfil"},

/* en */ {IDS_ATTACH_EPYX_FASTLOAD_IMAGE,    "Attach Epyx fastload cartridge image"},
/* de */ {IDS_ATTACH_EPYX_FASTLOAD_IMAGE_DE, "Epyx Fastload Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_EPYX_FASTLOAD_IMAGE_FR, "Ins�rer une cartouche Epyx FastLoad"},
/* hu */ {IDS_ATTACH_EPYX_FASTLOAD_IMAGE_HU, "Epyx gyorst�lt� cartridge k�pm�s csatol�sa"},
/* it */ {IDS_ATTACH_EPYX_FASTLOAD_IMAGE_IT, "Seleziona l'immagine di una cartuccia Epyx fastload"},
/* nl */ {IDS_ATTACH_EPYX_FASTLOAD_IMAGE_NL, "Koppel Epyx fastload cartridge bestand"},
/* pl */ {IDS_ATTACH_EPYX_FASTLOAD_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_EPYX_FASTLOAD_IMAGE_SV, "Anslut Epyx fastload-avbildningsfil"},

/* en */ {IDS_ATTACH_IEEE488_IMAGE,    "Attach IEEE interface cartridge image"},
/* de */ {IDS_ATTACH_IEEE488_IMAGE_DE, "IEEE Schnittstellen Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_IEEE488_IMAGE_FR, "Ins�rer une cartouche interface IEEE"},
/* hu */ {IDS_ATTACH_IEEE488_IMAGE_HU, "IEEE interf�sz cartridge k�pm�s csatol�sa"},
/* it */ {IDS_ATTACH_IEEE488_IMAGE_IT, "Seleziona l'immagine di una cartuccia di interfaccia IEEE"},
/* nl */ {IDS_ATTACH_IEEE488_IMAGE_NL, "Koppel IEEE interface cartridge bestand"},
/* pl */ {IDS_ATTACH_IEEE488_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_IEEE488_IMAGE_SV, "Anslut IEE-gr�nssnittsavbildningsfil"},

/* en */ {IDS_ATTACH_RETRO_REPLAY_IMAGE,    "Attach Retro Replay cartridge image"},
/* de */ {IDS_ATTACH_RETRO_REPLAY_IMAGE_DE, "Retro Replay Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_RETRO_REPLAY_IMAGE_FR, "Ins�rer une cartouche Retro Replay"},
/* hu */ {IDS_ATTACH_RETRO_REPLAY_IMAGE_HU, "Retro Replay cartridge k�pm�s csatol�sa"},
/* it */ {IDS_ATTACH_RETRO_REPLAY_IMAGE_IT, "Seleziona l'immagine di una cartuccia Retro Replay"},
/* nl */ {IDS_ATTACH_RETRO_REPLAY_IMAGE_NL, "Koppel Retro Replay cartridge bestand"},
/* pl */ {IDS_ATTACH_RETRO_REPLAY_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_RETRO_REPLAY_IMAGE_SV, "Anslut Retro Replay-avbildningsfil"},

/* en */ {IDS_ATTACH_IDE64_IMAGE,    "Attach IDE64 interface cartridge image"},
/* de */ {IDS_ATTACH_IDE64_IMAGE_DE, "IDE64 Schnittstellen Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_IDE64_IMAGE_FR, "Ins�rer une cartouche interface IDE64"},
/* hu */ {IDS_ATTACH_IDE64_IMAGE_HU, "IDE64 interf�sz k�pm�s csatol�sa"},
/* it */ {IDS_ATTACH_IDE64_IMAGE_IT, "Seleziona l'immagine di una cartuccia di interfaccia IDE64"},
/* nl */ {IDS_ATTACH_IDE64_IMAGE_NL, "Koppel IDE64 interface cartridge bestand"},
/* pl */ {IDS_ATTACH_IDE64_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_IDE64_IMAGE_SV, "Anslut IDE64-gr�nssnittsavbildningsfil"},

/* en */ {IDS_ATTACH_SS4_IMAGE,    "Attach Super Snapshot 4 cartridge image"},
/* de */ {IDS_ATTACH_SS4_IMAGE_DE, "Super Snapshot 4 Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_SS4_IMAGE_FR, "Ins�rer une cartouche Super Snapshot 4"},
/* hu */ {IDS_ATTACH_SS4_IMAGE_HU, "Super Snapshot 4 cartridge k�pm�s csatol�sa"},
/* it */ {IDS_ATTACH_SS4_IMAGE_IT, "Seleziona l'immagine di una cartuccia Super Snapshot 4"},
/* nl */ {IDS_ATTACH_SS4_IMAGE_NL, "Koppel Super Snapshot 4 cartridge bestand"},
/* pl */ {IDS_ATTACH_SS4_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_SS4_IMAGE_SV, "Anslut Super Snapshot 4-avbildningsfil"},

/* en */ {IDS_ATTACH_SS5_IMAGE,    "Attach Super Snapshot 5 cartridge image"},
/* de */ {IDS_ATTACH_SS5_IMAGE_DE, "Super Snapshot 5 Erweiterungsmodul Image einlegen"},
/* fr */ {IDS_ATTACH_SS5_IMAGE_FR, "Ins�rer une cartouche Super Snapshot 5"},
/* hu */ {IDS_ATTACH_SS5_IMAGE_HU, "Super Snapshot 5 cartridge k�pm�s csatol�sa"},
/* it */ {IDS_ATTACH_SS5_IMAGE_IT, "Seleziona l'immagine di una cartuccia Super Snapshot 5"},
/* nl */ {IDS_ATTACH_SS5_IMAGE_NL, "Koppel Super Snapshot 5 cartridge bestand"},
/* pl */ {IDS_ATTACH_SS5_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_SS5_IMAGE_SV, "Anslut Super Snapshot 5-avbildningsfil"},

/* en */ {IDS_ATTACH_SB_IMAGE,    "Attach Structured Basic cartridge image"},
/* de */ {IDS_ATTACH_SB_IMAGE_DE, "Erweiterungsmodul Structured Basic einlegen"},
/* fr */ {IDS_ATTACH_SB_IMAGE_FR, ""},  /* fuzzy */
/* hu */ {IDS_ATTACH_SB_IMAGE_HU, "Structured Basic cartridge k�pm�s csatol�sa"},
/* it */ {IDS_ATTACH_SB_IMAGE_IT, "Seleziona l'immagine di una cartuccia Structured Basic"},
/* nl */ {IDS_ATTACH_SB_IMAGE_NL, "Koppel Structured Basic cartridge bestand"},
/* pl */ {IDS_ATTACH_SB_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_SB_IMAGE_SV, "Anslut Structured Basic insticksavbildningsfil"},

/* en */ {IDS_AVAILABLE_CMDLINE_OPTIONS,    "\nAvailable command-line options:\n\n"},
/* de */ {IDS_AVAILABLE_CMDLINE_OPTIONS_DE, "\nZul�ssige Kommandozeilen Optionen:\n\n"},
/* fr */ {IDS_AVAILABLE_CMDLINE_OPTIONS_FR, "\nOptions de ligne de commande disponibles:\n\n"},
/* hu */ {IDS_AVAILABLE_CMDLINE_OPTIONS_HU, "\nLehets�ges parancssori opci�k:\n\n"},
/* it */ {IDS_AVAILABLE_CMDLINE_OPTIONS_IT, "\nOpzioni disponibili da riga di comando:\n\n"},
/* nl */ {IDS_AVAILABLE_CMDLINE_OPTIONS_NL, "\nBeschikbare command-line opties:\n\n"},
/* pl */ {IDS_AVAILABLE_CMDLINE_OPTIONS_PL, "\nTillg�ngliga kommandoradsflaggor:\n\n"},
/* sv */ {IDS_AVAILABLE_CMDLINE_OPTIONS_SV, "\nTillg�ngliga kommandoradsargument:\n\n"},

/* en */ {IDS_ATTACH_4_8_16KB_AT_2000,    "Attach 4/8/16KB cartridge image at $2000"},
/* de */ {IDS_ATTACH_4_8_16KB_AT_2000_DE, "4/8/16KB Modul Image bei $2000 einlegen"},
/* fr */ {IDS_ATTACH_4_8_16KB_AT_2000_FR, "Insertion d'une image 4/8/16Ko   $2000"},
/* hu */ {IDS_ATTACH_4_8_16KB_AT_2000_HU, "4/8/16KB k�pm�s csatol�sa $2000 c�mre"},
/* it */ {IDS_ATTACH_4_8_16KB_AT_2000_IT, "Seleziona l'immagine di una cartuccia da 4/8/16KB a $2000"},
/* nl */ {IDS_ATTACH_4_8_16KB_AT_2000_NL, "Koppel 4/8/16KB bestand op $2000"},
/* pl */ {IDS_ATTACH_4_8_16KB_AT_2000_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_4_8_16KB_AT_2000_SV, "Anslut 4/8/16KB-modulfil vid $2000"},

/* en */ {IDS_ATTACH_4_8_16KB_AT_4000,    "Attach 4/8/16KB cartridge image at $4000"},
/* de */ {IDS_ATTACH_4_8_16KB_AT_4000_DE, "4/8/16KB Modul Image bei $4000 einlegen"},
/* fr */ {IDS_ATTACH_4_8_16KB_AT_4000_FR, "Insertion d'une image 4/8/16Ko   $4000"},
/* hu */ {IDS_ATTACH_4_8_16KB_AT_4000_HU, "4/8/16KB k�pm�s csatol�sa $4000 c�mre"},
/* it */ {IDS_ATTACH_4_8_16KB_AT_4000_IT, "Seleziona l'immagine di una cartuccia da 4/8/16KB a $4000"},
/* nl */ {IDS_ATTACH_4_8_16KB_AT_4000_NL, "Koppel 4/8/16KB bestand op $4000"},
/* pl */ {IDS_ATTACH_4_8_16KB_AT_4000_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_4_8_16KB_AT_4000_SV, "Anslut 4/8/16KB-modulfil vid $4000"},

/* en */ {IDS_ATTACH_4_8_16KB_AT_6000,    "Attach 4/8/16KB cartridge image at $6000"},
/* de */ {IDS_ATTACH_4_8_16KB_AT_6000_DE, "4/8/16KB Modul Image bei $6000 einlegen"},
/* fr */ {IDS_ATTACH_4_8_16KB_AT_6000_FR, "Insertion d'une image 4/8/16Ko   $6000"},
/* hu */ {IDS_ATTACH_4_8_16KB_AT_6000_HU, "4/8/16KB k�pm�s csatol�sa $6000 c�mre"},
/* it */ {IDS_ATTACH_4_8_16KB_AT_6000_IT, "Seleziona l'immagine di una cartuccia da 4/8/16KB a $6000"},
/* nl */ {IDS_ATTACH_4_8_16KB_AT_6000_NL, "Koppel 4/8/16KB bestand op $6000"},
/* pl */ {IDS_ATTACH_4_8_16KB_AT_6000_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_4_8_16KB_AT_6000_SV, "Anslut 4/8/16KB-modulfil vid $6000"},

/* en */ {IDS_ATTACH_4_8KB_AT_A000,    "Attach 4/8KB cartridge image at $A000"},
/* de */ {IDS_ATTACH_4_8KB_AT_A000_DE, "4/8KB Modul Image bei $A000 einlegen"},
/* fr */ {IDS_ATTACH_4_8KB_AT_A000_FR, "Insertion d'une image 4/8Ko   $A000"},
/* hu */ {IDS_ATTACH_4_8KB_AT_A000_HU, "4/8KB k�pm�s csatol�sa $A000 c�mre"},
/* it */ {IDS_ATTACH_4_8KB_AT_A000_IT, "Seleziona l'immagine di una cartuccia da 4/8KB a $A000"},
/* nl */ {IDS_ATTACH_4_8KB_AT_A000_NL, "Koppel 4/8KB bestand op $A000"},
/* pl */ {IDS_ATTACH_4_8KB_AT_A000_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_4_8KB_AT_A000_SV, "Anslut 4/8KB-modulfil vid $A000"},

/* en */ {IDS_ATTACH_4KB_AT_B000,    "Attach 4KB cartridge image at $B000"},
/* de */ {IDS_ATTACH_4KB_AT_B000_DE, "4KB Modul Image bei $B000 einlegen"},
/* fr */ {IDS_ATTACH_4KB_AT_B000_FR, "Insertion d'une image 4Ko   $B000"},
/* hu */ {IDS_ATTACH_4KB_AT_B000_HU, "4KB k�pm�s csatol�sa $B000 c�mre"},
/* it */ {IDS_ATTACH_4KB_AT_B000_IT, "Seleziona l'immagine di una cartuccia da 4KB a $B000"},
/* nl */ {IDS_ATTACH_4KB_AT_B000_NL, "Koppel 4KB bestand op $B000"},
/* pl */ {IDS_ATTACH_4KB_AT_B000_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_4KB_AT_B000_SV, "Anslut 4KB-modulfil vid $B000"},

/* en */ {IDS_S_AT_D_SPEED,    "%s at %d%% speed, %d fps%s"},
/* de */ {IDS_S_AT_D_SPEED_DE, "%s bei %d%% Geschwindigkeit, %d fps%s"},
/* fr */ {IDS_S_AT_D_SPEED_FR, "%s   une vitesse de %d%%,   %d img/s%s"},
/* it */ {IDS_S_AT_D_SPEED_HU, "%s %d%% sebess�gen, %d fps%s"},
/* it */ {IDS_S_AT_D_SPEED_IT, "velocit� %s al %d%%, %d fps%s"},
/* nl */ {IDS_S_AT_D_SPEED_NL, "%s met %d%% snelheid, %d fps%s"},
/* pl */ {IDS_S_AT_D_SPEED_PL, ""},  /* fuzzy */
/* sv */ {IDS_S_AT_D_SPEED_SV, "%s med %d%% hastighet, %d b/s%s"},

/* en */ {IDS_ATTACH,    "Attach"},
/* de */ {IDS_ATTACH_DE, "Einlegen"},
/* fr */ {IDS_ATTACH_FR, ""},  /* fuzzy */
/* hu */ {IDS_ATTACH_HU, "Csatol�s"},
/* it */ {IDS_ATTACH_IT, "Seleziona"},
/* nl */ {IDS_ATTACH_NL, "Koppel"},
/* pl */ {IDS_ATTACH_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_SV, "Anslut"},

/* en */ {IDS_PARENT,    "Parent"},
/* de */ {IDS_PARENT_DE, "Vorg�nger"},
/* fr */ {IDS_PARENT_FR, ""},  /* fuzzy */
/* hu */ {IDS_PARENT_HU, "Sz�l�"},
/* it */ {IDS_PARENT_IT, "Livello superiore"},
/* nl */ {IDS_PARENT_NL, "Bovenliggende directory"},
/* pl */ {IDS_PARENT_PL, ""},  /* fuzzy */
/* sv */ {IDS_PARENT_SV, "Moderl�da"},

/* en */ {IDS_ATTACH_READ_ONLY,    "Attach read only"},
/* de */ {IDS_ATTACH_READ_ONLY_DE, "Schreibgesch�tzt einlegen"},
/* fr */ {IDS_ATTACH_READ_ONLY_FR, "Ins�rer en lecture seule"},
/* hu */ {IDS_ATTACH_READ_ONLY_HU, "Csatol�s csak olvashat� m�don"},
/* it */ {IDS_ATTACH_READ_ONLY_IT, "Seleziona in sola lettura"},
/* nl */ {IDS_ATTACH_READ_ONLY_NL, "Alleen lezen"},
/* pl */ {IDS_ATTACH_READ_ONLY_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_READ_ONLY_SV, "Anslut skrivskyddat"},

/* en */ {IDS_NAME,    "Name"},
/* de */ {IDS_NAME_DE, "Name"},
/* fr */ {IDS_NAME_FR, "Nom"},
/* hu */ {IDS_NAME_HU, "N�v"},
/* it */ {IDS_NAME_IT, "Nome"},
/* nl */ {IDS_NAME_NL, "Naam"},
/* pl */ {IDS_NAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_NAME_SV, "Namn"},

/* en */ {IDS_CREATE_IMAGE,    "Create Image"},
/* de */ {IDS_CREATE_IMAGE_DE, "Image Datei erzeugen"},
/* fr */ {IDS_CREATE_IMAGE_FR, "Cr�ation d'image"},
/* hu */ {IDS_CREATE_IMAGE_HU, "K�pm�s l�trehoz�sa"},
/* it */ {IDS_CREATE_IMAGE_IT, "Crea immagine"},
/* nl */ {IDS_CREATE_IMAGE_NL, "Maak Bestand"},
/* pl */ {IDS_CREATE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_CREATE_IMAGE_SV, "Skapa avbildning"},

/* en */ {IDS_NEW_IMAGE,    "New Image"},
/* de */ {IDS_NEW_IMAGE_DE, "Neue Image Datei"},
/* fr */ {IDS_NEW_IMAGE_FR, "Nouvelle image"},
/* hu */ {IDS_NEW_IMAGE_HU, "�j k�pm�s"},
/* it */ {IDS_NEW_IMAGE_IT, "Nuova immagine"},
/* nl */ {IDS_NEW_IMAGE_NL, "Nieuw Bestand"},
/* pl */ {IDS_NEW_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_NEW_IMAGE_SV, "Ny avbildning"},

/* en */ {IDS_NEW_TAP_IMAGE,    "New TAP Image"},
/* de */ {IDS_NEW_TAP_IMAGE_DE, "Neue TAP Image Datei"},
/* fr */ {IDS_NEW_TAP_IMAGE_FR, "Nouvelle image TAP"},
/* hu */ {IDS_NEW_TAP_IMAGE_HU, "�j TAP k�pm�s"},
/* it */ {IDS_NEW_TAP_IMAGE_IT, "Nuova immagine TAP"},
/* nl */ {IDS_NEW_TAP_IMAGE_NL, "Nieuw TAP Bestand"},
/* pl */ {IDS_NEW_TAP_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_NEW_TAP_IMAGE_SV, "Ny TAP-avbildning"},

/* en */ {IDS_OVERWRITE_EXISTING_IMAGE,    "Overwrite existing image?"},
/* de */ {IDS_OVERWRITE_EXISTING_IMAGE_DE, "Existierende Datei �berschreiben?"},
/* fr */ {IDS_OVERWRITE_EXISTING_IMAGE_FR, "�craser les images existantes?"},
/* hu */ {IDS_OVERWRITE_EXISTING_IMAGE_HU, "Fel�l�rjam a megl�v� k�pm�st?"},
/* it */ {IDS_OVERWRITE_EXISTING_IMAGE_IT, "Sovrascrivo immagine esistente?"},
/* nl */ {IDS_OVERWRITE_EXISTING_IMAGE_NL, "Bestaand bestand overschrijven?"},
/* pl */ {IDS_OVERWRITE_EXISTING_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_OVERWRITE_EXISTING_IMAGE_SV, "Skriva �ver existerande avbildning?"},

/* en */ {IDS_ABOUT,    "About"},
/* de */ {IDS_ABOUT_DE, "�ber VICE"},
/* fr */ {IDS_ABOUT_FR, "� Propos"},
/* hu */ {IDS_ABOUT_HU, "N�vjegy"},
/* it */ {IDS_ABOUT_IT, "Informazioni"},
/* nl */ {IDS_ABOUT_NL, "Over VICE"},
/* pl */ {IDS_ABOUT_PL, ""},  /* fuzzy */
/* sv */ {IDS_ABOUT_SV, "Om VICE"},

/* en */ {IDS_IMAGE_CONTENTS,    "Image Contents"},
/* de */ {IDS_IMAGE_CONTENTS_DE, "Image Datei Inhalt"},
/* fr */ {IDS_IMAGE_CONTENTS_FR, "Contenu de l'image"},
/* hu */ {IDS_IMAGE_CONTENTS_HU, "Tartalom"},
/* it */ {IDS_IMAGE_CONTENTS_IT, "Contenuti immagine"},
/* nl */ {IDS_IMAGE_CONTENTS_NL, "Bestand Inhoud"},
/* pl */ {IDS_IMAGE_CONTENTS_PL, ""},  /* fuzzy */
/* sv */ {IDS_IMAGE_CONTENTS_SV, "Innehall i avbildning"},

/* en */ {IDMS_DISABLED,    "Disabled"},
/* de */ {IDMS_DISABLED_DE, "Deaktiviert"},
/* fr */ {IDMS_DISABLED_FR, "D�sactiv�"},
/* hu */ {IDMS_DISABLED_HU, "Tiltva"},
/* it */ {IDMS_DISABLED_IT, "Disattivo"},
/* nl */ {IDMS_DISABLED_NL, "Uit"},
/* pl */ {IDMS_DISABLED_PL, ""},  /* fuzzy */
/* sv */ {IDMS_DISABLED_SV, "Av"},

/* en */ {IDS_ENABLED,    "Enabled"},
/* de */ {IDS_ENABLED_DE, "Aktiviert"},
/* fr */ {IDS_ENABLED_FR, ""},  /* fuzzy */
/* hu */ {IDS_ENABLED_HU, "Enged�lyezve"},
/* it */ {IDS_ENABLED_IT, "Attivo"},
/* nl */ {IDS_ENABLED_NL, "Aan"},
/* pl */ {IDS_ENABLED_PL, ""},  /* fuzzy */
/* sv */ {IDS_ENABLED_SV, "P�"},

/* en */ {IDS_RS232_DEVICE_1,    "RS232 Device 1"},
/* de */ {IDS_RS232_DEVICE_1_DE, "RS232 Ger�t 1"},
/* fr */ {IDS_RS232_DEVICE_1_FR, "1er P�riph�rique RS232"},
/* hu */ {IDS_RS232_DEVICE_1_HU, "RS232 1-es eszk�z"},
/* it */ {IDS_RS232_DEVICE_1_IT, "Dispositivo RS232 1"},
/* nl */ {IDS_RS232_DEVICE_1_NL, "RS232 apparaat 1"},
/* pl */ {IDS_RS232_DEVICE_1_PL, ""},  /* fuzzy */
/* sv */ {IDS_RS232_DEVICE_1_SV, "RS232-enhet 1"},

/* en */ {IDS_RS232_DEVICE_2,    "RS232 Device 2"},
/* de */ {IDS_RS232_DEVICE_2_DE, "RS232 Ger�t 2"},
/* fr */ {IDS_RS232_DEVICE_2_FR, "2e P�riph�rique RS232"},
/* hu */ {IDS_RS232_DEVICE_2_HU, "RS232 2-es eszk�z"},
/* it */ {IDS_RS232_DEVICE_2_IT, "Dispositivo RS232 2"},
/* nl */ {IDS_RS232_DEVICE_2_NL, "RS232 apparaat 2"},
/* pl */ {IDS_RS232_DEVICE_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_RS232_DEVICE_2_SV, "RS232-enhet 2"},

/* en */ {IDS_RS232_DEVICE_3,    "RS232 Device 3"},
/* de */ {IDS_RS232_DEVICE_3_DE, "RS232 Ger�t 3"},
/* fr */ {IDS_RS232_DEVICE_3_FR, "3e P�riph�rique RS232"},
/* hu */ {IDS_RS232_DEVICE_3_HU, "RS232 3-as eszk�z"},
/* it */ {IDS_RS232_DEVICE_3_IT, "Dispositivo RS232 3"},
/* nl */ {IDS_RS232_DEVICE_3_NL, "RS232 apparaat 3"},
/* pl */ {IDS_RS232_DEVICE_3_PL, ""},  /* fuzzy */
/* sv */ {IDS_RS232_DEVICE_3_SV, "RS232-enhet 3"},

/* en */ {IDS_RS232_DEVICE_4,    "RS232 Device 4"},
/* de */ {IDS_RS232_DEVICE_4_DE, "RS232 Ger�t 4"},
/* fr */ {IDS_RS232_DEVICE_4_FR, "4e P�riph�rique RS232"},
/* hu */ {IDS_RS232_DEVICE_4_HU, "RS232 4-es eszk�z"},
/* it */ {IDS_RS232_DEVICE_4_IT, "Dispositivo RS232 4"},
/* nl */ {IDS_RS232_DEVICE_4_NL, "RS232 apparaat 4"},
/* pl */ {IDS_RS232_DEVICE_4_PL, ""},  /* fuzzy */
/* sv */ {IDS_RS232_DEVICE_4_SV, "RS232-enhet 4"},

/* en */ {IDS_NONE,    "None"},
/* de */ {IDS_NONE_DE, "Kein"},
/* fr */ {IDS_NONE_FR, "Aucun"},
/* hu */ {IDS_NONE_HU, "Nincs"},
/* it */ {IDS_NONE_IT, "Nessuno"},
/* nl */ {IDS_NONE_NL, "Geen"},
/* pl */ {IDS_NONE_PL, ""},  /* fuzzy */
/* sv */ {IDS_NONE_SV, "Ingen"},

/* en */ {IDS_IRQ,    "IRQ"},
/* de */ {IDS_IRQ_DE, "IRQ"},
/* fr */ {IDS_IRQ_FR, "IRQ"},
/* hu */ {IDS_IRQ_HU, "IRQ"},
/* it */ {IDS_IRQ_IT, "IRQ"},
/* nl */ {IDS_IRQ_NL, "IRQ"},
/* pl */ {IDS_IRQ_PL, "IRQ"},
/* sv */ {IDS_IRQ_SV, "IRQ"},

/* en */ {IDS_NMI,    "NMI"},
/* de */ {IDS_NMI_DE, "NMI"},
/* fr */ {IDS_NMI_FR, "NMI"},
/* hu */ {IDS_NMI_HU, "NMI"},
/* it */ {IDS_NMI_IT, "NMI"},
/* nl */ {IDS_NMI_NL, "NMI"},
/* pl */ {IDS_NMI_PL, "NMI"},
/* sv */ {IDS_NMI_SV, "NMI"},

/* en */ {IDS_ACIA_DEVICE,    "ACIA Device"},
/* de */ {IDS_ACIA_DEVICE_DE, "ACIA Ger�t"},
/* fr */ {IDS_ACIA_DEVICE_FR, "P�riph�rique ACIA"},
/* hu */ {IDS_ACIA_DEVICE_HU, "ACIA eszk�z"},
/* it */ {IDS_ACIA_DEVICE_IT, "Dispositivo ACIA"},
/* nl */ {IDS_ACIA_DEVICE_NL, "ACIA apparaat"},
/* pl */ {IDS_ACIA_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ACIA_DEVICE_SV, "ACIA-enhet"},

/* en */ {IDS_ACIA_INTERRUPT,    "ACIA Interrupt"},
/* de */ {IDS_ACIA_INTERRUPT_DE, "ACIA Interrupt"},
/* fr */ {IDS_ACIA_INTERRUPT_FR, "Interruption ACIA"},
/* hu */ {IDS_ACIA_INTERRUPT_HU, "ACIA megszak�t�s"},
/* it */ {IDS_ACIA_INTERRUPT_IT, "Interrupt dell'ACIA"},
/* nl */ {IDS_ACIA_INTERRUPT_NL, "ACIA Interrupt"},
/* pl */ {IDS_ACIA_INTERRUPT_PL, ""},  /* fuzzy */
/* sv */ {IDS_ACIA_INTERRUPT_SV, "ACIA-avbrott"},

/* en */ {IDS_ACIA_MODE,    "ACIA Mode"},
/* de */ {IDS_ACIA_MODE_DE, "ACIA Modus"},
/* fr */ {IDS_ACIA_MODE_FR, ""},  /* fuzzy */
/* hu */ {IDS_ACIA_MODE_HU, "ACIA m�d"},
/* it */ {IDS_ACIA_MODE_IT, "Modalit� ACIA"},
/* nl */ {IDS_ACIA_MODE_NL, "ACIA Modus"},
/* pl */ {IDS_ACIA_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ACIA_MODE_SV, "ACIA l�ge"},

/* en */ {IDS_ACIA_SETTINGS,    "ACIA Settings"},
/* de */ {IDS_ACIA_SETTINGS_DE, "ACIA Einstellungen"},
/* fr */ {IDS_ACIA_SETTINGS_FR, "Parametres ACIA"},
/* hu */ {IDS_ACIA_SETTINGS_HU, "ACIA be�ll�t�sai"},
/* it */ {IDS_ACIA_SETTINGS_IT, "Impostazioni ACIA"},
/* nl */ {IDS_ACIA_SETTINGS_NL, "ACIA Instellingen"},
/* pl */ {IDS_ACIA_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_ACIA_SETTINGS_SV, "ACIA-inst�llningar"},

/* en */ {IDS_256K_ENABLED,    "256K Enabled"},
/* de */ {IDS_256K_ENABLED_DE, "256K aktiviert"},
/* fr */ {IDS_256K_ENABLED_FR, ""},  /* fuzzy */
/* hu */ {IDS_256K_ENABLED_HU, "256K enged�lyezve"},
/* it */ {IDS_256K_ENABLED_IT, "256K attivi"},
/* nl */ {IDS_256K_ENABLED_NL, "256K Aan/Uit"},
/* pl */ {IDS_256K_ENABLED_PL, ""},  /* fuzzy */
/* sv */ {IDS_256K_ENABLED_SV, "256K aktiverat"},

/* en */ {IDS_256K_BASE,    "256K Base"},
/* de */ {IDS_256K_BASE_DE, "256K Basis"},
/* fr */ {IDS_256K_BASE_FR, ""},  /* fuzzy */
/* hu */ {IDS_256K_BASE_HU, "256K b�zisc�m"},
/* it */ {IDS_256K_BASE_IT, "Indirizzo base 256K"},
/* nl */ {IDS_256K_BASE_NL, "256K basis adres"},
/* pl */ {IDS_256K_BASE_PL, ""},  /* fuzzy */
/* sv */ {IDS_256K_BASE_SV, "256K bas-adress"},

/* en */ {IDS_256K_FILENAME,    "256K file"},
/* de */ {IDS_256K_FILENAME_DE, "256K Datei"},
/* fr */ {IDS_256K_FILENAME_FR, "Fichier 256K"},
/* hu */ {IDS_256K_FILENAME_HU, "256K f�jl"},
/* it */ {IDS_256K_FILENAME_IT, "File 256K"},
/* nl */ {IDS_256K_FILENAME_NL, "256K bestand"},
/* pl */ {IDS_256K_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_256K_FILENAME_SV, "256K-fil"},

/* en */ {IDS_256K_FILENAME_SELECT,    "Select file for 256K"},
/* de */ {IDS_256K_FILENAME_SELECT_DE, "Datei f�r 256K selektieren"},
/* fr */ {IDS_256K_FILENAME_SELECT_FR, "S�lectionner fichier pour 256K"},
/* hu */ {IDS_256K_FILENAME_SELECT_HU, "V�lasszon f�jlt a 256K-hoz"},
/* it */ {IDS_256K_FILENAME_SELECT_IT, "Seleziona il file per il 256K"},
/* nl */ {IDS_256K_FILENAME_SELECT_NL, "Selecteer bestand voor 256K"},
/* pl */ {IDS_256K_FILENAME_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_256K_FILENAME_SELECT_SV, "Ange fil f�r 256K"},

/* en */ {IDS_256K_SETTINGS,    "256K Settings"},
/* de */ {IDS_256K_SETTINGS_DE, "256K Einstellungen"},
/* fr */ {IDS_256K_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_256K_SETTINGS_HU, "256K be�ll�t�sai"},
/* it */ {IDS_256K_SETTINGS_IT, "Impostazioni 256K"},
/* nl */ {IDS_256K_SETTINGS_NL, "256K Instellingen"},
/* pl */ {IDS_256K_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_256K_SETTINGS_SV, "256K-inst�llningar"},

/* en */ {IDS_MODEL_LINE,    "model line"},
/* de */ {IDS_MODEL_LINE_DE, "modell linie"},
/* fr */ {IDS_MODEL_LINE_FR, "ligne du modele"},
/* hu */ {IDS_MODEL_LINE_HU, "modell sor"},
/* it */ {IDS_MODEL_LINE_IT, "impostazioni del modello"},
/* nl */ {IDS_MODEL_LINE_NL, "model lijn"},
/* pl */ {IDS_MODEL_LINE_PL, ""},  /* fuzzy */
/* sv */ {IDS_MODEL_LINE_SV, "modelltyp"},

/* en */ {IDS_RAM_SIZE,    "RAM Size"},
/* de */ {IDS_RAM_SIZE_DE, "RAM Gr��e"},
/* fr */ {IDS_RAM_SIZE_FR, "Taille du RAM"},
/* hu */ {IDS_RAM_SIZE_HU, "RAM m�rete"},
/* it */ {IDS_RAM_SIZE_IT, "Dimensione RAM"},
/* nl */ {IDS_RAM_SIZE_NL, "RAM grootte"},
/* pl */ {IDS_RAM_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_SIZE_SV, "RAM-storlek"},

/* en */ {IDS_RAM_BLOCK_0400_0FFF,    "RAM Block $0400-$0FFF"},
/* de */ {IDS_RAM_BLOCK_0400_0FFF_DE, "RAM in $0400-$0FFF"},
/* fr */ {IDS_RAM_BLOCK_0400_0FFF_FR, "RAM dans $0400-$0FFF"},
/* hu */ {IDS_RAM_BLOCK_0400_0FFF_HU, "Mem�ria blokk $0400-$0FFF"},
/* it */ {IDS_RAM_BLOCK_0400_0FFF_IT, "Blocco RAM $0400-$0FFF"},
/* nl */ {IDS_RAM_BLOCK_0400_0FFF_NL, "RAM Blok $0400-$0FFF"},
/* pl */ {IDS_RAM_BLOCK_0400_0FFF_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_BLOCK_0400_0FFF_SV, "RAM pa $0400-$0FFF"},

/* en */ {IDS_RAM_BLOCK_0800_0FFF,    "RAM Block $0800-$0FFF"},
/* de */ {IDS_RAM_BLOCK_0800_0FFF_DE, "RAM in $0800-$0FFF"},
/* fr */ {IDS_RAM_BLOCK_0800_0FFF_FR, "RAM dans $0800-$0FFF"},
/* hu */ {IDS_RAM_BLOCK_0800_0FFF_HU, "Mem�ria blokk $0800-$0FFF"},
/* it */ {IDS_RAM_BLOCK_0800_0FFF_IT, "Blocco RAM $0800-$0FFF"},
/* nl */ {IDS_RAM_BLOCK_0800_0FFF_NL, "RAM Blok $0800-$0FFF"},
/* pl */ {IDS_RAM_BLOCK_0800_0FFF_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_BLOCK_0800_0FFF_SV, "RAM pa $0800-$0FFF"},

/* en */ {IDS_RAM_BLOCK_1000_1FFF,    "RAM Block $1000-$1FFF"},
/* de */ {IDS_RAM_BLOCK_1000_1FFF_DE, "RAM in $1000-$1FFF"},
/* fr */ {IDS_RAM_BLOCK_1000_1FFF_FR, "RAM dans $1000-$1FFF"},
/* hu */ {IDS_RAM_BLOCK_1000_1FFF_HU, "Mem�ria blokk $1000-$1FFF"},
/* it */ {IDS_RAM_BLOCK_1000_1FFF_IT, "Blocco RAM $1000-$1FFF"},
/* nl */ {IDS_RAM_BLOCK_1000_1FFF_NL, "RAM Blok $1000-$1FFF"},
/* pl */ {IDS_RAM_BLOCK_1000_1FFF_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_BLOCK_1000_1FFF_SV, "RAM pa $1000-$1FFF"},

/* en */ {IDS_RAM_BLOCK_2000_3FFF,    "RAM Block $2000-$3FFF"},
/* de */ {IDS_RAM_BLOCK_2000_3FFF_DE, "RAM in $2000-$3FFF"},
/* fr */ {IDS_RAM_BLOCK_2000_3FFF_FR, "RAM dans $2000-$3FFF"},
/* hu */ {IDS_RAM_BLOCK_2000_3FFF_HU, "Mem�ria blokk $2000-$3FFF"},
/* it */ {IDS_RAM_BLOCK_2000_3FFF_IT, "Blocco RAM $2000-$3FFF"},
/* nl */ {IDS_RAM_BLOCK_2000_3FFF_NL, "RAM Blok $2000-$3FFF"},
/* pl */ {IDS_RAM_BLOCK_2000_3FFF_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_BLOCK_2000_3FFF_SV, "RAM pa $2000-$3FFF"},

/* en */ {IDS_RAM_BLOCK_4000_5FFF,    "RAM Block $4000-$5FFF"},
/* de */ {IDS_RAM_BLOCK_4000_5FFF_DE, "RAM in $4000-$5FFF"},
/* fr */ {IDS_RAM_BLOCK_4000_5FFF_FR, "RAM dans $4000-$5FFF"},
/* hu */ {IDS_RAM_BLOCK_4000_5FFF_HU, "Mem�ria blokk $4000-$5FFF"},
/* it */ {IDS_RAM_BLOCK_4000_5FFF_IT, "Blocco RAM $4000-$5FFF"},
/* nl */ {IDS_RAM_BLOCK_4000_5FFF_NL, "RAM Blok $4000-$5FFF"},
/* pl */ {IDS_RAM_BLOCK_4000_5FFF_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_BLOCK_4000_5FFF_SV, "RAM pa $4000-$5FFF"},

/* en */ {IDS_RAM_BLOCK_6000_7FFF,    "RAM Block $6000-$7FFF"},
/* de */ {IDS_RAM_BLOCK_6000_7FFF_DE, "RAM in $6000-$7FFF"},
/* fr */ {IDS_RAM_BLOCK_6000_7FFF_FR, "RAM dans $6000-$7FFF"},
/* hu */ {IDS_RAM_BLOCK_6000_7FFF_HU, "Mem�ria blokk $6000-$7FFF"},
/* it */ {IDS_RAM_BLOCK_6000_7FFF_IT, "Blocco RAM $6000-$7FFF"},
/* nl */ {IDS_RAM_BLOCK_6000_7FFF_NL, "RAM Blok $6000-$7FFF"},
/* pl */ {IDS_RAM_BLOCK_6000_7FFF_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_BLOCK_6000_7FFF_SV, "RAM pa $6000-$7FFF"},

/* en */ {IDS_RAM_BLOCK_A000_BFFF,    "RAM Block $A000-$BFFF"},
/* de */ {IDS_RAM_BLOCK_A000_BFFF_DE, "RAM in $A000-$BFFF"},
/* fr */ {IDS_RAM_BLOCK_A000_BFFF_FR, "RAM dans $A000-$BFFF"},
/* hu */ {IDS_RAM_BLOCK_A000_BFFF_HU, "Mem�ria blokk $A000-$BFFF"},
/* it */ {IDS_RAM_BLOCK_A000_BFFF_IT, "Blocco RAM $A000-$BFFF"},
/* nl */ {IDS_RAM_BLOCK_A000_BFFF_NL, "RAM Blok $A000-$BFFF"},
/* pl */ {IDS_RAM_BLOCK_A000_BFFF_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_BLOCK_A000_BFFF_SV, "RAM pa $A000-$BFFF"},

/* en */ {IDS_RAM_BLOCK_C000_CFFF,    "RAM Block $C000-$CFFF"},
/* de */ {IDS_RAM_BLOCK_C000_CFFF_DE, "RAM in $C000-$CFFF"},
/* fr */ {IDS_RAM_BLOCK_C000_CFFF_FR, "RAM dans $C000-$CFFF"},
/* hu */ {IDS_RAM_BLOCK_C000_CFFF_HU, "Mem�ria blokk $C000-$CFFF"},
/* it */ {IDS_RAM_BLOCK_C000_CFFF_IT, "Blocco RAM $C000-$CFFF"},
/* nl */ {IDS_RAM_BLOCK_C000_CFFF_NL, "RAM Blok $C000-$CFFF"},
/* pl */ {IDS_RAM_BLOCK_C000_CFFF_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_BLOCK_C000_CFFF_SV, "RAM pa $C000-$CFFF"},

/* en */ {IDS_CBM2_SETTINGS,    "CBM2 Settings"},
/* de */ {IDS_CBM2_SETTINGS_DE, "CBM2 Einstellungen"},
/* fr */ {IDS_CBM2_SETTINGS_FR, "Parametres CBM2"},
/* hu */ {IDS_CBM2_SETTINGS_HU, "CBM2 Be�ll�t�sai"},
/* it */ {IDS_CBM2_SETTINGS_IT, "Impostazioni CBM2"},
/* nl */ {IDS_CBM2_SETTINGS_NL, "CBM2 Instellingen"},
/* pl */ {IDS_CBM2_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_CBM2_SETTINGS_SV, "CBM2-inst�llningar"},

/* en */ {IDS_RESET_DATASETTE_WITH_CPU,    "Reset Datasette with CPU"},
/* de */ {IDS_RESET_DATASETTE_WITH_CPU_DE, "Zur�cksetzen des Bandlaufwerks bei CPU Reset"},
/* fr */ {IDS_RESET_DATASETTE_WITH_CPU_FR, "R�initialiser la datassette avec le CPU"},
/* hu */ {IDS_RESET_DATASETTE_WITH_CPU_HU, "Alaphelyzetbe hoz�s CPU Reset eset�n"},
/* it */ {IDS_RESET_DATASETTE_WITH_CPU_IT, "Reset del registratore al reset della CPU"},
/* nl */ {IDS_RESET_DATASETTE_WITH_CPU_NL, "Reset Datasette met CPU"},
/* pl */ {IDS_RESET_DATASETTE_WITH_CPU_PL, ""},  /* fuzzy */
/* sv */ {IDS_RESET_DATASETTE_WITH_CPU_SV, "Nollst�ll Datasette med CPU"},

/* en */ {IDS_ADDITIONAL_DELAY,    "Additional Delay"},
/* de */ {IDS_ADDITIONAL_DELAY_DE, "Zus�tzliche Verz�gerung"},
/* fr */ {IDS_ADDITIONAL_DELAY_FR, ""},  /* fuzzy */
/* hu */ {IDS_ADDITIONAL_DELAY_HU, "T�bblet sz�net"},
/* it */ {IDS_ADDITIONAL_DELAY_IT, "Ritardo aggiuntivo"},
/* nl */ {IDS_ADDITIONAL_DELAY_NL, "Extra vertraging"},
/* pl */ {IDS_ADDITIONAL_DELAY_PL, ""},  /* fuzzy */
/* sv */ {IDS_ADDITIONAL_DELAY_SV, "Ytterligare f�rdr�jning"},

/* en */ {IDS_DELAY_AT_ZERO_VALUES,    "Delay at Zero Values"},
/* de */ {IDS_DELAY_AT_ZERO_VALUES_DE, "Verz�gerung bei Zero-Werte"},
/* fr */ {IDS_DELAY_AT_ZERO_VALUES_FR, "D�lai aux valeurs-z�ro"},
/* hu */ {IDS_DELAY_AT_ZERO_VALUES_HU, "Sz�net nulla �rt�kekn�l"},
/* it */ {IDS_DELAY_AT_ZERO_VALUES_IT, "Ritardo ai valori di zero"},
/* nl */ {IDS_DELAY_AT_ZERO_VALUES_NL, "Nul-waarden vertraging"},
/* pl */ {IDS_DELAY_AT_ZERO_VALUES_PL, ""},  /* fuzzy */
/* sv */ {IDS_DELAY_AT_ZERO_VALUES_SV, "F�rdr�jning vid &nollv�rden"},

/* en */ {IDS_DATASETTE_SETTINGS,    "Datasette Settings"},
/* de */ {IDS_DATASETTE_SETTINGS_DE, "Datasette Einstellungen"},
/* fr */ {IDS_DATASETTE_SETTINGS_FR, "R�glages datassette"},
/* hu */ {IDS_DATASETTE_SETTINGS_HU, "Magn� be�ll�t�sai"},
/* it */ {IDS_DATASETTE_SETTINGS_IT, "Impostazioni registratore"},
/* nl */ {IDS_DATASETTE_SETTINGS_NL, "Datasette Instellingen"},
/* pl */ {IDS_DATASETTE_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_DATASETTE_SETTINGS_SV, "Datasetteinst�llningar"},

/* en */ {IDS_NEVER_EXTEND,    "Never extend"},
/* de */ {IDS_NEVER_EXTEND_DE, "Nie Erweitern"},
/* fr */ {IDS_NEVER_EXTEND_FR, "Ne jamais �&tendre"},
/* hu */ {IDS_NEVER_EXTEND_HU, "Nincs kiterjeszt�s"},
/* it */ {IDS_NEVER_EXTEND_IT, "Non estendere mai"},
/* nl */ {IDS_NEVER_EXTEND_NL, "Nooit uitbreiden"},
/* pl */ {IDS_NEVER_EXTEND_PL, ""},  /* fuzzy */
/* sv */ {IDS_NEVER_EXTEND_SV, "Ut�ka aldrig"},

/* en */ {IDS_ASK_ON_EXTEND,    "Ask on extend"},
/* de */ {IDS_ASK_ON_EXTEND_DE, "Bei Erweiterung R�ckfrage"},
/* fr */ {IDS_ASK_ON_EXTEND_FR, "Demander avant d'�tendre"},
/* hu */ {IDS_ASK_ON_EXTEND_HU, "K�rd�s kiterjeszt�s eset�n"},
/* it */ {IDS_ASK_ON_EXTEND_IT, "Estendi su richiesta"},
/* nl */ {IDS_ASK_ON_EXTEND_NL, "Vragen bij uitbreiden"},
/* pl */ {IDS_ASK_ON_EXTEND_PL, ""},  /* fuzzy */
/* sv */ {IDS_ASK_ON_EXTEND_SV, "Fraga vid ut�kning"},

/* en */ {IDS_EXTEND_ON_ACCESS,    "Extend on access"},
/* de */ {IDS_EXTEND_ON_ACCESS_DE, "Erweitern wenn n�tig"},
/* fr */ {IDS_EXTEND_ON_ACCESS_FR, "�tendre   l'acces"},
/* hu */ {IDS_EXTEND_ON_ACCESS_HU, "Kiterjeszt�s hozz�f�r�skor"},
/* it */ {IDS_EXTEND_ON_ACCESS_IT, "All'accesso"},
/* nl */ {IDS_EXTEND_ON_ACCESS_NL, "Uitbreiden bij toegang"},
/* pl */ {IDS_EXTEND_ON_ACCESS_PL, ""},  /* fuzzy */
/* sv */ {IDS_EXTEND_ON_ACCESS_SV, "Ut�ka vid atkomst"},

/* en */ {IDS_NO_TRAPS,    "No traps"},
/* de */ {IDS_NO_TRAPS_DE, "Kein Traps"},
/* fr */ {IDS_NO_TRAPS_FR, "No traps"},
/* hu */ {IDS_NO_TRAPS_HU, "Folyamatos emul�ci�"},
/* it */ {IDS_NO_TRAPS_IT, "Non rilevare"},
/* nl */ {IDS_NO_TRAPS_NL, "Geen traps"},
/* pl */ {IDS_NO_TRAPS_PL, ""},  /* fuzzy */
/* sv */ {IDS_NO_TRAPS_SV, "Inga Traps"},

/* en */ {IDS_SKIP_CYCLES,    "Skip cycles"},
/* de */ {IDS_SKIP_CYCLES_DE, "Zyklen verwerfen"},
/* fr */ {IDS_SKIP_CYCLES_FR, "Sauter des cycles"},
/* hu */ {IDS_SKIP_CYCLES_HU, "Ciklusok kihagy�sa"},
/* it */ {IDS_SKIP_CYCLES_IT, "Salta cicli"},
/* nl */ {IDS_SKIP_CYCLES_NL, "Sla cycli over"},
/* pl */ {IDS_SKIP_CYCLES_PL, ""},  /* fuzzy */
/* sv */ {IDS_SKIP_CYCLES_SV, "Hoppa cykler"},

/* en */ {IDS_TRAP_IDLE,    "Trap idle"},
/* de */ {IDS_TRAP_IDLE_DE, "Trap idle"},
/* fr */ {IDS_TRAP_IDLE_FR, "Trap idle"},
/* hu */ {IDS_TRAP_IDLE_HU, "�res ciklusok kihagy�sa"},
/* it */ {IDS_TRAP_IDLE_IT, "Rileva inattivit�"},
/* nl */ {IDS_TRAP_IDLE_NL, "Trap idle"},
/* pl */ {IDS_TRAP_IDLE_PL, ""},  /* fuzzy */
/* sv */ {IDS_TRAP_IDLE_SV, "F�nga inaktiva"},

/* en */ {IDS_DRIVE_TYPE,    "Drive type"},
/* de */ {IDS_DRIVE_TYPE_DE, "Ger�te Typ"},
/* fr */ {IDS_DRIVE_TYPE_FR, "Type de lecteur"},
/* hu */ {IDS_DRIVE_TYPE_HU, "Lemezegys�g t�pus"},
/* it */ {IDS_DRIVE_TYPE_IT, "Tipo di drive"},
/* nl */ {IDS_DRIVE_TYPE_NL, "Drive soort"},
/* pl */ {IDS_DRIVE_TYPE_PL, ""},  /* fuzzy */
/* sv */ {IDS_DRIVE_TYPE_SV, "Enhetstyp"},

/* en */ {IDS_40_TRACK_HANDLING,    "40 track handling"},
/* de */ {IDS_40_TRACK_HANDLING_DE, "40 Spur Umgang"},
/* fr */ {IDS_40_TRACK_HANDLING_FR, "Prise en charge du 40 pistes"},
/* hu */ {IDS_40_TRACK_HANDLING_HU, "40 s�v kezel�se"},
/* it */ {IDS_40_TRACK_HANDLING_IT, "Gestione 40 tracce"},
/* nl */ {IDS_40_TRACK_HANDLING_NL, "40 sporen ondersteuning"},
/* pl */ {IDS_40_TRACK_HANDLING_PL, ""},  /* fuzzy */
/* sv */ {IDS_40_TRACK_HANDLING_SV, "Hantering av 40 spar"},

/* en */ {IDS_DRIVE_EXPANSION,    "Drive expansion"},
/* de */ {IDS_DRIVE_EXPANSION_DE, "Laufwerk Erweiterung"},
/* fr */ {IDS_DRIVE_EXPANSION_FR, "Expansion de lecteur"},
/* hu */ {IDS_DRIVE_EXPANSION_HU, "Lemezegys�g b�v�t�s"},
/* it */ {IDS_DRIVE_EXPANSION_IT, "Espasione del drive"},
/* nl */ {IDS_DRIVE_EXPANSION_NL, "Drive uitbreiding"},
/* pl */ {IDS_DRIVE_EXPANSION_PL, ""},  /* fuzzy */
/* sv */ {IDS_DRIVE_EXPANSION_SV, "Enhetsut�kning"},

/* en */ {IDS_IDLE_METHOD,    "Idle method"},
/* de */ {IDS_IDLE_METHOD_DE, "Idle Methode"},
/* fr */ {IDS_IDLE_METHOD_FR, "M�thode d'idle"},
/* hu */ {IDS_IDLE_METHOD_HU, "�resj�rat m�d"},
/* it */ {IDS_IDLE_METHOD_IT, "Rileva inattivit�"},
/* nl */ {IDS_IDLE_METHOD_NL, "Idle methode"},
/* pl */ {IDS_IDLE_METHOD_PL, ""},  /* fuzzy */
/* sv */ {IDS_IDLE_METHOD_SV, "Pausmetod"},

/* en */ {IDS_PARALLEL_CABLE,    "Parallel cable"},
/* de */ {IDS_PARALLEL_CABLE_DE, "Parallel Kabel"},
/* fr */ {IDS_PARALLEL_CABLE_FR, "C�ble parallele"},
/* hu */ {IDS_PARALLEL_CABLE_HU, "P�rhuzamos k�bel"},
/* it */ {IDS_PARALLEL_CABLE_IT, "Cavo parallelo"},
/* nl */ {IDS_PARALLEL_CABLE_NL, "Parallel kabel"},
/* pl */ {IDS_PARALLEL_CABLE_PL, ""},  /* fuzzy */
/* sv */ {IDS_PARALLEL_CABLE_SV, "Parallellkabel"},

/* en */ {IDS_DRIVE_SETTINGS,    "Drive Settings"},
/* de */ {IDS_DRIVE_SETTINGS_DE, "Floppy Einstellungen"},
/* fr */ {IDS_DRIVE_SETTINGS_FR, "Parametres des lecteurs"},
/* hu */ {IDS_DRIVE_SETTINGS_HU, "Lemezegys�g be�ll�t�sai"},
/* it */ {IDS_DRIVE_SETTINGS_IT, "Impostazioni drive"},
/* nl */ {IDS_DRIVE_SETTINGS_NL, "Drive instellingen"},
/* pl */ {IDS_DRIVE_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_DRIVE_SETTINGS_SV, "Diskettenhetsinst�llningar"},

/* en */ {IDS_GEORAM_SIZE,    "GEORAM Size"},
/* de */ {IDS_GEORAM_SIZE_DE, "GEORAM Gr��e"},
/* fr */ {IDS_GEORAM_SIZE_FR, "Taille du GEORAM"},
/* hu */ {IDS_GEORAM_SIZE_HU, "GEORAM m�rete"},
/* it */ {IDS_GEORAM_SIZE_IT, "Dimensione GEORAM"},
/* nl */ {IDS_GEORAM_SIZE_NL, "GEORAM grootte"},
/* pl */ {IDS_GEORAM_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDS_GEORAM_SIZE_SV, "GEORAM-storlek"},

/* en */ {IDS_GEORAM_FILENAME,    "GEORAM file"},
/* de */ {IDS_GEORAM_FILENAME_DE, "GEORAM Datei"},
/* fr */ {IDS_GEORAM_FILENAME_FR, "Fichier GEORAM"},
/* hu */ {IDS_GEORAM_FILENAME_HU, "GEORAM f�jl"},
/* it */ {IDS_GEORAM_FILENAME_IT, "File GEORAM"},
/* nl */ {IDS_GEORAM_FILENAME_NL, "GEORAM bestand"},
/* pl */ {IDS_GEORAM_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_GEORAM_FILENAME_SV, "GEORAM-fil"},

/* en */ {IDS_GEORAM_FILENAME_SELECT,    "Select file for GEORAM"},
/* de */ {IDS_GEORAM_FILENAME_SELECT_DE, "Datei f�r GEORAM selektieren"},
/* fr */ {IDS_GEORAM_FILENAME_SELECT_FR, "S�lectionner fichier pour GEORAM"},
/* hu */ {IDS_GEORAM_FILENAME_SELECT_HU, "V�lasszon f�jlt a GEORAM-hoz"},
/* it */ {IDS_GEORAM_FILENAME_SELECT_IT, "Seleziona il file per il GEORAM"},
/* nl */ {IDS_GEORAM_FILENAME_SELECT_NL, "Selecteer bestand voor GEORAM"},
/* pl */ {IDS_GEORAM_FILENAME_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_GEORAM_FILENAME_SELECT_SV, "V�lj fil f�r GEORAM"},

/* en */ {IDS_GEORAM_SETTINGS,    "GEORAM Settings"},
/* de */ {IDS_GEORAM_SETTINGS_DE, "GEORAM Einstellungen"},
/* fr */ {IDS_GEORAM_SETTINGS_FR, "Parametres GEORAM"},
/* hu */ {IDS_GEORAM_SETTINGS_HU, "GEORAM be�ll�t�sai"},
/* it */ {IDS_GEORAM_SETTINGS_IT, "Impostazioni GEORAM"},
/* nl */ {IDS_GEORAM_SETTINGS_NL, "GEORAM Instellingen"},
/* pl */ {IDS_GEORAM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_GEORAM_SETTINGS_SV, "GEORAM-inst�llningar"},

/* en */ {IDS_JOYSTICK_IN_PORT_1,    "Joystick in port #1"},
/* de */ {IDS_JOYSTICK_IN_PORT_1_DE, "Joystick in Port #1"},
/* fr */ {IDS_JOYSTICK_IN_PORT_1_FR, "Joystick dans le port #1"},
/* hu */ {IDS_JOYSTICK_IN_PORT_1_HU, "Botkorm�ny az 1-es porton"},
/* it */ {IDS_JOYSTICK_IN_PORT_1_IT, "Joystick nella porta #1"},
/* nl */ {IDS_JOYSTICK_IN_PORT_1_NL, "Joystick in poort #1"},
/* pl */ {IDS_JOYSTICK_IN_PORT_1_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOYSTICK_IN_PORT_1_SV, "Joystick i spelport 1"},

/* en */ {IDS_JOYSTICK_IN_PORT_2,    "Joystick in port #2"},
/* de */ {IDS_JOYSTICK_IN_PORT_2_DE, "Joystick in Port #2"},
/* fr */ {IDS_JOYSTICK_IN_PORT_2_FR, "Joystick dans le port #2"},
/* hu */ {IDS_JOYSTICK_IN_PORT_2_HU, "Botkorm�ny a 2-es porton"},
/* it */ {IDS_JOYSTICK_IN_PORT_2_IT, "Joystick nella porta #2"},
/* nl */ {IDS_JOYSTICK_IN_PORT_2_NL, "Joystick in poort #2"},
/* pl */ {IDS_JOYSTICK_IN_PORT_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOYSTICK_IN_PORT_2_SV, "Joystick i spelport 2"},

/* en */ {IDS_JOYSTICK_SETTINGS,    "Joystick Settings"},
/* de */ {IDS_JOYSTICK_SETTINGS_DE, "Joystick Einstellungen"},
/* fr */ {IDS_JOYSTICK_SETTINGS_FR, "Parametres des joysticks"},
/* hu */ {IDS_JOYSTICK_SETTINGS_HU, "Botkorm�ny be�ll�t�sai"},
/* it */ {IDS_JOYSTICK_SETTINGS_IT, "Impostazioni joystick"},
/* nl */ {IDS_JOYSTICK_SETTINGS_NL, "Joystick instellingen"},
/* pl */ {IDS_JOYSTICK_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOYSTICK_SETTINGS_SV, "Inst�llningar f�r joystick"},

/* en */ {IDS_KEYPAD,    "Numpad"},
/* de */ {IDS_KEYPAD_DE, "Ziffernblock"},
/* fr */ {IDS_KEYPAD_FR, ""},  /* fuzzy */
/* hu */ {IDS_KEYPAD_HU, "Numerikus billenty�k"},
/* it */ {IDS_KEYPAD_IT, "Tastierino numerico"},
/* nl */ {IDS_KEYPAD_NL, "Numpad"},
/* pl */ {IDS_KEYPAD_PL, ""},  /* fuzzy */
/* sv */ {IDS_KEYPAD_SV, "Numreriskt tangentbord"},

/* en */ {IDS_JOY_PORT_0,    "Joy Port 0"},
/* de */ {IDS_JOY_PORT_0_DE, "Joystick Port 0"},
/* fr */ {IDS_JOY_PORT_0_FR, ""},  /* fuzzy */
/* hu */ {IDS_JOY_PORT_0_HU, "Botkorm�ny port 0"},
/* it */ {IDS_JOY_PORT_0_IT, "Joy Porta 0"},
/* nl */ {IDS_JOY_PORT_0_NL, "Joy Poort 0"},
/* pl */ {IDS_JOY_PORT_0_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOY_PORT_0_SV, "Joy port 0"},

/* en */ {IDS_JOY_PORT_1,    "Joy Port 1"},
/* de */ {IDS_JOY_PORT_1_DE, "Joystick Port 1"},
/* fr */ {IDS_JOY_PORT_1_FR, ""},  /* fuzzy */
/* hu */ {IDS_JOY_PORT_1_HU, "Botkorm�ny port 1"},
/* it */ {IDS_JOY_PORT_1_IT, "Joy Porta 1"},
/* nl */ {IDS_JOY_PORT_1_NL, "Joy Poort 1"},
/* pl */ {IDS_JOY_PORT_1_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOY_PORT_1_SV, "Joy Port 1"},

/* en */ {IDS_JOY_PORT_2,    "Joy Port 2"},
/* de */ {IDS_JOY_PORT_2_DE, "Joystick Port 2"},
/* fr */ {IDS_JOY_PORT_2_FR, ""},  /* fuzzy */
/* hu */ {IDS_JOY_PORT_2_HU, "Botkorm�ny port 2"},
/* it */ {IDS_JOY_PORT_2_IT, "Joy Porta 2"},
/* nl */ {IDS_JOY_PORT_2_NL, "Joy Poort 2"},
/* pl */ {IDS_JOY_PORT_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOY_PORT_2_SV, "Joy Port 2"},

/* en */ {IDS_JOY_PORT_3,    "Joy Port 3"},
/* de */ {IDS_JOY_PORT_3_DE, "Joystick Port 3"},
/* fr */ {IDS_JOY_PORT_3_FR, ""},  /* fuzzy */
/* hu */ {IDS_JOY_PORT_3_HU, "Botkorm�ny port 3"},
/* it */ {IDS_JOY_PORT_3_IT, "Joy Porta 3"},
/* nl */ {IDS_JOY_PORT_3_NL, "Joy Poort 3"},
/* pl */ {IDS_JOY_PORT_3_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOY_PORT_3_SV, "Joy Port 3"},

/* en */ {IDS_STOP_BLUE,    "Stop/Blue"},
/* de */ {IDS_STOP_BLUE_DE, "Stop/Blau"},
/* fr */ {IDS_STOP_BLUE_FR, ""},  /* fuzzy */
/* hu */ {IDS_STOP_BLUE_HU, "Le�ll�t/K�k"},
/* it */ {IDS_STOP_BLUE_IT, "Stop/Blu"},
/* nl */ {IDS_STOP_BLUE_NL, "Stop/Blauw"},
/* pl */ {IDS_STOP_BLUE_PL, ""},  /* fuzzy */
/* sv */ {IDS_STOP_BLUE_SV, "Stop/Bl�"},

/* en */ {IDS_SELECT_RED,    "Select/Red"},
/* de */ {IDS_SELECT_RED_DE, "Auswahl/Rot"},
/* fr */ {IDS_SELECT_RED_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_RED_HU, "Kiv�laszt/Piros"},
/* it */ {IDS_SELECT_RED_IT, "Seleziona/Rosso"},
/* nl */ {IDS_SELECT_RED_NL, "Select/Rood"},
/* pl */ {IDS_SELECT_RED_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_RED_SV, "Select/R�d"},

/* en */ {IDS_REPEAT_YELLOW,    "Repeat/Yellow"},
/* de */ {IDS_REPEAT_YELLOW_DE, "Wiederhole/Gelb"},
/* fr */ {IDS_REPEAT_YELLOW_FR, ""},  /* fuzzy */
/* hu */ {IDS_REPEAT_YELLOW_HU, "Ism�tel/S�rga"},
/* it */ {IDS_REPEAT_YELLOW_IT, "Ripeti/Giallo"},
/* nl */ {IDS_REPEAT_YELLOW_NL, "Repeat/Geel"},
/* pl */ {IDS_REPEAT_YELLOW_PL, ""},  /* fuzzy */
/* sv */ {IDS_REPEAT_YELLOW_SV, "Repeat/Gul"},

/* en */ {IDS_SHUFFLE_GREEN,    "Shuffle/Green"},
/* de */ {IDS_SHUFFLE_GREEN_DE, "Mische/Gr�n"},
/* fr */ {IDS_SHUFFLE_GREEN_FR, ""},  /* fuzzy */
/* hu */ {IDS_SHUFFLE_GREEN_HU, "V�letlenszer�/Z�ld"},
/* it */ {IDS_SHUFFLE_GREEN_IT, "Mischia/Verde"},
/* nl */ {IDS_SHUFFLE_GREEN_NL, "Shuffle/Groen"},
/* pl */ {IDS_SHUFFLE_GREEN_PL, ""},  /* fuzzy */
/* sv */ {IDS_SHUFFLE_GREEN_SV, "Shuffle/Gr�n"},

/* en */ {IDS_FORWARD_CHARCOAL,    "Forward/Charcoal"},
/* de */ {IDS_FORWARD_CHARCOAL_DE, "Vorw�rts/Kohle"},
/* fr */ {IDS_FORWARD_CHARCOAL_FR, ""},  /* fuzzy */
/* hu */ {IDS_FORWARD_CHARCOAL_HU, "El�re/Fekete"},
/* it */ {IDS_FORWARD_CHARCOAL_IT, "Avanti/Carboncino"},
/* nl */ {IDS_FORWARD_CHARCOAL_NL, "Forward/Charcoal"},
/* pl */ {IDS_FORWARD_CHARCOAL_PL, ""},  /* fuzzy */
/* sv */ {IDS_FORWARD_CHARCOAL_SV, "Forward/Tr�kol"},

/* en */ {IDS_REVERSE_CHARCOAL,    "Reverse/Charcoal"},
/* de */ {IDS_REVERSE_CHARCOAL_DE, "R�ckw�rts/Kohle"},
/* fr */ {IDS_REVERSE_CHARCOAL_FR, ""},  /* fuzzy */
/* hu */ {IDS_REVERSE_CHARCOAL_HU, "Vissza/Fekete"},
/* it */ {IDS_REVERSE_CHARCOAL_IT, "Indietro/Carboncino"},
/* nl */ {IDS_REVERSE_CHARCOAL_NL, "Reverse/Charcoal"},
/* pl */ {IDS_REVERSE_CHARCOAL_PL, ""},  /* fuzzy */
/* sv */ {IDS_REVERSE_CHARCOAL_SV, "Reverse/Tr�kol"},

/* en */ {IDS_PLAY_PAUSE_GREY,    "Play-Pause/Grey"},
/* de */ {IDS_PLAY_PAUSE_GREY_DE, "Play-Pause/Grau"},
/* fr */ {IDS_PLAY_PAUSE_GREY_FR, ""},  /* fuzzy */
/* hu */ {IDS_PLAY_PAUSE_GREY_HU, "Lej�tsz�s-Meg�ll�t�s/Sz�rke"},
/* it */ {IDS_PLAY_PAUSE_GREY_IT, "Avvia-Pausa/Grigio"},
/* nl */ {IDS_PLAY_PAUSE_GREY_NL, "Play-Pause/Grijs"},
/* pl */ {IDS_PLAY_PAUSE_GREY_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLAY_PAUSE_GREY_SV, "[Play/Pause]/Gr�"},

/* en */ {IDS_JOY_1_DEVICE,    "Joy 1 Device"},
/* de */ {IDS_JOY_1_DEVICE_DE, "Joystick 1 Ger�t"},
/* fr */ {IDS_JOY_1_DEVICE_FR, ""},  /* fuzzy */
/* hu */ {IDS_JOY_1_DEVICE_HU, "1-es botkorm�ny eszk�z"},
/* it */ {IDS_JOY_1_DEVICE_IT, "Dispositivo Joy 1"},
/* nl */ {IDS_JOY_1_DEVICE_NL, "Joy 1 Apparaat"},
/* pl */ {IDS_JOY_1_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOY_1_DEVICE_SV, "Joy 1-enhet"},

/* en */ {IDS_JOY_2_DEVICE,    "Joy 2 Device"},
/* de */ {IDS_JOY_2_DEVICE_DE, "Joystick 2 Ger�t"},
/* fr */ {IDS_JOY_2_DEVICE_FR, ""},  /* fuzzy */
/* hu */ {IDS_JOY_2_DEVICE_HU, "2-es botkorm�ny eszk�z"},
/* it */ {IDS_JOY_2_DEVICE_IT, "Dispositivo Joy 2"},
/* nl */ {IDS_JOY_2_DEVICE_NL, "Joy 2 Apparaat"},
/* pl */ {IDS_JOY_2_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOY_2_DEVICE_SV, "Joy 2-enhet"},

/* en */ {IDS_JOY_1_FIRE,    "Joy 1 Fire"},
/* de */ {IDS_JOY_1_FIRE_DE, "Joystick 1 Feuer"},
/* fr */ {IDS_JOY_1_FIRE_FR, ""},  /* fuzzy */
/* hu */ {IDS_JOY_1_FIRE_HU, "1-es botkorm�ny t�z"},
/* it */ {IDS_JOY_1_FIRE_IT, "Fuoco Joy 1"},
/* nl */ {IDS_JOY_1_FIRE_NL, "Joy 1 Vuur"},
/* pl */ {IDS_JOY_1_FIRE_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOY_1_FIRE_SV, "Joy 1 Fire"},

/* en */ {IDS_JOY_2_FIRE,    "Joy 2 Fire"},
/* de */ {IDS_JOY_2_FIRE_DE, "Joystick 2 Feuer"},
/* fr */ {IDS_JOY_2_FIRE_FR, ""},  /* fuzzy */
/* hu */ {IDS_JOY_2_FIRE_HU, "2-es botkorm�ny t�z"},
/* it */ {IDS_JOY_2_FIRE_IT, "Fuoco Joy 2"},
/* nl */ {IDS_JOY_2_FIRE_NL, "Joy 1 Vuur"},
/* pl */ {IDS_JOY_2_FIRE_PL, ""},  /* fuzzy */
/* sv */ {IDS_JOY_2_FIRE_SV, "Joy 2 Fire"},

/* en */ {IDS_SET_INPUT_JOYSTICK_1,    "Joystick #1 emulation: (0:None, 1: Numpad, 2: Keyset A/AI, 3: Keyset B/AI)"},
/* de */ {IDS_SET_INPUT_JOYSTICK_1_DE, "Joystick #1 Emulation: (0:Keine, 1: Numpad, 2: Keysatz A/AI, 3: Keysatz B/AI)"},
/* fr */ {IDS_SET_INPUT_JOYSTICK_1_FR, ""},  /* fuzzy */
/* hu */ {IDS_SET_INPUT_JOYSTICK_1_HU, "#1-es botkorm�ny emul�ci�: (0: Nincs, 1: numerikus, 2: 'A' gombk�szlet, 3: 'B' gombk�szlet)"},
/* it */ {IDS_SET_INPUT_JOYSTICK_1_IT, "Emulazione Joystick #1: (0:Nessuna, 1: Tastierino numerico, 2: Keyset A/AI, 3: Keyset B/AI)"},
/* nl */ {IDS_SET_INPUT_JOYSTICK_1_NL, "Joystick #1 emulatie: (0: Geen, 1: Numpad, 2: Keyset A/AI, 3: Keyset B/AI)"},
/* pl */ {IDS_SET_INPUT_JOYSTICK_1_PL, ""},  /* fuzzy */
/* sv */ {IDS_SET_INPUT_JOYSTICK_1_SV, "Joystick #1 emulation: (0:Ingen, 1: Numeriskt tangentbord, 2: Teckenupps�ttning A/AI, 3: Teckenupps�ttning B/AI)" },

/* en */ {IDS_SET_INPUT_JOYSTICK_2,    "Joystick #2 emulation: (0: None, 1: Numpad, 2: Keyset A/AI, 3: Keyset B/AI)"},
/* de */ {IDS_SET_INPUT_JOYSTICK_2_DE, "Joystick #2 Emulation: (0:Keine, 1: Numpad, 2: Keysatz A/AI, 3: Keysatz B/AI)"},
/* fr */ {IDS_SET_INPUT_JOYSTICK_2_FR, ""},  /* fuzzy */
/* hu */ {IDS_SET_INPUT_JOYSTICK_2_HU, "#2-es botkorm�ny emul�ci�: (0: Nincs, 1: numerikus, 2: 'A' gombk�szlet, 3: 'B' gombk�szlet)"},
/* it */ {IDS_SET_INPUT_JOYSTICK_2_IT, "Emulazione Joystick #2: (0:Nessuna, 1: Tastierino numerico, 2: Keyset A/AI, 3: Keyset B/AI)"},
/* nl */ {IDS_SET_INPUT_JOYSTICK_2_NL, "Joystick #2 emulatie: (0: Geen, 1: Numpad, 2: Keyset A/AI, 3: Keyset B/AI)"},
/* pl */ {IDS_SET_INPUT_JOYSTICK_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_SET_INPUT_JOYSTICK_2_SV, "Joystick #2 emulation: (0:Ingen, 1: Numeriskt tangentbord, 2: Teckenupps�ttning A/AI, 3: Teckenupps�ttning B/AI)" },

/* en */ {IDS_SET_INPUT_JOYLL_1,    "Joystick #1 emulation: (0: None, 1: Numpad, 2-5: Amiga Joy 0-3"},
/* de */ {IDS_SET_INPUT_JOYLL_1_DE, "Joystick #1 Emulation: (0: Keine, 1: Numpad, 2-5: Amiga Joystick 0-3"},
/* fr */ {IDS_SET_INPUT_JOYLL_1_FR, ""},  /* fuzzy */
/* hu */ {IDS_SET_INPUT_JOYLL_1_HU, "#1-es botkorm�ny emul�ci�: (0: nincs, 1: numerikus billenty�k, 2-5: amiga botkorm�ny 0-3"},
/* it */ {IDS_SET_INPUT_JOYLL_1_IT, "Emulazione Joystick #1: (0: Nessuna, 1: Tastierino numerico, 2-5: Joy Amiga 0-3"},
/* nl */ {IDS_SET_INPUT_JOYLL_1_NL, "Joystick #1 emulatie: (0: Geen, 1: Numpad, 2-5: Amiga Joy 0-3"},
/* pl */ {IDS_SET_INPUT_JOYLL_1_PL, ""},  /* fuzzy */
/* sv */ {IDS_SET_INPUT_JOYLL_1_SV, "Joystick #1 emulation: (0:Ingen, 1: Numeriskt tangentbord, 2-5: Amiga Joy 0-3"},

/* en */ {IDS_SET_INPUT_JOYLL_2,    "Joystick #2 emulation: (0: None, 1: Numpad, 2-5: Amiga Joy 0-3"},
/* de */ {IDS_SET_INPUT_JOYLL_2_DE, "Joystick #2 Emulation: (0: Keine, 1: Numpad, 2-5: Amiga Joystick 0-3"},
/* fr */ {IDS_SET_INPUT_JOYLL_2_FR, ""},  /* fuzzy */
/* hu */ {IDS_SET_INPUT_JOYLL_2_HU, "#2-es botkorm�ny emul�ci�: (0: nincs, 1: numerikus billenty�k, 2-5: amiga botkorm�ny 0-3"},
/* it */ {IDS_SET_INPUT_JOYLL_2_IT, "Emulazione Joystick #2: (0: Nessuna, 1: Tastierino numerico, 2-5: Joy Amiga 0-3"},
/* nl */ {IDS_SET_INPUT_JOYLL_2_NL, "Joystick #2 emulatie (0: Geen, 1: Numpad, 2-5: Amiga Joy 0-3"},
/* pl */ {IDS_SET_INPUT_JOYLL_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_SET_INPUT_JOYLL_2_SV, "Joystick #2 emulation: (0:Ingen, 1: Numeriskt tangentbord, 2-5: Amiga Joy 0-3"},

/* en */ {IDS_SAVE_SETTINGS_ON_EXIT,    "Save settings (resources) on exit"},
/* de */ {IDS_SAVE_SETTINGS_ON_EXIT_DE, "Einstellungen (Ressourcen) beim Beenden speichern"},
/* fr */ {IDS_SAVE_SETTINGS_ON_EXIT_FR, "Sauvegarder les parametres   la sortie"},
/* hu */ {IDS_SAVE_SETTINGS_ON_EXIT_HU, "Er�forr�sbe�ll�t�sok ment�se kil�p�skor"},
/* it */ {IDS_SAVE_SETTINGS_ON_EXIT_IT, "Salva le impostazioni in uscita"},
/* nl */ {IDS_SAVE_SETTINGS_ON_EXIT_NL, "Instellingen (resources) opslaan bij afsluiten"},
/* pl */ {IDS_SAVE_SETTINGS_ON_EXIT_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_SETTINGS_ON_EXIT_SV, "Spara inst�llningar (resurser) vid avslut"},

/* en */ {IDS_NEVER_SAVE_SETTINGS_EXIT,    "Never save settings (resources) on exit"},
/* de */ {IDS_NEVER_SAVE_SETTINGS_EXIT_DE, "Einstellungen beim Beenden nie speichern"},
/* fr */ {IDS_NEVER_SAVE_SETTINGS_EXIT_FR, "Ne jamais enregistrer les parametres   la sortie"},
/* hu */ {IDS_NEVER_SAVE_SETTINGS_EXIT_HU, "Sose mentse az er�forr�sbe�ll�t�sokat kil�p�skor"},
/* it */ {IDS_NEVER_SAVE_SETTINGS_EXIT_IT, "Non salvare mai le impostazioni in uscita"},
/* nl */ {IDS_NEVER_SAVE_SETTINGS_EXIT_NL, "Instellingen (resources) niet opslaan bij afsluiten"},
/* pl */ {IDS_NEVER_SAVE_SETTINGS_EXIT_PL, ""},  /* fuzzy */
/* sv */ {IDS_NEVER_SAVE_SETTINGS_EXIT_SV, "Spara aldrig inst�llningar (resurser) vid avslut"},

/* en */ {IDS_CONFIRM_QUITING_VICE,    "Confirm quiting VICE"},
/* de */ {IDS_CONFIRM_QUITING_VICE_DE, "Best�tige beenden von VICE"},
/* fr */ {IDS_CONFIRM_QUITING_VICE_FR, "Demander une confirmation avant de quitter VICE"},
/* hu */ {IDS_CONFIRM_QUITING_VICE_HU, "Meger�s�t�s kil�p�s el�tt"},
/* it */ {IDS_CONFIRM_QUITING_VICE_IT, "Conferma l'uscita da VICE"},
/* nl */ {IDS_CONFIRM_QUITING_VICE_NL, "Bevestigen van het aflsuiten van VICE"},
/* pl */ {IDS_CONFIRM_QUITING_VICE_PL, ""},  /* fuzzy */
/* sv */ {IDS_CONFIRM_QUITING_VICE_SV, "Bekr�fta f�r att avsluta VICE"},

/* en */ {IDS_NEVER_CONFIRM_QUITING_VICE,    "Never confirm quiting VICE"},
/* de */ {IDS_NEVER_CONFIRM_QUITING_VICE_DE, "Keine Best�tigung beim Beenden von VICE"},
/* fr */ {IDS_NEVER_CONFIRM_QUITING_VICE_FR, "Ne pas demander de confirmation pour quitter VICE"},
/* hu */ {IDS_NEVER_CONFIRM_QUITING_VICE_HU, "Sosem k�r meger�s�t�st kil�p�s el�tt"},
/* it */ {IDS_NEVER_CONFIRM_QUITING_VICE_IT, "Non confermare mai l'uscita da VICE"},
/* nl */ {IDS_NEVER_CONFIRM_QUITING_VICE_NL, "Niet bevestigen van het afsluiten van VICE"},
/* pl */ {IDS_NEVER_CONFIRM_QUITING_VICE_PL, ""},  /* fuzzy */
/* sv */ {IDS_NEVER_CONFIRM_QUITING_VICE_SV, "Bekr�fta inte f�r att avsluta VICE"},

/* en */ {IDS_PET_REU_SIZE,    "PET REU Size"},
/* de */ {IDS_PET_REU_SIZE_DE, "PET REU Gr��e"},
/* fr */ {IDS_PET_REU_SIZE_FR, "Taille du PET REU"},
/* hu */ {IDS_PET_REU_SIZE_HU, "PET REU m�rete"},
/* it */ {IDS_PET_REU_SIZE_IT, "Dimensione PET REU"},
/* nl */ {IDS_PET_REU_SIZE_NL, "PET REU grootte"},
/* pl */ {IDS_PET_REU_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDS_PET_REU_SIZE_SV, "PET REU-storlek"},

/* en */ {IDS_PET_REU_FILENAME,    "PET REU file"},
/* de */ {IDS_PET_REU_FILENAME_DE, "PET REU Datei"},
/* fr */ {IDS_PET_REU_FILENAME_FR, "Fichier PET REU"},
/* hu */ {IDS_PET_REU_FILENAME_HU, "PET REU f�jl"},
/* it */ {IDS_PET_REU_FILENAME_IT, "File PET REU"},
/* nl */ {IDS_PET_REU_FILENAME_NL, "PET REU bestand"},
/* pl */ {IDS_PET_REU_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_PET_REU_FILENAME_SV, "PET REU-fil"},

/* en */ {IDS_PET_REU_FILENAME_SELECT,    "Select file for PET REU"},
/* de */ {IDS_PET_REU_FILENAME_SELECT_DE, "Datei f�r PET REU selektieren"},
/* fr */ {IDS_PET_REU_FILENAME_SELECT_FR, "S�lectionner fichier pour PET REU"},
/* hu */ {IDS_PET_REU_FILENAME_SELECT_HU, "V�lasszon f�jlt a PET REU-hoz"},
/* it */ {IDS_PET_REU_FILENAME_SELECT_IT, "Seleziona il file per il PET REU"},
/* nl */ {IDS_PET_REU_FILENAME_SELECT_NL, "Selecteer bestand voor PET REU"},
/* pl */ {IDS_PET_REU_FILENAME_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_PET_REU_FILENAME_SELECT_SV, "V�lj fil f�r PET REU"},

/* en */ {IDS_PET_REU_SETTINGS,    "PET REU Settings"},
/* de */ {IDS_PET_REU_SETTINGS_DE, "PET REU Einstellungen"},
/* fr */ {IDS_PET_REU_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_PET_REU_SETTINGS_HU, "PET REU be�ll�t�sai"},
/* it */ {IDS_PET_REU_SETTINGS_IT, "Impostazioni PET REU"},
/* nl */ {IDS_PET_REU_SETTINGS_NL, "PET REU Instellingen"},
/* pl */ {IDS_PET_REU_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_PET_REU_SETTINGS_SV, "PET REU-Inst�llningar"},

/* en */ {IDS_PLUS256K_SETTINGS,    "PLUS256K Settings"},
/* de */ {IDS_PLUS256K_SETTINGS_DE, "PLUS256K Einstellungen"},
/* fr */ {IDS_PLUS256K_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_PLUS256K_SETTINGS_HU, "PLUS256K be�ll�t�sai"},
/* it */ {IDS_PLUS256K_SETTINGS_IT, "Impostazioni PLUS256K"},
/* nl */ {IDS_PLUS256K_SETTINGS_NL, "PLUS256K Instellingen"},
/* pl */ {IDS_PLUS256K_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLUS256K_SETTINGS_SV, "PLUS256K-inst�llningar"},

/* en */ {IDS_PLUS60K_BASE,    "PLUS60K Base"},
/* de */ {IDS_PLUS60K_BASE_DE, "PLUS60K Basis Adresse"},
/* fr */ {IDS_PLUS60K_BASE_FR, ""},  /* fuzzy */
/* hu */ {IDS_PLUS60K_BASE_HU, "PLUS60K b�zisc�me"},
/* it */ {IDS_PLUS60K_BASE_IT, "Indirizzo base PLUS60K"},
/* nl */ {IDS_PLUS60K_BASE_NL, "PLUS60K basis adres"},
/* pl */ {IDS_PLUS60K_BASE_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLUS60K_BASE_SV, "PLUS60K bas-adress"},

/* en */ {IDS_PLUS60K_FILENAME,    "PLUS60K file"},
/* de */ {IDS_PLUS60K_FILENAME_DE, "PLUS60K Datei"},
/* fr */ {IDS_PLUS60K_FILENAME_FR, "Fichier PLUS60K"},
/* hu */ {IDS_PLUS60K_FILENAME_HU, "PLUS60K f�jl"},
/* it */ {IDS_PLUS60K_FILENAME_IT, "File PLUS60K"},
/* nl */ {IDS_PLUS60K_FILENAME_NL, "PLUS60K bestand"},
/* pl */ {IDS_PLUS60K_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLUS60K_FILENAME_SV, "PLUS60K-fil"},

/* en */ {IDS_PLUS60K_FILENAME_SELECT,    "Select file for PLUS60K"},
/* de */ {IDS_PLUS60K_FILENAME_SELECT_DE, "Datei f�r PLUS60K selektieren"},
/* fr */ {IDS_PLUS60K_FILENAME_SELECT_FR, "S�lectionner fichier pour PLUS60K"},
/* hu */ {IDS_PLUS60K_FILENAME_SELECT_HU, "V�lasszon f�jlt a PLUS60K-hoz"},
/* it */ {IDS_PLUS60K_FILENAME_SELECT_IT, "Seleziona file per PLUS60K"},
/* nl */ {IDS_PLUS60K_FILENAME_SELECT_NL, "Selecteer bestand voor PLUS60K"},
/* pl */ {IDS_PLUS60K_FILENAME_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLUS60K_FILENAME_SELECT_SV, "Ange fil f�r PLUS60K"},

/* en */ {IDS_PLUS60K_SETTINGS,    "PLUS60K Settings"},
/* de */ {IDS_PLUS60K_SETTINGS_DE, "PLUS60K Einstellungen"},
/* fr */ {IDS_PLUS60K_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_PLUS60K_SETTINGS_HU, "PLUS60K be�ll�t�sai"},
/* it */ {IDS_PLUS60K_SETTINGS_IT, "Impostazioni PLUS60K"},
/* nl */ {IDS_PLUS60K_SETTINGS_NL, "PLUS60K Instellingen"},
/* pl */ {IDS_PLUS60K_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLUS60K_SETTINGS_SV, "PLUS60K-inst�llningar"},

/* en */ {IDS_VALUE_FIRST_BYTE,    "Value of first byte"},
/* de */ {IDS_VALUE_FIRST_BYTE_DE, "Wert erstes Byte"},
/* fr */ {IDS_VALUE_FIRST_BYTE_FR, "Valeur du premier octet"},
/* hu */ {IDS_VALUE_FIRST_BYTE_HU, "Els� b�jt �rt�ke"},
/* it */ {IDS_VALUE_FIRST_BYTE_IT, "Valore del primo byte"},
/* nl */ {IDS_VALUE_FIRST_BYTE_NL, "Waarde van eerste byte"},
/* pl */ {IDS_VALUE_FIRST_BYTE_PL, ""},  /* fuzzy */
/* sv */ {IDS_VALUE_FIRST_BYTE_SV, "V�rde pa f�rsta byte"},

/* en */ {IDS_LENGTH_CONSTANT_VALUES,    "Length of constant values"},
/* de */ {IDS_LENGTH_CONSTANT_VALUES_DE, "L�nge der konstaten Werte"},
/* fr */ {IDS_LENGTH_CONSTANT_VALUES_FR, "Longeur des valeurs constantes"},
/* hu */ {IDS_LENGTH_CONSTANT_VALUES_HU, "�lland� �rt�kek hossza"},
/* it */ {IDS_LENGTH_CONSTANT_VALUES_IT, "Numero di valori costanti"},
/* nl */ {IDS_LENGTH_CONSTANT_VALUES_NL, "Lengte van constante waarden"},
/* pl */ {IDS_LENGTH_CONSTANT_VALUES_PL, ""},  /* fuzzy */
/* sv */ {IDS_LENGTH_CONSTANT_VALUES_SV, "L�ngd pa konstanta v�rden"},

/* en */ {IDS_LENGTH_CONSTANT_PATTERN,    "Length of constant pattern"},
/* de */ {IDS_LENGTH_CONSTANT_PATTERN_DE, "L�nge des konstanten Musters"},
/* fr */ {IDS_LENGTH_CONSTANT_PATTERN_FR, "Longeur des contantes pattern"},
/* hu */ {IDS_LENGTH_CONSTANT_PATTERN_HU, "�lland� minta hossza"},
/* it */ {IDS_LENGTH_CONSTANT_PATTERN_IT, "Dimensione del pattern costante"},
/* nl */ {IDS_LENGTH_CONSTANT_PATTERN_NL, "Lengte van constant patroon"},
/* pl */ {IDS_LENGTH_CONSTANT_PATTERN_PL, ""},  /* fuzzy */
/* sv */ {IDS_LENGTH_CONSTANT_PATTERN_SV, "L�ngd pa konstant m�nster"},

/* en */ {IDS_RAM_SETTINGS,    "RAM Settings"},
/* de */ {IDS_RAM_SETTINGS_DE, "RAM Einstellungen"},
/* fr */ {IDS_RAM_SETTINGS_FR, "Parametres de la RAM"},
/* hu */ {IDS_RAM_SETTINGS_HU, "RAM be�ll�t�sai"},
/* it */ {IDS_RAM_SETTINGS_IT, "Impostazioni RAM"},
/* nl */ {IDS_RAM_SETTINGS_NL, "RAM instellingen"},
/* pl */ {IDS_RAM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAM_SETTINGS_SV, "RAM-inst�llningar"},

/* en */ {IDS_READ_WRITE,    "read/write"},
/* de */ {IDS_READ_WRITE_DE, "lese/schreibe"},
/* fr */ {IDS_READ_WRITE_FR, ""},  /* fuzzy */
/* hu */ {IDS_READ_WRITE_HU, "�rhat�/olvashat�"},
/* it */ {IDS_READ_WRITE_IT, "lettura/scrittura"},
/* nl */ {IDS_READ_WRITE_NL, "lezen/schrijven"},
/* pl */ {IDS_READ_WRITE_PL, ""},  /* fuzzy */
/* sv */ {IDS_READ_WRITE_SV, "l�s/skriv"},

/* en */ {IDS_READ_ONLY,    "read-only"},
/* de */ {IDS_READ_ONLY_DE, "Schreibgschutz"},
/* fr */ {IDS_READ_ONLY_FR, "Lecture seule"},
/* hu */ {IDS_READ_ONLY_HU, "csak olvashat�"},
/* it */ {IDS_READ_ONLY_IT, "a sola lettura"},
/* nl */ {IDS_READ_ONLY_NL, "alleen lezen"},
/* pl */ {IDS_READ_ONLY_PL, ""},  /* fuzzy */
/* sv */ {IDS_READ_ONLY_SV, "Endast l�satkomst"},

/* en */ {IDS_RAMCART_READ_WRITE,    "RAMCART Read/Write"},
/* de */ {IDS_RAMCART_READ_WRITE_DE, "RAMCART Lesen/Schreiben"},
/* fr */ {IDS_RAMCART_READ_WRITE_FR, ""},  /* fuzzy */
/* hu */ {IDS_RAMCART_READ_WRITE_HU, "RAMCART �rhat�/olvashat�"},
/* it */ {IDS_RAMCART_READ_WRITE_IT, "Lettura/Scrittura RAMCART"},
/* nl */ {IDS_RAMCART_READ_WRITE_NL, "RAMCART Lezen/Schrijven"},
/* pl */ {IDS_RAMCART_READ_WRITE_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAMCART_READ_WRITE_SV, "RAMCART l�s/skriv"},

/* en */ {IDS_RAMCART_SIZE,    "RAMCART Size"},
/* de */ {IDS_RAMCART_SIZE_DE, "RAMCART Gr��e"},
/* fr */ {IDS_RAMCART_SIZE_FR, "Taille du RAMCART"},
/* hu */ {IDS_RAMCART_SIZE_HU, "RAMCART m�rete"},
/* it */ {IDS_RAMCART_SIZE_IT, "Dimensione RAMCART"},
/* nl */ {IDS_RAMCART_SIZE_NL, "RAMCART grootte"},
/* pl */ {IDS_RAMCART_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAMCART_SIZE_SV, "Storlek pa RAMCART"},

/* en */ {IDS_RAMCART_FILENAME,    "RAMCART file"},
/* de */ {IDS_RAMCART_FILENAME_DE, "RAMCART Datei"},
/* fr */ {IDS_RAMCART_FILENAME_FR, "Fichier RAMCART"},
/* hu */ {IDS_RAMCART_FILENAME_HU, "RAMCART f�jl"},
/* it */ {IDS_RAMCART_FILENAME_IT, "File RAMCART"},
/* nl */ {IDS_RAMCART_FILENAME_NL, "RAMCART bestand"},
/* pl */ {IDS_RAMCART_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAMCART_FILENAME_SV, "RAMCART-fil"},

/* en */ {IDS_RAMCART_FILENAME_SELECT,    "Select file for RAMCART"},
/* de */ {IDS_RAMCART_FILENAME_SELECT_DE, "Datei f�r RAMCART selektieren"},
/* fr */ {IDS_RAMCART_FILENAME_SELECT_FR, "S�lectionner fichier pour RAMCART"},
/* hu */ {IDS_RAMCART_FILENAME_SELECT_HU, "V�lasszon f�jlt a RAMCART-hoz"},
/* it */ {IDS_RAMCART_FILENAME_SELECT_IT, "Seleziona il file per il RAMCART"},
/* nl */ {IDS_RAMCART_FILENAME_SELECT_NL, "Selecteer bestand voor RAMCART"},
/* pl */ {IDS_RAMCART_FILENAME_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAMCART_FILENAME_SELECT_SV, "V�lj fil f�r RAMCART"},

/* en */ {IDS_RAMCART_SETTINGS,    "RAMCART Settings"},
/* de */ {IDS_RAMCART_SETTINGS_DE, "RAMCART Einstellungen"},
/* fr */ {IDS_RAMCART_SETTINGS_FR, "Parametres RAMCART"},
/* hu */ {IDS_RAMCART_SETTINGS_HU, "RAMCART be�ll�t�sai"},
/* it */ {IDS_RAMCART_SETTINGS_IT, "Impostazioni RAMCART"},
/* nl */ {IDS_RAMCART_SETTINGS_NL, "RAMCART instellingen"},
/* pl */ {IDS_RAMCART_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_RAMCART_SETTINGS_SV, "RAMCART-inst�llningar"},

/* en */ {IDS_REU_SIZE,    "REU Size"},
/* de */ {IDS_REU_SIZE_DE, "REU Gr��e"},
/* fr */ {IDS_REU_SIZE_FR, "Taille du REU"},
/* hu */ {IDS_REU_SIZE_HU, "REU m�rete"},
/* it */ {IDS_REU_SIZE_IT, "Dimensione REU"},
/* nl */ {IDS_REU_SIZE_NL, "REU grootte"},
/* pl */ {IDS_REU_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDS_REU_SIZE_SV, "Storlek pa minnesexpansion"},

/* en */ {IDS_REU_FILENAME,    "REU file"},
/* de */ {IDS_REU_FILENAME_DE, "REU Datei"},
/* fr */ {IDS_REU_FILENAME_FR, "Fichier REU"},
/* hu */ {IDS_REU_FILENAME_HU, "REU f�jl"},
/* it */ {IDS_REU_FILENAME_IT, "File REU"},
/* nl */ {IDS_REU_FILENAME_NL, "REU bestand"},
/* pl */ {IDS_REU_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_REU_FILENAME_SV, "REU-fil"},

/* en */ {IDS_REU_FILENAME_SELECT,    "Select file for REU"},
/* de */ {IDS_REU_FILENAME_SELECT_DE, "Datei f�r REU selektieren"},
/* fr */ {IDS_REU_FILENAME_SELECT_FR, "S�lectionner fichier pour REU"},
/* hu */ {IDS_REU_FILENAME_SELECT_HU, "V�lasszon f�jlt a REU-hoz"},
/* it */ {IDS_REU_FILENAME_SELECT_IT, "Seleziona il file per il REU"},
/* nl */ {IDS_REU_FILENAME_SELECT_NL, "Selecteer bestand voor REU"},
/* pl */ {IDS_REU_FILENAME_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_REU_FILENAME_SELECT_SV, "V�lj fil f�r REU"},

/* en */ {IDS_REU_SETTINGS,    "REU Settings"},
/* de */ {IDS_REU_SETTINGS_DE, "REU Einstellungen"},
/* fr */ {IDS_REU_SETTINGS_FR, "Parametres REU"},
/* hu */ {IDS_REU_SETTINGS_HU, "REU be�ll�t�sai"},
/* it */ {IDS_REU_SETTINGS_IT, "Impostazioni REU"},
/* nl */ {IDS_REU_SETTINGS_NL, "REU Instellingen"},
/* pl */ {IDS_REU_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_REU_SETTINGS_SV, "REU-Inst�llningar"},

/* en */ {IDS_USERPORT_RS232,    "Userport RS232"},
/* de */ {IDS_USERPORT_RS232_DE, "Userport RS232"},
/* fr */ {IDS_USERPORT_RS232_FR, "Port utilisateur RS232"},
/* hu */ {IDS_USERPORT_RS232_HU, "Userport RS232"},
/* it */ {IDS_USERPORT_RS232_IT, "Userport RS232"},
/* nl */ {IDS_USERPORT_RS232_NL, "Userport RS232"},
/* pl */ {IDS_USERPORT_RS232_PL, ""},  /* fuzzy */
/* sv */ {IDS_USERPORT_RS232_SV, "Anv�ndarportens RS232"},

/* en */ {IDS_USERPORT_DEVICE,    "Userport RS232 Device"},
/* de */ {IDS_USERPORT_DEVICE_DE, "Userport RS232 Ger�t"},
/* fr */ {IDS_USERPORT_DEVICE_FR, "P�riph�rique de port utilisateur RS232"},
/* hu */ {IDS_USERPORT_DEVICE_HU, "Userport RS232 eszk�z"},
/* it */ {IDS_USERPORT_DEVICE_IT, "Dispositivo Userport RS232"},
/* nl */ {IDS_USERPORT_DEVICE_NL, "Userport RS232 apparaat"},
/* pl */ {IDS_USERPORT_DEVICE_PL, ""},  /* fuzzy */
/* sv */ {IDS_USERPORT_DEVICE_SV, "Anv�ndarportens RS232-enhet"},

/* en */ {IDS_USERPORT_BAUD_RATE,    "Userport RS232 baud rate"},
/* de */ {IDS_USERPORT_BAUD_RATE_DE, "Userport RS232 Baud Rate"},
/* fr */ {IDS_USERPORT_BAUD_RATE_FR, "P�riph�rique de port utilisateur RS232"},
/* hu */ {IDS_USERPORT_BAUD_RATE_HU, "Userport RS232 �tviteli r�ta"},
/* it */ {IDS_USERPORT_BAUD_RATE_IT, "Velocit� della RS232 su Userport"},
/* nl */ {IDS_USERPORT_BAUD_RATE_NL, "Userport RS232 baudrate"},
/* pl */ {IDS_USERPORT_BAUD_RATE_PL, ""},  /* fuzzy */
/* sv */ {IDS_USERPORT_BAUD_RATE_SV, "Anv�ndarportens RS232-baudhastighet"},

/* en */ {IDS_RS232_USERPORT_SETTINGS,    "RS232 Userport Settings"},
/* de */ {IDS_RS232_USERPORT_SETTINGS_DE, "RS232 Userport Einstellungen"},
/* fr */ {IDS_RS232_USERPORT_SETTINGS_FR, "Parametres RS232 Userport"},
/* hu */ {IDS_RS232_USERPORT_SETTINGS_HU, "RS232 Userport be�ll�t�sai"},
/* it */ {IDS_RS232_USERPORT_SETTINGS_IT, "Impostazioni RS232 su userport"},
/* nl */ {IDS_RS232_USERPORT_SETTINGS_NL, "RS232 Userport Instellingen"},
/* pl */ {IDS_RS232_USERPORT_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_RS232_USERPORT_SETTINGS_SV, "RS232-anv�ndarportinst�llningar"},

/* en */ {IDS_FAST,    "fast"},
/* de */ {IDS_FAST_DE, "Schnell"},
/* fr */ {IDS_FAST_FR, "rapide"},
/* hu */ {IDS_FAST_HU, "gyors"},
/* it */ {IDS_FAST_IT, "veloce"},
/* nl */ {IDS_FAST_NL, "snel"},
/* pl */ {IDS_FAST_PL, ""},  /* fuzzy */
/* sv */ {IDS_FAST_SV, "snabbt"},

/* en */ {IDS_INTERPOLATING,    "interpolating"},
/* de */ {IDS_INTERPOLATING_DE, "Interpolierend"},
/* fr */ {IDS_INTERPOLATING_FR, "par interpolation"},
/* hu */ {IDS_INTERPOLATING_HU, "interpol�l�"},
/* it */ {IDS_INTERPOLATING_IT, "Interpolazione"},
/* nl */ {IDS_INTERPOLATING_NL, "interpolating"},
/* pl */ {IDS_INTERPOLATING_PL, ""},  /* fuzzy */
/* sv */ {IDS_INTERPOLATING_SV, "interpolerande"},

/* en */ {IDS_RESAMPLING,    "resampling"},
/* de */ {IDS_RESAMPLING_DE, "Resampling"},
/* fr */ {IDS_RESAMPLING_FR, "r��chantillonage"},
/* hu */ {IDS_RESAMPLING_HU, "�jramintav�telez�"},
/* it */ {IDS_RESAMPLING_IT, "ricampionamento"},
/* nl */ {IDS_RESAMPLING_NL, "resampling"},
/* pl */ {IDS_RESAMPLING_PL, ""},  /* fuzzy */
/* sv */ {IDS_RESAMPLING_SV, "omsamplande"},

/* en */ {IDS_FAST_RESAMPLING,    "fast resampling"},
/* de */ {IDS_FAST_RESAMPLING_DE, "Schnelles Resampling"},
/* fr */ {IDS_FAST_RESAMPLING_FR, "r��chantillonage rapide"},
/* hu */ {IDS_FAST_RESAMPLING_HU, "gyors �jramintav�telez�"},
/* it */ {IDS_FAST_RESAMPLING_IT, "ricampionamento veloce"},
/* nl */ {IDS_FAST_RESAMPLING_NL, "snelle resampling"},
/* pl */ {IDS_FAST_RESAMPLING_PL, ""},  /* fuzzy */
/* sv */ {IDS_FAST_RESAMPLING_SV, "snabb omsampling"},

/* en */ {IDS_6581_OLD,    "6581 (old)"},
/* de */ {IDS_6581_OLD_DE, "6581 (alt)"},
/* fr */ {IDS_6581_OLD_FR, "6581 (ancien)"},
/* hu */ {IDS_6581_OLD_HU, "6581 (r�gi)"},
/* it */ {IDS_6581_OLD_IT, "6581 (vecchio)"},
/* nl */ {IDS_6581_OLD_NL, "6581 (oud)"},
/* pl */ {IDS_6581_OLD_PL, ""},  /* fuzzy */
/* sv */ {IDS_6581_OLD_SV, "6581 (gammal)"},

/* en */ {IDS_8580_NEW,    "8580 (new)"},
/* de */ {IDS_8580_NEW_DE, "8580 (neu)"},
/* fr */ {IDS_8580_NEW_FR, "8580 (nouveau)"},
/* hu */ {IDS_8580_NEW_HU, "8580 (�j)"},
/* it */ {IDS_8580_NEW_IT, "8580 (nuovo)"},
/* nl */ {IDS_8580_NEW_NL, "8580 (nieuw)"},
/* pl */ {IDS_8580_NEW_PL, ""},  /* fuzzy */
/* sv */ {IDS_8580_NEW_SV, "8580 (ny)"},

/* en */ {IDS_SID_MODEL,    "SID Model"},
/* de */ {IDS_SID_MODEL_DE, "SID Modell"},
/* fr */ {IDS_SID_MODEL_FR, "Modele SID"},
/* hu */ {IDS_SID_MODEL_HU, "SID t�pus"},
/* it */ {IDS_SID_MODEL_IT, "Modello SID"},
/* nl */ {IDS_SID_MODEL_NL, "SID Model"},
/* pl */ {IDS_SID_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDS_SID_MODEL_SV, "SID-modell"},

/* en */ {IDS_SID_STEREO,    "SID Stereo"},
/* de */ {IDS_SID_STEREO_DE, "SID Stereo"},
/* fr */ {IDS_SID_STEREO_FR, "SID St�r�o"},
/* hu */ {IDS_SID_STEREO_HU, "SID sztere�"},
/* it */ {IDS_SID_STEREO_IT, "SID Stereo"},
/* nl */ {IDS_SID_STEREO_NL, "Stereo SID"},
/* pl */ {IDS_SID_STEREO_PL, ""},  /* fuzzy */
/* sv */ {IDS_SID_STEREO_SV, "SID-stereo"},

/* en */ {IDS_STEREO_SID_AT,    "Stereo SID at"},
/* de */ {IDS_STEREO_SID_AT_DE, "Stereo SID bei"},
/* fr */ {IDS_STEREO_SID_AT_FR, "SID St�r�o  "},
/* hu */ {IDS_STEREO_SID_AT_HU, "Sztere� SID itt:"},
/* it */ {IDS_STEREO_SID_AT_IT, "SID Stereo a"},
/* nl */ {IDS_STEREO_SID_AT_NL, "Stereo SID op"},
/* pl */ {IDS_STEREO_SID_AT_PL, ""},  /* fuzzy */
/* sv */ {IDS_STEREO_SID_AT_SV, "Stereo-SID pa"},

/* en */ {IDS_SID_FILTERS,    "SID Filters"},
/* de */ {IDS_SID_FILTERS_DE, "SID Filter"},
/* fr */ {IDS_SID_FILTERS_FR, "Filtres SID"},
/* hu */ {IDS_SID_FILTERS_HU, "SID sz�r�k"},
/* it */ {IDS_SID_FILTERS_IT, "Filtri SID"},
/* nl */ {IDS_SID_FILTERS_NL, "SID Filters"},
/* pl */ {IDS_SID_FILTERS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SID_FILTERS_SV, "SID-filter"},

/* en */ {IDS_SAMPLE_METHOD,    "Sample method"},
/* de */ {IDS_SAMPLE_METHOD_DE, "Sample Methode"},
/* fr */ {IDS_SAMPLE_METHOD_FR, "M�thode d'�chantillonnage"},
/* hu */ {IDS_SAMPLE_METHOD_HU, "Mintav�telez�s m�dja"},
/* it */ {IDS_SAMPLE_METHOD_IT, "Metodo di campionamento"},
/* nl */ {IDS_SAMPLE_METHOD_NL, "Sample methode"},
/* pl */ {IDS_SAMPLE_METHOD_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAMPLE_METHOD_SV, "Samplingsmetod"},

/* en */ {IDS_PASSBAND_0_90,    "Passband (0-90%)"},
/* de */ {IDS_PASSBAND_0_90_DE, "Passband (0-90%)"},
/* fr */ {IDS_PASSBAND_0_90_FR, "Bande passante (0-90%)"},
/* hu */ {IDS_PASSBAND_0_90_HU, "�tereszt�s�g (0-90%)"},
/* it */ {IDS_PASSBAND_0_90_IT, "Passabanda (0-90%)"},
/* nl */ {IDS_PASSBAND_0_90_NL, "Passband (0-90%)"},
/* pl */ {IDS_PASSBAND_0_90_PL, ""},  /* fuzzy */
/* sv */ {IDS_PASSBAND_0_90_SV, "Passband (0-90%)"},

/* en */ {IDS_NOT_IMPLEMENTED_YET,    "Not implemented yet!"},
/* de */ {IDS_NOT_IMPLEMENTED_YET_DE, "Noch nicht implementiert!"},
/* fr */ {IDS_NOT_IMPLEMENTED_YET_FR, ""},  /* fuzzy */
/* hu */ {IDS_NOT_IMPLEMENTED_YET_HU, "Nincs m�g implement�lva!"},
/* it */ {IDS_NOT_IMPLEMENTED_YET_IT, "Non ancora implementato!"},
/* nl */ {IDS_NOT_IMPLEMENTED_YET_NL, "Nog niet ondersteund!"},
/* pl */ {IDS_NOT_IMPLEMENTED_YET_PL, ""},  /* fuzzy */
/* sv */ {IDS_NOT_IMPLEMENTED_YET_SV, "Ej implementerat!"},

/* en */ {IDS_SID_SETTINGS,    "SID Settings"},
/* de */ {IDS_SID_SETTINGS_DE, "SID Einstellungen"},
/* fr */ {IDS_SID_SETTINGS_FR, "Parametres SID"},
/* hu */ {IDS_SID_SETTINGS_HU, "SID be�ll�t�sai"},
/* it */ {IDS_SID_SETTINGS_IT, "Impostazioni SID"},
/* nl */ {IDS_SID_SETTINGS_NL, "SID instellingen"},
/* pl */ {IDS_SID_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SID_SETTINGS_SV, "SID-inst�llningar"},

/* en */ {IDS_FLEXIBLE,    "Flexible"},
/* de */ {IDS_FLEXIBLE_DE, "Flexibel"},
/* fr */ {IDS_FLEXIBLE_FR, "Flexible"},
/* hu */ {IDS_FLEXIBLE_HU, "Rugalmas"},
/* it */ {IDS_FLEXIBLE_IT, "Flessibile"},
/* nl */ {IDS_FLEXIBLE_NL, "Flexibel"},
/* pl */ {IDS_FLEXIBLE_PL, ""},  /* fuzzy */
/* sv */ {IDS_FLEXIBLE_SV, "Flexibel"},

/* en */ {IDS_ADJUSTING,    "Adjusting"},
/* de */ {IDS_ADJUSTING_DE, "Anpassend"},
/* fr */ {IDS_ADJUSTING_FR, "Ajust�"},
/* hu */ {IDS_ADJUSTING_HU, "Igazod�"},
/* it */ {IDS_ADJUSTING_IT, "Adattamento in corso"},
/* nl */ {IDS_ADJUSTING_NL, "Aanpassend"},
/* pl */ {IDS_ADJUSTING_PL, ""},  /* fuzzy */
/* sv */ {IDS_ADJUSTING_SV, "Anpassande"},

/* en */ {IDS_EXACT,    "Exact"},
/* de */ {IDS_EXACT_DE, "Exakt"},
/* fr */ {IDS_EXACT_FR, "Exact"},
/* hu */ {IDS_EXACT_HU, "Pontos"},
/* it */ {IDS_EXACT_IT, "Esatta"},
/* nl */ {IDS_EXACT_NL, "Exact"},
/* pl */ {IDS_EXACT_PL, ""},  /* fuzzy */
/* sv */ {IDS_EXACT_SV, "Exakt"},

/* en */ {IDS_SAMPLE_RATE,    "Sample Rate"},
/* de */ {IDS_SAMPLE_RATE_DE, "Sample Rate"},
/* fr */ {IDS_SAMPLE_RATE_FR, "Taux d'�chantillonage"},
/* hu */ {IDS_SAMPLE_RATE_HU, "Mintav�telez�si r�ta"},
/* it */ {IDS_SAMPLE_RATE_IT, "Frequenza di campionamento"},
/* nl */ {IDS_SAMPLE_RATE_NL, "Sample snelheid"},
/* pl */ {IDS_SAMPLE_RATE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAMPLE_RATE_SV, "Samplingshastighet"},

/* en */ {IDS_BUFFER_SIZE,    "Buffer Size"},
/* de */ {IDS_BUFFER_SIZE_DE, "Puffergr��e"},
/* fr */ {IDS_BUFFER_SIZE_FR, "Taille du tampon"},
/* hu */ {IDS_BUFFER_SIZE_HU, "Puffer m�ret"},
/* it */ {IDS_BUFFER_SIZE_IT, "Dimensione del buffer"},
/* nl */ {IDS_BUFFER_SIZE_NL, "Buffer grootte"},
/* pl */ {IDS_BUFFER_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDS_BUFFER_SIZE_SV, "Buffertstorlek"},

/* en */ {IDS_OVERSAMPLE,    "Oversample"},
/* de */ {IDS_OVERSAMPLE_DE, "Oversample"},
/* fr */ {IDS_OVERSAMPLE_FR, "Sur�chantillonage"},
/* hu */ {IDS_OVERSAMPLE_HU, "T�lmintav�telez�s"},
/* it */ {IDS_OVERSAMPLE_IT, "Sovracampionamento"},
/* nl */ {IDS_OVERSAMPLE_NL, "Oversample"},
/* pl */ {IDS_OVERSAMPLE_PL, ""},  /* fuzzy */
/* sv */ {IDS_OVERSAMPLE_SV, "-versampling"},

/* en */ {IDS_SPEED_ADJUSTMENT,    "Speed Adjustment"},
/* de */ {IDS_SPEED_ADJUSTMENT_DE, "Geschwindigkeitsanpassung"},
/* fr */ {IDS_SPEED_ADJUSTMENT_FR, ""},  /* fuzzy */
/* hu */ {IDS_SPEED_ADJUSTMENT_HU, "Sebess�g m�dos�t�s"},
/* it */ {IDS_SPEED_ADJUSTMENT_IT, "Variazione velocit�"},
/* nl */ {IDS_SPEED_ADJUSTMENT_NL, "Snelheid aanpassing"},
/* pl */ {IDS_SPEED_ADJUSTMENT_PL, ""},  /* fuzzy */
/* sv */ {IDS_SPEED_ADJUSTMENT_SV, "Hastighetsjustering"},

/* en */ {IDS_SOUND_SETTINGS,    "Sound Settings"},
/* de */ {IDS_SOUND_SETTINGS_DE, "Sound Einstellungen"},
/* fr */ {IDS_SOUND_SETTINGS_FR, "Parametres son"},
/* hu */ {IDS_SOUND_SETTINGS_HU, "Hang be�ll�t�sai"},
/* it */ {IDS_SOUND_SETTINGS_IT, "Impostazioni audio"},
/* nl */ {IDS_SOUND_SETTINGS_NL, "Geluid instellingen"},
/* pl */ {IDS_SOUND_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SOUND_SETTINGS_SV, "Ljudinst�llningar"},

/* en */ {IDS_VIC_SETTINGS,    "VIC Settings"},
/* de */ {IDS_VIC_SETTINGS_DE, "VIC Einstellungen"},
/* fr */ {IDS_VIC_SETTINGS_FR, "Parametres VIC"},
/* hu */ {IDS_VIC_SETTINGS_HU, "VIC be�ll�t�sai"},
/* it */ {IDS_VIC_SETTINGS_IT, "Impostazioni VIC"},
/* nl */ {IDS_VIC_SETTINGS_NL, "VIC instellingen"},
/* pl */ {IDS_VIC_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_VIC_SETTINGS_SV, "VIC-inst�llningar"},

/* en */ {IDS_SPRITE_SPRITE_COL,    "Sprite Sprite Collisions"},
/* de */ {IDS_SPRITE_SPRITE_COL_DE, "Sprite-sprite Kollisionen"},
/* fr */ {IDS_SPRITE_SPRITE_COL_FR, "Collisions sprite-sprite"},
/* hu */ {IDS_SPRITE_SPRITE_COL_HU, "Sprite-sprite �tk�z�sek"},
/* it */ {IDS_SPRITE_SPRITE_COL_IT, "Collisioni sprite-sprite"},
/* nl */ {IDS_SPRITE_SPRITE_COL_NL, "Sprite-sprite botsingen"},
/* pl */ {IDS_SPRITE_SPRITE_COL_PL, ""},  /* fuzzy */
/* sv */ {IDS_SPRITE_SPRITE_COL_SV, "Sprite-sprite-kollisioner"},

/* en */ {IDS_SPRITE_BACKGROUND_COL,    "Sprite Background Collisions"},
/* de */ {IDS_SPRITE_BACKGROUND_COL_DE, "Sprite-Hintergrund Kollisionen"},
/* fr */ {IDS_SPRITE_BACKGROUND_COL_FR, "Collisions sprite-arriere-plan"},
/* hu */ {IDS_SPRITE_BACKGROUND_COL_HU, "Sprite-h�tt�r �tk�z�sek"},
/* it */ {IDS_SPRITE_BACKGROUND_COL_IT, "Collisioni sprite-sfondo"},
/* nl */ {IDS_SPRITE_BACKGROUND_COL_NL, "Sprite-achtergrond botsingen"},
/* pl */ {IDS_SPRITE_BACKGROUND_COL_PL, ""},  /* fuzzy */
/* sv */ {IDS_SPRITE_BACKGROUND_COL_SV, "Sprite-bakgrundkollisioner"},

/* en */ {IDS_NEW_LUMINANCES,    "New Luminances"},
/* de */ {IDS_NEW_LUMINANCES_DE, "Neue Helligkeitsemulation"},
/* fr */ {IDS_NEW_LUMINANCES_FR, "Nouvelles luminescences"},
/* hu */ {IDS_NEW_LUMINANCES_HU, "�j f�nyer�k"},
/* it */ {IDS_NEW_LUMINANCES_IT, "Nuove luminanze"},
/* nl */ {IDS_NEW_LUMINANCES_NL, "Nieuwe kleuren"},
/* pl */ {IDS_NEW_LUMINANCES_PL, ""},  /* fuzzy */
/* sv */ {IDS_NEW_LUMINANCES_SV, "Nya ljusstyrkor"},

/* en */ {IDS_VICII_SETTINGS,    "VIC-II Settings"},
/* de */ {IDS_VICII_SETTINGS_DE, "VIC-II Einstellungen"},
/* fr */ {IDS_VICII_SETTINGS_FR, "Parametres VIC-II"},
/* hu */ {IDS_VICII_SETTINGS_HU, "VIC-II be�ll�t�sai"},
/* it */ {IDS_VICII_SETTINGS_IT, "Impostazioni VIC-II"},
/* nl */ {IDS_VICII_SETTINGS_NL, "VIC-II Settings"},
/* pl */ {IDS_VICII_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_VICII_SETTINGS_SV, "VIC-II-inst�llningar"},

/* en */ {IDS_ATTACH_TAPE_IMAGE,    "Attach tape image"},
/* de */ {IDS_ATTACH_TAPE_IMAGE_DE, "Band Image einlegen"},
/* fr */ {IDS_ATTACH_TAPE_IMAGE_FR, "Ins�rer une image datassette"},
/* hu */ {IDS_ATTACH_TAPE_IMAGE_HU, "Szalag k�pm�s csatol�sa"},
/* it */ {IDS_ATTACH_TAPE_IMAGE_IT, "Seleziona l'immagine di una cassetta"},
/* nl */ {IDS_ATTACH_TAPE_IMAGE_NL, "Koppel tape bestand"},
/* pl */ {IDS_ATTACH_TAPE_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_TAPE_IMAGE_SV, "Anslut en bandavbildningsfil"},

/* en */ {IDS_AUTOSTART_IMAGE,    "Autostart disk/tape image"},
/* de */ {IDS_AUTOSTART_IMAGE_DE, "Autostart Disk/Band Image"},
/* fr */ {IDS_AUTOSTART_IMAGE_FR, "D�marrage automatique d'une image datassette"},
/* hu */ {IDS_AUTOSTART_IMAGE_HU, "Lemez/szalag k�pm�s automatikus ind�t�sa"},
/* it */ {IDS_AUTOSTART_IMAGE_IT, "Avvia automaticamente l'immagine di un disco/cassetta"},
/* nl */ {IDS_AUTOSTART_IMAGE_NL, "Autostart disk/tape bestand"},
/* pl */ {IDS_AUTOSTART_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_AUTOSTART_IMAGE_SV, "Autostarta disk-/bandavbildningsfil"},

/* en */ {IDS_IDE64_FILENAME,    "HD image file"},
/* de */ {IDS_IDE64_FILENAME_DE, "HD Image Name"},
/* fr */ {IDS_IDE64_FILENAME_FR, "Nom d'image DD"},
/* hu */ {IDS_IDE64_FILENAME_HU, "Merevlemez k�pm�s f�jl"},
/* it */ {IDS_IDE64_FILENAME_IT, "File immagine dell'HD"},
/* nl */ {IDS_IDE64_FILENAME_NL, "HD bestand"},
/* pl */ {IDS_IDE64_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_IDE64_FILENAME_SV, "HD-avbildningsfil"},

/* en */ {IDS_IDE64_FILENAME_SELECT,    "Select HD image file"},
/* de */ {IDS_IDE64_FILENAME_SELECT_DE, "HD Image Datei selektieren"},
/* fr */ {IDS_IDE64_FILENAME_SELECT_FR, "S�lectionnez le fichier image du D.D."},
/* hu */ {IDS_IDE64_FILENAME_SELECT_HU, "V�lassza ki a merevlemez k�pm�s f�jlt"},
/* it */ {IDS_IDE64_FILENAME_SELECT_IT, "Seleziona il file immagine dell'HD"},
/* nl */ {IDS_IDE64_FILENAME_SELECT_NL, "Selecteer HD bestand"},
/* pl */ {IDS_IDE64_FILENAME_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_IDE64_FILENAME_SELECT_SV, "Ange HD-avbildningsfil"},

/* en */ {IDS_AUTODETECT,    "Autodetect"},
/* de */ {IDS_AUTODETECT_DE, "Automatische Erkennung"},
/* fr */ {IDS_AUTODETECT_FR, "Autod�tection"},
/* hu */ {IDS_AUTODETECT_HU, "Automatikus �rz�kel�s"},
/* it */ {IDS_AUTODETECT_IT, "Rileva automaticamente"},
/* nl */ {IDS_AUTODETECT_NL, "Autodetectie"},
/* pl */ {IDS_AUTODETECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_AUTODETECT_SV, "Autodetektera"},

/* en */ {IDS_CYLINDERS,    "Cylinders"},
/* de */ {IDS_CYLINDERS_DE, "Zylinder"},
/* fr */ {IDS_CYLINDERS_FR, "Cylindres"},
/* hu */ {IDS_CYLINDERS_HU, "Cilinderek"},
/* it */ {IDS_CYLINDERS_IT, "Cilindri"},
/* nl */ {IDS_CYLINDERS_NL, "Cylinders"},
/* pl */ {IDS_CYLINDERS_PL, ""},  /* fuzzy */
/* sv */ {IDS_CYLINDERS_SV, "Cylindrar"},

/* en */ {IDS_HEADS,    "Heads"},
/* de */ {IDS_HEADS_DE, "K�pfe"},
/* fr */ {IDS_HEADS_FR, "T�tes"},
/* hu */ {IDS_HEADS_HU, "Fejek"},
/* it */ {IDS_HEADS_IT, "Testine"},
/* nl */ {IDS_HEADS_NL, "Koppen"},
/* pl */ {IDS_HEADS_PL, ""},  /* fuzzy */
/* sv */ {IDS_HEADS_SV, "Huvuden"},

/* en */ {IDS_SECTORS,    "Sectors"},
/* de */ {IDS_SECTORS_DE, "Sektoren"},
/* fr */ {IDS_SECTORS_FR, "Secteurs"},
/* hu */ {IDS_SECTORS_HU, "Szektorok"},
/* it */ {IDS_SECTORS_IT, "Settori"},
/* nl */ {IDS_SECTORS_NL, "Sectors"},
/* pl */ {IDS_SECTORS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SECTORS_SV, "Sektorer"},

/* en */ {IDS_IDE64_SETTINGS,    "IDE64 settings"},
/* de */ {IDS_IDE64_SETTINGS_DE, "IDE64 Einstellungen"},
/* fr */ {IDS_IDE64_SETTINGS_FR, "Param�tres IDE64"},
/* hu */ {IDS_IDE64_SETTINGS_HU, "IDE64 be�ll�t�sai"},
/* it */ {IDS_IDE64_SETTINGS_IT, "Impostazioni IDE64"},
/* nl */ {IDS_IDE64_SETTINGS_NL, "IDE64 instellingen"},
/* pl */ {IDS_IDE64_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_IDE64_SETTINGS_SV, "IDE64-inst�llningar"},

/* en */ {IDS_BIOS_REVISION,    "BIOS Revision"},
/* de */ {IDS_BIOS_REVISION_DE, "BIOS Revision"},
/* fr */ {IDS_BIOS_REVISION_FR, ""},  /* fuzzy */
/* hu */ {IDS_BIOS_REVISION_HU, "BIOS v�ltozat"},
/* it */ {IDS_BIOS_REVISION_IT, "Revisione BIOS"},
/* nl */ {IDS_BIOS_REVISION_NL, "Revisie"},
/* pl */ {IDS_BIOS_REVISION_PL, ""},  /* fuzzy */
/* sv */ {IDS_BIOS_REVISION_SV, "BIOS-utg�va"},

/* en */ {IDS_BIOS_FLASH_JUMPER,    "BIOS Flash Jumper"},
/* de */ {IDS_BIOS_FLASH_JUMPER_DE, "BIOS Flashjumper"},
/* fr */ {IDS_BIOS_FLASH_JUMPER_FR, ""},  /* fuzzy */
/* hu */ {IDS_BIOS_FLASH_JUMPER_HU, "BIOS Flash jumper"},
/* it */ {IDS_BIOS_FLASH_JUMPER_IT, "Flash Jumper del BIOS"},
/* nl */ {IDS_BIOS_FLASH_JUMPER_NL, "BIOS Flash Jumper"},
/* pl */ {IDS_BIOS_FLASH_JUMPER_PL, ""},  /* fuzzy */
/* sv */ {IDS_BIOS_FLASH_JUMPER_SV, "BIOS-flashbygel"},

/* en */ {IDS_SAVE_BIOS_WHEN_CHANGED,    "Save BIOS when changed"},
/* de */ {IDS_SAVE_BIOS_WHEN_CHANGED_DE, "BIOS bei �nderung speichern"},
/* fr */ {IDS_SAVE_BIOS_WHEN_CHANGED_FR, ""},  /* fuzzy */
/* hu */ {IDS_SAVE_BIOS_WHEN_CHANGED_HU, "BIOS ment�se v�ltoz�s ut�n"},
/* it */ {IDS_SAVE_BIOS_WHEN_CHANGED_IT, "Salva il BIOS al cambio"},
/* nl */ {IDS_SAVE_BIOS_WHEN_CHANGED_NL, "BIOS Opslaan indien gewijzigd"},
/* pl */ {IDS_SAVE_BIOS_WHEN_CHANGED_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_BIOS_WHEN_CHANGED_SV, "Spara BIOS vid �ndring"},

/* en */ {IDS_BIOS_FILE,    "BIOS File"},
/* de */ {IDS_BIOS_FILE_DE, "BIOS Datei"},
/* fr */ {IDS_BIOS_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_BIOS_FILE_HU, "BIOS f�jl"},
/* it */ {IDS_BIOS_FILE_IT, "File del BIOS"},
/* nl */ {IDS_BIOS_FILE_NL, "BIOS Bestand"},
/* pl */ {IDS_BIOS_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_BIOS_FILE_SV, "BIOS-fil"},

/* en */ {IDS_SELECT_BIOS_FILE,    "Select BIOS file"},
/* de */ {IDS_SELECT_BIOS_FILE_DE, "BIOS Datei w�hlen"},
/* fr */ {IDS_SELECT_BIOS_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_BIOS_FILE_HU, "V�lasszon BIOS f�jlt"},
/* it */ {IDS_SELECT_BIOS_FILE_IT, "Seleziona il file del BIOS"},
/* nl */ {IDS_SELECT_BIOS_FILE_NL, "Selecteer BIOS bestand"},
/* pl */ {IDS_SELECT_BIOS_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_BIOS_FILE_SV, "V�lj BIOS-fil"},

/* en */ {IDS_MMC64_IMAGE_READ_ONLY,    "MMC/SD Image read-only"},
/* de */ {IDS_MMC64_IMAGE_READ_ONLY_DE, "MMC/SD Image Schreibschutz"},
/* fr */ {IDS_MMC64_IMAGE_READ_ONLY_FR, ""},  /* fuzzy */
/* hu */ {IDS_MMC64_IMAGE_READ_ONLY_HU, "MMC/SD k�pm�s csak olvashat�"},
/* it */ {IDS_MMC64_IMAGE_READ_ONLY_IT, "Immagine MMC/SD in sola lettura"},
/* nl */ {IDS_MMC64_IMAGE_READ_ONLY_NL, "MMC/SD Bestand alleen-lezen"},
/* pl */ {IDS_MMC64_IMAGE_READ_ONLY_PL, ""},  /* fuzzy */
/* sv */ {IDS_MMC64_IMAGE_READ_ONLY_SV, "MMC/SD-avbildning skrivskyddad"},

/* en */ {IDS_MMC64_IMAGE_FILE,    "MMC/SD Image File"},
/* de */ {IDS_MMC64_IMAGE_FILE_DE, "MMC/SD Image Name"},
/* fr */ {IDS_MMC64_IMAGE_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_MMC64_IMAGE_FILE_HU, "MMC/SD k�pm�s f�jl"},
/* it */ {IDS_MMC64_IMAGE_FILE_IT, "File immagine MMC/SD"},
/* nl */ {IDS_MMC64_IMAGE_FILE_NL, "MMC/SD Bestand"},
/* pl */ {IDS_MMC64_IMAGE_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_MMC64_IMAGE_FILE_SV, "MMC/SD-avbildningsfil"},

/* en */ {IDS_SELECT_MMC64_IMAGE_FILE,    "Select MMC/SD image file"},
/* de */ {IDS_SELECT_MMC64_IMAGE_FILE_DE, "MMC/SD Image Date w�hlen"},
/* fr */ {IDS_SELECT_MMC64_IMAGE_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_MMC64_IMAGE_FILE_HU, "V�lassza ki az MMC/SD k�pm�s f�jlt"},
/* it */ {IDS_SELECT_MMC64_IMAGE_FILE_IT, "Seleziona il file immagine MMC/SD"},
/* nl */ {IDS_SELECT_MMC64_IMAGE_FILE_NL, "Selecteer MMC/SD bestand"},
/* pl */ {IDS_SELECT_MMC64_IMAGE_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_MMC64_IMAGE_FILE_SV, "Ange MMC/SD-avbildningsfil"},

/* en */ {IDS_MMC64_SETTINGS,    "MMC64 settings"},
/* de */ {IDS_MMC64_SETTINGS_DE, "MMC64 Einstellungen"},
/* fr */ {IDS_MMC64_SETTINGS_FR, "Param�tres MMC64"},
/* hu */ {IDS_MMC64_SETTINGS_HU, "MMC64 be�ll�t�sai"},
/* it */ {IDS_MMC64_SETTINGS_IT, "Impostazioni MMC64"},
/* nl */ {IDS_MMC64_SETTINGS_NL, "MMC64 instellingen"},
/* pl */ {IDS_MMC64_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_MMC64_SETTINGS_SV, "MMC64-inst�llningar"},

/* en */ {IDS_SID_CART,    "SID Cart"},
/* de */ {IDS_SID_CART_DE, "SID Modul"},
/* fr */ {IDS_SID_CART_FR, ""},  /* fuzzy */
/* hu */ {IDS_SID_CART_HU, "SID k�rtya"},
/* it */ {IDS_SID_CART_IT, "Cartuccia SID"},
/* nl */ {IDS_SID_CART_NL, "SID Cart"},
/* pl */ {IDS_SID_CART_PL, ""},  /* fuzzy */
/* sv */ {IDS_SID_CART_SV, "SID-modul"},

/* en */ {IDS_SID_ENGINE,    "SID Engine"},
/* de */ {IDS_SID_ENGINE_DE, "SID Engine"},
/* fr */ {IDS_SID_ENGINE_FR, "Engin SID"},
/* hu */ {IDS_SID_ENGINE_HU, "SID Engine"},
/* it */ {IDS_SID_ENGINE_IT, "Motore SID"},
/* nl */ {IDS_SID_ENGINE_NL, "SID Kern"},
/* pl */ {IDS_SID_ENGINE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SID_ENGINE_SV, "SID-motor"},

/* en */ {IDS_SID_ADDRESS,    "SID Address"},
/* de */ {IDS_SID_ADDRESS_DE, "SID Adresse"},
/* fr */ {IDS_SID_ADDRESS_FR, ""},  /* fuzzy */
/* hu */ {IDS_SID_ADDRESS_HU, "SID c�m"},
/* it */ {IDS_SID_ADDRESS_IT, "Indirizzo SID"},
/* nl */ {IDS_SID_ADDRESS_NL, "SID Adres"},
/* pl */ {IDS_SID_ADDRESS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SID_ADDRESS_SV, "SID-adress"},

/* en */ {IDS_SID_CLOCK,    "SID Clock"},
/* de */ {IDS_SID_CLOCK_DE, "SID Uhr"},
/* fr */ {IDS_SID_CLOCK_FR, ""},  /* fuzzy */
/* hu */ {IDS_SID_CLOCK_HU, "SID �rajel"},
/* it */ {IDS_SID_CLOCK_IT, "SID Clock"},
/* nl */ {IDS_SID_CLOCK_NL, "SID Klok"},
/* pl */ {IDS_SID_CLOCK_PL, ""},  /* fuzzy */
/* sv */ {IDS_SID_CLOCK_SV, "SID-klocka"},

/* en */ {IDS_SIDCART_SETTINGS,    "SID cart settings"},
/* de */ {IDS_SIDCART_SETTINGS_DE, "SID Modul Einstellungen"},
/* fr */ {IDS_SIDCART_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_SIDCART_SETTINGS_HU, "SID k�rtya be�ll�t�sai..."},
/* it */ {IDS_SIDCART_SETTINGS_IT, "Impostazioni cartuccia SID"},
/* nl */ {IDS_SIDCART_SETTINGS_NL, "SID cart instellingen"},
/* pl */ {IDS_SIDCART_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SIDCART_SETTINGS_SV, "SID-modulinst�llningarg"},

/* en */ {IDS_LOAD_FLIP_LIST_FILE,    "Load flip list file"},
/* de */ {IDS_LOAD_FLIP_LIST_FILE_DE, "Fliplist Datei Laden"},
/* fr */ {IDS_LOAD_FLIP_LIST_FILE_FR, "Charger un fichier de groupement de disques"},
/* hu */ {IDS_LOAD_FLIP_LIST_FILE_HU, "Lemezlista bet�lt�se"},
/* it */ {IDS_LOAD_FLIP_LIST_FILE_IT, "Carica il file della fliplist"},
/* nl */ {IDS_LOAD_FLIP_LIST_FILE_NL, "Laad flip lijst bestand"},
/* pl */ {IDS_LOAD_FLIP_LIST_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_LOAD_FLIP_LIST_FILE_SV, "L�s vallistefil"},

/* en */ {IDS_CANNOT_READ_FLIP_LIST,    "Cannot read flip list file"},
/* de */ {IDS_CANNOT_READ_FLIP_LIST_DE, "Lesen von Fliplist Datei fehlgeschlagen"},
/* fr */ {IDS_CANNOT_READ_FLIP_LIST_FR, ""},  /* fuzzy */
/* hu */ {IDS_CANNOT_READ_FLIP_LIST_HU, "Nem lehet olvasni a lemezlista f�jlt"},
/* it */ {IDS_CANNOT_READ_FLIP_LIST_IT, "Non � possibile caricare il file della flip list"},
/* nl */ {IDS_CANNOT_READ_FLIP_LIST_NL, "Kan het flip lijst bestand niet lezen"},
/* pl */ {IDS_CANNOT_READ_FLIP_LIST_PL, ""},  /* fuzzy */
/* sv */ {IDS_CANNOT_READ_FLIP_LIST_SV, "Kunde inte l�sa vallistefil"},

/* en */ {IDS_SAVE_FLIP_LIST_FILE,    "Save flip list file"},
/* de */ {IDS_SAVE_FLIP_LIST_FILE_DE, "Fliplist Datei Speichern"},
/* fr */ {IDS_SAVE_FLIP_LIST_FILE_FR, "Enregistrer le fichier de groupement de disques actuel"},
/* hu */ {IDS_SAVE_FLIP_LIST_FILE_HU, "Lemezlista ment�se f�jlba"},
/* it */ {IDS_SAVE_FLIP_LIST_FILE_IT, "Salva il file della fliplist"},
/* nl */ {IDS_SAVE_FLIP_LIST_FILE_NL, "Flip lijst bestand opslaan"},
/* pl */ {IDS_SAVE_FLIP_LIST_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_FLIP_LIST_FILE_SV, "Spara vallistefil"},

/* en */ {IDS_CANNOT_WRITE_FLIP_LIST,    "Cannot write flip list file"},
/* de */ {IDS_CANNOT_WRITE_FLIP_LIST_DE, "Schreiben von Fliplist Datei fehlgeschlagen"},
/* fr */ {IDS_CANNOT_WRITE_FLIP_LIST_FR, "Impossible d'�crire le fichier de groupement de disques"},
/* hu */ {IDS_CANNOT_WRITE_FLIP_LIST_HU, "Nem siker�lt a lemezlist�t f�jlba �rni"},
/* it */ {IDS_CANNOT_WRITE_FLIP_LIST_IT, "Non � possibile scrivere il file della flip list"},
/* nl */ {IDS_CANNOT_WRITE_FLIP_LIST_NL, "Kan flip lijst bestand niet schrijven"},
/* pl */ {IDS_CANNOT_WRITE_FLIP_LIST_PL, ""},  /* fuzzy */
/* sv */ {IDS_CANNOT_WRITE_FLIP_LIST_SV, "Kan inte skriva vallistefil"},

/* en */ {IDS_ATTACH_FUNCTION_LOW_CART,    "Attach cartridge image for Function Low"},
/* de */ {IDS_ATTACH_FUNCTION_LOW_CART_DE, "Funktions ROM (low) Erweiterungsmodul einlegen"},
/* fr */ {IDS_ATTACH_FUNCTION_LOW_CART_FR, "Ins�rer une cartouche Fonction basse"},
/* hu */ {IDS_ATTACH_FUNCTION_LOW_CART_HU, "Cartridge k�pm�s csatol�sa Function Low-hoz"},
/* it */ {IDS_ATTACH_FUNCTION_LOW_CART_IT, "Seleziona l'immagine di una cartuccia per la Function Low"},
/* nl */ {IDS_ATTACH_FUNCTION_LOW_CART_NL, "Koppel cartridge bestand voor 'Function Low'"},
/* pl */ {IDS_ATTACH_FUNCTION_LOW_CART_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_FUNCTION_LOW_CART_SV, "Anslut insticksmodulfil f�r funktion l�g"},

/* en */ {IDS_ATTACH_FUNCTION_HIGH_CART,    "Attach cartridge image for Function High"},
/* de */ {IDS_ATTACH_FUNCTION_HIGH_CART_DE, "Funktions ROM (high) Erweiterungsmodul einlegen"},
/* fr */ {IDS_ATTACH_FUNCTION_HIGH_CART_FR, "Ins�rer une cartouche Fonction haute"},
/* hu */ {IDS_ATTACH_FUNCTION_HIGH_CART_HU, "Cartridge k�pm�s csatol�sa Function High-hez"},
/* it */ {IDS_ATTACH_FUNCTION_HIGH_CART_IT, "Seleziona l'immagine di una cartuccia per la Function High"},
/* nl */ {IDS_ATTACH_FUNCTION_HIGH_CART_NL, "Koppel cartridge bestand voor 'Function High'"},
/* pl */ {IDS_ATTACH_FUNCTION_HIGH_CART_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_FUNCTION_HIGH_CART_SV, "Anslut insticksmodulfil f�r funktion h�g"},

/* en */ {IDS_ATTACH_CART1_LOW,    "Attach cartridge image for Cartridge1 Low"},
/* de */ {IDS_ATTACH_CART1_LOW_DE, "Cartridge1 (low) Erweiterungsmodul einlegen"},
/* fr */ {IDS_ATTACH_CART1_LOW_FR, "Ins�rer une cartouche pour Cartouche1 basse"},
/* hu */ {IDS_ATTACH_CART1_LOW_HU, "Cartridge k�pm�s csatol�sa als� Cartridge1-hez"},
/* it */ {IDS_ATTACH_CART1_LOW_IT, "Seleziona l'immagine di una cartuccia per la Cartridge1 Low"},
/* nl */ {IDS_ATTACH_CART1_LOW_NL, "Koppel cartridge bestand voor 'Cartridge1 Low'"},
/* pl */ {IDS_ATTACH_CART1_LOW_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_CART1_LOW_SV, "Anslut insticksmodulfil f�r modul1 l�g"},

/* en */ {IDS_ATTACH_CART1_HIGH,    "Attach cartridge image for Cartridge1 High"},
/* de */ {IDS_ATTACH_CART1_HIGH_DE, "Cartridge1 (high) Erweiterungsmodul einlegen"},
/* fr */ {IDS_ATTACH_CART1_HIGH_FR, "Ins�rer une cartouche pour Cartouche1 haute"},
/* hu */ {IDS_ATTACH_CART1_HIGH_HU, "Cartridge k�pm�s csatol�sa fels� Cartridge1-hez"},
/* it */ {IDS_ATTACH_CART1_HIGH_IT, "Seleziona l'immagine di una cartuccia per la Cartridge1 High"},
/* nl */ {IDS_ATTACH_CART1_HIGH_NL, "Koppel cartridge bestand voor 'Cartridge1 High'"},
/* pl */ {IDS_ATTACH_CART1_HIGH_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_CART1_HIGH_SV, "Anslut insticksmodulfil f�r modul1 h�g"},

/* en */ {IDS_ATTACH_CART2_LOW,    "Attach cartridge image for Cartridge2 Low"},
/* de */ {IDS_ATTACH_CART2_LOW_DE, "Cartridge2 (low) Erweiterungsmodul einlegen"},
/* fr */ {IDS_ATTACH_CART2_LOW_FR, "Ins�rer une cartouche pour Cartouche2 basse"},
/* hu */ {IDS_ATTACH_CART2_LOW_HU, "Cartridge k�pm�s csatol�sa als� Cartridge2-h�z"},
/* it */ {IDS_ATTACH_CART2_LOW_IT, "Seleziona l'immagine di una cartuccia per la Cartridge2 Low"},
/* nl */ {IDS_ATTACH_CART2_LOW_NL, "Koppel cartridge bestand voor 'Cartridge2 Low'"},
/* pl */ {IDS_ATTACH_CART2_LOW_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_CART2_LOW_SV, "Anslut insticksmodulfil f�r modul2 l�g"},

/* en */ {IDS_ATTACH_CART2_HIGH,    "Attach cartridge image for Cartridge2 High"},
/* de */ {IDS_ATTACH_CART2_HIGH_DE, "Cartridge2 (high) Erweiterungsmodul einlegen"},
/* fr */ {IDS_ATTACH_CART2_HIGH_FR, "Ins�rer une cartouche pour Cartouche2 haute"},
/* hu */ {IDS_ATTACH_CART2_HIGH_HU, "Cartridge k�pm�s csatol�sa fels� Cartridge2-h�z"},
/* it */ {IDS_ATTACH_CART2_HIGH_IT, "Seleziona l'immagine di una cartuccia per la Cartridge2 High"},
/* nl */ {IDS_ATTACH_CART2_HIGH_NL, "Koppel cartridge bestand voor 'Cartridge2 High'"},
/* pl */ {IDS_ATTACH_CART2_HIGH_PL, ""},  /* fuzzy */
/* sv */ {IDS_ATTACH_CART2_HIGH_SV, "Anslut insticksmodulfil f�r modul2 h�g"},

/* en */ {IDS_SAVE_SNAPSHOT_IMAGE,    "Save snapshot image"},
/* de */ {IDS_SAVE_SNAPSHOT_IMAGE_DE, "Snapshot Datei speichern"},
/* fr */ {IDS_SAVE_SNAPSHOT_IMAGE_FR, "Enregistrer le fichier de sauvegarde"},
/* hu */ {IDS_SAVE_SNAPSHOT_IMAGE_HU, "Pillanatk�p f�jl ment�se"},
/* it */ {IDS_SAVE_SNAPSHOT_IMAGE_IT, "Salva l'immagine dello snapshot"},
/* nl */ {IDS_SAVE_SNAPSHOT_IMAGE_NL, "Momentopname bestand opslaan"},
/* pl */ {IDS_SAVE_SNAPSHOT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_SNAPSHOT_IMAGE_SV, "Spara �gonblicksbildfil"},

/* en */ {IDS_LOAD_SNAPSHOT_IMAGE,    "Load snapshot image"},
/* de */ {IDS_LOAD_SNAPSHOT_IMAGE_DE, "Snapshot laden"},
/* fr */ {IDS_LOAD_SNAPSHOT_IMAGE_FR, "Charger le fichier de sauvegarde"},
/* hu */ {IDS_LOAD_SNAPSHOT_IMAGE_HU, "Pillanatk�p bet�lt�se"},
/* it */ {IDS_LOAD_SNAPSHOT_IMAGE_IT, "Carica immagine dello snapshot"},
/* nl */ {IDS_LOAD_SNAPSHOT_IMAGE_NL, "Laad momentopname bestand"},
/* pl */ {IDS_LOAD_SNAPSHOT_IMAGE_PL, ""},  /* fuzzy */
/* sv */ {IDS_LOAD_SNAPSHOT_IMAGE_SV, "L�ser in �gonblicksbild"},

/* en */ {IDS_SAVE_CURRENTLY_ATTACHED_DISKS,    "Save currently attached disk images"},
/* de */ {IDS_SAVE_CURRENTLY_ATTACHED_DISKS_DE, "Aktuell eingelegte Diskimages speichern"},
/* fr */ {IDS_SAVE_CURRENTLY_ATTACHED_DISKS_FR, "Sauvegarder les disques pr�sentement ins�r�s"},
/* hu */ {IDS_SAVE_CURRENTLY_ATTACHED_DISKS_HU, "A jelenleg csatolt lemezek ment�se"},
/* it */ {IDS_SAVE_CURRENTLY_ATTACHED_DISKS_IT, "Salva le immagini dei dischi attualmente selezionati"},
/* nl */ {IDS_SAVE_CURRENTLY_ATTACHED_DISKS_NL, "Opslaan huidig gekoppelde disks"},
/* pl */ {IDS_SAVE_CURRENTLY_ATTACHED_DISKS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_CURRENTLY_ATTACHED_DISKS_SV, "Spara anslutna disketter diskettavbildningar"},

/* en */ {IDS_SAVE_CURRENTLY_ATTACHED_ROMS,    "Save currently loaded ROM images"},
/* de */ {IDS_SAVE_CURRENTLY_ATTACHED_ROMS_DE, "Aktuelle ROMs speichern"},
/* fr */ {IDS_SAVE_CURRENTLY_ATTACHED_ROMS_FR, "Sauvegarder les images ROM pr�sentement charg�s"},
/* hu */ {IDS_SAVE_CURRENTLY_ATTACHED_ROMS_HU, "A jelenleg bet�lt�tt ROM-ok ment�se"},
/* it */ {IDS_SAVE_CURRENTLY_ATTACHED_ROMS_IT, "Salva le immagini delle ROM attualmente caricate"},
/* nl */ {IDS_SAVE_CURRENTLY_ATTACHED_ROMS_NL, "Opslaan huidig gekoppelde ROMs"},
/* pl */ {IDS_SAVE_CURRENTLY_ATTACHED_ROMS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SAVE_CURRENTLY_ATTACHED_ROMS_SV, "Spara l�sta ROM-avbildningar"},

/* en */ {IDS_CANNOT_WRITE_SNAPSHOT_S,    "Cannot write snapshot file `%s'."},
/* de */ {IDS_CANNOT_WRITE_SNAPSHOT_S_DE, "Kann Snapshot Datei `%s' nicht speichern."},
/* fr */ {IDS_CANNOT_WRITE_SNAPSHOT_S_FR, ""},  /* fuzzy */
/* hu */ {IDS_CANNOT_WRITE_SNAPSHOT_S_HU, "Nem siker�lt a pillanatk�p �r�sa %s f�jlba."},
/* it */ {IDS_CANNOT_WRITE_SNAPSHOT_S_IT, "Non � possibile scrivere il file di snapshot `%s'."},
/* nl */ {IDS_CANNOT_WRITE_SNAPSHOT_S_NL, "Kan momentopname bestand `%s' niet schrijven."},
/* pl */ {IDS_CANNOT_WRITE_SNAPSHOT_S_PL, ""},  /* fuzzy */
/* sv */ {IDS_CANNOT_WRITE_SNAPSHOT_S_SV, "Kan inte skriva �gonblicksbildfil \"%s\"."},

/* en */ {IDS_CANNOT_READ_SNAPSHOT_IMG,    "Cannot read snapshot image"},
/* de */ {IDS_CANNOT_READ_SNAPSHOT_IMG_DE, "Kann Snapshot Datei nicht laden"},
/* fr */ {IDS_CANNOT_READ_SNAPSHOT_IMG_FR, ""},  /* fuzzy */
/* hu */ {IDS_CANNOT_READ_SNAPSHOT_IMG_HU, "Nem siker�lt bet�lteni a pillanatk�p f�jlt"},
/* it */ {IDS_CANNOT_READ_SNAPSHOT_IMG_IT, "Non � possibile leggere l'immagine dello snapshot"},
/* nl */ {IDS_CANNOT_READ_SNAPSHOT_IMG_NL, "Kan momentopname bestand niet lezen"},
/* pl */ {IDS_CANNOT_READ_SNAPSHOT_IMG_PL, ""},  /* fuzzy */
/* sv */ {IDS_CANNOT_READ_SNAPSHOT_IMG_SV, "Kan inte l�sa in �gonblicksbildfil"},

/* en */ {IDS_SNAPSHOT_FILE,    "snapshot file"},
/* de */ {IDS_SNAPSHOT_FILE_DE, "Snapshot Datei"},
/* fr */ {IDS_SNAPSHOT_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_SNAPSHOT_FILE_HU, "pillanatk�p f�jl"},
/* it */ {IDS_SNAPSHOT_FILE_IT, "File di snapshot"},
/* nl */ {IDS_SNAPSHOT_FILE_NL, "momentopname bestand"},
/* pl */ {IDS_SNAPSHOT_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SNAPSHOT_FILE_SV, "�gonblicksbildfil"},

/* en */ {IDS_SELECT_SNAPSHOT_FILE,    "Select snapshot file"},
/* de */ {IDS_SELECT_SNAPSHOT_FILE_DE, "Snapshot Datei w�hlen"},
/* fr */ {IDS_SELECT_SNAPSHOT_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_SNAPSHOT_FILE_HU, "V�lasszon pillanatk�p f�jlt"},
/* it */ {IDS_SELECT_SNAPSHOT_FILE_IT, "Seleziona il file di snapshot"},
/* nl */ {IDS_SELECT_SNAPSHOT_FILE_NL, "Selecteer momentopname bestand"},
/* pl */ {IDS_SELECT_SNAPSHOT_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_SNAPSHOT_FILE_SV, "V�lj �gonblicksbildfil"},

/* en */ {IDS_FAST_PAL,    "Fast PAL"},
/* de */ {IDS_FAST_PAL_DE, "Schnelle PAL"},
/* fr */ {IDS_FAST_PAL_FR, ""},  /* fuzzy */
/* hu */ {IDS_FAST_PAL_HU, "Gyors PAL"},
/* it */ {IDS_FAST_PAL_IT, "PAL veloce"},
/* nl */ {IDS_FAST_PAL_NL, "Snelle PAL"},
/* pl */ {IDS_FAST_PAL_PL, ""},  /* fuzzy */
/* sv */ {IDS_FAST_PAL_SV, "Snabb PAL"},

/* en */ {IDS_PAL_EMULATION,    "PAL Emulation"},
/* de */ {IDS_PAL_EMULATION_DE, "PAL Emulation"},
/* fr */ {IDS_PAL_EMULATION_FR, "�mulation PAL"},
/* hu */ {IDS_PAL_EMULATION_HU, "PAL emul�ci�"},
/* it */ {IDS_PAL_EMULATION_IT, "Emulazione PAL"},
/* nl */ {IDS_PAL_EMULATION_NL, "PAL Emulatie"},
/* pl */ {IDS_PAL_EMULATION_PL, ""},  /* fuzzy */
/* sv */ {IDS_PAL_EMULATION_SV, "PAL-emulering"},

/* en */ {IDS_EXTERNAL_PALETTE_SELECT,    "Select External Palette file"},
/* de */ {IDS_EXTERNAL_PALETTE_SELECT_DE, "Benutze externe Palette (Datei)"},
/* fr */ {IDS_EXTERNAL_PALETTE_SELECT_FR, ""},  /* fuzzy */
/* hu */ {IDS_EXTERNAL_PALETTE_SELECT_HU, "V�lasszon k�ls� paletta f�jlt"},
/* it */ {IDS_EXTERNAL_PALETTE_SELECT_IT, "Seleziona il file per la palette esterna"},
/* nl */ {IDS_EXTERNAL_PALETTE_SELECT_NL, "Selecteer extern palette bestand"},
/* pl */ {IDS_EXTERNAL_PALETTE_SELECT_PL, ""},  /* fuzzy */
/* sv */ {IDS_EXTERNAL_PALETTE_SELECT_SV, "Anv�nd en extern palett (fil)"},

/* en */ {IDS_GAMMA_0_2,    "Gamma (0..2)"},
/* de */ {IDS_GAMMA_0_2_DE, "Gamma (0..2)"},
/* fr */ {IDS_GAMMA_0_2_FR, "Intensit� (0..2)"},
/* hu */ {IDS_GAMMA_0_2_HU, "Gamma (0..2)"},
/* it */ {IDS_GAMMA_0_2_IT, "Gamma (0..2)"},
/* nl */ {IDS_GAMMA_0_2_NL, "Gamma (0..2)"},
/* pl */ {IDS_GAMMA_0_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_GAMMA_0_2_SV, "Gamma (0..2)"},

/* en */ {IDS_PAL_SHADE_0_1,    "PAL Shade (0..1)"},
/* de */ {IDS_PAL_SHADE_0_1_DE, "PAL Schatten (0..1)"},
/* fr */ {IDS_PAL_SHADE_0_1_FR, "Ombrage PAL (0..1)"},
/* hu */ {IDS_PAL_SHADE_0_1_HU, "PAL p�szta �rnyal�s (0..2)"},
/* it */ {IDS_PAL_SHADE_0_1_IT, "Ombra PAL (0..1)"},
/* nl */ {IDS_PAL_SHADE_0_1_NL, "PAL Schaduw (0..1)"},
/* pl */ {IDS_PAL_SHADE_0_1_PL, ""},  /* fuzzy */
/* sv */ {IDS_PAL_SHADE_0_1_SV, "PAL-skuggning (0..1)"},

/* en */ {IDS_PAL_BLUR_0_1,    "PAL Blur (0..1)"},
/* de */ {IDS_PAL_BLUR_0_1_DE, "PAL Unsch�rfe (0..1)"},
/* fr */ {IDS_PAL_BLUR_0_1_FR, "Flou PAL (0..1)"},
/* hu */ {IDS_PAL_BLUR_0_1_HU, "PAL elmos�s (0..1)"},
/* it */ {IDS_PAL_BLUR_0_1_IT, "Sfuocatura PAL (0..1)"},
/* nl */ {IDS_PAL_BLUR_0_1_NL, "PAL Wazigheid (0..1)"},
/* pl */ {IDS_PAL_BLUR_0_1_PL, ""},  /* fuzzy */
/* sv */ {IDS_PAL_BLUR_0_1_SV, "PAL-suddighet (0..1)"},

/* en */ {IDS_PAL_MODE,    "PAL Mode"},
/* de */ {IDS_PAL_MODE_DE, "PAL Modus"},
/* fr */ {IDS_PAL_MODE_FR, "Mode PAL"},
/* hu */ {IDS_PAL_MODE_HU, "PAL m�d"},
/* it */ {IDS_PAL_MODE_IT, "Modalit� PAL"},
/* nl */ {IDS_PAL_MODE_NL, "PAL Modus"},
/* pl */ {IDS_PAL_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDS_PAL_MODE_SV, "PAL-l�ge"},

/* en */ {IDS_EXTERNAL_PALETTE,    "External Palette"},
/* de */ {IDS_EXTERNAL_PALETTE_DE, "Externe Palette"},
/* fr */ {IDS_EXTERNAL_PALETTE_FR, "Palette externe"},
/* hu */ {IDS_EXTERNAL_PALETTE_HU, "K�ls� sz�nk�szlet"},
/* it */ {IDS_EXTERNAL_PALETTE_IT, "Palette esterna"},
/* nl */ {IDS_EXTERNAL_PALETTE_NL, "Extern Palette"},
/* pl */ {IDS_EXTERNAL_PALETTE_PL, ""},  /* fuzzy */
/* sv */ {IDS_EXTERNAL_PALETTE_SV, "Extern palett"},

/* en */ {IDS_PALETTE_FILENAME,    "Palette file"},
/* de */ {IDS_PALETTE_FILENAME_DE, "Palette Datei"},
/* fr */ {IDS_PALETTE_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDS_PALETTE_FILENAME_HU, "Paletta f�jl"},
/* it */ {IDS_PALETTE_FILENAME_IT, "File di palette"},
/* nl */ {IDS_PALETTE_FILENAME_NL, "Palette bestand"},
/* pl */ {IDS_PALETTE_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_PALETTE_FILENAME_SV, "Palettfiler"},

/* en */ {IDS_SATURATION_0_2,    "Saturation (0..2)"},
/* de */ {IDS_SATURATION_0_2_DE, "S�ttigung (0..2)"},
/* fr */ {IDS_SATURATION_0_2_FR, "Saturation (0..2)"},
/* hu */ {IDS_SATURATION_0_2_HU, "Tel�tetts�g (0..2)"},
/* it */ {IDS_SATURATION_0_2_IT, "Saturazione (0..2)"},
/* nl */ {IDS_SATURATION_0_2_NL, "Verzadiging (0..2)"},
/* pl */ {IDS_SATURATION_0_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_SATURATION_0_2_SV, "M�ttnad (0..2)"},

/* en */ {IDS_CONTRAST_0_2,    "Contrast (0..2)"},
/* de */ {IDS_CONTRAST_0_2_DE, "Kontrast (0..2)"},
/* fr */ {IDS_CONTRAST_0_2_FR, "Contraste (0..2)"},
/* hu */ {IDS_CONTRAST_0_2_HU, "Kontraszt (0..2)"},
/* it */ {IDS_CONTRAST_0_2_IT, "Contrasto (0..2)"},
/* nl */ {IDS_CONTRAST_0_2_NL, "Contrast (0..2)"},
/* pl */ {IDS_CONTRAST_0_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_CONTRAST_0_2_SV, "Kontrast (0..2)"},

/* en */ {IDS_BRIGHTNESS_0_2,    "Brightness (0..2)"},
/* de */ {IDS_BRIGHTNESS_0_2_DE, "Helligkeit (0..2)"},
/* fr */ {IDS_BRIGHTNESS_0_2_FR, "Luminosit� (0..2)"},
/* hu */ {IDS_BRIGHTNESS_0_2_HU, "F�nyess�g (0..2)"},
/* it */ {IDS_BRIGHTNESS_0_2_IT, "Luminosit� (0..2)"},
/* nl */ {IDS_BRIGHTNESS_0_2_NL, "Helderheid (0..2)"},
/* pl */ {IDS_BRIGHTNESS_0_2_PL, ""},  /* fuzzy */
/* sv */ {IDS_BRIGHTNESS_0_2_SV, "Ljusstyrka (0..2)"},

/* en */ {IDS_VIDEO_SETTINGS,    "Video settings"},
/* de */ {IDS_VIDEO_SETTINGS_DE, "Video Einstellungen"},
/* fr */ {IDS_VIDEO_SETTINGS_FR, "Param�tres vid�o"},
/* hu */ {IDS_VIDEO_SETTINGS_HU, "Vide� be�ll�t�sai"},
/* it */ {IDS_VIDEO_SETTINGS_IT, "Impostazioni video"},
/* nl */ {IDS_VIDEO_SETTINGS_NL, "Video Instellingen"},
/* pl */ {IDS_VIDEO_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_VIDEO_SETTINGS_SV, "Grafikinst�llningar"},

/* en */ {IDS_VICII_PALETTE_FILENAME,    "VICII Palette file"},
/* de */ {IDS_VICII_PALETTE_FILENAME_DE, "VICII Paletten Datei"},
/* fr */ {IDS_VICII_PALETTE_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDS_VICII_PALETTE_FILENAME_HU, "VICII paletta f�jl"},
/* it */ {IDS_VICII_PALETTE_FILENAME_IT, "File di palette VICII"},
/* nl */ {IDS_VICII_PALETTE_FILENAME_NL, "VICII Palette bestand"},
/* pl */ {IDS_VICII_PALETTE_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_VICII_PALETTE_FILENAME_SV, "L�s in VICII-palettfil"},

/* en */ {IDS_VDC_PALETTE_FILENAME,    "VDC Palette file"},
/* de */ {IDS_VDC_PALETTE_FILENAME_DE, "VDC Paletten Datei"},
/* fr */ {IDS_VDC_PALETTE_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDS_VDC_PALETTE_FILENAME_HU, "VDC paletta f�jl"},
/* it */ {IDS_VDC_PALETTE_FILENAME_IT, "File di palette VDC"},
/* nl */ {IDS_VDC_PALETTE_FILENAME_NL, "VDC Palette bestand"},
/* pl */ {IDS_VDC_PALETTE_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_VDC_PALETTE_FILENAME_SV, "L�s in VDC-palettfil"},

/* en */ {IDS_AUTO_FROM_ROM,    "Auto (from ROM)"},
/* de */ {IDS_AUTO_FROM_ROM_DE, "Automatisch (von ROM)"},
/* fr */ {IDS_AUTO_FROM_ROM_FR, ""},  /* fuzzy */
/* hu */ {IDS_AUTO_FROM_ROM_HU, "Automatikus (ROM-b�l)"},
/* it */ {IDS_AUTO_FROM_ROM_IT, "Auto (dalla ROM)"},
/* nl */ {IDS_AUTO_FROM_ROM_NL, "Automatisch (van ROM)"},
/* pl */ {IDS_AUTO_FROM_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDS_AUTO_FROM_ROM_SV, "Automatisk (fr�n ROM)"},

/* en */ {IDS_40_COLUMNS,    "40 Columns"},
/* de */ {IDS_40_COLUMNS_DE, "40 Spalten"},
/* fr */ {IDS_40_COLUMNS_FR, ""},  /* fuzzy */
/* hu */ {IDS_40_COLUMNS_HU, "40 Oszlop"},
/* it */ {IDS_40_COLUMNS_IT, "40 Colonne"},
/* nl */ {IDS_40_COLUMNS_NL, "40 Kolommen"},
/* pl */ {IDS_40_COLUMNS_PL, ""},  /* fuzzy */
/* sv */ {IDS_40_COLUMNS_SV, "40 kolumner"},

/* en */ {IDS_80_COLUMNS,    "80 Columns"},
/* de */ {IDS_80_COLUMNS_DE, "80 Spalten"},
/* fr */ {IDS_80_COLUMNS_FR, ""},  /* fuzzy */
/* hu */ {IDS_80_COLUMNS_HU, "80 Oszlop"},
/* it */ {IDS_80_COLUMNS_IT, "80 Colonne"},
/* nl */ {IDS_80_COLUMNS_NL, "80 Kolommen"},
/* pl */ {IDS_80_COLUMNS_PL, ""},  /* fuzzy */
/* sv */ {IDS_80_COLUMNS_SV, "80 kolumner"},

/* en */ {IDS_256_BYTE,    "256 Byte"},
/* de */ {IDS_256_BYTE_DE, "256 Byte"},
/* fr */ {IDS_256_BYTE_FR, ""},  /* fuzzy */
/* hu */ {IDS_256_BYTE_HU, "256 B�jt"},
/* it */ {IDS_256_BYTE_IT, "256 Byte"},
/* nl */ {IDS_256_BYTE_NL, "256 Byte"},
/* pl */ {IDS_256_BYTE_PL, ""},  /* fuzzy */
/* sv */ {IDS_256_BYTE_SV, "256 byte"},

/* en */ {IDS_2_KBYTE,    "2 kByte"},
/* de */ {IDS_2_KBYTE_DE, "2 kByte"},
/* fr */ {IDS_2_KBYTE_FR, ""},  /* fuzzy */
/* hu */ {IDS_2_KBYTE_HU, "2 kB�jt"},
/* it */ {IDS_2_KBYTE_IT, "2 kByte"},
/* nl */ {IDS_2_KBYTE_NL, "2 kByte"},
/* pl */ {IDS_2_KBYTE_PL, ""},  /* fuzzy */
/* sv */ {IDS_2_KBYTE_SV, "2 kbyte"},

/* en */ {IDS_GRAPHICS,    "Graphics"},
/* de */ {IDS_GRAPHICS_DE, "Grafik"},
/* fr */ {IDS_GRAPHICS_FR, ""},  /* fuzzy */
/* hu */ {IDS_GRAPHICS_HU, "Grafikus"},
/* it */ {IDS_GRAPHICS_IT, "Grafica"},
/* nl */ {IDS_GRAPHICS_NL, "Graphics"},
/* pl */ {IDS_GRAPHICS_PL, ""},  /* fuzzy */
/* sv */ {IDS_GRAPHICS_SV, "Grafisk"},

/* en */ {IDS_BUSINESS_UK,    "Business (UK)"},
/* de */ {IDS_BUSINESS_UK_DE, "Business (UK)"},
/* fr */ {IDS_BUSINESS_UK_FR, ""},  /* fuzzy */
/* hu */ {IDS_BUSINESS_UK_HU, "Hivatalos (UK)"},
/* it */ {IDS_BUSINESS_UK_IT, "Business (UK)"},
/* nl */ {IDS_BUSINESS_UK_NL, "Business (UK)"},
/* pl */ {IDS_BUSINESS_UK_PL, ""},  /* fuzzy */
/* sv */ {IDS_BUSINESS_UK_SV, "Business (Storbritannien)"},

/* en */ {IDS_SET_PET_MODEL,    "Set PET Model"},
/* de */ {IDS_SET_PET_MODEL_DE, "W�hle PET Modell"},
/* fr */ {IDS_SET_PET_MODEL_FR, ""},  /* fuzzy */
/* hu */ {IDS_SET_PET_MODEL_HU, "PET modell kiv�laszt�sa"},
/* it */ {IDS_SET_PET_MODEL_IT, "Imposta modello di PET"},
/* nl */ {IDS_SET_PET_MODEL_NL, "Zet PET Model"},
/* pl */ {IDS_SET_PET_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDS_SET_PET_MODEL_SV, "V�lj PET-modell"},

/* en */ {IDS_MEMORY,    "Memory"},
/* de */ {IDS_MEMORY_DE, "Speicher"},
/* fr */ {IDS_MEMORY_FR, "M�moire"},
/* hu */ {IDS_MEMORY_HU, "Mem�ria"},
/* it */ {IDS_MEMORY_IT, "Memoria"},
/* nl */ {IDS_MEMORY_NL, "Geheugen"},
/* pl */ {IDS_MEMORY_PL, ""},  /* fuzzy */
/* sv */ {IDS_MEMORY_SV, "Minne"},

/* en */ {IDS_VIDEO_SIZE,    "Video size"},
/* de */ {IDS_VIDEO_SIZE_DE, "Videospeichergr��e"},
/* fr */ {IDS_VIDEO_SIZE_FR, "Taille vid�o"},
/* hu */ {IDS_VIDEO_SIZE_HU, "Vide� m�ret"},
/* it */ {IDS_VIDEO_SIZE_IT, "Dimensione dello schermo"},
/* nl */ {IDS_VIDEO_SIZE_NL, "Video grootte"},
/* pl */ {IDS_VIDEO_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDS_VIDEO_SIZE_SV, "Sk�rmbredd"},

/* en */ {IDS_IO_SIZE,    "I/O size"},
/* de */ {IDS_IO_SIZE_DE, "I/O Gr��e"},
/* fr */ {IDS_IO_SIZE_FR, "Taille d'E/S"},
/* hu */ {IDS_IO_SIZE_HU, "I/O m�ret"},
/* it */ {IDS_IO_SIZE_IT, "Dimensione di I/O"},
/* nl */ {IDS_IO_SIZE_NL, "I/O grootte"},
/* pl */ {IDS_IO_SIZE_PL, ""},  /* fuzzy */
/* sv */ {IDS_IO_SIZE_SV, "I/O-utrymme"},

/* en */ {IDS_KEYBOARD,    "Keyboard"},
/* de */ {IDS_KEYBOARD_DE, "Tastatur"},
/* fr */ {IDS_KEYBOARD_FR, ""},  /* fuzzy */
/* hu */ {IDS_KEYBOARD_HU, "Billenty�zet"},
/* it */ {IDS_KEYBOARD_IT, "Tastiera"},
/* nl */ {IDS_KEYBOARD_NL, "Toetsenbord"},
/* pl */ {IDS_KEYBOARD_PL, ""},  /* fuzzy */
/* sv */ {IDS_KEYBOARD_SV, "Tangentbord"},

/* en */ {IDS_CRTC_CHIP,    "CRTC chip"},
/* de */ {IDS_CRTC_CHIP_DE, "CRTC Chip"},
/* fr */ {IDS_CRTC_CHIP_FR, ""},  /* fuzzy */
/* hu */ {IDS_CRTC_CHIP_HU, "CRTC csip"},
/* it */ {IDS_CRTC_CHIP_IT, "Chip CRTC"},
/* nl */ {IDS_CRTC_CHIP_NL, "CRTC chip"},
/* pl */ {IDS_CRTC_CHIP_PL, ""},  /* fuzzy */
/* sv */ {IDS_CRTC_CHIP_SV, "CRTC-krets"},

/* en */ {IDS_SUPERPET_IO,    "SuperPET I/O enable (disables 8x96)"},
/* de */ {IDS_SUPERPET_IO_DE, "SuperPET I/O aktivieren (deaktiviert 8x96)"},
/* fr */ {IDS_SUPERPET_IO_FR, ""},  /* fuzzy */
/* hu */ {IDS_SUPERPET_IO_HU, "SuperPET I/O enged�lyez�se (8x96-ot tiltja)"},
/* it */ {IDS_SUPERPET_IO_IT, "Attiva I/O del SuperPET (disattiva 8x96)"},
/* nl */ {IDS_SUPERPET_IO_NL, "SuperPET I/O aktiveren (zet 8x96 uit)"},
/* pl */ {IDS_SUPERPET_IO_PL, ""},  /* fuzzy */
/* sv */ {IDS_SUPERPET_IO_SV, "Aktivera SuperPET-I/O (inaktiverar 8x96)"},

/* en */ {IDS_9xxx_AS_RAM,    "$9*** as RAM"},
/* de */ {IDS_9xxx_AS_RAM_DE, "$9*** als RAM"},
/* fr */ {IDS_9xxx_AS_RAM_FR, ""},  /* fuzzy */
/* hu */ {IDS_9xxx_AS_RAM_HU, "$9*** RAM-k�nt"},
/* it */ {IDS_9xxx_AS_RAM_IT, "$9*** come RAM"},
/* nl */ {IDS_9xxx_AS_RAM_NL, "$9*** als RAM"},
/* pl */ {IDS_9xxx_AS_RAM_PL, ""},  /* fuzzy */
/* sv */ {IDS_9xxx_AS_RAM_SV, "$9*** som RAM"},

/* en */ {IDS_Axxx_AS_RAM,    "$A*** as RAM"},
/* de */ {IDS_Axxx_AS_RAM_DE, "$A*** als RAM"},
/* fr */ {IDS_Axxx_AS_RAM_FR, ""},  /* fuzzy */
/* hu */ {IDS_Axxx_AS_RAM_HU, "$A*** RAM-k�nt"},
/* it */ {IDS_Axxx_AS_RAM_IT, "$A*** come RAM"},
/* nl */ {IDS_Axxx_AS_RAM_NL, "$A*** als RAM"},
/* pl */ {IDS_Axxx_AS_RAM_PL, ""},  /* fuzzy */
/* sv */ {IDS_Axxx_AS_RAM_SV, "$A*** som RAM"},

/* en */ {IDS_PET_SETTINGS,    "PET settings"},
/* de */ {IDS_PET_SETTINGS_DE, "PET Einstellungen"},
/* fr */ {IDS_PET_SETTINGS_FR, "Param�tres PET"},
/* hu */ {IDS_PET_SETTINGS_HU, "PET be�ll�t�sai"},
/* it */ {IDS_PET_SETTINGS_IT, "Impostazioni PET"},
/* nl */ {IDS_PET_SETTINGS_NL, "PET instellingen"},
/* pl */ {IDS_PET_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_PET_SETTINGS_SV, "PET-inst�llningar"},

/* en */ {IDS_CURRENT_MODEL,    "Current Model"},
/* de */ {IDS_CURRENT_MODEL_DE, "Aktuelles Modell"},
/* fr */ {IDS_CURRENT_MODEL_FR, ""},  /* fuzzy */
/* hu */ {IDS_CURRENT_MODEL_HU, "Jelenlegi modell"},
/* it */ {IDS_CURRENT_MODEL_IT, "Modello attuale"},
/* nl */ {IDS_CURRENT_MODEL_NL, "Huidig Model"},
/* pl */ {IDS_CURRENT_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDS_CURRENT_MODEL_SV, "Aktuell modell"},

/* en */ {IDS_PET_MODEL,    "PET Model"},
/* de */ {IDS_PET_MODEL_DE, "PET Modell"},
/* fr */ {IDS_PET_MODEL_FR, ""},  /* fuzzy */
/* hu */ {IDS_PET_MODEL_HU, "PET modell"},
/* it */ {IDS_PET_MODEL_IT, "Modello PET"},
/* nl */ {IDS_PET_MODEL_NL, "PET Model"},
/* pl */ {IDS_PET_MODEL_PL, ""},  /* fuzzy */
/* sv */ {IDS_PET_MODEL_SV, "PET-modell"},

/* en */ {IDS_PLUS4_SETTINGS,    "PLUS4 settings"},
/* de */ {IDS_PLUS4_SETTINGS_DE, "PLUS4 Einstellungen"},
/* fr */ {IDS_PLUS4_SETTINGS_FR, "Param�tres PLUS4"},
/* hu */ {IDS_PLUS4_SETTINGS_HU, "PLUS4 be�ll�t�sai"},
/* it */ {IDS_PLUS4_SETTINGS_IT, "Impostazioni PLUS4"},
/* nl */ {IDS_PLUS4_SETTINGS_NL, "PLUS4 instellingen"},
/* pl */ {IDS_PLUS4_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_PLUS4_SETTINGS_SV, "PLUS4-inst�llningar"},

/* en */ {IDS_SELECT_INTERNAL_FUNCTION_ROM_FILENAME,    "Select Internal Function ROM file"},
/* de */ {IDS_SELECT_INTERNAL_FUNCTION_ROM_FILENAME_DE, "Internes Funktions ROM Image Datei selektieren"},
/* fr */ {IDS_SELECT_INTERNAL_FUNCTION_ROM_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_INTERNAL_FUNCTION_ROM_FILENAME_HU, "Adja meg a bels� Function ROM k�pm�s nev�t"},
/* it */ {IDS_SELECT_INTERNAL_FUNCTION_ROM_FILENAME_IT, "Seleziona il file della function ROM interna"},
/* nl */ {IDS_SELECT_INTERNAL_FUNCTION_ROM_FILENAME_NL, "Selecteer Intern Functie ROM bestand"},
/* pl */ {IDS_SELECT_INTERNAL_FUNCTION_ROM_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_INTERNAL_FUNCTION_ROM_FILENAME_SV, "V�lj avbildning f�r internt funktions-ROM"},

/* en */ {IDS_SELECT_EXTERNAL_FUNCTION_ROM_FILENAME,    "Select External Function ROM file"},
/* de */ {IDS_SELECT_EXTERNAL_FUNCTION_ROM_FILENAME_DE, "Externes Funktions ROM Image Datei selektieren"},
/* fr */ {IDS_SELECT_EXTERNAL_FUNCTION_ROM_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_EXTERNAL_FUNCTION_ROM_FILENAME_HU, "Adja meg a k�ls� Function ROM k�pm�s nev�t"},
/* it */ {IDS_SELECT_EXTERNAL_FUNCTION_ROM_FILENAME_IT, "Seleziona il file della function ROM esterna"},
/* nl */ {IDS_SELECT_EXTERNAL_FUNCTION_ROM_FILENAME_NL, "Selecteer Extern Functie ROM bestand"},
/* pl */ {IDS_SELECT_EXTERNAL_FUNCTION_ROM_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_EXTERNAL_FUNCTION_ROM_FILENAME_SV, "V�lj avbildning f�r externt funktions-ROM"},

/* en */ {IDS_MACHINE_TYPE,    "Machine type"},
/* de */ {IDS_MACHINE_TYPE_DE, "Maschinen Typ"},
/* fr */ {IDS_MACHINE_TYPE_FR, "Type de machine"},
/* hu */ {IDS_MACHINE_TYPE_HU, "G�p t�pus"},
/* it */ {IDS_MACHINE_TYPE_IT, "Tipo di macchina"},
/* nl */ {IDS_MACHINE_TYPE_NL, "Machine soort"},
/* pl */ {IDS_MACHINE_TYPE_PL, ""},  /* fuzzy */
/* sv */ {IDS_MACHINE_TYPE_SV, "Maskintyp"},

/* en */ {IDS_INTERNAL_FUNCTION_ROM,    "Internal Function ROM"},
/* de */ {IDS_INTERNAL_FUNCTION_ROM_DE, "Internes Funktions ROM"},
/* fr */ {IDS_INTERNAL_FUNCTION_ROM_FR, "Image ROM Fonction interne"},
/* hu */ {IDS_INTERNAL_FUNCTION_ROM_HU, "Bels� Function ROM"},
/* it */ {IDS_INTERNAL_FUNCTION_ROM_IT, "Function ROM interna"},
/* nl */ {IDS_INTERNAL_FUNCTION_ROM_NL, "Interne Functie ROM"},
/* pl */ {IDS_INTERNAL_FUNCTION_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDS_INTERNAL_FUNCTION_ROM_SV, "Internt funktions-ROM"},

/* en */ {IDS_INTERNAL_FUNCTION_ROM_FILENAME,    "Internal Function ROM file"},
/* de */ {IDS_INTERNAL_FUNCTION_ROM_FILENAME_DE, "Interne Funktions ROM Datei"},
/* fr */ {IDS_INTERNAL_FUNCTION_ROM_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDS_INTERNAL_FUNCTION_ROM_FILENAME_HU, "Bels� Function ROM f�jl"},
/* it */ {IDS_INTERNAL_FUNCTION_ROM_FILENAME_IT, "File della function ROM interna"},
/* nl */ {IDS_INTERNAL_FUNCTION_ROM_FILENAME_NL, "Interne Functie ROM bestand"},
/* pl */ {IDS_INTERNAL_FUNCTION_ROM_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_INTERNAL_FUNCTION_ROM_FILENAME_SV, "Internt funktions-ROM"},

/* en */ {IDS_EXTERNAL_FUNCTION_ROM,    "External Function ROM"},
/* de */ {IDS_EXTERNAL_FUNCTION_ROM_DE, "Externes Funktions ROM"},
/* fr */ {IDS_EXTERNAL_FUNCTION_ROM_FR, "Image ROM Fonction externe"},
/* hu */ {IDS_EXTERNAL_FUNCTION_ROM_HU, "K�ls� Function ROM"},
/* it */ {IDS_EXTERNAL_FUNCTION_ROM_IT, "Function ROM esterna"},
/* nl */ {IDS_EXTERNAL_FUNCTION_ROM_NL, "Externe Functie ROM"},
/* pl */ {IDS_EXTERNAL_FUNCTION_ROM_PL, ""},  /* fuzzy */
/* sv */ {IDS_EXTERNAL_FUNCTION_ROM_SV, "Externt funktions-ROM"},

/* en */ {IDS_EXTERNAL_FUNCTION_ROM_FILENAME,    "External Function ROM file"},
/* de */ {IDS_EXTERNAL_FUNCTION_ROM_FILENAME_DE, "Externe Funktions ROM Datei"},
/* fr */ {IDS_EXTERNAL_FUNCTION_ROM_FILENAME_FR, ""},  /* fuzzy */
/* hu */ {IDS_EXTERNAL_FUNCTION_ROM_FILENAME_HU, "K�ls� Function ROM f�jl"},
/* it */ {IDS_EXTERNAL_FUNCTION_ROM_FILENAME_IT, "File della function ROM esterna"},
/* nl */ {IDS_EXTERNAL_FUNCTION_ROM_FILENAME_NL, "Externe Functie ROM bestand"},
/* pl */ {IDS_EXTERNAL_FUNCTION_ROM_FILENAME_PL, ""},  /* fuzzy */
/* sv */ {IDS_EXTERNAL_FUNCTION_ROM_FILENAME_SV, "Externt funktions-ROM"},

/* en */ {IDS_C128_SETTINGS,    "C128 settings"},
/* de */ {IDS_C128_SETTINGS_DE, "C128 Einstellungen"},
/* fr */ {IDS_C128_SETTINGS_FR, "Param�tres C128"},
/* hu */ {IDS_C128_SETTINGS_HU, "C128 be�ll�t�sai"},
/* it */ {IDS_C128_SETTINGS_IT, "Impostazioni C128"},
/* nl */ {IDS_C128_SETTINGS_NL, "C128 instellingen"},
/* pl */ {IDS_C128_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_C128_SETTINGS_SV, "C128-inst�llningar"},

/* en */ {IDS_SELECT_ROM_FILE,    "Select ROM file"},
/* de */ {IDS_SELECT_ROM_FILE_DE, "ROM Datei w�hlen"},
/* fr */ {IDS_SELECT_ROM_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_ROM_FILE_HU, "V�lasszon ROM f�jlt"},
/* it */ {IDS_SELECT_ROM_FILE_IT, "Seleziona file della ROM"},
/* nl */ {IDS_SELECT_ROM_FILE_NL, "Selecteer ROM bestand"},
/* pl */ {IDS_SELECT_ROM_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_ROM_FILE_SV, "V�lj ROM-fil"},

/* en */ {IDS_COMPUTER_ROM_SETTINGS,    "Computer ROM settings"},
/* de */ {IDS_COMPUTER_ROM_SETTINGS_DE, "Computer ROM Einstellungen"},
/* fr */ {IDS_COMPUTER_ROM_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_COMPUTER_ROM_SETTINGS_HU, "Sz�m�t�g�p ROM be�ll�t�sai"},
/* it */ {IDS_COMPUTER_ROM_SETTINGS_IT, "Impostazioni ROM del computer"},
/* nl */ {IDS_COMPUTER_ROM_SETTINGS_NL, "Computer ROM instellingen"},
/* pl */ {IDS_COMPUTER_ROM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_COMPUTER_ROM_SETTINGS_SV, "ROM-inst�llningar f�r dator"},

/* en */ {IDS_DRIVE_ROM_SETTINGS,    "Drive ROM settings"},
/* de */ {IDS_DRIVE_ROM_SETTINGS_DE, "Floppy ROM Einstellungen"},
/* fr */ {IDS_DRIVE_ROM_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_DRIVE_ROM_SETTINGS_HU, "Lemezegys�g ROM be�ll�t�sai"},
/* it */ {IDS_DRIVE_ROM_SETTINGS_IT, "Impostazioni ROM del drive"},
/* nl */ {IDS_DRIVE_ROM_SETTINGS_NL, "Drive ROM instellingen"},
/* pl */ {IDS_DRIVE_ROM_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_DRIVE_ROM_SETTINGS_SV, "ROM-inst�llningar f�r diskettenhet"},

/* en */ {IDS_DIGIMAX_ENABLED,    "Digimax Enabled"},
/* de */ {IDS_DIGIMAX_ENABLED_DE, "Digimax aktiviert"},
/* fr */ {IDS_DIGIMAX_ENABLED_FR, ""},  /* fuzzy */
/* hu */ {IDS_DIGIMAX_ENABLED_HU, "Digimax enged�lyezve"},
/* it */ {IDS_DIGIMAX_ENABLED_IT, "Digimax attivato"},
/* nl */ {IDS_DIGIMAX_ENABLED_NL, "Digimax Aan/Uit"},
/* pl */ {IDS_DIGIMAX_ENABLED_PL, ""},  /* fuzzy */
/* sv */ {IDS_DIGIMAX_ENABLED_SV, "Digimax aktiverad"},

/* en */ {IDS_DIGIMAX_BASE,    "Digimax Base"},
/* de */ {IDS_DIGIMAX_BASE_DE, "Digimax Basis"},
/* fr */ {IDS_DIGIMAX_BASE_FR, ""},  /* fuzzy */
/* hu */ {IDS_DIGIMAX_BASE_HU, "Digimax b�zisc�m"},
/* it */ {IDS_DIGIMAX_BASE_IT, "Base Digimax"},
/* nl */ {IDS_DIGIMAX_BASE_NL, "Digimax basis adres"},
/* pl */ {IDS_DIGIMAX_BASE_PL, ""},  /* fuzzy */
/* sv */ {IDS_DIGIMAX_BASE_SV, "Digimaxbas"},

/* en */ {IDS_DIGIMAX_SETTINGS,    "Digimax Settings"},
/* de */ {IDS_DIGIMAX_SETTINGS_DE, "Digimax Einstellungen"},
/* fr */ {IDS_DIGIMAX_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_DIGIMAX_SETTINGS_HU, "Digimax be�ll�t�sai"},
/* it */ {IDS_DIGIMAX_SETTINGS_IT, "Impostazioni digimax"},
/* nl */ {IDS_DIGIMAX_SETTINGS_NL, "Digimax Instellingen"},
/* pl */ {IDS_DIGIMAX_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_DIGIMAX_SETTINGS_SV, "Digimaxinst�llningar"},

/* en */ {IDS_SOUND_RECORDING_STOPPED,    "Sound Recording stopped..."},
/* de */ {IDS_SOUND_RECORDING_STOPPED_DE, "Tonaufnahme gestoppt..."},
/* fr */ {IDS_SOUND_RECORDING_STOPPED_FR, ""},  /* fuzzy */
/* hu */ {IDS_SOUND_RECORDING_STOPPED_HU, "Hangfelv�tel le�ll�tva..."},
/* it */ {IDS_SOUND_RECORDING_STOPPED_IT, "Registrazione audio interrotta..."},
/* nl */ {IDS_SOUND_RECORDING_STOPPED_NL, "Geluid Opname gestopt..."},
/* pl */ {IDS_SOUND_RECORDING_STOPPED_PL, ""},  /* fuzzy */
/* sv */ {IDS_SOUND_RECORDING_STOPPED_SV, "Ljudinspelning stoppad..."},

/* en */ {IDS_SOUND_RECORDING_STARTED,    "Sound Recording started..."},
/* de */ {IDS_SOUND_RECORDING_STARTED_DE, "Tonaufnahme gestartet..."},
/* fr */ {IDS_SOUND_RECORDING_STARTED_FR, ""},  /* fuzzy */
/* hu */ {IDS_SOUND_RECORDING_STARTED_HU, "Hangfelv�tel elind�tva..."},
/* it */ {IDS_SOUND_RECORDING_STARTED_IT, "Registrazione audio avviata..."},
/* nl */ {IDS_SOUND_RECORDING_STARTED_NL, "Geluid Opname gestart..."},
/* pl */ {IDS_SOUND_RECORDING_STARTED_PL, ""},  /* fuzzy */
/* sv */ {IDS_SOUND_RECORDING_STARTED_SV, "Ljudinspelning startad..."},

/* en */ {IDS_SELECT_RECORD_FILE,    "Select Sound Record File"},
/* de */ {IDS_SELECT_RECORD_FILE_DE, "Tonaufnahmedatei definieren"},
/* fr */ {IDS_SELECT_RECORD_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_SELECT_RECORD_FILE_HU, "V�lassza ki a hangfelv�tel kimeneti f�jlt"},
/* it */ {IDS_SELECT_RECORD_FILE_IT, "Seleziona il file per la registrazione dell'audio"},
/* nl */ {IDS_SELECT_RECORD_FILE_NL, "Selecteer Geluid Opname Bestand"},
/* pl */ {IDS_SELECT_RECORD_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SELECT_RECORD_FILE_SV, "V�lj ljudinspelningsfil"},

/* en */ {IDS_SOUND_RECORD_FORMAT,    "Sound Record Format"},
/* de */ {IDS_SOUND_RECORD_FORMAT_DE, "Tonaufnahmeformat"},
/* fr */ {IDS_SOUND_RECORD_FORMAT_FR, ""},  /* fuzzy */
/* hu */ {IDS_SOUND_RECORD_FORMAT_HU, "Hangfelv�tel form�tuma"},
/* it */ {IDS_SOUND_RECORD_FORMAT_IT, "Formato di registrazione dell'audio"},
/* nl */ {IDS_SOUND_RECORD_FORMAT_NL, "Geluid Opname Formaat"},
/* pl */ {IDS_SOUND_RECORD_FORMAT_PL, ""},  /* fuzzy */
/* sv */ {IDS_SOUND_RECORD_FORMAT_SV, "Format f�r ljudinspelning"},

/* en */ {IDS_SOUND_RECORD_FILE,    "Sound Record File"},
/* de */ {IDS_SOUND_RECORD_FILE_DE, "Tonaufnahmedatei"},
/* fr */ {IDS_SOUND_RECORD_FILE_FR, ""},  /* fuzzy */
/* hu */ {IDS_SOUND_RECORD_FILE_HU, "Hangfelv�tel f�jl"},
/* it */ {IDS_SOUND_RECORD_FILE_IT, "File per la registrazione dell'audio"},
/* nl */ {IDS_SOUND_RECORD_FILE_NL, "Geluid Opname Bestand"},
/* pl */ {IDS_SOUND_RECORD_FILE_PL, ""},  /* fuzzy */
/* sv */ {IDS_SOUND_RECORD_FILE_SV, "Ljudinspelningsfil"},

/* en */ {IDS_SOUND_RECORD_SETTINGS,    "Sound Record Settings"},
/* de */ {IDS_SOUND_RECORD_SETTINGS_DE, "Tonaufnahme Einstellungen"},
/* fr */ {IDS_SOUND_RECORD_SETTINGS_FR, ""},  /* fuzzy */
/* hu */ {IDS_SOUND_RECORD_SETTINGS_HU, "Hangfelv�tel be�ll�t�sai"},
/* it */ {IDS_SOUND_RECORD_SETTINGS_IT, "Impostazioni di registrazione audio"},
/* nl */ {IDS_SOUND_RECORD_SETTINGS_NL, "Geluid Opname Instellingen"},
/* pl */ {IDS_SOUND_RECORD_SETTINGS_PL, ""},  /* fuzzy */
/* sv */ {IDS_SOUND_RECORD_SETTINGS_SV, "Inst�llningar f�r ljudinspelning"},

/* en */ {IDS_BORDER_MODE,    "Border mode"},
/* de */ {IDS_BORDER_MODE_DE, "Rahmen Modus"},
/* fr */ {IDS_BORDER_MODE_FR, ""},  /* fuzzy */
/* hu */ {IDS_BORDER_MODE_HU, "Keret m�d"},
/* it */ {IDS_BORDER_MODE_IT, "Modalit� del bordo"},
/* nl */ {IDS_BORDER_MODE_NL, "Border modus"},
/* pl */ {IDS_BORDER_MODE_PL, ""},  /* fuzzy */
/* sv */ {IDS_BORDER_MODE_SV, "Raml�ge"},

/* en */ {IDMS_NORMAL,    "Normal"},
/* de */ {IDMS_NORMAL_DE, "Normal"},
/* fr */ {IDMS_NORMAL_FR, ""},  /* fuzzy */
/* hu */ {IDMS_NORMAL_HU, "Norm�l"},
/* it */ {IDMS_NORMAL_IT, "Normale"},
/* nl */ {IDMS_NORMAL_NL, "Normaal"},
/* pl */ {IDMS_NORMAL_PL, ""},  /* fuzzy */
/* sv */ {IDMS_NORMAL_SV, "Normal"},

/* en */ {IDS_FULL,    "Full"},
/* de */ {IDS_FULL_DE, "Full"},
/* fr */ {IDS_FULL_FR, ""},  /* fuzzy */
/* hu */ {IDS_FULL_HU, "Teljes"},
/* it */ {IDS_FULL_IT, "Intero"},
/* nl */ {IDS_FULL_NL, "Volledig"},
/* pl */ {IDS_FULL_PL, ""},  /* fuzzy */
/* sv */ {IDS_FULL_SV, "Full"},

/* en */ {IDS_DEBUG,    "Debug"},
/* de */ {IDS_DEBUG_DE, "Debug"},
/* fr */ {IDS_DEBUG_FR, ""},  /* fuzzy */
/* hu */ {IDS_DEBUG_HU, "Nyomk�vet�s"},
/* it */ {IDS_DEBUG_IT, "Debug"},
/* nl */ {IDS_DEBUG_NL, "Debug"},
/* pl */ {IDS_DEBUG_PL, ""},  /* fuzzy */
/* sv */ {IDS_DEBUG_SV, "Fels�k"},

/* en */ {IDS_VOLUME,    "Volume (0-100%)"},
/* de */ {IDS_VOLUME_DE, "Lautst�rke (0-100%)"},
/* fr */ {IDS_VOLUME_FR, ""},  /* fuzzy */
/* hu */ {IDS_VOLUME_HU, "Hanger� (0-100%)"},
/* it */ {IDS_VOLUME_IT, "Volume (0-100%)"},
/* nl */ {IDS_VOLUME_NL, "Volume (0-100%)"},
/* pl */ {IDS_VOLUME_PL, ""},  /* fuzzy */
/* sv */ {IDS_VOLUME_SV, "Volym (0-100 %)"},

};

#include "intl_table.h"

/* --------------------------------------------------------------------- */

static char *intl_text_table[countof(intl_translate_text_table)][countof(language_table)];

void intl_convert_mui_table(int text_id[], char *text[])
{
  int i;

  for (i=0; text_id[i]>0; i++)
  {
    text[i]=intl_translate_text(text_id[i]);
  }
  if (text_id[i]==0)
    text[i]=NULL;
}

/* codepage conversion is not yet present */

char *intl_convert_cp(char *text, int cp)
{
  int len;
  char *buffer;

  if (text==NULL)
    return NULL;

  len=strlen(text);

  if (len==0)
    return NULL;

  buffer=lib_stralloc(text);

  return buffer;
}

static char *intl_get_string_by_id(int id)
{
  int k;

  for (k = 0; k < countof(intl_string_table); k++)
  {
    if (intl_string_table[k].resource_id==id)
      return intl_string_table[k].text;
  }
  return NULL;
}

char *intl_translate_text(int en_resource)
{
  int i;

  for (i = 0; i < countof(intl_translate_text_table); i++)
  {
    if (intl_translate_text_table[i][0]==en_resource)
    {
      if (intl_translate_text_table[i][current_language_index]!=0 &&
          intl_text_table[i][current_language_index]!=NULL &&
          strlen(intl_text_table[i][current_language_index])!=0)
        return intl_text_table[i][current_language_index];
      else
        return intl_text_table[i][0];
    }
  }
  return "";
}

/* pre-translated main window caption text so the emulation won't
   slow down because of all the translation calls */

char *intl_speed_at_text;

/* --------------------------------------------------------------------- */

void intl_init(void)
{
  int i,j;

  for (i = 0; i < countof(language_table); i++)
  {
    for (j = 0; j < countof(intl_translate_text_table); j++)
    {
      if (intl_translate_text_table[j][i]==0)
        intl_text_table[j][i]=NULL;
      else
        intl_text_table[j][i]=(char *)intl_get_string_by_id(intl_translate_text_table[j][i]);
    }
  }
}

int intl_translate_res(int resource)
{
  return resource;
}

void intl_shutdown(void)
{
}

/* --------------------------------------------------------------------- */


static void intl_update_pre_translated_text(void)
{
  intl_speed_at_text=intl_translate_text(IDS_S_AT_D_SPEED);
}

char *intl_arch_language_init(void)
{
  int i;
  struct Locale *amigalang;

  amigalang=OpenLocale(NULL);
  CloseLocale(amigalang);
  for (i = 0; amiga_to_iso[i].iso_language_code != NULL; i++)
  {
    if (!strcasecmp(amiga_to_iso[i].amiga_locale_language, amigalang->loc_LanguageName))
      return amiga_to_iso[i].iso_language_code;
  }
  return "en";
}

void intl_update_ui(void)
{
  intl_update_pre_translated_text();
  ui_update_menus();
}
