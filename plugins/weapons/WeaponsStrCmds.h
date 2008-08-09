/* ANSI-C code produced by gperf version 3.0.1 */
/* Command-line: gperf -L ANSI-C -tCG -m 10 -K name -H WeaponsStrCommandHash -N WeaponsStrCommandLookup -W WeaponsStrCommandList --output-file=gperf.out WeaponsStrCmds.gperf  */
/* Computed positions: -k'1,10' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "WeaponsStrCmds.gperf"

/***************************************************************************
    NWNXWeapons.h - Interface for the CNWNXWeapons class.
    Copyright (C) 2007 Doug Swarin (zac@intertex.net)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ***************************************************************************/

#ifndef NWNX_WEAPONS_STRCMDS_H
#define NWNX_WEAPONS_STRCMDS_H

#ifdef __cplusplus
extern "C" {
#endif

#line 30 "WeaponsStrCmds.gperf"
struct WeaponsStrCommand_s {
    const char  *name;
    void       (*func)(CGameObject *, char *);
};

#define NWNX_WEAPONS_STRCMD_TOTAL_KEYWORDS 28
#define NWNX_WEAPONS_STRCMD_MIN_WORD_LENGTH 18
#define NWNX_WEAPONS_STRCMD_MAX_WORD_LENGTH 60
#define NWNX_WEAPONS_STRCMD_MIN_HASH_VALUE 18
#define NWNX_WEAPONS_STRCMD_MAX_HASH_VALUE 61
/* maximum key range = 44, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
WeaponsStrCommandHash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 16, 15,
       0,  0, 62, 14, 62, 62, 14, 62, 62,  0,
      13, 62, 62,  1, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
      62, 62, 62, 62, 62, 62
    };
  return len + asso_values[(unsigned char)str[9]] + asso_values[(unsigned char)str[0]];
}

static const struct WeaponsStrCommand_s WeaponsStrCommandList[] =
  {
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 39 "WeaponsStrCmds.gperf"
    {"GETWEAPONFOCUSFEAT",                     Func_GetWeaponFocusFeat               },
#line 53 "WeaponsStrCmds.gperf"
    {"SETWEAPONFOCUSFEAT",                     Func_SetWeaponFocusFeat               },
    {""},
#line 44 "WeaponsStrCmds.gperf"
    {"GETWEAPONOFCHOICEFEAT",                  Func_GetWeaponOfChoiceFeat            },
#line 58 "WeaponsStrCmds.gperf"
    {"SETWEAPONOFCHOICEFEAT",                  Func_SetWeaponOfChoiceFeat            },
    {""}, {""},
#line 40 "WeaponsStrCmds.gperf"
    {"GETWEAPONGREATERFOCUSFEAT",              Func_GetWeaponGreaterFocusFeat        },
#line 54 "WeaponsStrCmds.gperf"
    {"SETWEAPONGREATERFOCUSFEAT",              Func_SetWeaponGreaterFocusFeat        },
    {""},
#line 48 "WeaponsStrCmds.gperf"
    {"GETWEAPONSPECIALIZATIONFEAT",            Func_GetWeaponSpecializationFeat      },
#line 62 "WeaponsStrCmds.gperf"
    {"SETWEAPONSPECIALIZATIONFEAT",            Func_SetWeaponSpecializationFeat      },
#line 49 "WeaponsStrCmds.gperf"
    {"GETWEAPONSUPERIORCRITICALFEAT",          Func_GetWeaponSuperiorCriticalFeat    },
#line 63 "WeaponsStrCmds.gperf"
    {"SETWEAPONSUPERIORCRITICALFEAT",          Func_SetWeaponSuperiorCriticalFeat    },
    {""},
#line 46 "WeaponsStrCmds.gperf"
    {"GETWEAPONOVERWHELMINGCRITICALFEAT",      Func_GetWeaponOverwhelmingCriticalFeat},
#line 60 "WeaponsStrCmds.gperf"
    {"SETWEAPONOVERWHELMINGCRITICALFEAT",      Func_SetWeaponOverwhelmingCriticalFeat},
#line 42 "WeaponsStrCmds.gperf"
    {"GETWEAPONISMONKWEAPON",                  Func_GetWeaponIsMonkWeapon            },
#line 56 "WeaponsStrCmds.gperf"
    {"SETWEAPONISMONKWEAPON",                  Func_SetWeaponIsMonkWeapon            },
#line 37 "WeaponsStrCmds.gperf"
    {"GETWEAPONEPICFOCUSFEAT",                 Func_GetWeaponEpicFocusFeat           },
#line 51 "WeaponsStrCmds.gperf"
    {"SETWEAPONEPICFOCUSFEAT",                 Func_SetWeaponEpicFocusFeat           },
#line 47 "WeaponsStrCmds.gperf"
    {"GETWEAPONPOWERCRITICALFEAT",             Func_GetWeaponPowerCriticalFeat       },
#line 61 "WeaponsStrCmds.gperf"
    {"SETWEAPONPOWERCRITICALFEAT",             Func_SetWeaponPowerCriticalFeat       },
#line 43 "WeaponsStrCmds.gperf"
    {"GETWEAPONLEGENDARYFOCUSFEAT",            Func_GetWeaponLegendaryFocusFeat      },
#line 57 "WeaponsStrCmds.gperf"
    {"SETWEAPONLEGENDARYFOCUSFEAT",            Func_SetWeaponLegendaryFocusFeat      },
#line 41 "WeaponsStrCmds.gperf"
    {"GETWEAPONIMPROVEDCRITICALFEAT",          Func_GetWeaponImprovedCriticalFeat    },
#line 55 "WeaponsStrCmds.gperf"
    {"SETWEAPONIMPROVEDCRITICALFEAT",          Func_SetWeaponImprovedCriticalFeat    },
    {""},
#line 38 "WeaponsStrCmds.gperf"
    {"GETWEAPONEPICSPECIALIZATIONFEAT",        Func_GetWeaponEpicSpecializationFeat  },
#line 52 "WeaponsStrCmds.gperf"
    {"SETWEAPONEPICSPECIALIZATIONFEAT",        Func_SetWeaponEpicSpecializationFeat  },
#line 36 "WeaponsStrCmds.gperf"
    {"GETWEAPONDEVASTATINGCRITICALFEAT",       Func_GetWeaponDevastatingCriticalFeat },
#line 50 "WeaponsStrCmds.gperf"
    {"SETWEAPONDEVASTATINGCRITICALFEAT",       Func_SetWeaponDevastatingCriticalFeat },
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""},
#line 45 "WeaponsStrCmds.gperf"
    {"GETWEAPONOPTION                         Func_GetWeaponOption"},
#line 59 "WeaponsStrCmds.gperf"
    {"SETWEAPONOPTION                         Func_SetWeaponOption"}
  };

#ifdef __GNUC__
__inline
#endif
static const struct WeaponsStrCommand_s *
WeaponsStrCommandLookup (register const char *str, register unsigned int len)
{
  if (len <= NWNX_WEAPONS_STRCMD_MAX_WORD_LENGTH && len >= NWNX_WEAPONS_STRCMD_MIN_WORD_LENGTH)
    {
      register int key = WeaponsStrCommandHash (str, len);

      if (key <= NWNX_WEAPONS_STRCMD_MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = WeaponsStrCommandList[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &WeaponsStrCommandList[key];
        }
    }
  return 0;
}
#line 64 "WeaponsStrCmds.gperf"


#ifdef __cplusplus
}
#endif

#endif /* NWNX_WEAPONS_STRCMDS_H */

/* vim: set sw=4: */