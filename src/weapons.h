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

#ifndef WEAPONS_H_INLUDED
#define WEAPONS_H_INLUDED

#define MAXWPN 300

typedef struct {
  obj_t o;
  byte t, s;
  int own;
  short target;
} weapon_t;

extern weapon_t wp[MAXWPN];

void WP_alloc (void);
void WP_init (void);
void WP_act (void);
void WP_gun (int x, int y, int xd, int yd, int o, int v);
void WP_punch (int x, int y, int d, int own);
int WP_chainsaw (int x, int y, int d, int own);
void WP_rocket (int x, int y, int xd, int yd, int o);
void WP_revf (int x, int y, int xd, int yd, int o, int t);
void WP_plasma (int x, int y, int xd, int yd, int o);
void WP_ball1 (int x, int y, int xd, int yd, int o);
void WP_ball2 (int x, int y, int xd, int yd, int o);
void WP_ball7 (int x, int y, int xd, int yd, int o);
void WP_aplasma (int x, int y, int xd, int yd, int o);
void WP_manfire (int x, int y, int xd, int yd, int o);
void WP_bfgshot (int x, int y, int xd, int yd, int o);
void WP_bfghit (int x, int y, int o);
void WP_pistol (int x,int y,int xd,int yd,int o);
void WP_mgun (int x, int y, int xd, int yd, int o);
void WP_shotgun (int x, int y, int xd, int yd, int o);
void WP_dshotgun (int x, int y, int xd, int yd, int o);
void WP_ognemet (int x, int y, int xd, int yd, int xv, int yv, int o);

#endif /* WEAPONS_H_INLUDES */
