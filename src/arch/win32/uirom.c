/*
 * uirom.c - Implementation of the ROM settings dialog box.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
#include <windows.h>
#include <tchar.h>

#ifndef DUMMYUNIONNAME
#define DUMMYUNIONNAME  u1
#endif

#include "lib.h"
#include "machine.h"
#include "res.h"
#include "resources.h"
#include "romset.h"
#include "system.h"
#include "translate.h"
#include "ui.h"
#include "uilib.h"
#include "uirom.h"
#include "winmain.h"


static const uirom_settings_t *settings;

static const unsigned int *romset_dialog_resources;


static void init_rom_dialog(HWND hwnd, unsigned int type)
{
    unsigned int n = 0;

    while (settings[n].realname != NULL) {
        if (settings[n].type == type) {
            const char *filename;
            TCHAR *st_filename;

            resources_get_string(settings[n].resname, &filename);
            st_filename = system_mbstowcs_alloc(filename);
            SetDlgItemText(hwnd, settings[n].idc_filename,
                           st_filename != NULL ? st_filename : TEXT(""));
            system_mbstowcs_free(st_filename);
        }
        n++;
    }
}

static void set_dialog_proc(HWND hwnd, unsigned int type)
{
    unsigned int n = 0;

    while (settings[n].realname != NULL) {
        if (settings[n].type == type) {
            char filename[MAX_PATH];
            TCHAR st_filename[MAX_PATH];

            GetDlgItemText(hwnd, settings[n].idc_filename, st_filename,
                           MAX_PATH);
            system_wcstombs(filename, st_filename, MAX_PATH);
            resources_set_string(settings[n].resname, filename);
        }
        n++;
    }
}

static BOOL browse_command(HWND hwnd, unsigned int command)
{
    unsigned int n = 0;

    while (settings[n].realname != NULL) {
        if ((unsigned int)command == settings[n].idc_browse) {
            TCHAR st_realname[100];

            _stprintf(st_realname, translate_text(IDS_LOAD_S_ROM_IMAGE),
                      settings[n].realname);

            uilib_select_browse(hwnd, st_realname,
                                UILIB_FILTER_ALL,
                                UILIB_SELECTOR_TYPE_FILE_LOAD,
                                settings[n].idc_filename);
            return TRUE;
        }
        n++;
    }

    return FALSE;
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam, unsigned int type)
{
    int command;

    switch (msg) {
      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_rom_dialog(hwnd, type);
        return TRUE;
      case WM_NOTIFY:
        switch (((NMHDR FAR *)lparam)->code) {
          case PSN_KILLACTIVE:
            set_dialog_proc(hwnd, type);
            return TRUE;
        }
        return FALSE;
      case WM_COMMAND:
        command = LOWORD(wparam);
        return browse_command(hwnd, command);
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
    }
    return FALSE;
}

static BOOL CALLBACK dialog_proc_main(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    return dialog_proc(hwnd, msg, wparam, lparam, UIROM_TYPE_MAIN);
}

static BOOL CALLBACK dialog_proc_drive(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    return dialog_proc(hwnd, msg, wparam, lparam, UIROM_TYPE_DRIVE);
}

static void enable_controls_for_romset(HWND hwnd, int idc_active)
{
    int res;

    res = idc_active == IDC_ROMSET_SELECT_ARCHIVE;

    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_NAME), res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_BROWSE), res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_LOAD), res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_SAVE), res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_ACTIVE), res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_APPLY),  res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_NEW), res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_DELETE), res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_FILE_NAME), !res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_FILE_BROWSE), !res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_FILE_LOAD), !res);
    EnableWindow(GetDlgItem(hwnd, IDC_ROMSET_FILE_SAVE), !res);
}

static void update_romset_list(HWND hwnd)
{
    char *list;
    TCHAR *st_list;

    if (IsDlgButtonChecked(hwnd, IDC_ROMSET_SELECT_ARCHIVE) == BST_CHECKED)
        list = romset_archive_list();
    else
        list = machine_romset_file_list();

    st_list = system_mbstowcs_alloc(list);
    SetDlgItemText(hwnd, IDC_ROMSET_PREVIEW, st_list);
    system_mbstowcs_free(st_list);

    lib_free(list);
}

static void update_romset_archive(HWND hwnd)
{
    HWND temp_hwnd;
    int num, index, active;
    const char *conf;

    active = 0;
    num = romset_archive_get_number();

    resources_get_string("RomsetArchiveActive", &conf);

    temp_hwnd = GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_ACTIVE);
    SendMessage(temp_hwnd, CB_RESETCONTENT, 0, 0);
    for (index = 0; index < num; index++) {
        TCHAR *st_name;
        char *name;

        name = romset_archive_get_item(index);
        if (!strcmp(conf, name))
            active = index;
        st_name = system_mbstowcs_alloc(name);
        SendMessage(temp_hwnd, CB_ADDSTRING, 0, (LPARAM)st_name);
        system_mbstowcs_free(st_name);
    }
    SendMessage(temp_hwnd, CB_SETCURSEL, (WPARAM)active, 0);

    update_romset_list(hwnd);
}

static void update_romset_dialog(HWND hwnd, int idc_active)
{
    if (idc_active == IDC_ROMSET_SELECT_ARCHIVE
        || idc_active == IDC_ROMSET_SELECT_FILE) {
        CheckRadioButton(hwnd, IDC_ROMSET_SELECT_ARCHIVE,
                         IDC_ROMSET_SELECT_FILE, idc_active);
        enable_controls_for_romset(hwnd, idc_active);

        update_romset_archive(hwnd);
    }
}

static void init_romset_dialog(HWND hwnd)
{
    int res_value, idc_active;
    const char *name;
    TCHAR *st_name;

    resources_get_int("RomsetSourceFile", &res_value);
    idc_active = IDC_ROMSET_SELECT_ARCHIVE + res_value;
    update_romset_dialog(hwnd, idc_active);

    resources_get_string("RomsetArchiveName", &name);
    st_name = system_mbstowcs_alloc(name);
    SetDlgItemText(hwnd, IDC_ROMSET_ARCHIVE_NAME,
                   name != NULL ? st_name : TEXT(""));
    system_mbstowcs_free(st_name);

    resources_get_string("RomsetFileName", &name);
    st_name = system_mbstowcs_alloc(name);
    SetDlgItemText(hwnd, IDC_ROMSET_FILE_NAME,
                   name != NULL ? st_name : TEXT(""));
    system_mbstowcs_free(st_name);
}

static void end_romset_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    if (IsDlgButtonChecked(hwnd, IDC_ROMSET_SELECT_ARCHIVE) == BST_CHECKED)
        resources_set_int("RomsetSourceFile", 0);
    if (IsDlgButtonChecked(hwnd, IDC_ROMSET_SELECT_FILE) == BST_CHECKED)
        resources_set_int("RomsetSourceFile", 1);

    GetDlgItemText(hwnd, IDC_ROMSET_ARCHIVE_NAME, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_string("RomsetArchiveName", s);

    GetDlgItemText(hwnd, IDC_ROMSET_FILE_NAME, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    resources_set_string("RomsetFileName", s);
}

/*
static void browse_archive_romset_dialog(HWND hwnd)
{
    uilib_select_browse(hwnd, translate_text(IDS_SELECT_ROMSET_ARCHIVE),
                        UILIB_FILTER_ALL, UILIB_SELECTOR_TYPE_FILE_LOAD,
                        IDC_ROMSET_ARCHIVE_NAME);
}
*/

static void load_archive_romset_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    GetDlgItemText(hwnd, IDC_ROMSET_ARCHIVE_NAME, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    if (romset_archive_load(s, 0) < 0)
        ui_error(translate_text(IDS_CANNOT_LOAD_ROMSET_ARCH));

    update_romset_archive(hwnd);
}

static void save_archive_romset_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    GetDlgItemText(hwnd, IDC_ROMSET_ARCHIVE_NAME, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    if (romset_archive_save(s) < 0)
        ui_error(translate_text(IDS_CANNOT_SAVE_ROMSET_ARCH));
}

static TCHAR *active_archive_name(HWND hwnd)
{
    HWND temp_hwnd;
    int active, len;
    TCHAR *st_name;

    temp_hwnd = GetDlgItem(hwnd, IDC_ROMSET_ARCHIVE_ACTIVE);
    active = SendMessage(temp_hwnd, CB_GETCURSEL, 0, 0);
    len = SendMessage(temp_hwnd, CB_GETLBTEXTLEN, active, 0);
    st_name = lib_malloc((len + 1) * sizeof(TCHAR));
    SendMessage(temp_hwnd, CB_GETLBTEXT, active, (LPARAM)st_name);

    return st_name;
}

static void apply_archive_romset_dialog(HWND hwnd)
{
    char *name;
    TCHAR *st_name;

    st_name = active_archive_name(hwnd);
    name = system_wcstombs_alloc(st_name);
    romset_archive_item_select(name);
    system_wcstombs_free(name);
    lib_free(st_name);
}

static void new_archive_romset_dialog(HWND hwnd)
{
    uilib_dialogbox_param_t param;

    param.hwnd = hwnd;
    param.idd_dialog = translate_res(IDD_ROMSET_ENTER_NAME_DIALOG);
    param.idc_dialog = IDC_ROMSET_ENTER_NAME;
    _tcscpy(param.string, TEXT(""));

    uilib_dialogbox(&param);

    if (param.updated > 0) {
        machine_romset_archive_item_create(param.string);
        update_romset_archive(hwnd);
    }
}

static void delete_archive_romset_dialog(HWND hwnd)
{
    char *name;
    TCHAR *st_name;

    st_name = active_archive_name(hwnd);
    name = system_wcstombs_alloc(st_name);
    romset_archive_item_delete(name);
    system_wcstombs_free(name);
    lib_free(st_name);

    update_romset_archive(hwnd);
}

static void load_file_romset_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    GetDlgItemText(hwnd, IDC_ROMSET_FILE_NAME, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);

    if (machine_romset_file_load(s) < 0)
        ui_error(translate_text(IDS_CANNOT_LOAD_ROMSET_FILE));

    update_romset_list(hwnd);
}

static void save_file_romset_dialog(HWND hwnd)
{
    TCHAR st[MAX_PATH];
    char s[MAX_PATH];

    GetDlgItemText(hwnd, IDC_ROMSET_FILE_NAME, st, MAX_PATH);
    system_wcstombs(s, st, MAX_PATH);
    if (machine_romset_file_save(s) < 0)
        ui_error(translate_text(IDS_CANNOT_SAVE_ROMSET_FILE));
}

static void init_resources_dialog(HWND hwnd, unsigned int type)
{
    unsigned int n = 0;

    while (settings[n].realname != NULL) {
        if (settings[n].type == type) {
            int enable;

            resources_get_int_sprintf("Romset%s", &enable,
                                      settings[n].resname);

            CheckDlgButton(hwnd, settings[n].idc_resource,
                           enable ? BST_CHECKED : BST_UNCHECKED);
        }
        n++;
    }
}

static void end_resources_dialog(HWND hwnd, unsigned int type)
{
    unsigned int n = 0;

    while (settings[n].realname != NULL) {
        if (settings[n].type == type) {
            int enable;

            enable = (IsDlgButtonChecked(hwnd, settings[n].idc_resource)
                     == BST_CHECKED) ? 1 : 0;

            resources_set_int_sprintf("Romset%s", enable, settings[n].resname);
        }
        n++;
    }
}

static BOOL CALLBACK resources_dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                           LPARAM lparam, unsigned int type)
{
    int command;

    switch (msg) {
      case WM_INITDIALOG:
        init_resources_dialog(hwnd, type);
        return TRUE;
      case WM_COMMAND:
        command = LOWORD(wparam);
        switch (command) {
          case IDOK:
            end_resources_dialog(hwnd, type);
          case IDCANCEL:
            EndDialog(hwnd, 0);
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
    }
    return FALSE;
}

static BOOL CALLBACK resources_computer_dialog_proc(HWND hwnd, UINT msg,
                                                    WPARAM wparam,
                                                    LPARAM lparam)
{
    return resources_dialog_proc(hwnd, msg, wparam, lparam, UIROM_TYPE_MAIN);
}

static BOOL CALLBACK resources_drive_dialog_proc(HWND hwnd, UINT msg,
                                                 WPARAM wparam, LPARAM lparam)
{
    return resources_dialog_proc(hwnd, msg, wparam, lparam, UIROM_TYPE_DRIVE);
}

static BOOL CALLBACK resources_other_dialog_proc(HWND hwnd, UINT msg,
                                                 WPARAM wparam, LPARAM lparam)
{
    return resources_dialog_proc(hwnd, msg, wparam, lparam, UIROM_TYPE_OTHER);
}

static void uirom_resources_computer(HWND hwnd)
{
    DialogBox(winmain_instance,
              (LPCTSTR)translate_res(romset_dialog_resources[UIROM_TYPE_MAIN]), hwnd,
              resources_computer_dialog_proc);
    update_romset_list(hwnd);
}

static void uirom_resources_drive(HWND hwnd)
{
    DialogBox(winmain_instance,
              (LPCTSTR)translate_res(romset_dialog_resources[UIROM_TYPE_DRIVE]), hwnd,
              resources_drive_dialog_proc);
    update_romset_list(hwnd);
}

static void uirom_resources_other(HWND hwnd)
{
    DialogBox(winmain_instance,
              (LPCTSTR)translate_res(romset_dialog_resources[UIROM_TYPE_OTHER]), hwnd,
              resources_other_dialog_proc);
    update_romset_list(hwnd);
}

static BOOL CALLBACK dialog_proc_romset(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    switch (msg) {
      case WM_INITDIALOG:
        system_init_dialog(hwnd);
        init_romset_dialog(hwnd);
        return TRUE;
      case WM_NOTIFY:
        switch (((NMHDR FAR *)lparam)->code) {
          case PSN_KILLACTIVE:
            end_romset_dialog(hwnd);
            return TRUE;
        }
        return FALSE;
      case WM_COMMAND:
        switch (LOWORD(wparam)) {
          case IDC_ROMSET_SELECT_ARCHIVE:
          case IDC_ROMSET_SELECT_FILE:
            update_romset_dialog(hwnd, LOWORD(wparam));
            break;
          case IDC_ROMSET_ARCHIVE_BROWSE:
            uilib_select_browse(hwnd, translate_text(IDS_SELECT_ROMSET_ARCHIVE),
                                UILIB_FILTER_ROMSET_ARCHIVE,
                                UILIB_SELECTOR_TYPE_FILE_SAVE,
                                IDC_ROMSET_ARCHIVE_NAME);
            break;
          case IDC_ROMSET_ARCHIVE_LOAD:
            load_archive_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_ARCHIVE_SAVE:
            save_archive_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_ARCHIVE_APPLY:
            apply_archive_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_ARCHIVE_NEW:
            new_archive_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_ARCHIVE_DELETE:
            delete_archive_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_FILE_BROWSE:
            uilib_select_browse(hwnd, translate_text(IDS_SELECT_ROMSET_FILE),
                                UILIB_FILTER_ROMSET_FILE,
                                UILIB_SELECTOR_TYPE_FILE_SAVE,
                                IDC_ROMSET_FILE_NAME);
            break;
          case IDC_ROMSET_FILE_LOAD:
            load_file_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_FILE_SAVE:
            save_file_romset_dialog(hwnd);
            break;
          case IDC_ROMSET_RESOURCE_COMPUTER:
            uirom_resources_computer(hwnd);
            break;
          case IDC_ROMSET_RESOURCE_DRIVE:
            uirom_resources_drive(hwnd);
            break;
          case IDC_ROMSET_RESOURCE_OTHER:
            uirom_resources_other(hwnd);
            break;
        }
        return TRUE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
    }
    return FALSE;
}

void uirom_settings_dialog(HWND hwnd, unsigned int idd_dialog_main,
                           unsigned int idd_dialog_drive,
                           const unsigned int *idd_dialog_resources,
                           const uirom_settings_t *uirom_settings)
{
    PROPSHEETPAGE psp[3];
    PROPSHEETHEADER psh;

    settings = uirom_settings;
    romset_dialog_resources = idd_dialog_resources;

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
    psp[0].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
    psp[0].pszTemplate = MAKEINTRESOURCE(translate_res(IDD_ROMSET_SETTINGS_DIALOG));
    psp[0].pszIcon = NULL;
#else
    psp[0].DUMMYUNIONNAME.pszTemplate
        = MAKEINTRESOURCE(translate_res(IDD_ROMSET_SETTINGS_DIALOG));
    psp[0].u2.pszIcon = NULL;
#endif
    psp[0].lParam = 0;
    psp[0].pfnCallback = NULL;

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
    psp[1].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
    psp[1].pszTemplate = MAKEINTRESOURCE(idd_dialog_main);
    psp[1].pszIcon = NULL;
#else
    psp[1].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(idd_dialog_main);
    psp[1].u2.pszIcon = NULL;
#endif
    psp[1].lParam = 0;
    psp[1].pfnCallback = NULL;

    psp[2].dwSize = sizeof(PROPSHEETPAGE);
    psp[2].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
    psp[2].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
    psp[2].pszTemplate = MAKEINTRESOURCE(idd_dialog_drive);
    psp[2].pszIcon = NULL;
#else
    psp[2].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(idd_dialog_drive);
    psp[2].u2.pszIcon = NULL;
#endif
    psp[2].lParam = 0;
    psp[2].pfnCallback = NULL;

    psp[0].pfnDlgProc = dialog_proc_romset;
    psp[0].pszTitle = translate_text(IDS_ROMSET);
    psp[1].pfnDlgProc = dialog_proc_main;
    psp[1].pszTitle = translate_text(IDS_COMPUTER);
    psp[2].pfnDlgProc = dialog_proc_drive;
    psp[2].pszTitle = translate_text(IDS_DRIVE);

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.pszCaption = translate_text(IDS_ROM_SETTINGS);
    psh.nPages = 3;
#ifdef _ANONYMOUS_UNION
    psh.pszIcon = NULL;
    psh.nStartPage = 0;
    psh.ppsp = psp;
#else
    psh.DUMMYUNIONNAME.pszIcon = NULL;
    psh.u2.nStartPage = 0;
    psh.u3.ppsp = psp;
#endif
    psh.pfnCallback = NULL;

    PropertySheet(&psh);
}

