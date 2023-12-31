/* Copyright (C) 1996-1997 Aleksey Volynskov
 * Copyright (C) 2011 Rambo
 * Copyright (C) 2020 SovietPony
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

#include "glob.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "files.h"
#include "memory.h"
#include "sound.h"
#include "view.h"
#include "bmap.h"
#include "fx.h"
#include "switch.h"
#include "weapons.h"
#include "items.h"
#include "dots.h"
#include "smoke.h"
#include "player.h"
#include "monster.h"
#include "menu.h"
#include "misc.h"
#include "map.h"
#include "game.h"
#include "config.h"
#include "music.h"
#include "a8.h"
#include "error.h"
#include "input.h"

#include "save.h"

#define LT_DELAY 8
#define LT_HITTIME 6

#define GETIME 1092

byte transdraw;
byte _2pl;
byte g_dm;
byte g_st = GS_TITLE;
byte g_exit;
byte g_map = 1;
char g_music[8] = "MENU";
dword g_time;
int dm_pnum;
int dm_pl1p;
int dm_pl2p;
pos_t dm_pos[100];
byte cheat;

static void *telepsnd;

int lt_time;
int lt_type;
int lt_side;
int lt_ypos;
static int lt_force;
static void *ltnsnd[2];

int g_trans;
static int g_transt;

static void set_trans(int st) {
  switch(g_st) {
    case GS_ENDANIM: case GS_END2ANIM: case GS_DARKEN:
    case GS_BVIDEO: case GS_EVIDEO: case GS_END3ANIM:
      g_st=st;return;
  }
  switch(g_st=st) {
    case GS_ENDANIM: case GS_END2ANIM: case GS_DARKEN:
    case GS_BVIDEO: case GS_EVIDEO: case GS_END3ANIM:
      return;
  }
  g_trans=1;g_transt=0;
}

void load_game (int n) {
  MUS_free();
  W_init();
  F_loadgame(n);
  set_trans(GS_GAME);
  pl1.drawst=0xFF;
  if(_2pl) pl2.drawst=0xFF;
  BM_remapfld();
  BM_clear(BM_PLR1|BM_PLR2|BM_MONSTER);
  BM_mark(&pl1.o,BM_PLR1);
  if(_2pl) BM_mark(&pl2.o,BM_PLR2);
  MN_mark();
  //MUS_start(music_time);
  MUS_start(0);
}

void G_start (void) {
  char s[8];
  MUS_free();
  sprintf(s,"MAP%02u",(word)g_map);
  F_loadmap(s);
  set_trans(GS_GAME);
  pl1.drawst=0xFF;
  if(_2pl) pl2.drawst=0xFF;
  g_exit=0;
  itm_rtime=(g_dm)?1092:0;
  p_immortal=0;PL_JUMP=10;
  g_time=0;
  lt_time=1000;
  lt_force=1;
  if(!_2pl) pl1.lives=3;
  BM_remapfld();
  BM_clear(BM_PLR1|BM_PLR2|BM_MONSTER);
  BM_mark(&pl1.o,BM_PLR1);
  if(_2pl) BM_mark(&pl2.o,BM_PLR2);
  MN_mark();
  //MUS_start(music_time);
  MUS_start(0);
}

#define GGAS_TOTAL (MN__LAST-MN_DEMON+16+10)

void G_init (void) {
  int i,j;
  char s[9];

  logo("G_init: setup game resources ");
  logo_gas(5,GGAS_TOTAL);
  telepsnd=Z_getsnd("TELEPT");
  ltnsnd[0]=Z_getsnd("THUND1");
  ltnsnd[1]=Z_getsnd("THUND2");
  DOT_alloc();
  SMK_alloc();
  FX_alloc();
  WP_alloc();
  IT_alloc();
  SW_alloc();
  PL_alloc();
  MN_alloc();
  Z_initst();
  logo_gas(GGAS_TOTAL,GGAS_TOTAL);
  logo("\n");
  GM_init();
  pl1.color=0x70;
  pl2.color=0x60;
  g_trans=0;
}

static int G_beg_video (void) {
/*
  switch(g_map) {
    case 3: return A8_start("FALL");
    case 4: return A8_start("KORIDOR");
    case 5: return A8_start("SKULL");
    case 6: return A8_start("TORCHES");
    case 7: return A8_start("CACO");
    case 8: return A8_start("DARTS");
    case 9: return A8_start("FISH");
    case 10: return A8_start("TRAP");
    case 11: return A8_start("JAIL");
    case 12: return A8_start("MMON1");
    case 13: return A8_start("TOWER");
    case 14: return A8_start("SAPOG");
    case 15: return A8_start("SWITCH");
    case 16: return A8_start("ACCEL");
    case 17: return A8_start("MEAT");
    case 18: return A8_start("LEGION");
    case 19: return A8_start("CLOUDS");
  }
*/
  return 0;
}


static int G_end_video (void) {
/*
  switch(g_map) {
    case 1: return A8_start("TRUBA");
    case 10: return A8_start("GOTCHA");
  }
*/
  return 0;
}

void G_act (void) {
  static byte pcnt=0;
/*
  if(g_trans) {
    if(g_transt==0) {
      V_setscr(NULL);memcpy(fx_scr1,scra,64000);
      V_setscr(fx_scr2);
      transdraw=1;G_draw();transdraw=0;
      V_setscr(scrbuf);
    }
    FX_trans1(g_transt*2);
    V_copytoscr(0,320,0,200);
    if(++g_transt>32) {
      g_trans=0;
    }
    return;
    g_trans=0;
  }
*/ g_trans=0;

  
  if(g_st==GS_BVIDEO || g_st==GS_EVIDEO) {
    if (!A8_nextframe() || lastkey == KEY_ESCAPE) {
      if (lastkey == KEY_ESCAPE) lastkey = KEY_UNKNOWN;
      A8_close();
      if(g_st==GS_BVIDEO) G_start();
      else goto inter;
    }
//    V_copytoscr(0,SCRW,0,SCRH);//V_copytoscr(0,320,0,200);
    return;
  }else if(g_st==GS_ENDANIM || g_st==GS_END2ANIM || g_st==GS_END3ANIM) {
    if(!A8_nextframe()) {
      switch(g_st) {
        case GS_ENDANIM: g_st=GS_DARKEN;break;
        case GS_END2ANIM: g_st=GS_END3ANIM;A8_start("KONEC");break;
        case GS_END3ANIM: g_st=GS_ENDSCR;lastkey=0;break;
      }
      return;
    }
//    V_copytoscr(0,SCRW,0,SCRH);//V_copytoscr(0,320,0,200);
    return;
  }else if(g_st==GS_DARKEN) {
      g_st=GS_END2ANIM;A8_start("CREDITS");
    return;
  }
  
  if(GM_act()) return;

  

  switch(g_st) {
	case GS_TITLE: case GS_ENDSCR:
            
	  return;
	case GS_INTER:
#ifdef DEMO
	  if(keys[0x39] || keys[0x1C] || keys[0x9C]) {
	    set_trans(GS_TITLE);
	  }
#else
    if (I_pressed(KEY_SPACE) || I_pressed(KEY_RETURN) || I_pressed(KEY_KP_ENTER)) {
	    if (!G_beg_video()) {
        G_start();
      } else {
	      g_st = GS_BVIDEO;
        MUS_free();
	    }
    }
#endif
	  return;
  }


  if(sky_type==2) {
    if(lt_time>LT_DELAY || lt_force) {
      if(!(rand()&31) || lt_force) {
        lt_force=0;
        lt_time=-LT_HITTIME;
        lt_type=rand()%2;
        lt_side=rand()&1;
        lt_ypos=rand()&31;
        Z_sound(ltnsnd[rand()&1],128);
      }
    }else ++lt_time;
  }
  ++g_time;
  pl1.hit=0;pl1.hito=-3;
  if(_2pl) {pl2.hit=0;pl2.hito=-3;}
  G_code();

  IT_act();
  SW_act();
  if(_2pl) {
	if(pcnt) {PL_act(&pl1);PL_act(&pl2);}
	else {PL_act(&pl2);PL_act(&pl1);}
	pcnt^=1;
  }else PL_act(&pl1);
  MN_act();
  if(fld_need_remap) BM_remapfld();
  BM_clear(BM_PLR1|BM_PLR2|BM_MONSTER);
  BM_mark(&pl1.o,BM_PLR1);
  if(_2pl) BM_mark(&pl2.o,BM_PLR2);
  MN_mark();
  WP_act();
  DOT_act();
  SMK_act();
  FX_act();
  if(_2pl) {
	PL_damage(&pl1);PL_damage(&pl2);
	if(!(pl1.f&PLF_PNSND) && pl1.pain) PL_cry(&pl1);
	if(!(pl2.f&PLF_PNSND) && pl2.pain) PL_cry(&pl2);
	if((pl1.pain-=5) < 0) {pl1.pain=0;pl1.f&=(0xFFFF-PLF_PNSND);}
	if((pl2.pain-=5) < 0) {pl2.pain=0;pl2.f&=(0xFFFF-PLF_PNSND);}
  }else{
	PL_damage(&pl1);
	if(!(pl1.f&PLF_PNSND) && pl1.pain) PL_cry(&pl1);
	if((pl1.pain-=5) < 0) {pl1.pain=0;pl1.f&=(0xFFFF-PLF_PNSND);}
  }
  if(g_exit==1) {

	if(G_end_video()) {
	  MUS_free();
	  g_st=GS_EVIDEO;
	  return;
	}

inter:
	switch(g_map) {
	  case 19: g_st=GS_ENDANIM;A8_start("FINAL");break;
	  case 31: case 32: g_map=16;set_trans(GS_INTER);break;
	  default: ++g_map;set_trans(GS_INTER);break;
	}
	MUS_free();
	if (g_st == GS_INTER) {
	  MUS_load("INTERMUS");
  } else {
    MUS_load("\x8a\x8e\x8d\x85\x96\x0");
    MUS_volume(128);
  }
	MUS_start(0);
  }else if(g_exit==2) {
	switch(g_map) {
	  case 31: g_map=32;set_trans(GS_INTER);break;
	  case 32: g_map=16;set_trans(GS_INTER);break;
	  default: g_map=31;set_trans(GS_INTER);break;
	}
	MUS_free();
	MUS_load("INTERMUS");
	MUS_start(0);
  }
  
#ifdef DEMO
  if(g_dm && g_time>10920) {set_trans(GS_INTER);}
#endif
}

void G_respawn_player (player_t *p) {
  int i;

  if(dm_pnum==2) {
    if(p==&pl1) i=dm_pl1p^=1;
    else i=dm_pl2p^=1;
    p->o.x=dm_pos[i].x;p->o.y=dm_pos[i].y;p->d=dm_pos[i].d;
    FX_tfog(dm_pos[i].x,dm_pos[i].y);Z_sound(telepsnd,128);
    return;
  }
  do{i=myrand(dm_pnum);}while(i==dm_pl1p || i==dm_pl2p);
  p->o.x=dm_pos[i].x;p->o.y=dm_pos[i].y;p->d=dm_pos[i].d;
  if(p==&pl1) dm_pl1p=i; else dm_pl2p=i;
  FX_tfog(dm_pos[i].x,dm_pos[i].y);Z_sound(telepsnd,128);
}
