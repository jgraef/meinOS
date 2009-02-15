/*
    meinOS - A unix-like x86 microkernel operating system
    Copyright (C) 2008  Janosch Gräf <janosch.graef@gmx.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _LOCALE_H_
#define _LOCALE_H_

#ifndef NULL
  #define NULL ((void*)0)
#endif

#define LC_ALL      1
#define LC_COLLATE  2
#define LC_CTYPE    3
#define LC_MESSAGES 4
#define LC_MONETARY 5
#define LC_NUMERIC  6
#define LC_TIME     7

struct lconv {
  char *currency_symbol;
  char *decimal_point;
  char frac_digits;
  char *grouping;
  char *int_curr_symbol;
  char int_frac_digits;
  char int_n_cs_precedes;
  char int_n_sep_by_space;
  char int_n_sign_posn;
  char int_p_cs_precedes;
  char int_p_sep_by_space;
  char int_p_sign_posn;
  char *mon_decimal_point;
  char *mon_grouping;
  char *mon_thousands_sep;
  char *negative_sign;
  char n_cs_precedes;
  char n_sep_by_space;
  char n_sign_posn;
  char *positive_sign;
  char p_cs_precedes;
  char p_sep_by_space;
  char p_sign_posn;
  char *thousands_sep;
};

struct lconv *localeconv(void);
char *setlocale(int category,const char *locale);

#endif /* _LOCALE_H_ */
