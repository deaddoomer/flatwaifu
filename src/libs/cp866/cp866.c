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

#include "cp866.h"

int cp866_isalpha (int ch) {
  return (ch >= 0x41 && ch <= 0x5A) || (ch >= 0x61 && ch <= 0x7A) || (ch >= 0x80 && ch <= 0xAF) || (ch >= 0xE0 && ch <= 0xF7);
}

int cp866_isupper (int ch) {
  return (ch >= 0x41 && ch <= 0x5A) || (ch >= 0x80 && ch <= 0x9F) || ch == 0xF0 || ch == 0xF2 || ch == 0xF4 || ch == 0xF6;
}

int cp866_islower (int ch) {
  return (ch >= 0x61 && ch <= 0x7A) || (ch >= 0xA0 && ch <= 0xAF) || (ch >= 0xE0 && ch <= 0xEF) || ch == 0xF1 || ch == 0xF3 || ch == 0xF5 || ch == 0xF7;
}

int cp866_toupper (int ch) {
  if (ch >= 0x61 && ch <= 0x7A) {
    return ch - 0x61 + 0x41;
  } else if (ch >= 0xA0 && ch <= 0xAF) {
    return ch - 0xA0 + 0x80;
  } else if (ch >= 0xE0 && ch <= 0xEF) {
    return ch - 0xE0 + 0x90;
  } else {
    switch (ch) {
      case 0xF1:
      case 0xF3:
      case 0xF5:
      case 0xF7:
        return ch - 1;
      default:
        return ch;
    }
  }
}

int cp866_tolower (int ch) {
  if (ch >= 0x41 && ch <= 0x5A) {
    return ch - 0x41 + 0x61;
  } else if (ch >= 0x80 && ch <= 0x8F) {
    return ch - 0x80 + 0xA0;
  } else if (ch >= 0x90 && ch <= 0x9F) {
    return ch - 0x90 + 0xE0;
  } else {
    switch (ch) {
      case 0xF0:
      case 0xF2:
      case 0xF4:
      case 0xF6:
        return ch + 1;
      default:
        return ch;
    }
  }
}

int cp866_strcasecmp (const char *a, const char *b) {
  const char *aa = a;
  const char *bb = b;
  while (*aa != 0 && cp866_tolower(*aa) == cp866_tolower(*bb)) {
    aa++;
    bb++;
  }
  return *(const unsigned char *)aa - *(const unsigned char *)bb;
}

int cp866_strncasecmp (const char *a, const char *b, unsigned int n) {
  const char *aa = a;
  const char *bb = b;
  unsigned int i = n;
  while (i > 0 && *aa != 0 && cp866_tolower(*aa) == cp866_tolower(*bb)) {
    i--;
    aa++;
    bb++;
  }
  return i == 0 ? 0 : *(const unsigned char *)aa - *(const unsigned char *)bb;
}

int cp866_ctou (int ch) {
  switch (ch) {
    case 0xB0: return 0x2591;
    case 0xB1: return 0x2592;
    case 0xB2: return 0x2593;
    case 0xB3: return 0x2502;
    case 0xB4: return 0x2524;
    case 0xB5: return 0x2561;
    case 0xB6: return 0x2562;
    case 0xB7: return 0x2556;
    case 0xB8: return 0x2555;
    case 0xB9: return 0x2563;
    case 0xBA: return 0x2551;
    case 0xBB: return 0x2557;
    case 0xBC: return 0x255D;
    case 0xBD: return 0x255C;
    case 0xBE: return 0x255B;
    case 0xBF: return 0x2510;
    case 0xC0: return 0x2514;
    case 0xC1: return 0x2534;
    case 0xC2: return 0x252C;
    case 0xC3: return 0x251C;
    case 0xC4: return 0x2500;
    case 0xC5: return 0x253C;
    case 0xC6: return 0x255E;
    case 0xC7: return 0x255F;
    case 0xC8: return 0x255A;
    case 0xC9: return 0x2554;
    case 0xCA: return 0x2569;
    case 0xCB: return 0x2566;
    case 0xCC: return 0x2560;
    case 0xCD: return 0x2550;
    case 0xCE: return 0x256C;
    case 0xCF: return 0x2567;
    case 0xD0: return 0x2568;
    case 0xD1: return 0x2564;
    case 0xD2: return 0x2565;
    case 0xD3: return 0x2559;
    case 0xD4: return 0x2558;
    case 0xD5: return 0x2552;
    case 0xD6: return 0x2553;
    case 0xD7: return 0x256B;
    case 0xD8: return 0x256A;
    case 0xD9: return 0x2518;
    case 0xDA: return 0x250C;
    case 0xDB: return 0x2588;
    case 0xDC: return 0x2584;
    case 0xDD: return 0x258C;
    case 0xDE: return 0x2590;
    case 0xDF: return 0x2580;
    case 0xF0: return 0x0401;
    case 0xF1: return 0x0451;
    case 0xF2: return 0x0404;
    case 0xF3: return 0x0454;
    case 0xF4: return 0x0407;
    case 0xF5: return 0x0457;
    case 0xF6: return 0x040E;
    case 0xF7: return 0x045E;
    case 0xF8: return 0x00B0;
    case 0xF9: return 0x2219;
    case 0xFA: return 0x00B7;
    case 0xFB: return 0x221A;
    case 0xFC: return 0x2116;
    case 0xFD: return 0x00A4;
    case 0xFE: return 0x25A0;
    case 0xFF: return 0x00A0;
    default:
      if (ch >= 0x20 && ch <= 0x7E) {
        return ch;
      } else if (ch >= 0x80 && ch <= 0xAF) {
        return ch - 0x80 + 0x410;
      } else if (ch >= 0xE0 && ch <= 0xEF) {
        return ch - 0xE0 + 0x440;
      } else {
        return ch;
      }
  }
}

int cp866_ctoug (int ch) {
  switch (ch) {
    case 0x00: return 0x0000;
    case 0x01: return 0x263A;
    case 0x02: return 0x263B;
    case 0x03: return 0x2665;
    case 0x04: return 0x2666;
    case 0x05: return 0x2663;
    case 0x06: return 0x2660;
    case 0x07: return 0x2022;
    case 0x08: return 0x25D8;
    case 0x09: return 0x25CB;
    case 0x0A: return 0x25D9;
    case 0x0B: return 0x2642;
    case 0x0C: return 0x2640;
    case 0x0D: return 0x266A;
    case 0x0E: return 0x266B;
    case 0x0F: return 0x263C;
    case 0x10: return 0x25BA;
    case 0x11: return 0x25C4;
    case 0x12: return 0x2195;
    case 0x13: return 0x203C;
    case 0x14: return 0x00B6;
    case 0x15: return 0x00A7;
    case 0x16: return 0x25AC;
    case 0x17: return 0x21A8;
    case 0x18: return 0x2191;
    case 0x19: return 0x2193;
    case 0x1A: return 0x2192;
    case 0x1B: return 0x2190;
    case 0x1C: return 0x221F;
    case 0x1D: return 0x2194;
    case 0x1E: return 0x25B2;
    case 0x1F: return 0x25BC;
    case 0x7F: return 0x2302;
    default: return cp866_ctou(ch);
  }
}

int cp866_utoc (int ch) {
  if (ch >= 0x00 && ch <= 0x7F) {
    return ch;
  } else {
    switch (ch) {
      case 0x263A: return 0x01;
      case 0x263B: return 0x02;
      case 0x2665: return 0x03;
      case 0x2666: return 0x04;
      case 0x2663: return 0x05;
      case 0x2660: return 0x06;
      case 0x2022: return 0x07;
      case 0x25D8: return 0x08;
      case 0x25CB: return 0x09;
      case 0x25D9: return 0x0A;
      case 0x2642: return 0x0B;
      case 0x2640: return 0x0C;
      case 0x266A: return 0x0D;
      case 0x266B: return 0x0E;
      case 0x263C: return 0x0F;
      case 0x25BA: return 0x10;
      case 0x25C4: return 0x11;
      case 0x2195: return 0x12;
      case 0x203C: return 0x13;
      case 0x00B6: return 0x14;
      case 0x00A7: return 0x15;
      case 0x25AC: return 0x16;
      case 0x21A8: return 0x17;
      case 0x2191: return 0x18;
      case 0x2193: return 0x19;
      case 0x2192: return 0x1A;
      case 0x2190: return 0x1B;
      case 0x221F: return 0x1C;
      case 0x2194: return 0x1D;
      case 0x25B2: return 0x1E;
      case 0x25BC: return 0x1F;
      case 0x2302: return 0x7F;
      case 0x2591: return 0xB0;
      case 0x2592: return 0xB1;
      case 0x2593: return 0xB2;
      case 0x2502: return 0xB3;
      case 0x2524: return 0xB4;
      case 0x2561: return 0xB5;
      case 0x2562: return 0xB6;
      case 0x2556: return 0xB7;
      case 0x2555: return 0xB8;
      case 0x2563: return 0xB9;
      case 0x2551: return 0xBA;
      case 0x2557: return 0xBB;
      case 0x255D: return 0xBC;
      case 0x255C: return 0xBD;
      case 0x255B: return 0xBE;
      case 0x2510: return 0xBF;
      case 0x2514: return 0xC0;
      case 0x2534: return 0xC1;
      case 0x252C: return 0xC2;
      case 0x251C: return 0xC3;
      case 0x2500: return 0xC4;
      case 0x253C: return 0xC5;
      case 0x255E: return 0xC6;
      case 0x255F: return 0xC7;
      case 0x255A: return 0xC8;
      case 0x2554: return 0xC9;
      case 0x2569: return 0xCA;
      case 0x2566: return 0xCB;
      case 0x2560: return 0xCC;
      case 0x2550: return 0xCD;
      case 0x256C: return 0xCE;
      case 0x2567: return 0xCF;
      case 0x2568: return 0xD0;
      case 0x2564: return 0xD1;
      case 0x2565: return 0xD2;
      case 0x2559: return 0xD3;
      case 0x2558: return 0xD4;
      case 0x2552: return 0xD5;
      case 0x2553: return 0xD6;
      case 0x256B: return 0xD7;
      case 0x256A: return 0xD8;
      case 0x2518: return 0xD9;
      case 0x250C: return 0xDA;
      case 0x2588: return 0xDB;
      case 0x2584: return 0xDC;
      case 0x258C: return 0xDD;
      case 0x2590: return 0xDE;
      case 0x2580: return 0xDF;
      case 0x0401: return 0xF0;
      case 0x0451: return 0xF1;
      case 0x0404: return 0xF2;
      case 0x0454: return 0xF3;
      case 0x0407: return 0xF4;
      case 0x0457: return 0xF5;
      case 0x040E: return 0xF6;
      case 0x045E: return 0xF7;
      case 0x00B0: return 0xF8;
      case 0x2219: return 0xF9;
      case 0x00B7: return 0xFA;
      case 0x221A: return 0xFB;
      case 0x2116: return 0xFC;
      case 0x00A4: return 0xFD;
      case 0x25A0: return 0xFE;
      case 0x00A0: return 0xFF;
      default:
        if (ch >= 0x410 && ch <= 0x43F) {
          return ch - 0x410 + 0x80;
        } else if (ch >= 0x440 && ch <= 0x44F) {
          return ch - 0x440 + 0xE0;
        } else {
          return -1;
        }
    }
  }
}
