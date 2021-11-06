/* Copyright (C) 2020 SovietPony
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License ONLY.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef __EMSCRIPTEN__
#  include <emscripten.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h> // srand exit
#include <assert.h>
#include <time.h>
#include "system.h"
#include "input.h"

#include "player.h" // pl1 pl2
#include "menu.h" // G_keyf
#include "error.h" // logo
#include "monster.h" // nomon

#include "files.h" // F_startup F_addwad F_initwads F_allocres
#include "config.h" // CFG_args CFG_load CFG_save
#include "args.h" // ARG_parse
#include "memory.h" // M_startup
#include "game.h" // G_init G_act
#include "sound.h" // S_init S_done
#include "music.h" // S_initmusic S_updatemusic S_donemusic
#include "render.h" // R_init R_draw R_done

#define MODE_NONE 0
#define MODE_OPENGL 1
#define MODE_SOFTWARE 2

static clock_t ticks;
static int quit = 0;
static int mode = MODE_NONE;
static int text_input = 0;
static int width = 0;
static int height = 0;
static int fullscreen = 0;
static void *softbuffer = NULL;
static videomode_size_t vsize = { 320, 200, 0 };
static videomode_t vlist = { 1, &vsize };

static const cfg_t arg[] = {
  {"file", NULL, Y_FILES},
  {"cheat", &cheat, Y_SW_ON},
//  {"vga", &shot_vga, Y_SW_ON},
//  {"musvol", &mus_vol, Y_WORD},
  {"mon", &nomon, Y_SW_OFF},
  {"warp", &_warp, Y_BYTE},
//  {"config", NULL, cfg_file, Y_STRING},
  {NULL, NULL, 0} // end
};

static const cfg_t cfg[] = {
//  {"screenshot", &shot_vga, Y_SW_ON},
//  {"music_volume", &mus_vol, Y_WORD},
//  {"music_random", &music_random, Y_SW_ON},
//  {"music_time", &music_time, Y_DWORD},
//  {"music_fade", &music_fade, Y_DWORD},
  {"pl1_left", &pl1.kl, Y_KEY},
  {"pl1_right",&pl1.kr, Y_KEY},
  {"pl1_up", &pl1.ku, Y_KEY},
  {"pl1_down", &pl1.kd, Y_KEY},
  {"pl1_jump", &pl1.kj, Y_KEY},
  {"pl1_fire", &pl1.kf, Y_KEY},
  {"pl1_next", &pl1.kwr, Y_KEY},
  {"pl1_prev", &pl1.kwl, Y_KEY},
  {"pl1_use", &pl1.kp, Y_KEY},
  {"pl2_left", &pl2.kl, Y_KEY},
  {"pl2_right", &pl2.kr, Y_KEY},
  {"pl2_up", &pl2.ku, Y_KEY},
  {"pl2_down", &pl2.kd, Y_KEY},
  {"pl2_jump", &pl2.kj, Y_KEY},
  {"pl2_fire", &pl2.kf, Y_KEY},
  {"pl2_next", &pl2.kwr, Y_KEY},
  {"pl2_prev", &pl2.kwl, Y_KEY},
  {"pl2_use", &pl2.kp, Y_KEY},
  {NULL, NULL, 0} // end
};

static void CFG_args (int argc, char **argv) {
  const cfg_t *list[4];
  list[0] = arg;
  list[1] = R_args();
  list[2] = S_args();
  list[3] = MUS_args();
  ARG_parse(argc, argv, 4, list);
}

static void CFG_load (void) {
  const cfg_t *list[4];
  list[0] = cfg;
  list[1] = R_conf();
  list[2] = S_conf();
  list[3] = MUS_conf();
  CFG_read_config("default.cfg", 4, list);
  CFG_read_config("doom2d.cfg", 4, list);
}

static void CFG_save (void) {
  const cfg_t *list[4];
  list[0] = cfg;
  list[1] = R_conf();
  list[2] = S_conf();
  list[3] = MUS_conf();
  CFG_update_config("doom2d.cfg", "doom2d.cfg", 4, list, "generated by doom2d, do not modify");
}

/* --- error.h --- */

void logo (const char *s, ...) {
  va_list ap;
  va_start(ap, s);
  vprintf(s, ap);
  va_end(ap);
  fflush(stdout);
}

void logo_gas (int cur, int all) {
  // stub
}

void ERR_failinit (char *s, ...) {
  va_list ap;
  va_start(ap, s);
  vprintf(s, ap);
  va_end(ap);
  puts("");
  abort();
}

void ERR_fatal (char *s, ...) {
  va_list ap;
  R_done();
  MUS_done();
  S_done();
  puts("\nCRITICAL ERROR:");
  va_start(ap, s);
  vprintf(s, ap);
  va_end(ap);
  puts("");
  abort();
}

void ERR_quit (void) {
  quit = 1;
}

/* --- system.h --- */

int Y_set_videomode_opengl (int w, int h, int fullscreen) {
  assert(w > 0);
  assert(h > 0);
  mode = MODE_OPENGL;
  width = w;
  height = h;
  return 1;
}

int Y_set_videomode_software (int w, int h, int fullscreen) {
  void *buf;
  assert(w > 0);
  assert(h > 0);
  buf = softbuffer ? realloc(softbuffer, w * h) : malloc(w * h);
  if (buf) {
    mode = MODE_SOFTWARE;
    softbuffer = buf;
    width = w;
    height = h;
  }
  return buf != NULL;
}

const videomode_t *Y_get_videomode_list_opengl (int fullscreen) {
  return &vlist;
}

const videomode_t *Y_get_videomode_list_software (int fullscreen) {
  return &vlist;
}

void Y_get_videomode (int *w, int *h) {
  if (mode != MODE_NONE) {
    *w = width;
    *h = height;
  } else {
    *w = 0;
    *h = 0;
  }
}

int Y_videomode_setted (void) {
  return mode != MODE_NONE;
}

void Y_unset_videomode (void) {
  mode = MODE_NONE;
  free(softbuffer);
  softbuffer = NULL;
}

void Y_set_fullscreen (int yes) {
  fullscreen = yes;
}

int Y_get_fullscreen (void) {
  return (mode != MODE_NONE) && fullscreen;
}

void Y_swap_buffers (void) {
  assert(mode == MODE_OPENGL);
}

void Y_get_buffer (byte **buf, int *w, int *h, int *pitch) {
  assert(mode == MODE_SOFTWARE);
  *buf = softbuffer;
  *w = width;
  *h = height;
  *pitch = width;
}

void Y_set_vga_palette (byte *vgapal) {
  int i;
  //byte *p = vgapal;
  assert(vgapal != NULL);
  assert(mode == MODE_SOFTWARE);
}

void Y_repaint_rect (int x, int y, int w, int h) {
  assert(mode == MODE_SOFTWARE);
}

void Y_repaint (void) {
  assert(mode == MODE_SOFTWARE);
}

void Y_enable_text_input (void) {
  text_input = 1;
}

void Y_disable_text_input (void) {
  text_input = 0;
}

/* --- main --- */

static void poll_events (void) {
  // stub
}

static void step (void) {
  clock_t t;
  poll_events();
  MUS_update();
  t = clock();
  if ((t - ticks) * 1000 / CLOCKS_PER_SEC > DELAY) {
    ticks = t;
    G_act();
  }
  R_draw();
}

int main (int argc, char *argv[]) {
  logo("main: initialize\n");
  // Player 1 defaults
  pl1.ku = KEY_KP_8;
  pl1.kd = KEY_KP_5;
  pl1.kl = KEY_KP_4;
  pl1.kr = KEY_KP_6;
  pl1.kf = KEY_PAGEDOWN;
  pl1.kj = KEY_DELETE;
  pl1.kwl = KEY_HOME;
  pl1.kwr = KEY_END;
  pl1.kp = KEY_KP_8;
  // Player 2 defaults
  pl2.ku = KEY_E;
  pl2.kd = KEY_D;
  pl2.kl = KEY_S;
  pl2.kr = KEY_F;
  pl2.kf = KEY_A;
  pl2.kj = KEY_Q;
  pl2.kwl = KEY_1;
  pl2.kwr = KEY_2;
  pl2.kp = KEY_E;
  //srand(SDL_GetTicks());
  F_addwad("doom2d.wad");
  CFG_args(argc, argv);
  CFG_load();
  F_initwads();
  S_init();
  MUS_init();
  R_init();
  G_init();
  ticks = clock();
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(step, 0, 1);
#else
  while (!quit) {
    step();
  }
#endif
  CFG_save();
  R_done();
  MUS_done();
  S_done();
  return 0;
}