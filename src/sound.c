/*
 * sound.c - General code for the sound interface.
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *
 * Resource and cmdline code by
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
#include <time.h>

#include "archdep.h"
#include "clkguard.h"
#include "cmdline.h"
#include "debug.h"
#include "fixpoint.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "resources.h"
#include "sound.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "types.h"
#include "ui.h"
#include "util.h"
#include "vsync.h"


static log_t sound_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* ------------------------------------------------------------------------- */

/* Resource handling -- Added by Ettore 98-04-26.  */

/* FIXME: We need sanity checks!  And do we really need all of these
   `sound_close()' calls?  */

static int playback_enabled;          /* app_resources.sound */
static int sample_rate;               /* app_resources.soundSampleRate */
static char *device_name = NULL;      /* app_resources.soundDeviceName */
static char *device_arg = NULL;       /* app_resources.soundDeviceArg */
static char *recorddevice_name = NULL;/* app_resources.soundDeviceName */
static char *recorddevice_arg = NULL; /* app_resources.soundDeviceArg */
static int buffer_size;               /* app_resources.soundBufferSize */
static int suspend_time;              /* app_resources.soundSuspendTime */
static int speed_adjustment_setting;  /* app_resources.soundSpeedAdjustment */
static int oversampling_factor;       /* app_resources.soundOversample */
static int volume;

/* I need this to serialize close_sound and enablesound/sound_open in
   the OS/2 Multithreaded environment                              */
static int sdev_open = FALSE;

/* I need this to serialize close_sound and enablesound/sound_open in
   the OS/2 Multithreaded environment                              */
int sound_state_changed;
int sid_state_changed;

/* Sample based or cycle based sound engine. */
static int cycle_based = 0;

static int set_playback_enabled(int val, void *param)
{
    if (val)
        vsync_disable_timer();

    playback_enabled = val;
    sound_machine_enable(playback_enabled);
    return 0;
}

static int set_sample_rate(int val, void *param)
{
    sample_rate = val;
    sound_state_changed = TRUE;
    return 0;
}

static int set_device_name(const char *val, void *param)
{
    util_string_set(&device_name, val);
    sound_state_changed = TRUE;
    return 0;
}

static int set_device_arg(const char *val, void *param)
{
    util_string_set(&device_arg, val);
    sound_state_changed = TRUE;
    return 0;
}

static int set_recorddevice_name(const char *val, void *param)
{
    util_string_set(&recorddevice_name, val);
    sound_state_changed = TRUE;
    return 0;
}

static int set_recorddevice_arg(const char *val, void *param)
{
    util_string_set(&recorddevice_arg, val);
    sound_state_changed = TRUE;
    return 0;
}

static int set_buffer_size(int val, void *param)
{
    buffer_size   = val;
    sound_state_changed = TRUE;
    return 0;
}

static int set_suspend_time(int val, void *param)
{
    suspend_time = val;

    if (suspend_time < 0)
        suspend_time = 0;

    sound_state_changed = TRUE;
    return 0;
}

static int set_speed_adjustment_setting(int val, void *param)
{
    speed_adjustment_setting = val;
    return 0;
}

static int set_oversampling_factor(int val, void *param)
{
    oversampling_factor = val;

    if (oversampling_factor < 0 || oversampling_factor > 3) {
        log_warning(sound_log, "Invalid oversampling factor %d.  Forcing 3.",
                    oversampling_factor);
        oversampling_factor = 3;
    }

    sound_state_changed = TRUE;
    return 0;
}

static int set_volume(int val, void *param)
{
    volume = val;

    if (volume < 0)
        volume = 0;

    if (volume > 100)
        volume = 100;

    ui_display_volume(volume);

    return 0;
}

static const resource_string_t resources_string[] = {
    { "SoundDeviceName", "", RES_EVENT_NO, NULL,
      &device_name, set_device_name, NULL },
    { "SoundDeviceArg", "", RES_EVENT_NO, NULL,
      &device_arg, set_device_arg, NULL },
    { "SoundRecordDeviceName", "", RES_EVENT_STRICT, (resource_value_t)"",
      &recorddevice_name, set_recorddevice_name, NULL },
    { "SoundRecordDeviceArg", "", RES_EVENT_NO, NULL,
      &recorddevice_arg, set_recorddevice_arg, NULL },
    { NULL }
};

static const resource_int_t resources_int[] = {
    { "Sound", 1, RES_EVENT_SAME, NULL,
      (void *)&playback_enabled, set_playback_enabled, NULL },
    { "SoundSampleRate", SOUND_SAMPLE_RATE, RES_EVENT_NO, NULL,
      (void *)&sample_rate, set_sample_rate, NULL },
    { "SoundBufferSize", SOUND_SAMPLE_BUFFER_SIZE, RES_EVENT_NO, NULL,
      (void *)&buffer_size, set_buffer_size, NULL },
    { "SoundSuspendTime", 0, RES_EVENT_NO, NULL,
      (void *)&suspend_time, set_suspend_time, NULL },
    { "SoundSpeedAdjustment", SOUND_ADJUST_FLEXIBLE, RES_EVENT_NO, NULL,
      (void *)&speed_adjustment_setting, set_speed_adjustment_setting, NULL },
    { "SoundOversample", 0, RES_EVENT_NO, NULL,
      (void *)&oversampling_factor, set_oversampling_factor, NULL },
    { "SoundVolume", 100, RES_EVENT_NO, NULL,
      (void *)&volume, set_volume, NULL },
    { NULL }
};

int sound_resources_init(void)
{
    if (resources_register_string(resources_string) < 0)
        return -1;

    return resources_register_int(resources_int);
}

void sound_resources_shutdown(void)
{
    lib_free(device_name);
    lib_free(device_arg);
    lib_free(recorddevice_name);
    lib_free(recorddevice_arg);
}

/* ------------------------------------------------------------------------- */

#ifdef HAS_TRANSLATION
static const cmdline_option_t cmdline_options[] = {
    { "-sound", SET_RESOURCE, 0, NULL, NULL, "Sound", (resource_value_t)1,
      0, IDCLS_ENABLE_SOUND_PLAYBACK },
    { "+sound", SET_RESOURCE, 0, NULL, NULL, "Sound", (resource_value_t)0,
      0, IDCLS_DISABLE_SOUND_PLAYBACK },
    { "-soundrate", SET_RESOURCE, 1, NULL, NULL, "SoundSampleRate", NULL,
      IDCLS_P_VALUE, IDCLS_SET_SAMPLE_RATE_VALUE_HZ },
    { "-soundbufsize", SET_RESOURCE, 1, NULL, NULL, "SoundBufferSize", NULL,
      IDCLS_P_VALUE, IDCLS_SET_SOUND_BUFFER_SIZE_MSEC },
    { "-sounddev", SET_RESOURCE, 1, NULL, NULL, "SoundDeviceName", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_SOUND_DRIVER },
    { "-soundarg", SET_RESOURCE, 1, NULL, NULL, "SoundDeviceArg", NULL,
      IDCLS_P_ARGS, IDCLS_SPECIFY_SOUND_DRIVER_PARAM },
    { "-soundrecdev", SET_RESOURCE, 1, NULL, NULL, "SoundRecordDeviceName",
      NULL, IDCLS_P_NAME, IDCLS_SPECIFY_RECORDING_SOUND_DRIVER },
    { "-soundrecarg", SET_RESOURCE, 1, NULL, NULL, "SoundRecordDeviceArg", NULL,
      IDCLS_P_ARGS, IDCLS_SPECIFY_REC_SOUND_DRIVER_PARAM },
    { "-soundsync", SET_RESOURCE, 1, NULL, NULL, "SoundSpeedAdjustment", NULL,
      IDCLS_P_SYNC, IDCLS_SET_SOUND_SPEED_ADJUST },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] = {
    { "-sound", SET_RESOURCE, 0, NULL, NULL, "Sound", (resource_value_t)1,
      NULL, N_("Enable sound playback") },
    { "+sound", SET_RESOURCE, 0, NULL, NULL, "Sound", (resource_value_t)0,
      NULL, N_("Disable sound playback") },
    { "-soundrate", SET_RESOURCE, 1, NULL, NULL, "SoundSampleRate", NULL,
      N_("<value>"), N_("Set sound sample rate to <value> Hz") },
    { "-soundbufsize", SET_RESOURCE, 1, NULL, NULL, "SoundBufferSize", NULL,
      N_("<value>"), N_("Set sound buffer size to <value> msec") },
    { "-sounddev", SET_RESOURCE, 1, NULL, NULL, "SoundDeviceName", NULL,
      N_("<name>"), N_("Specify sound driver") },
    { "-soundarg", SET_RESOURCE, 1, NULL, NULL, "SoundDeviceArg", NULL,
      N_("<args>"), N_("Specify initialization parameters for sound driver") },
    { "-soundrecdev", SET_RESOURCE, 1, NULL, NULL, "SoundRecordDeviceName",
      NULL, N_("<name>"), N_("Specify recording sound driver") },
    { "-soundrecarg", SET_RESOURCE, 1, NULL, NULL, "SoundRecordDeviceArg", NULL,
      N_("<args>"), N_("Specify initialization parameters for recording sound driver") },
    { "-soundsync", SET_RESOURCE, 1, NULL, NULL, "SoundSpeedAdjustment", NULL,
      N_("<sync>"), N_("Set sound speed adjustment (0: flexible, 1: adjusting, 2: exact)") },
    { NULL }
};
#endif

int sound_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* timing constants */
static unsigned int cycles_per_sec;
static unsigned int cycles_per_rfsh;
static double rfsh_per_sec;

/* Speed in percent, tracks relative_speed from vsync.c */
static int speed_percent;

/* Flag: Is warp mode enabled?  */
static int warp_mode_enabled;

typedef struct
{
    /* Number of sound channels (for multiple SIDs) */
    int channels;

    /* sid itself */
    sound_t *psid[SOUND_CHANNELS_MAX];

    /* number of clocks between each sample. used value */
    soundclk_t clkstep;

    /* number of clocks between each sample. original value */
    soundclk_t origclkstep;

    /* factor between those two clksteps */
    soundclk_t clkfactor;

    /* time of last sample generated */
    soundclk_t fclk;

    /* time of last write to sid. used for pdev->dump() */
    CLOCK wclk;

    /* time of last call to sound_run_sound() */
    CLOCK lastclk;

    /* sample buffer */
    SWORD buffer[SOUND_CHANNELS_MAX * SOUND_BUFSIZE];

    /* sample buffer pointer */
    int bufptr;

    /* pointer to playback device structure in use */
    sound_device_t *playdev;

    /* pointer to playback device structure in use */
    sound_device_t *recdev;

    /* number of samples in a fragment */
    int fragsize;

    /* number of fragments in kernel buffer */
    int fragnr;

    /* number of samples in kernel buffer */
    int bufsize;

    /* constants related to adjusting sound */
    int prevused;
    int prevfill;

    /* is the device suspended? */
    int issuspended;
    SWORD lastsample[SOUND_CHANNELS_MAX];

    /* nr of samples to oversame / real sample */
    int oversamplenr;

    /* number of shift needed on oversampling */
    int oversampleshift;
} snddata_t;

static snddata_t snddata;

/* device registration code */
static sound_device_t *sound_devices[32];

static char *devlist;

int sound_register_device(sound_device_t *pdevice)
{
    const int max = sizeof(sound_devices) / sizeof(sound_devices[0]);
    int i;
    char *tmplist;

    for (i = 0; sound_devices[i] && i < max; i++);

    if (i < max) {
        sound_devices[i] = pdevice;
        tmplist = lib_msprintf("%s %s", devlist, pdevice->name);
        lib_free(devlist);
        devlist = tmplist;
    } else {
        log_error(sound_log, "available sound devices exceed VICEs storage");
    }

    return 0;
}

unsigned int sound_device_num(void)
{
    const unsigned int max = sizeof(sound_devices) / sizeof(sound_devices[0]);
    unsigned int i;

    for (i = 0; sound_devices[i] && i < max; i++);

    return i;
}

const char *sound_device_name(unsigned int num)
{
    return sound_devices[num]->name;
}


/* code to disable sid for a given number of seconds if needed */
static int disabletime;

static void suspendsound(const char *reason)
{
    disabletime = time(0);
    log_warning(sound_log, "suspend, disabling sound for %d secs (%s)",
         suspend_time, reason);
    sound_state_changed = TRUE;
}

static void enablesound(void)
{
    int diff;
    if (!disabletime)
        return;
    diff = time(0) - disabletime;
    if (diff < 0 || diff >= suspend_time) {
        disabletime = 0;
    }
}

/* close sid device and show error dialog */
static int sound_error(const char *msg)
{
    sound_close();

    if (console_mode || vsid_mode) {
        log_message(sound_log, msg);
    } else {
        char *txt = lib_msprintf("Sound: %s", msg);
        ui_error(txt);
        lib_free(txt);
    }

    playback_enabled = 0;

    if (!console_mode)
        ui_update_menus();

    return 1;
}

/* Fill buffer with last sample.
 rise  < 0 : attenuation
 rise == 0 : constant value
 rise  > 0 : gain
*/
static void fill_buffer(int size, int rise)
{
    int c, i;
    SWORD *p;

    p = (SWORD *)lib_malloc(size * sizeof(SWORD) * snddata.channels);

    if (!p)
        return;

    for (c = 0; c < snddata.channels; c++) {
        for (i = 0; i < size; i++) {
            double factor;
            if (rise < 0)
                factor = (double)(size - i) / size;
            else
                if (rise > 0)
                    factor = (double)i / size;
                else
                    factor = 1.0;

            p[i * snddata.channels + c] = (SWORD)(snddata.lastsample[c]
                                          * factor);
        }
    }

    i = snddata.playdev->write(p, size * snddata.channels);

    lib_free(p);

    if (i)
#ifdef HAS_TRANSLATION
        sound_error(translate_text(IDGS_WRITE_TO_SOUND_DEVICE_FAILED));
#else
        sound_error(_("write to sound device failed."));
#endif
}


/* open SID engine */
static int sid_open(void)
{
    int c;

    for (c = 0; c < snddata.channels; c++) {
        if (!(snddata.psid[c] = sound_machine_open(c))) {
#ifdef HAS_TRANSLATION
            return sound_error(translate_text(IDGS_CANNOT_OPEN_SID_ENGINE));
#else
            return sound_error(_("Cannot open SID engine"));
#endif
        }
    }

    return 0;
}

/* initialize SID engine */
static int sid_init(void)
{
    int c, speed;

    /* Special handling for cycle based as opposed to sample based sound
       engines. reSID is cycle based. */
    cycle_based = sound_machine_cycle_based();

    /* Cycle based sound engines must do their own filtering,
       and handle sample rate conversion. */
    if (cycle_based) {
        /* "No limit" doesn't make sense for cycle based sound engines,
           which have a fixed sampling rate. */
        int speed_factor = speed_percent ? speed_percent : 100;
        snddata.oversampleshift = 0;
        snddata.oversamplenr = 1;
        speed = sample_rate * 100 / speed_factor;
    } else {
        /* For sample based sound engines, both simple average filtering
           and sample rate conversion is handled here. */
        snddata.oversampleshift = oversampling_factor;
        snddata.oversamplenr = 1 << snddata.oversampleshift;
        speed = sample_rate*snddata.oversamplenr;
    }

    for (c = 0; c < snddata.channels; c++) {
        if (!sound_machine_init(snddata.psid[c], speed, cycles_per_sec)) {
#ifdef HAS_TRANSLATION
            return sound_error(translate_text(IDGS_CANNOT_INIT_SID_ENGINE));
#else
            return sound_error(_("Cannot initialize SID engine"));
#endif
        }
    }

    snddata.clkstep = SOUNDCLK_CONSTANT(cycles_per_sec) / sample_rate;

    if (snddata.oversamplenr > 1) {
        snddata.clkstep /= snddata.oversamplenr;
        log_message(sound_log, "Using %dx oversampling",
                    snddata.oversamplenr);
    }

    snddata.origclkstep = snddata.clkstep;
    snddata.clkfactor = SOUNDCLK_CONSTANT(1.0);
    snddata.fclk = SOUNDCLK_CONSTANT(maincpu_clk);
    snddata.wclk = maincpu_clk;
    snddata.lastclk = maincpu_clk;

    return 0;
}

/* close SID engine */
static void sid_close(void)
{
    int c;
    for (c = 0; c < snddata.channels; c++) {
        if (snddata.psid[c]) {
            sound_machine_close(snddata.psid[c]);
            snddata.psid[c] = NULL;
        }
    }
}

sound_t *sound_get_psid(unsigned int channel)
{
    return snddata.psid[channel];
}

/* open sound device */
int sound_open(void)
{
    int c, i, j;
    sound_device_t *pdev, *rdev;
    char *playname, *recname;
    char *playparam, *recparam;
    int speed;
    int fragsize;
    int fragnr;
    double bufsize;

    if (suspend_time > 0 && disabletime)
        return 1;

    /* Opening the sound device and initializing the sound engine
       might take some time. */
    vsync_suspend_speed_eval();

    /* Second SID. */
    snddata.channels = sound_machine_channels();

    playname = device_name;
    if (playname && playname[0] == '\0')
        playname = NULL;

    playparam = device_arg;
    if (playparam && playparam[0] == '\0')
        playparam = NULL;

    recname = recorddevice_name;
    if (recname && recname[0] == '\0')
        recname = NULL;

    recparam = recorddevice_arg;
    if (recparam && recparam[0] == '\0')
        recparam = NULL;

    /* Calculate buffer size in seconds. */
    bufsize = ((buffer_size < 100 || buffer_size > 1000)
              ? SOUND_SAMPLE_BUFFER_SIZE : buffer_size) / 1000.0;
    speed = (sample_rate < 8000 || sample_rate > 96000)
            ? SOUND_SAMPLE_RATE : sample_rate;

    /* Calculate optimal fragments.
       fragsize is rounded up to 2^i.
       fragnr is rounded up to bufsize/fragsize. */
    fragsize = speed / ((rfsh_per_sec < 1.0) ? 1 : ((int)rfsh_per_sec));
    for (i = 1; 1 << i < fragsize; i++);
    fragsize = 1 << i;
    fragnr = (int)((speed * bufsize + fragsize - 1) / fragsize);
    if (fragnr < 3)
        fragnr = 3;

    for (i = 0; (pdev = sound_devices[i]); i++) {
        if (!playname || (pdev->name && !strcasecmp(playname, pdev->name)))
            break;
    }

    if (pdev) {
        if (pdev->init) {
            int channels_cap = snddata.channels;
            if (pdev->init(playparam, &speed, &fragsize, &fragnr, &channels_cap)) {
                char *err;
#ifdef HAS_TRANSLATION
                err = lib_msprintf(translate_text(IDGS_INIT_FAILED_FOR_DEVICE_S),
                                   pdev->name);
#else
                err = lib_msprintf(_("initialization failed for device `%s'."),
                                   pdev->name);
#endif
                sound_error(err);
                lib_free(err);
                return 1;
            }
            if (channels_cap != snddata.channels) {
                log_warning(sound_log,
                            _("sound device lacks stereo capability"));
                snddata.channels = 1;
            }
        }
        snddata.issuspended = 0;

        for (c = 0; c < snddata.channels; c++) {
            snddata.lastsample[c] = 0;
        }

        snddata.playdev = pdev;
        snddata.fragsize = fragsize;
        snddata.fragnr = fragnr;
        snddata.bufsize = fragsize*fragnr;
        snddata.bufptr = 0;
        log_message(sound_log,
                    "Opened device `%s', speed %dHz, fragment size %dms, buffer size %dms%s",
                    pdev->name, speed,
                    (int)(1000.0 * fragsize / speed),
                    (int)(1000.0 * snddata.bufsize / speed),
                    snddata.channels > 1 ? ", stereo" : "");
        sample_rate = speed;

        if (sid_open() != 0 || sid_init() != 0) {
            return 1;
        }

        sid_state_changed = FALSE;

        /* Set warp mode for non-realtime sound devices in vsid mode. */
        if (vsid_mode && !pdev->bufferspace) {
            resources_set_int("WarpMode", 1);
        }

        /* Fill up the sound hardware buffer. */
        if (pdev->bufferspace) {
            /* Fill to bufsize - fragsize. */
            j = pdev->bufferspace() - snddata.fragsize;
            if (j > 0) {
                /* Whole fragments. */
                j -= j % snddata.fragsize;

                fill_buffer(j, 0);
            }
        }
    } else {
#ifdef HAS_TRANSLATION
        char *err = lib_msprintf(translate_text(IDGS_DEVICE_S_NOT_FOUND_SUPPORT),
                                 playname);
#else
        char *err = lib_msprintf(_("device '%s' not found or not supported."),
                                 playname);
#endif
        sound_error(err);
        lib_free(err);
        return 1;
    }

    /* now the playback sound device is open */
    sdev_open = TRUE;
    sound_state_changed = FALSE;

    for (i = 0; (rdev = sound_devices[i]); i++) {
        if (recname && rdev->name && !strcasecmp(recname, rdev->name))
            break;
    }

    if (recname && rdev == NULL)
#ifdef HAS_TRANSLATION
        ui_error(translate_text(IDGS_RECORD_DEVICE_S_NOT_EXIST), recname);
#else
        ui_error(_("Recording device %s doesn't exist!"), recname);
#endif

    if (rdev) {
        if (rdev == pdev) {
#ifdef HAS_TRANSLATION
            ui_error(translate_text(IDGS_RECORD_DIFFERENT_PLAYBACK));
#else
            ui_error(_("Recording device must be different from playback device"));
#endif
            resources_set_string("SoundRecordDeviceName", "");
            return 0;
        }

        if (rdev->bufferspace != NULL) {
#ifdef HAS_TRANSLATION
            ui_error(translate_text(IDGS_WARNING_RECORDING_REALTIME));
#else
            ui_error(_("Warning! Recording device %s seems to be a realtime device!"));
#endif
        }

        if (rdev->init) {
            int channels_cap = snddata.channels;
            if (rdev->init(recparam, &speed, &fragsize, &fragnr, &channels_cap)) {
#ifdef HAS_TRANSLATION
                ui_error(translate_text(IDGS_INIT_FAILED_FOR_DEVICE_S),
                                   rdev->name);
#else
                ui_error(_("Initialization failed for device `%s'."),
                                   rdev->name);
#endif
                resources_set_string("SoundRecordDeviceName", "");
                return 0;
            }

            if (sample_rate != speed
                || snddata.fragsize != fragsize
                || snddata.fragnr != fragnr
                || snddata.channels != channels_cap)
            {
#ifdef HAS_TRANSLATION
                ui_error(translate_text(IDGS_RECORD_NOT_SUPPORT_SOUND_PAR));
#else
                ui_error(_("The recording device doesn't support current sound parameters"));
#endif
                rdev->close();
                resources_set_string("SoundRecordDeviceName", "");
            } else {
                snddata.recdev = rdev;
                log_message(sound_log,
                    "Opened recording device device `%s'", rdev->name);
            }
        }
    }
    return 0;
}

/* close sid */
void sound_close(void)
{
    if (snddata.playdev) {
        log_message(sound_log, "Closing device `%s'", snddata.playdev->name);
        if (snddata.playdev->close)
            snddata.playdev->close();
        snddata.playdev = NULL;
    }

    if (snddata.recdev) {
        log_message(sound_log, "Closing recording device `%s'", snddata.recdev->name);
        if (snddata.recdev->close)
            snddata.recdev->close();
        snddata.recdev = NULL;
    }

    sid_close();

    snddata.prevused = snddata.prevfill = 0;

    sdev_open = FALSE;
    sound_state_changed = FALSE;

    /* Closing the sound device might take some time, and displaying
       UI dialogs certainly does. */
    vsync_suspend_speed_eval();
}

/* run sid */
static int sound_run_sound(void)
{
    int nr = 0, c, i;
    int delta_t = 0;
    SWORD *bufferptr;

    /* XXX: implement the exact ... */
    if (!playback_enabled || (suspend_time > 0 && disabletime))
        return 1;

    if (!snddata.playdev) {
        i = sound_open();
        if (i)
            return i;
    }

#ifdef __riscos
    /* RISC OS vidc device uses a different approach... */
    SoundMachineReady = 1;
    if (SoundThreadActive != 0)
        return 0;
#endif

    /* Handling of cycle based sound engines. */
    if (cycle_based) {
        for (c = 0; c < snddata.channels; c++) {
            delta_t = maincpu_clk - snddata.lastclk;
            bufferptr = snddata.buffer + snddata.bufptr * snddata.channels + c;
            nr = sound_machine_calculate_samples(snddata.psid[c],
                                                 bufferptr,
                                                 SOUND_BUFSIZE - snddata.bufptr,
                                                 snddata.channels,
                                                 &delta_t);
            if (volume < 100) {
                for (i = 0; i < nr; i ++)
                    bufferptr[i] = (volume!=0) ? 
                                   (bufferptr[i]/(100 / volume)) : 0;
            }

            if (delta_t) {
#ifdef HAS_TRANSLATION
                return sound_error(translate_text(IDGS_SOUND_BUFFER_OVERFLOW_CYCLE));
#else
                return sound_error(_("Sound buffer overflow (cycle based)"));
#endif
            }
        }
    } else {
        /* Handling of sample based sound engines. */
        nr = (int)((SOUNDCLK_CONSTANT(maincpu_clk) - snddata.fclk)
             / snddata.clkstep);
        if (!nr)
            return 0;
        if (snddata.bufptr + nr > SOUND_BUFSIZE) {
#ifdef HAS_TRANSLATION
            return sound_error(translate_text(IDGS_SOUND_BUFFER_OVERFLOW));
#else
            return sound_error(_("Sound buffer overflow."));
#endif
        }
        for (c = 0; c < snddata.channels; c++) {
            bufferptr = snddata.buffer + snddata.bufptr * snddata.channels + c;
            sound_machine_calculate_samples(snddata.psid[c],
                                            bufferptr,
                                            nr,
                                            snddata.channels,
                                            &delta_t);
            if (volume < 100) {
                for (i = 0; i < nr; i ++)
                    bufferptr[i] = (volume != 0) ?
                                   (bufferptr[i] / (100 / volume)) : 0;
            }
        }
        snddata.fclk += nr * snddata.clkstep;
    }

    snddata.bufptr += nr;
    snddata.lastclk = maincpu_clk;

    return 0;
}

/* reset sid */
void sound_reset(void)
{
    int c;

    snddata.fclk = SOUNDCLK_CONSTANT(maincpu_clk);
    snddata.wclk = maincpu_clk;
    snddata.lastclk = maincpu_clk;
    snddata.bufptr = 0;         /* ugly hack! */
    for (c = 0; c < snddata.channels; c++) {
        if (snddata.psid[c])
            sound_machine_reset(snddata.psid[c], maincpu_clk);
    }
}

static void prevent_clk_overflow_callback(CLOCK sub, void *data)
{
    int c;

    snddata.lastclk -= sub;
    snddata.fclk -= SOUNDCLK_CONSTANT(sub);
    snddata.wclk -= sub;
    for (c = 0; c < snddata.channels; c++) {
        if (snddata.psid[c])
            sound_machine_prevent_clk_overflow(snddata.psid[c], sub);
    }
}

#ifdef __riscos
void sound_synthesize(SWORD *buffer, int length)
{
    /* Handling of cycle based sound engines. */
    if (cycle_based) {
        /* FIXME: This is not implemented yet. A possible solution is
        to make the main thread call sound_run at shorter intervals,
        and reduce the responsibility of the sound thread to only
        flush the sample buffer. On the other hand if sound_run were
        called at shorter intervals the sound thread would probably
        not be necessary at all. */
        snddata.lastclk = maincpu_clk;
    }
    /* Handling of sample based sound engines. */
    else {
        int delta_t = 0;
        int c;
        for (c = 0; c < snddata.channels; c++) {
            sound_machine_calculate_samples(snddata.psid[c], buffer + c,
                                            length, snddata.channels,
                                            &delta_t);
        }
        snddata.fclk += length * snddata.clkstep;
    }
}
#endif

/* flush all generated samples from buffer to sounddevice. adjust sid runspeed
   to match real running speed of program */
#if defined(__MSDOS__) || defined(__riscos)
int sound_flush(int relative_speed)
#else
double sound_flush(int relative_speed)
#endif
{
    int c, i, nr, space = 0, used;

    if (!playback_enabled) {
        if (sdev_open)
            sound_close();
        return 0;
    }

    if (sound_state_changed) {
        if (sdev_open)
            sound_close();
        sound_state_changed = FALSE;
    }

    if (suspend_time > 0)
        enablesound();
    if (sound_run_sound())
        return 0;

    if (sid_state_changed) {
        if (sid_init() != 0) {
            return 0;
        }
        sid_state_changed = FALSE;
    }

    if (warp_mode_enabled 
        && snddata.playdev->bufferspace != NULL
        && snddata.recdev == NULL) {
        snddata.bufptr = 0;
        return 0;
    }
    sound_resume();

    if (snddata.playdev->flush) {
        char *state = sound_machine_dump_state(snddata.psid[0]);
        i = snddata.playdev->flush(state);
        lib_free(state);
        if (i) {
#ifdef HAS_TRANSLATION
            sound_error(translate_text(IDGS_CANNOT_FLUSH));
#else
            sound_error(_("cannot flush."));
#endif
            return 0;
        }
    }

    /* Calculate the number of samples to flush - whole fragments. */
    nr = snddata.bufptr -
         snddata.bufptr % (snddata.fragsize * snddata.oversamplenr);
    if (!nr)
        return 0;

    /* handle oversampling */
    if (snddata.oversamplenr > 1) {
        int j, newnr;

        newnr = nr >> snddata.oversampleshift;

        /* Simple average filtering. */
        for (c = 0; c < snddata.channels; c++) {
            SDWORD v;
            for (i = 0; i < newnr; i++) {
                for (v = j = 0; j < snddata.oversamplenr; j++)
                    v += snddata.buffer[(i * snddata.oversamplenr + j)
                         * snddata.channels + c];
                snddata.buffer[i * snddata.channels + c] =
                    v >> snddata.oversampleshift;
            }
        }

        /* Move remaining n % oversamplenr samples to new end of buffer. */
        for (c = 0; c < snddata.channels; c++) {
            for (i = 0; i < snddata.bufptr - nr; i++)
                snddata.buffer[(newnr + i) * snddata.channels + c] =
                      snddata.buffer[(nr + i) * snddata.channels + c];
        }
        snddata.bufptr -= (nr - newnr);
        nr = newnr;
    }
    /* adjust speed */
    if (snddata.playdev->bufferspace) {
        space = snddata.playdev->bufferspace();
        if (space < 0 || space > snddata.bufsize) {
            log_warning(sound_log, "fragment problems %d %d",
                 space, snddata.bufsize);

#ifdef HAS_TRANSLATE
            sound_error(translate_text(IDGS_FRAGMENT_PROBLEMS));
#else
            sound_error(_("fragment problems."));
#endif
            return 0;
        }
        used = snddata.bufsize - space;
        /* buffer empty */
        if (used <= snddata.fragsize) {
            int j;
            static int prev;
            int now;
            if (suspend_time > 0) {
                now = time(0);
                if (now == prev) {
                    suspendsound("buffer overruns");
                    return 0;
                }
                prev = now;
            }

            /* Calculate unused space in buffer. Leave one fragment. */
            j = snddata.bufsize - snddata.fragsize;

            /* Fill up sound hardware buffer. */
            if (j > 0) {
                fill_buffer(j, 0);
            }
            snddata.prevfill = j;

            /* Fresh start for vsync. */
#ifndef DEBUG
            log_warning(sound_log, _("Buffer drained"));
#endif
            vsync_sync_reset();
            return 0;
        }
        if (cycle_based || speed_adjustment_setting
            != SOUND_ADJUST_ADJUSTING) {
            if (relative_speed > 0)
                snddata.clkfactor = SOUNDCLK_CONSTANT(relative_speed) / 100;
        } else {
            if (snddata.prevfill)
                snddata.prevused = used;
            snddata.clkfactor = SOUNDCLK_MULT(snddata.clkfactor,
                                              SOUNDCLK_CONSTANT(1.0)
                                              + (SOUNDCLK_CONSTANT(0.9)
                                              *(used - snddata.prevused))
                                              / snddata.bufsize);
        }
        snddata.prevused = used;
        snddata.prevfill = 0;

        if (!cycle_based && speed_adjustment_setting != SOUND_ADJUST_EXACT
            && snddata.recdev == NULL) {
            snddata.clkfactor = SOUNDCLK_MULT(snddata.clkfactor,
                                              SOUNDCLK_CONSTANT(0.9)
                                              + ((used+nr)
                                              * SOUNDCLK_CONSTANT(0.12))
                                              / snddata.bufsize);
        }
        snddata.clkstep = SOUNDCLK_MULT(snddata.origclkstep,
                                        snddata.clkfactor);
        if (SOUNDCLK_CONSTANT(cycles_per_rfsh) / snddata.clkstep
            >= snddata.bufsize) {
            if (suspend_time > 0)
                suspendsound("running too slow");
            else {
#ifdef HAS_TRANSLATION
                sound_error(translate_text(IDGS_RUNNING_TOO_SLOW));
#else
                sound_error(_("running too slow."));
#endif
            }
            return 0;
        }

        /* Don't block on write unless we're seriously out of sync. */
        if (nr > space && nr < used)
            nr = space;
    }

    /* Flush buffer, all channels are already mixed into it. */
    if (snddata.playdev->write(snddata.buffer, nr * snddata.channels)) {
#ifdef HAS_TRANSLATION
        sound_error(translate_text(IDGS_WRITE_TO_SOUND_DEVICE_FAILED));
#else
        sound_error(_("write to sounddevice failed."));
#endif
        return 0;
    }

    if (snddata.recdev) {
        if (snddata.recdev->write(snddata.buffer, nr * snddata.channels)) {
#ifdef HAS_TRANSLATION
            sound_error(translate_text(IDGS_WRITE_TO_SOUND_DEVICE_FAILED));
#else
            sound_error(_("write to sounddevice failed."));
#endif
            return 0;
        }
    }

    snddata.bufptr -= nr;

    for (c = 0; c < snddata.channels; c++) {
        snddata.lastsample[c] = snddata.buffer[(nr - 1)
                                * snddata.channels + c];
        for (i = 0; i < snddata.bufptr; i++) {
            snddata.buffer[i*snddata.channels + c] =
                snddata.buffer[(i + nr)*snddata.channels + c];
        }
    }

    if (snddata.playdev->bufferspace
        && (cycle_based || speed_adjustment_setting == SOUND_ADJUST_EXACT))
#if defined(__MSDOS__) || (__riscos)
    {
        /* finetune VICE timer */
        static int lasttime = 0;
        int t = time(0);
        if (t != lasttime) {
            /* Aim for utilization of bufsize - fragsize. */
            int dir = 0;
            int remspace = space - snddata.bufptr;
            if (remspace <= 0)
                dir = -1;
            if (remspace > snddata.fragsize)
                dir = 1;
            lasttime = t;
            return dir;
        }
    }
#else
    {
        /* finetune VICE timer */
        /* Read bufferspace() just before returning to minimize the possibility
           of getting interrupted before vsync delay calculation. */
        /* Aim for utilization of bufsize - fragsize. */
        int remspace =
            snddata.playdev->bufferspace() - snddata.fragsize - snddata.bufptr;
        /* Return delay in seconds. */
        return (double)remspace/sample_rate;
    }
#endif

    return 0;
}

/* suspend sid (eg. before pause) */
void sound_suspend(void)
{
    if (!snddata.playdev)
        return;

    if (snddata.playdev->write && !snddata.issuspended
        && snddata.playdev->need_attenuation) {
        fill_buffer(snddata.fragsize, -1);
    }
    if (snddata.playdev->suspend && !snddata.issuspended) {
        if (snddata.playdev->suspend())
            return;
    }
    snddata.issuspended = 1;
}

/* resume sid */
void sound_resume(void)
{
    if (!snddata.playdev)
        return;

    if (snddata.issuspended) {
        if (snddata.playdev->resume) {
            snddata.issuspended = snddata.playdev->resume();
        }
        else {
            snddata.issuspended = 0;
        }

        if (snddata.playdev->write && !snddata.issuspended
            && snddata.playdev->need_attenuation) {
            fill_buffer(snddata.fragsize, 1);
        }
    }
}

/* set PAL/NTSC clock speed */
void sound_set_machine_parameter(long clock_rate, long ticks_per_frame)
{
    sid_state_changed = TRUE;

    cycles_per_sec  = clock_rate;
    cycles_per_rfsh = ticks_per_frame;
    rfsh_per_sec    = (1.0 /
                      ((double)cycles_per_rfsh / (double)cycles_per_sec));
}

/* initialize sid at program start -time */
void sound_init(unsigned int clock_rate, unsigned int ticks_per_frame)
{
    sound_log = log_open("Sound");

    sound_state_changed = FALSE;
    sid_state_changed = FALSE;

    cycles_per_sec = clock_rate;
    cycles_per_rfsh = ticks_per_frame;
    rfsh_per_sec = (1.0 / ((double)cycles_per_rfsh / (double)cycles_per_sec));

    clk_guard_add_callback(maincpu_clk_guard, prevent_clk_overflow_callback,
                           NULL);

    devlist = lib_stralloc("");

#ifdef USE_ARTS
    sound_init_arts_device();
#endif
#ifdef USE_ALSA
    sound_init_alsa_device();
#endif
#ifdef USE_COREAUDIO
    sound_init_coreaudio_device();
#endif
#ifdef USE_OSS

/* don't use oss for FreeBSD or BSDI */

#if !defined(__FreeBSD__) && !defined(__bsdi__)
    sound_init_uss_device();
#endif
#endif
#ifdef USE_ESD
    sound_init_esd_device();
#endif
#ifdef USE_DMEDIA
    sound_init_sgi_device();
#endif

/* Don't use the NetBSD/SUN sound driver for OpenBSD */
#if defined(HAVE_SYS_AUDIOIO_H) && !defined(__OpenBSD__)
    sound_init_sun_device();
#endif
#if defined(HAVE_SYS_AUDIO_H)
    sound_init_hpux_device();
#endif
#ifdef USE_AIX_AUDIO
    sound_init_aix_device();
#endif
#ifdef USE_SDL_AUDIO
    sound_init_sdl_device();
#endif

#ifdef __MSDOS__
#ifdef USE_MIDAS_SOUND
    sound_init_midas_device();
#else
    sound_init_allegro_device();
#endif
#endif

#ifdef WIN32
    sound_init_dx_device();
    sound_init_wmm_device();
#endif

#ifdef WINCE
    sound_init_ce_device();
#endif

#ifdef __OS2__
    // sound_init_mmos2_device();
    sound_init_dart_device();
    // sound_init_dart2_device();
#endif

#ifdef __BEOS__
        sound_init_beos_device();
#endif

#ifdef __riscos
    sound_init_vidc_device();
#endif

#ifdef PSP
    sound_init_psp_device();
#endif

#if defined(AMIGA_SUPPORT) && defined(HAVE_DEVICES_AHI_H)
    sound_init_ahi_device();
#endif

    sound_init_dummy_device();
    sound_init_fs_device();
    sound_init_speed_device();
    sound_init_dump_device();
    sound_init_wav_device();
    sound_init_voc_device();
    sound_init_iff_device();
    sound_init_aiff_device();

#ifdef USE_LAMEMP3
    sound_init_mp3_device();
#endif

#ifdef HAVE_FFMPEG
    sound_init_ffmpegaudio_device();
#endif

#if 0
    sound_init_test_device();   /* XXX: missing */
#endif

    log_message(sound_log, "Available sound devices:%s", devlist);

    lib_free(devlist);
}

long sound_sample_position(void)
{
    return (snddata.clkstep == 0)
        ? 0 : (long)((SOUNDCLK_CONSTANT(maincpu_clk) - snddata.fclk)
        / snddata.clkstep);
}

int sound_read(WORD addr, int chipno)
{
    if (sound_run_sound())
        return -1;

    if (chipno >= snddata.channels)
        return -1;

    return sound_machine_read(snddata.psid[chipno], addr);
}

void sound_store(WORD addr, BYTE val, int chipno)
{
    int i;

    if (sound_run_sound())
        return;

    if (chipno >= snddata.channels)
        return;

    sound_machine_store(snddata.psid[chipno], addr, val);

    if (!snddata.playdev->dump)
        return;

    i = snddata.playdev->dump(addr, val, maincpu_clk - snddata.wclk);

    snddata.wclk = maincpu_clk;

    if (i)
#ifdef HAS_TRANSLATION
        sound_error(translate_text(IDGS_STORE_SOUNDDEVICE_FAILED));
#else
        sound_error(_("store to sounddevice failed."));
#endif
}


void sound_set_relative_speed(int value)
{
    if (value != speed_percent)
        sid_state_changed = TRUE;

    speed_percent = value;
}

void sound_set_warp_mode(int value)
{
    warp_mode_enabled = value;

    if (value)
        sound_suspend();
    else
        sound_resume();
}

void sound_snapshot_prepare(void)
{
    /* Update lastclk.  */
    sound_run_sound();
}

void sound_snapshot_finish(void)
{
    snddata.lastclk = maincpu_clk;
}

