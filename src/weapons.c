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
#include <stdlib.h>
#include "view.h"
#include "bmap.h"
#include "dots.h"
#include "smoke.h"
#include "weapons.h"
#include "misc.h"
#include "files.h"
#include "game.h"
#include "player.h"
#include "monster.h"
#include "switch.h"

enum{NONE=0,ROCKET,PLASMA,APLASMA,BALL1,BALL2,BALL7,BFGBALL,BFGHIT,
     MANF,REVF,FIRE};

weapon_t wp[MAXWPN];

static void *snd[14];
static void throw(int,int,int,int,int,int,int,int);

void WP_alloc (void) {
  int i;
  static char nm[14][6]={
	"PISTOL",
	"SHOTGN",
	"DSHTGN",
	"RLAUNC",
	"RXPLOD",
	"PLASMA",
	"FIRSHT",
	"FIRXPL",
	"BAREXP",
	"PUNCH",
	"SAWHIT",
	"MGUN",
	"SPARK1",
	"SPARK2"
  };
  for(i=0;i<14;++i) snd[i]=Z_getsnd(nm[i]);
}

void WP_init (void) {
  int i;

  for(i=0;i<MAXWPN;++i) wp[i].t=NONE;
}

void WP_act (void) {
  int i,st;
  static obj_t o;

  for(i=0;i<MAXWPN;++i) if(wp[i].t) {
	if(wp[i].t==ROCKET || wp[i].t==REVF)
	  SMK_gas(wp[i].o.x+Z_sign(wp[i].o.xv)*2,
	    wp[i].o.y-wp[i].o.h/2,3,3,
	    wp[i].o.xv+wp[i].o.vx,wp[i].o.yv+wp[i].o.vy,64
	  );
	--wp[i].o.yv;st=Z_moveobj(&wp[i].o);
	if(st&Z_FALLOUT) {wp[i].t=0;continue;}
	if(st&Z_HITWATER) switch(wp[i].t) {
	  case PLASMA: case APLASMA:
	  case BFGBALL:
		break;
	  default:
		Z_splash(&wp[i].o,wp[i].o.r+wp[i].o.h);break;
	}
	switch(wp[i].t) {
	  case REVF:
		if(Z_getobjpos(wp[i].target,&o))
		  throw(i,wp[i].o.x,wp[i].o.y-2,o.x+o.xv+o.vx,o.y+o.yv+o.vy,2,5,12);
	  case ROCKET:
		if(wp[i].s>=2) {if(++wp[i].s>=8) wp[i].t=0; break;}
		if(st&Z_HITAIR) Z_set_speed(&wp[i].o,12);
		if(st&(Z_HITWALL|Z_HITCEIL|Z_HITLAND)) {
		  wp[i].s=2;wp[i].o.xv=wp[i].o.yv=0;Z_sound(snd[4],128);
		  Z_explode(wp[i].o.x,wp[i].o.y,30,wp[i].own);break;}
		else if(Z_hit(&wp[i].o,10,wp[i].own,HIT_SOME)) {
		  wp[i].s=2;wp[i].o.xv=wp[i].o.yv=0;Z_sound(snd[4],128);
		  Z_explode(wp[i].o.x,wp[i].o.y-wp[i].o.h/2,30,wp[i].own);break;}
		bfg_fly(wp[i].o.x,wp[i].o.y-wp[i].o.h/2,wp[i].own);
		break;
	  case PLASMA:
	  case APLASMA:
		if(st&Z_INWATER) {
		  Z_sound(snd[12],128);
		  Z_water_trap(&wp[i].o);
		  Z_chktrap(1,10,wp[i].own,HIT_ELECTRO);
		  Z_untrap(5);
		  wp[i].t=0;break;
		}
	  case BALL1:
	  case BALL7:
	  case BALL2:
	  case MANF:
		if(wp[i].s>=2)
		  {if(++wp[i].s>=((wp[i].t==BALL1 || wp[i].t==BALL7 || wp[i].t==BALL2 || wp[i].t==MANF)?8:12)) wp[i].t=0; break;}
		if(st&Z_HITAIR) Z_set_speed(&wp[i].o,16);
		if(st&(Z_HITWALL|Z_HITCEIL|Z_HITLAND))
		  {wp[i].s=2;wp[i].o.xv=wp[i].o.yv=0;Z_sound(snd[7],128);break;}
		else if(Z_hit(&wp[i].o,(wp[i].t==BALL7 || wp[i].t==MANF)?40:((wp[i].t==BALL2)?20:5),wp[i].own,HIT_SOME))
		  {wp[i].s=2;wp[i].o.xv=wp[i].o.yv=0;Z_sound(snd[7],128);break;}
		wp[i].s^=1;break;
	  case BFGBALL:
		if(st&Z_INWATER) {
		  Z_sound(snd[8],40);Z_sound(snd[13],128);
		  Z_water_trap(&wp[i].o);
		  Z_chktrap(1,1000,wp[i].own,HIT_ELECTRO);
		  Z_untrap(5);
		  wp[i].t=0;break;
		}
		if(wp[i].s>=2) {if(++wp[i].s>=14) wp[i].t=0; break;}
		else if(st&(Z_HITWALL|Z_HITCEIL|Z_HITLAND)) {
		  Z_bfg9000(wp[i].o.x,wp[i].o.y,wp[i].own);
		  wp[i].s=2;wp[i].o.xv=wp[i].o.yv=0;Z_sound(snd[8],128);break;}
		else if(Z_hit(&wp[i].o,100,wp[i].own,HIT_BFG)) {
		  Z_bfg9000(wp[i].o.x,wp[i].o.y,wp[i].own);
		  wp[i].s=2;wp[i].o.xv=wp[i].o.yv=0;Z_sound(snd[8],128);break;}
		bfg_fly(wp[i].o.x,wp[i].o.y-wp[i].o.h/2,wp[i].own);
		wp[i].s^=1;break;
	  case BFGHIT:
		if(++wp[i].s>=8) wp[i].t=0;
		break;
	  default: break;
	}
  }
}

void WP_gun (int x, int y, int xd, int yd, int o, int v) {
  register dword d,m;
  int sx,sy,lx,ly;
  dword xe,ye,s;
  byte f;

  f=BM_MONSTER|BM_WALL;
  if(o!=-1) f|=BM_PLR1;
  if(o!=-2) f|=BM_PLR2;
  if((xd-=x)>0) sx=1;
  else if(xd<0) sx=-1;
  else sx=0;
  if((yd-=y)>0) sy=1;
  else if(yd<0) sy=-1;
  else sy=0;
  if(!xd && !yd) return;
  if((xd=abs(xd)) > (yd=abs(yd))) d=xd; else d=yd;
  hit_xv=xd*10/d*sx;
  hit_yv=yd*10/d*sy;
  xe=ye=0;
  lx=x;ly=y;
  for(;;) {
	if(x<0 || x>=FLDW*8 || y<0 || y>=FLDH*8) break;
	if(((m=bmap[y>>5][x>>5])&f)) {
	  if(m&BM_WALL) if(fld[y>>3][x>>3]==1 || fld[y>>3][x>>3]==2) {
		for(x=lx,y=ly,xe=ye=0;fld[y>>3][x>>3]!=1 && fld[y>>3][x>>3]!=2;) {
		  lx=x;ly=y;
		  if((xe+=xd)>=d) {xe-=d;x+=sx;}
		  if((ye+=yd)>=d) {ye-=d;y+=sy;}
		}
		DOT_spark(lx,ly,sx*10,sy*10,1);
		break;
	  }
	  if(m&(BM_MONSTER|BM_PLR1|BM_PLR2)) if(Z_gunhit(x,y,o,sx*v,sy*v)) break;
	  lx=x;ly=y;
	  if((xe+=(xd<<3))>=d) {
		x+=xe/d*sx;xe=xe%d;
	  }
	  if((ye+=(yd<<3))>=d) {
		y+=ye/d*sy;ye=ye%d;
	  }
	}else{
	  if(sx==0) m=0;
	  else{m=x&31;if(sx>0) m^=31; ++m;}
	  if(sy==0) s=0;
	  else{s=y&31;if(sy>0) s^=31; ++s;}
	  if((s<m && s!=0) || m==0) m=s;
	  lx=x;ly=y;
		x+=(xd*m+xe)/d*sx;xe=(xd*m+xe)%d;
		y+=(yd*m+ye)/d*sy;ye=(yd*m+ye)%d;
	}
  }
}

void WP_punch (int x, int y, int d, int own) {
  obj_t o;

  o.x=x;o.y=y;o.r=12;o.h=26;
  o.xv=o.yv=o.vx=o.vy=0;
  if(Z_hit(&o,d,own,HIT_SOME)) Z_sound(snd[9],128);
}

int WP_chainsaw (int x, int y, int d, int own) {
  obj_t o;

  o.x=x;o.y=y;o.r=12;o.h=26;
  o.xv=o.yv=o.vx=o.vy=0;
  if(Z_hit(&o,d,own,HIT_SOME)) return 1;
  return 0;
}

static void throw(int i,int x,int y,int xd,int yd,int r,int h,int s) {
  int m;

  wp[i].o.x=x;wp[i].o.y=y+h/2;
  yd-=y;xd-=x;
  if(!(m=max(abs(xd),abs(yd)))) m=1;
  wp[i].o.xv=xd*s/m;wp[i].o.yv=yd*s/m;
  wp[i].o.r=r;wp[i].o.h=h;
  wp[i].o.vx=wp[i].o.vy=0;
}

void WP_rocket (int x, int y, int xd, int yd, int o) {
  int i;

  for(i=0;i<MAXWPN;++i) if(!wp[i].t) {
    Z_sound(snd[3],128);
    wp[i].t=ROCKET;wp[i].s=(xd>=x)?1:0;
	wp[i].own=o;
	throw(i,x,y,xd,yd,2,5,12);
	return;
  }
}

void WP_revf (int x, int y, int xd, int yd, int o, int t) {
  int i;

  for(i=0;i<MAXWPN;++i) if(!wp[i].t) {
    Z_sound(snd[3],128);
    wp[i].t=REVF;wp[i].s=(xd>=x)?1:0;
	wp[i].own=o;wp[i].target=t;
	throw(i,x,y,xd,yd,2,5,12);
	return;
  }
}

void WP_plasma (int x, int y, int xd, int yd, int o) {
  int i;

  for(i=0;i<MAXWPN;++i) if(!wp[i].t) {
    Z_sound(snd[5],64);
    wp[i].t=PLASMA;wp[i].s=0;
	wp[i].own=o;
	throw(i,x,y,xd,yd,2,5,16);
    return;
  }
}

void WP_ball1 (int x, int y, int xd, int yd, int o) {
  int i;

  for(i=0;i<MAXWPN;++i) if(!wp[i].t) {
	wp[i].t=BALL1;wp[i].s=0;
	wp[i].own=o;
	throw(i,x,y,xd,yd,2,5,16);
	return;
  }
}

void WP_ball2 (int x, int y, int xd, int yd, int o) {
  int i;

  for(i=0;i<MAXWPN;++i) if(!wp[i].t) {
	wp[i].t=BALL2;wp[i].s=0;
	wp[i].own=o;
	throw(i,x,y,xd,yd,2,5,16);
	return;
  }
}

void WP_ball7 (int x, int y, int xd, int yd, int o) {
  int i;

  for(i=0;i<MAXWPN;++i) if(!wp[i].t) {
	wp[i].t=BALL7;wp[i].s=0;
	wp[i].own=o;
	throw(i,x,y,xd,yd,2,5,16);
	return;
  }
}

void WP_aplasma (int x, int y, int xd, int yd, int o) {
  int i;

  for(i=0;i<MAXWPN;++i) if(!wp[i].t) {
	Z_sound(snd[5],64);
	wp[i].t=APLASMA;wp[i].s=0;
	wp[i].own=o;
	throw(i,x,y,xd,yd,2,5,16);
	return;
  }
}

void WP_manfire (int x, int y, int xd, int yd, int o) {
  int i;

  for(i=0;i<MAXWPN;++i) if(!wp[i].t) {
	Z_sound(snd[6],128);
	wp[i].t=MANF;wp[i].s=0;
	wp[i].own=o;
	throw(i,x,y,xd,yd,5,11,16);
	return;
  }
}

void WP_bfgshot (int x, int y, int xd, int yd, int o) {
  int i;

  for(i=0;i<MAXWPN;++i) if(!wp[i].t) {
    wp[i].t=BFGBALL;wp[i].s=0;
	wp[i].own=o;
	throw(i,x,y,xd,yd,5,12,16);
	return;
  }
}

void WP_bfghit (int x, int y, int o) {
  int i;

  for(i=0;i<MAXWPN;++i) if(!wp[i].t) {
    wp[i].t=BFGHIT;wp[i].s=0;
    wp[i].o.x=x;wp[i].o.y=y;
    wp[i].o.xv=wp[i].o.yv=0;
    wp[i].o.r=0;wp[i].o.h=1;
    wp[i].o.vx=wp[i].o.vy=0;
    wp[i].own=o;
    return;
  }
}

void WP_pistol (int x,int y,int xd,int yd,int o) {
  Z_sound(snd[0],96);
  WP_gun(x,y,xd,yd,o,1);
  if(g_dm) {
    WP_gun(x,y+1,xd,yd+1,o,1);
    WP_gun(x,y-1,xd,yd-1,o,1);
  }
}

void WP_mgun (int x, int y, int xd, int yd, int o) {
  Z_sound(snd[11],128);
  WP_gun(x,y,xd,yd,o,1);
}

void WP_shotgun (int x, int y, int xd, int yd, int o) {
  int i,j;

  Z_sound(snd[1],128);
  for(i=0;i<10;++i) {
    j=myrand(4*2+1)-4;
    WP_gun(x,y+j,xd,yd+j,o,i&1);
  }
}

void WP_dshotgun (int x, int y, int xd, int yd, int o) {
  int i,j;

  Z_sound(snd[2],128);
  for(i=(g_dm)?25:20;i>=0;--i) {
    j=myrand(10*2+1)-10;
    WP_gun(x,y+j,xd,yd+j,o,(i%3)?0:1);
  }
}

void WP_ognemet (int x, int y, int xd, int yd, int xv, int yv, int o) {
  int m;

  m=abs(xd-x);if(!m) m=abs(yd-y);
  SMK_flame(x,y,xv,yv,2,2,(xd-x)*3000/m,(yd-y)*3000/m,1,o);
}
